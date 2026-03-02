#include <cmath>
#include <cstddef>
#include <cstdint>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#include <conio.h>
#include <windows.h>

#include "cvm_23.h"

using namespace std;

const size_t CLIENTS_MAX = 70;                              // nombre maximal de clients
const size_t COMPTES_MAX = 3;                               // nombre de comptes par client

const double MARGE_CREDIT_MAX =
10000;                                                      // marge de crédit maximun d'un compte du client
const double SOLDE_COMPTE_MAX =
1000000;                                                    // maximun à ne pas dépasser dans un compte d'un client

enum class Commandes {
  AJOUTER,
  AFFICHER,
  DEPOSER,
  RETIRER,
  VIRER,
  LISTER,
  SUPPRIMER,
  QUITTER,
  INCONNUE
};                                                         // à compléter avec les autres commandes...
using Cmd = Commandes;

struct Nom                                                  // nom d'un client
{
  string prenom;
  string nom;
};

struct Adresse                                              // adresse d'un client
{
  string noCivique;
  string rue;
  string ville;
  string codePostal;
};

struct Personne                                             // informations personnelles d'un client
{
  Nom nom;
  Adresse adresse;
  string telephone;
  string nas;
};

struct Compte                                               // un compte d'un client
{
  double solde;
  double margeCredit;
};

struct Client                                               // informations relatives à un client
{
  Personne info;
  Compte comptes[COMPTES_MAX];
  time_t date;                                              // date d'ajout du client
};

struct Banque                                               // La banque de clients
{
  size_t cpt = 0;                                           // cpt: compteur de client [0..CLIENTS_MAX]
  Client clients[CLIENTS_MAX];
};

/* ----------------------------------- */
/* LES VARIABLES GLOBALES DU PROGRAMME */
/* ----------------------------------- */

// ATTENTION: les variables globales ne sont pas permises dans ce TP

/* ------------------------------------------ */
/* LES DÉCLARATIONS (PROTOTYPES) DE FONCTIONS */
/* ------------------------------------------ */

void io_bip();
void io_clean();
double
io_round(double v,
         size_t p = 2);

// Mes déclarations des fonctions

// INPUT (Élements d'interfaces pour recevoir des informations)
string recupererString(string question);
double recupererArgent(string question, double min, double max);
size_t recupererSizeT(string question, size_t min, size_t max);
bool questionOuiNon(string question);

// VIEWS (Afficher des informations)
void printBreaks(size_t n);
void printCenter(string text, size_t width);
void afficherClient(const Client & client);

// MODELS (Modifier des données)
bool ajouterClient(Banque &b, const Client& c);
bool supprimerClient(Banque &b, const Client& c);
Client getClient(const Banque &b, size_t numeroClient);
Compte getCompte(const Client &client, const size_t numeroCompte);
double getMaxSoldeForCompte(const Compte &compte);
string toArgentStr(double argent);
string toDateStr(time_t time);

// Mes définitions des fonctions

// INPUT

string recupererString(string question) {
  const uint32_t width = 19;
  string reponse;
  cout << left << setw(width) << question << " : ";
  struct { size_t x, y; } pos = { wherex(), wherey() };
  do {
    gotoxy(pos.x, pos.y);
    clreol();
    getline(cin, reponse);
  } while (reponse.empty());
  return reponse;
};

double recupererArgent(string question, double min, double max) {
  const uint32_t width = 19;
  double reponse = 0.0;
  cout << left << setw(width) << question << " : ";
  size_t x = wherex(), y = wherey();
  bool result = false;
  do {
    gotoxy(x, y);
    clreoscr();
    io_clean();
    result = (bool) (cin >> reponse);
  } while (cin.fail() || !result || cin.bad() || (reponse > max) || (reponse < min));
  reponse = io_round(reponse, 2);
  gotoxy(x, y);
  clreoscr();
  cout << fixed << reponse << endl;
  return reponse;
}

