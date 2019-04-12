6/15

Vytvořte jednoduchý HTTP server a klient, který bude volitelně využívat Chunked transfer encoding. Server zpřístupní obsah v adresáři ./www se základní podporou Content-Type: text/plain. 

K serveru bude možné připojit se přes běžný webový prohlížeč, který zobrazí požadovaný obsah.
 
Spuštění serveru bude následující: ./ipkhttpserver [-h] [-c chunk-max-size] [-p port]

-h --help zobrazí nápovědu
-c --chunk-max-size - maximální velikost dat v chunku
-p --port TCP port, na kterém bude server naslouchat 
-t --min-chunk-time (ms) minimalni cas cekani pred odeslanim dalsiho chunku, maximalni cas 2x min-chunk-time

Server se korektně vypne po obdržení signálu SIGINT.

Spuštění klientu bude následující: ./ipkhttpclient [-h] URI

-h --help zobrazí nápovědu
URI - rfc3986
 
Aplikace klient uloží do souboru "ipkResp-YYYY-MM-DD:hh:mm:ss.header" hlavičku, do souboru "ipkResp-YYYY-MM-DD:hh:mm:ss.payload" rekonstruovaný objekt.

Všechna příchozí komunikace se bude logovat do souboru "ipkHttp(Server|Client)-YYYY-MM-DD:hh:mm:ss.in.log", odchozí "ipkHttp(Server|Client)-YYYY-MM-DD:hh:mm:ss.out.log", kde časové razítko se určí při spuštění programu.

Při požadavku na neexistující dokument server odpoví chybou HTTP 404.

Implementační jazyk C/C++, Python, Perl, Bash, Assembler (x86), Brainfuck, Whitepace, Lisp, Prolog, Haskel, ObjectiveC, C#, MATLAB, Maple a jejich kombinace. Je možné využít libovolnou knihovnu, která byla schválena na fóru předmětu. 

Aplikace bude povinně obsahovat README s popisem implementace a příklady spuštění. Pokud nestihnete implementovat část funkcionality, bude to taktéž povinně zmíněno v README.

Přeložení aplikace zajistí Makefile, který při překladu stáhne požadované schválené knihovny a zajistí překlad bez manuálního zásahu uživatele. Překlad musí být úspěšný bez chyb a varování! Makefile bude dále povinně implementovat PHONY clean, test a pack s následující funkcionalitou:

make clean - smaže vše kromě zdrojových a testovacích souborů
make test - spustí navrženou sadu testů a vypíše výsledek (i postup) na STDOUT
make pack - vytvoří archiv k odevzdání
Je vyžadováno, aby sada testů pokrývala kompletně implementovanou funkcionalitu.

README - IPK projekt 2 varianta 1
Jan Pawlus
xpawlu00

--------------------SERVER--------------------

Priklady spusteni serveru:
./ipkhttpserver -p 10000 -c 100 -t 10	->	spusti server, ktery bude ocekavat spojeni na portu 10000, maximalni velikost chunku bude 100 a cas mezi odeslanim 	jednotlivych chunku bude 10 ms.
./ipkhttpserver -p 10000 -c 100 		->	spusti server, ktery bude ocekavat spojeni na serveru 10000, maximalni velikost chunku bude 100 a pokud neni zadany	cas mezi zaslanim jednotlivych chunku, bude tato hodnota primarne nastavena na 5 ms.
./ipkhttpserver -p 10000				-> 	spusti server, ktery bude ocekavat spojeni na portu 10000, maximalni velikost chunku bude defaultne nastavena na 2 Kb (defaultni hodnota v J2ME) a cas bude defaultne 5 ms.
./ipkhttpserver							-> 	spusti server, ktery ma defaultne nastaveny port na 10000, defaultni velikost chunku 100 a cas 0 ms.
./ipkhttpserver -h -p 10000				-> 	vypise napovedu (pokud je zadan argument -h, jsou veskere jine argumenty ignorovany) a zavre program.

Popis implementace serveru:
	Server je implementovan objektove - trida ArgParser rozpozna argumenty a nasledne je preda konstruktoru tridy Server, ktery si potrebne informace ulozi. Server prvni ulozi cas pri spusteni (kvuli logum), nasledne vytvori socket, provede bind a ceka na pripojeni od klienta. Nasledne se server dostane do smycky, kdy obsluhuje jednotlive pozadavky. Pokud je navazano spojeni, precte server HTTP hlavicku a rozpozna dulezite informace - metodu a soubor, ktery klient pozaduje (pokud neni zadan, bere se automaticky index.html). Zadany soubor se server pokusi otevrit. Pokud existuje, nacte se jeho obsah, pokud ne, jako obsah postaci Error 404 - File not found (podle toho se vygeneruje odchozi hlavicka). 
	Pokud je obsah mensi, nez maximalni chunk_size, posle se obsah v celku. Pokud je delka vetsi, obsah se rozdeli do vice casti (chunku). V takovem pripade se posle klientu hlavicka (s informaci o tom, zda je obsah rozkouskovan) a nasledne jednotlive chunky, ktere jsou ve formatu hexadecimalni_delka\r\nOBSAH\r\n. Po odeslani celeho obsahu se odesle delka chunku 0\r\n\r\n. Nakonec se ulozi logy o prichozi a odchozi komunikaci. Program se korektne ukonci pri prichozim signalu SIGINT (manualni obsluha signalu)


--------------------CLIENT--------------------

Priklady spusteni kleinta:
./ipkhttpclient http://localhost:10000/src/test.html	-> spusti klienta, ktery se bude pripojovat na localhost, port 10000 a bude chtit obsah souboru src/test.html.
./ipkhttpclient http://localhost:10000					-> spusti klienta, ktery se bude pripojovat na localhost, port 10000 a bude chtit obsah souboru index.html.
./ipkhttpclient http://localhost:10000 -h				-> vypise napovedu (pokud je zadan argument -h, jsou veskere jine argumenty ignorovany) a zavre program.

Popis implementace klienta:
	Take klient je implementovan objektove - tvori jej opet trida ArgParser, jejiz instance rozparsuje URI, pripadne vypise napovedu a potrebne informace preda konstruktoru tridy Client. Ten prvni ulozi aktualni cas pri spusteni, nasledne se pokusi navazat komunikaci se serverem na zadanem hostu a portu a poslat HTML pozadavek na zaklade URI. Pote klient ocekava hlavicku HTTP odpovedi. Na zaklade vyskytu radku "Transfer-encoding: chunked" klient pozna, zda bude obsah posilan v celku, nebo po kouscich (chunked). V prvnim pripade jednoduse precte celou dalsi zpravu jako obsah (delka obsahu je ulozena v prichozi hlavicce), v druhem cte zpravy do te doby, dokud neni prvni znak zpravy 0 (znaci, ze obsah byl kompletne odeslan). Nakonec se zapise veskera komunikace do logu.
