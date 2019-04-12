10.5/15

Vytvořte program pro klienta a server, s využitím rozhraní schránek (sockets API), který implementuje jednoduchou adresářovou službu. 
navrhněte aplikační protokol pro komunikaci mezi klientem a serverem
vytvořte oba programy v jazyce C/C++ přeložitelné na studentském unixovém serveru eva.fit.vutbr.cz (FreeBSD) včetně funkčního Makefile souboru (oba programy přeložitelné po zadání příkazu make)
vytvořte dokumentaci popisující aplikační protokol (max. 1 strana A4, formát pdf)
Programy využívají spojovanou službu (protokol TCP). Server musí být konkurentní tzn. bude požadována současná obsluha více klientů (implementujte pomocí procesů nebo vláken). Jméno programu pro server po přeložení bude server a pro klienta client. Server předpokládá jeden povinný parametry: -p následovaný číslem portu, na kterém bude očekávat spojení od klienta (příklad spuštění serveru: server -p 10000 &). Klient předpokládá alespoň tři povinné parametry, jméno serveru a číslo portu a jedním parametrem pro vyhledání (-l nebo -u). Protokol síťové vrstvy použijte IPv4. Oznámení o chybách, které mohou nastat (neznámé uživatelské jméno, neznámý login, chybný formát zdrojové databáze, chyba při komunikaci, ...), bude vytištěno na standardní chybový výstup (stderr). Data přenášená mezi klientem a serverem jsou pouze ta, která jsou požadována klientem (nepřenášejte celé záznamy). Tyto vlastnosti navrhněte a implementujte vhodným aplikačním protokolem.

Klient získává od serveru požadované informace o uživatelích OS Unix, na kterém je spuštěn server. Tyto informace server získává z /etc/passwd souboru. Každý záznam v souboru je na samostatném řádku, kde jednotlivé položky jsou odděleny : (podrobný formát je uveden v manuálové stránce passwd v sekci 5, viz: man 5 passwd). Získané informace (záznamy) klient vypíše na standardní výstup na jednotlivé řádky v pořadí, jaké je uvedeno u klienta. Jednotlivé položky ve výstupu oddělte mezerou. Položky, které klient může požadovat, jsou reprezentovány následujícími parametry:
 -L uživatelské jméno
 -U UID
 -G GID
 -N celé jméno, celý gecos pokud obsahuje další položky oddělené čárkou
 -H domovský adresář
 -S logovací shell
Vyhledávejte pouze podle jednoho kritéria, uživatelského jména nebo UIDu (podle posledního parametru -l nebo -u). Vyhledávání nerozlišuje malé a velké znaky. U výpisu uvažujte, že se daná položka může objevit maximálně jednou.