size_t recupererSizeT(string question, size_t min, size_t max) {
  cout << question;
  size_t x = wherex(), y = wherey();
  size_t reponse = 0;
  bool result = false;
  do {
    gotoxy(x, y);
    clreol();
    io_clean();
    result = (bool) (cin >> reponse);
  } while(cin.fail() || !result || cin.bad() || (reponse > max) || (reponse < min));
  return reponse;
}

size_t recupererNumeroClient(const Banque &b) {
  size_t x = wherex(), y = wherey();
  string question = "Entrez un numero de client (1 - " + to_string(b.cpt) + " ou 0 pour annuler).\nNumero : ";
  size_t numeroClient = recupererSizeT(question, 0, b.cpt);
  gotoxy(x, y);
  clreoscr();
  return numeroClient;
}

size_t recupererNumeroCompte() {
  string question = "Quel compte (1-" + to_string(COMPTES_MAX) + " ou 0 pour annuler) ? ";
  size_t numeroCompte = recupererSizeT(question, 0, COMPTES_MAX);
  return numeroCompte;
}

double recupererDepot(const Compte & compte) {
  if (double depotMax = getMaxSoldeForCompte(compte)) {
    string question = "Montant (max: " + toArgentStr(depotMax) + ")";
    return recupererArgent(question, 0, depotMax);
  }

  cerr << "Une erreur est survenu";
  return {};
}

bool questionOuiNon(string question) {
  cout << question << " (O/N) ";
  int x = wherex(), y = wherey();
  int ch;
  do {
    gotoxy(x, y);
    clreol();
    io_clean();
    ch = toupper(_getch());
  } while (ch != 'O' && ch != 'N');
  return ch == 'O';
}

// VIEWS

void printBreaks(size_t n) {
  while (n != 0) {
    cout << endl;
    n--;
  }
}
void printRepeat(char c, size_t n) {
  while (n != 0) {
    cout << c;
    n--;
  }
}
void printCenter(string text, size_t width) {
  size_t textSize = text.size();
  if (textSize < width) {
    // print center
    size_t pad = (width - textSize) / 2;
    size_t other = width - pad;
    cout << setw(pad) << "" << left << setw(other) << text;
  } else {
    // just print
    cout << text;
  }
}

void afficherNomClient(const Client & client) {
  cout << "Nom: " << client.info.nom.prenom
  << " " << client.info.nom.nom;
}

void afficherSoldesClient(const Client & client) {
  const size_t TABLE_WIDTH = 18;

  // Print table headers
  cout << left
  << setw(TABLE_WIDTH) << " No. de compte"
  << '|'
  << setw(TABLE_WIDTH) << " Solde"
  << '|'
  << setw(TABLE_WIDTH) << " Marge de credit";

  printBreaks(1);
  // Print <hr>
  printRepeat('-', (TABLE_WIDTH * 3) + 2);
  printBreaks(1);

  // Print each account info
  for (size_t i = 0; i < COMPTES_MAX; i++) {
    Compte compte = client.comptes[i];
    printCenter(to_string(i + 1), TABLE_WIDTH);
    cout << "|";
    printCenter(toArgentStr(compte.solde), TABLE_WIDTH);
    cout << "|";
    printCenter(toArgentStr(compte.margeCredit), TABLE_WIDTH);
    printBreaks(1);
  }
}

void afficherClient(const Client & client) {
  afficherNomClient(client);

  printBreaks(2);

  afficherSoldesClient(client);

  printBreaks(1);

  cout << "Informations personnelles";
  printBreaks(2);

  // Get infos
  struct Info {
    string key;
    string value;
  };
  Info infos[7] = {
    { "No. Civique", client.info.adresse.noCivique },
    { "Rue", client.info.adresse.rue },
    { "Ville", client.info.adresse.ville },
    { "Code postal", client.info.adresse.codePostal },
    { "Telephone", client.info.telephone },
    { "NAS", client.info.nas },
    { "\nMembre", toDateStr(client.date) },
  };

  const size_t WIDTH = 18;

  cout << left;
  for (size_t i = 0; i < 7; i++) {
    Info info = infos[i];
    cout << setw(WIDTH) << info.key;
    cout << ": ";
    cout << setw(WIDTH) << info.value;
    printBreaks(1);
  }
}

