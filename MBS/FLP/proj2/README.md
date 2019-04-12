6/8

FLP - logicky projekt - rubikova kostka
Jan Pawlus, xpawlu00

ODEVZDANE SOUBORY:
	- flp18-log.pl - zdrojovy kod programu
	- Makefile
	- input1.txt, input2.txt, input3.txt, input4.txt - vzorove vstupy

VZOROVE SPUSTENI:
	make
	./flp18.log < input1.txt > output

IMPLEMENTACE:
	- 12 tahu kostkou z primeho pohledu na stranu 1 - tahy kostkou vypozorovany z tahu realne kostky - vyuzito rotace matic, trnaspozice matic, reverze listu
	- vyhledavani - iterative deepening DFS - nejprve se vyhledava v hloubce 1, pote v hloubce 2 a tak dale.

VZOROVE VSTUPY A JEJICH CAS K NALEZENI VYSLEDKU:
	- input1.txt - 0,6s - 4 tahy
	- input2.txt - 90s - 6 tahu
	- input3.txt - 6s - 5 tahu
	- input4.txt - 0,6s - 4 tahy

ZKOPIROVANY KOD:
	- transpose - zkopirovano z knihovny clpfd
	- rotate - https://stackoverflow.com/questions/35594027/rotate-a-matrix-in-prolog
	- nacitani stringu ze stdin - vzorovy soubor ve WISu