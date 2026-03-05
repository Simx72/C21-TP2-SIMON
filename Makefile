prod:
	cmake --build build
	wine build/GestionBanque.exe

dev:
	DEBUG=1 cmake --build build
	wine build/GestionBanque.exe

teacher:
	wine exes/C21_TP2_original_AThiboutot.exe