// MODELS

bool ajouterClient(Banque &b, const Client& c) {
  if (b.cpt < CLIENTS_MAX) {
    b.clients[b.cpt] = c;
    b.cpt++;
    return true;
  } else {
    return false;
  }
}

bool supprimerClient(Banque &b, size_t id) {
  if (id >= b.cpt) {
    return false;
  } else {
    for (size_t i = id; i < b.cpt; i++) {
        b.clients[id] = b.clients[id + 1];
    }
    b.cpt--;
    return true;
  }
}

bool deposerArgent(Banque & b, size_t numeroClient, size_t numeroCompte, double montant) {
  if (numeroClient < b.cpt && numeroCompte < COMPTES_MAX) {
    b.clients[numeroClient].comptes[numeroCompte].solde += montant;
    return true;
  } else {
    return false;
  }
}

Client getClient(const Banque &b, size_t id) {
  if (id < b.cpt) {
    return b.clients[id];
  } else {
    return {};
  }
}

Compte getCompte(const Client & client, const size_t numeroCompte) {
  if (numeroCompte < COMPTES_MAX) {
    return client.comptes[numeroCompte];
  } else {
    return {};
  }
}

double getMaxSoldeForCompte(const Compte &compte) {
  return SOLDE_COMPTE_MAX - compte.solde;
}

string toArgentStr(double argent) {
  // save as fixed point
  auto fixedPoint = static_cast<int64_t>(argent * 100);
  // save as string
  string str = (fixedPoint == 0) ? "000" : to_string(fixedPoint);
  // Add dot to string
  str.insert(str.size() - 2, ".");
  // add dolar sign
  str.append(" $");

  return str;
}

string toDateStr(time_t time) {
  struct tm timeinfo;
  localtime_s(&timeinfo, &time);

  char buffer[20];
  size_t result = strftime(buffer, sizeof(buffer), "%d/%m/%Y", &timeinfo);

  if (result) {
    return static_cast<string>(buffer);
  } else {
    return "[Non disponible]";
  }
}


/*
 * Suggestion de fonctions
 * La banque est créée dans main().
 * Aucune variable globale n’est permise.
 * Les fonctions doivent donc recevoir la banque (b) en paramètre,
 * par référence (Banque& ou const Banque& selon le cas).
 *
 * Exemples :
 * size_t lireNoClientValide(const Banque& b);
 * size_t lireNoCompteValide(const Client& client);
 * double lireMontantValide(double maxMontant);
 * void afficherComptesClient(const Client& client);
 */

// écrire ici toutes vos déclarations de fonctions ...
// ...

/* ------------------------------------- */
/* FONCTIONS TRÈS GÉNÉRALES DU PROGRAMME */
/* ------------------------------------- */

void io_bip() { Beep(200, 300); }

void io_clean()                                             // pour vider les 2 tampons d'input
{
  cin.clear();
  cin.ignore(cin.rdbuf()->in_avail());
  // vide le tampon du cin
  //while (_kbhit())
    //(void)_getch();                                           // vide le tampon de la console

    //  En détail:
    //  cin.rdbuf() retourne le tampon qui contient les caractères déjà tapés
    //  mais non lus
    //  ->in_avail() in_avail() retourne le nombre de caractères
    //  cin.rdbuf()->in_avail() ne lit rien, mais compte simplement combien de
    //  caractères sont encore dans le tampon et retourne ce nombre à cin.ignore
    //  afin qu'il sache combien de caractères il faut supprimer
}

double
io_round(double v,
         size_t p)                                                 // tous les montants lus devront être arrondis à 2 décimales
// avec cette fonction (v:valeur et p:précision)
{                                                           // ex: montant = io_round(montant,2);
  double e = pow(10, p);
  return round(v * e) / e;
}

