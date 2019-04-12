12/15

Napište program imapcl, který bude umožňovat čtení elektronické pošty skrze protokol IMAP4rev1 (RFC 3501). Program po spuštění stáhne zprávy uložené na serveru a uloží je do zadaného adresáře (každou zprávu zvlášť). Na standardní výstup vypíše počet stažených zpráv. Pomocí dodatečných parametrů je možné funkcionalitu měnit.

Při vytváření programu je povoleno použít hlavičkové soubory pro práci se sokety a další obvyklé funkce používané v síťovém prostředí (jako je netinet/*, sys/*, arpa/* apod.), knihovnu pro práci s vlákny (pthread), signály, časem, stejně jako standardní knihovnu jazyka C (varianty ISO/ANSI i POSIX), C++ a STL. Pro práci se SSL/TLS je povolena knihovna openssl. Jiné knihovny nejsou povoleny.

Spuštění aplikace

Použití: imapcl server [-p port] [-T [-c certfile] [-C certaddr]] [-n] [-h] -a auth_file [-b MAILBOX] -o out_dir

Pořadí parametrů je libovolné. Popis parametrů:

Povinně je uveden název serveru (IP adresa, nebo doménové jméno) požadovaného zdroje.
Volitelný parametr -p port specifikuje číslo portu na serveru. Zvolte vhodnou výchozí hodnotu v závislosti na specifikaci parametru -T a číslech portů registrovaných organizací IANA.
Parametr -T zapíná šifrování (imaps), pokud není parametr uveden použije se nešifrovaná varianta protokolu.
Volitelný parametr -c soubor s certifikáty, který se použije pro ověření platnosti certifikátu SSL/TLS předloženého serverem.
Volitelný parametr -C určuje adresář, ve kterém se mají vyhledávat certifikáty, které se použijí pro ověření platnosti certifikátu SSL/TLS předloženého serverem. Výchozí hodnote je /etc/ssl/certs.
Při použití parametru parametru -n se bude pracovat (číst) pouze s novými zprávami.
Při použití parametru -h se budou stahovat pouze hlavičky zpráv.
Parametr -a auth_file vynucuje autentizaci (příkaz LOGIN), obsah konfiguračního souboru auth_file je zobrazený níže.
Parametr -b specifikuje název schránky se kterou se bude na serveru pracovat. Výchozí hodnota je INBOX.
Povinný parametr -o out_dir specifikuje výstupní adresář, do kterého má program stažené zprávy uložit.
Soubor s autentizačními údaji

Konfigurační soubor s autentizačními údaji bude obsahovat uživatelské jméno a heslo v jednoduchém formátu:

username = jmeno
password = heslo

Výstup aplikace

Po spuštění aplikace vypište pouze informaci o počtu stažených zpráv, text sdělení nesmí být delší než 1 řádek. Text sdělení vhodně upravte v závislosti na použití parametrů -n a -h.

Jednotlivé zprávy budou ukládány ve formátu Internet Message Format (RFC 5322) do vhodně pojmenovaných souborů v adresáři specifikovaným parametrem -o, každá zpráva v samostatném souboru. Příklad obsahu souboru je:

Date: Wed, 14 Sep 2016 03:54:39 -0700 
From: Sender <sender@example.com> 
To: receiver@example.com
Subject: Message
Message-ID: <20160914035439.03264562@mininet-vm> 

Toto je tělo e-mailu. Před tělem e-mailu jsou hlavičky a prázdný řádek. 
Příklad spuštění programu


$ imapcl eva.fit.vutbr.cz -o maildir -a cred
Staženo 15 zpráv ze schránky INBOX.

$ imapcl 10.10.10.1 -p 9993 -T -n -b Important -o maildir -a cred
Staženy 2 nové zprávy ze schránky Important.

$ imapcl eva.fit.vutbr.cz -o maildir -a /dev/null
Není možné ověřit identitu serveru eva.fit.vutbr.cz.

ISA 2016 - IMAP KLIENT
Autor: Jan Pawlus (xpawlu00)
Popis: IMAP klient stahující e-maily na základě zadaných parametrů. Program dodržuje požadovanou funkčnost na základě zadání, žádné rozšíření není implementováno.

- zadáním nespecifikované části klienta byly implementovány takto:
	- pokud uživatelem specifikovaná výstupní složka neexistuje, program se ji pokusí vytvořit. Pokud selže, informuje o chybě.
	- položky hlavičky se stahují všechny, které server poskytne.
	- funkce přepínačů -c a -C - pokud jsou zadány oba, ověří se certifikát zadaný argumentem -c. Pokud takový soubor neexistuje, program skončí s chybou. Pokud existuje, ale není platný, klient začne
		hledat ve složce specifikované argumentem -C. Pokud není -c vůbec zadán, automaticky se hledá ve složce zadané argumentem -C, v případě nezadání ve složce /etc/ssl/certs.
	- nápověda je vypisována při jakémkoliv chybném spuštění (nezadání povinných argumentů, chybné argumenty apod).

- program lze spustit s následujícími argumenty:
	[server] - název serveru, ke kterému se klient připojí. Povinný argument.
	-a [auth_file] - název, popř. cesta k souboru obsahující přihlašovací údaje - je povinný. Tento soubor musí být ve formátu
		username: [username]
		password: [password]
	-o [output] - název složky, do které se mají uložit e-maily. Pokud složka neexistuje, klient ji vytvoří. Povinný argument. Následující argumenty jsou nepovinné.
	-p [port] - číslo portu. V případě nezadání klient použije 993 pro šifrované a 143 pro nešifrované spojení.
	-b [mailbox] - název schránky, ze které se mají e-maily stáhnout. Při nezadání se použije INBOX.
	-T - zapíná šifrované spojení.
	-c - udává název souboru s certifikátem. Pokud je zadán neplatný certifikát, klient ověří dále certifikáty ve složce specifikované parametrem -C.
	-C - cesta k souboru s certifikátem. Při nezadání se použije /etc/ssl/certs.
	-n - zapíná stažení pouze nových e-mailů.
	-h - zapíná stažení pouze hlaviček e-mailů.

- příklady spuštění:
	./imapcl imap.seznam.cz -p 143 -o maildir -a cred -b Sent
	- aplikace se bude snažit připojit na port 143 k serveru imap.seznam.cz (není zapnuto zašifrované spojení), přihlašovací údaje se získají ze souboru cred a zprávy ze schránky Sent se uloží do složky maildir.
	./imapcl eva.fit.vutbr.cz -o maildir -a cred -T -C ./cert -c cert.pam -n -h
	- klient se zapne s šifrovaným spojením, certifikát se bude hledat v případě neúspěšného ověření souboru cert.pam be složce cert a stáhnou se pouze hlavičky nových zpráv ze schránky INBOX serveru eva.fit.vutbr.cz:993 (port se nastaví na základě zapnutí šifrovaného spojení) do složky maildir.
	./imapcl imap.gmail.com -o output -a cred -T
	- klient se připojí k serveru imap.gmail.com, certifikát se vyhledá ve složce /etc/ssl/certs.
	- další příklady spuštění viz zadání.

- odevzdané soubory:
	- imapcl.h
	- imapcl.cpp
	- main.h
	- main.cpp
	- Makefile	
	- manual.pdf
	- README