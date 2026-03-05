# Système de gestion de la banque – TUI in cpp

Ce système permet de gérer des clients et avoir le solde de leur comptes ainsi que leur marges de crédit.

## Compilation

La compilation est décrite dans le fichier CMakeLists.txt

Si compilé sur Linux, vous pouvez le compiler en mode **development** ou **production**

### Development

```bash
DEBUG=1 cmake --build build
```

### Production

```bash
cmake --build build
```

## Developpement

Pour developer le programme, tout le code est dans `main.cpp`.
`cvm_23.cpp` est une libraire de mon institution pour faire des manipulations simples dans la console.

Pour compiler et executer le code, vous pouvez utiliser la commande `make dev`, ou `make` si vous voulez tester le mode *production*.