/* ----------------------------------------------------------------------- */
/* FONCTIONS GÉNÉRALES POUR L'INTERFACE USAGER (UI) EN LECTURE OU ÉCRITURE */
/* ----------------------------------------------------------------------- */

void afficherMenu() {
  const size_t indent = 18;

  //  options
  int optionsMenu[8];
  optionsMenu[(size_t)Cmd::AJOUTER] = '1';
  optionsMenu[(size_t)Cmd::AFFICHER] = '2';
  optionsMenu[(size_t)Cmd::DEPOSER] = '3';
  optionsMenu[(size_t)Cmd::RETIRER] = '4';
  optionsMenu[(size_t)Cmd::VIRER] = '5';
  optionsMenu[(size_t)Cmd::LISTER] = '6';
  optionsMenu[(size_t)Cmd::SUPPRIMER] = '7';
  optionsMenu[(size_t)Cmd::QUITTER] = 'Q';

  // textes a montrer
  char const * textesMenu[9];
  textesMenu[(size_t)Cmd::AJOUTER] = "Ajouter un client";
  textesMenu[(size_t)Cmd::AFFICHER] = "Afficher les informations d'un client";
  textesMenu[(size_t)Cmd::DEPOSER] = "Deposer";
  textesMenu[(size_t)Cmd::RETIRER] = "Retrait";
  textesMenu[(size_t)Cmd::VIRER] = "Virement";
  textesMenu[(size_t)Cmd::LISTER] = "Lister les clients et leur credit actuel";
  textesMenu[(size_t)Cmd::SUPPRIMER] = "Supprimer un client";
  textesMenu[(size_t)Cmd::QUITTER] = "Quitter";

  clrscr();

  cout << left;
  cout << setw(indent) <<  "" << "BANQUE CVM - Succ C21\n\n\n";

  for (size_t i = 0; i < 8; i++) {
    cout << setw(indent) << "";
    cout << (char) optionsMenu[i] <<  ". ";
    cout << textesMenu[i];
    cout << "\n\n";
  }

  cout << "\n\n\n";
  cout << setw(indent) << "" << "Entrez votre choix: ";

}

Commandes lireChoixMenu() {
  Commandes cmd = Cmd::INCONNUE;                            // commande par défaut

  char c;

  do {
    c = toupper(_getch());
    switch (c) {
      // clang-format off
      case '1': cmd = Cmd::AJOUTER; break;
      case '2': cmd = Cmd::AFFICHER; break;
      case '3': cmd = Cmd::DEPOSER; break;
      case '4': cmd = Cmd::RETIRER; break;
      case '5': cmd = Cmd::VIRER; break;
      case '6': cmd = Cmd::LISTER; break;
      case '7': cmd = Cmd::SUPPRIMER; break;
      case 'Q': cmd = Cmd::QUITTER; break;
      default: cmd = Cmd::INCONNUE; break;
      // clang-format on
    }
  } while (cmd == Cmd::INCONNUE);
  /*
   *  FAIRE
   *
   *  lire une touche (cmd?)
   *
   *  TANT QUE (cmd == Cmd::inconnue)
   */

  return cmd;
}

/* ------------------------------------------------------------ */
/* LES FONCTIONS OBLIGATOIRES POUR CHAQUE COMMANDE DU PROGRAMME */
/* ------------------------------------------------------------ */

void cmd_ajouter(Banque & b)
{
  clrscr();
  cout << "CMD - ajouter un client\n\n";
  cout << "Creation d'un client #1\n\n";

  // Créer un client temporaire
  Client client = {};

  // Demander les informations
  client.info.nom.prenom = recupererString("Prenom");
  client.info.nom.nom = recupererString("Nom");
  cout << "\n";
  client.info.adresse.noCivique = recupererString("Numero civique");
  client.info.adresse.rue = recupererString("Rue");
  client.info.adresse.ville = recupererString("Ville");
  client.info.adresse.codePostal = recupererString("Code postal");
  client.info.telephone = recupererString("Telephone");
  cout << "\n";
  client.info.nas = recupererString("No d'assurance sociale");
  cout << "\n";
  for (uint8_t i = 0; i < 3; i++) {
    string question = "Marge de credit du compte #" + to_string(i + 1) + " (Max de " + toArgentStr(MARGE_CREDIT_MAX) + ")";
    client.comptes[i].margeCredit = recupererArgent(question, 0, MARGE_CREDIT_MAX);
    client.comptes[i].solde = 0;
  }

  // Prendre la date actuelle
  client.date = time(nullptr);
  cout << "Date de creation du dossier: ";
  cout << toDateStr(client.date);
  printBreaks(2);

  // Save ?
  bool enregistrer = questionOuiNon("Enregistrer? ");
  if (enregistrer) {
    ajouterClient(b, client);
  }
}

void cmd_afficher(const Banque & b) {
  clrscr();

  cout << "CMD - afficher toutes les informations d'un client";

  printBreaks(3);

  size_t numeroClient = recupererNumeroClient(b);

  if (numeroClient > 0) {
    Client client = getClient(b, numeroClient - 1);

    afficherClient(client);

    printBreaks(2);
    cout << "Appuyez sur une touche pour continuer ... ";
    _getch();
  }
}

void cmd_deposer(Banque & b) {
  clrscr();
  cout << "CMD - Faire un depot dans un compte client";
  printBreaks(3);

  size_t numeroClient = recupererNumeroClient(b);

  if (numeroClient > 0) {
    Client client = getClient(b, numeroClient - 1);

    afficherNomClient(client);
    printBreaks(2);
    afficherSoldesClient(client);
    printBreaks(3);

    size_t numeroCompte = recupererNumeroCompte();

    if (numeroCompte > 0) {
      Compte compte = getCompte(client, numeroCompte - 1);

      double montant = recupererDepot(compte);

      bool ok = deposerArgent(b, numeroClient - 1, numeroCompte - 1, montant);
      if (!ok) {
        cerr << "Une erreur est survenue.";
        _getch();
      }
    }
  }
}

void cmd_retirer(/* Paramètres ? */) {
  clrscr();

  cout << "Vous etes dans Retirer";

  _getch();
}

void cmd_virer(/* Paramètres ? */) {
  clrscr();

  cout << "Vous etes dans Virer";

  _getch();
}

void cmd_lister(/* Paramètres ? */) {
  clrscr();

  cout << "Vous etes dans Lister";

  _getch();
}

void cmd_supprimer(/* Paramètres ? */) {
  clrscr();

  cout << "Vous etes dans Supprimer";

  _getch();
}

void cmd_quitter(/* Paramètres ? */) {
  clrscr();
  gotoxy(10,  20);
  cout <<  "Au revoir!";
  Sleep(1000);
}

/* ---------------------- */
/* LA FONCTION PRINCIPALE */
/* ---------------------- */


int main() {

  // permet de faire un 'cout' avec les accents directement
  setcp(1252);
  // tous les montants sont affichés avec une précision à 2
  cout << fixed << setprecision(2);

  Cmd cmd = Cmd::INCONNUE;

  Banque b;                                                 // cout << b.cpt;    ==> afficherait le nombre actuel de client

  do {
    afficherMenu();
    cmd = lireChoixMenu();

    cout << (int)cmd;

    switch (cmd) {
      case Cmd::AJOUTER:      cmd_ajouter(b); break;
      case Cmd::AFFICHER:     cmd_afficher(b); break;
      case Cmd::DEPOSER:      cmd_deposer(b); break;
      case Cmd::RETIRER:      cmd_retirer(); break;
      case Cmd::VIRER:        cmd_virer(); break;
      case Cmd::LISTER:       cmd_lister(); break;
      case Cmd::SUPPRIMER:    cmd_supprimer(); break;
      case Cmd::QUITTER:      cmd_quitter(); break;
      case Cmd::INCONNUE: /* Pas de commande */ break;
    }

  } while (cmd != Cmd::QUITTER);
}
