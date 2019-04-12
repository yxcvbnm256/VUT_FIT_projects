25/25

Vašim úkolem je:
[1] Nastudovat problematiku DHCP útoků a relevantní informace uvést v projektové dokumentaci. (až 6 bodů)
[2] Naprogramovat aplikace realizující vybrané DHCP útoky :
[2a] DHCP Starvation - vyčerpávající adresní pool legitimního serveru; (až 6 bodů)
[2b] Rogue DHCP Server - provozující falešný DHCP server poskytující nicnetušícím klientům vlastní základní síťové parametry; (až 10 bodů)
[3] Demonstrovat činnost aplikací v podmínkách Vaší vlastní testovací sítě. (až 3 body) 

KONVENCE ODEVZDÁVANÉHO ZIP ARCHIVU xlogin00.zip
archiv bude povinně ve formátu ZIP ideálně bez adresářové struktury
Makefile bude přeložené binárky ukládat do kořenového adresáře (tam, kde je samotný Makefile)
součástí Makefile bude i specifikace překladového standardu (např. C99), nesázejte na implicitní nastavení
ohlídejte si správné pojmenování níže uvedených souborů
dokumentace.pdf - výstupy zadání [1] a [3]
readme - základní informace, případná omezení projektu
*.c, *.cpp, *.cc, *.h - zdrojové a hlavičkové soubory výstupů zadání [2]
DOPORUČENÍ/OMEZENÍ:

!!! Vzhledem k nátuře projektu se NEDOPORUČUJE projekt testovat na živé fakultní či kolejní síti. Provoz generovaný vašimi aplikacemi může být vyhodnocen jako bezpečnostní incident !!! 
K ad hoc testování použijte vaši domácí síť či si neváhejte vytvořit pomocí virtualizačních prostředí jako VirtualBox či VMWare Workstation jednoduchou laboratoř (např. 3×VM pro dvě oběti a jednoho útočníka) s uzavřeným síťovým segmentem (interní virtuální síťový adaptér). Pokud jste ještě nikdy nevirtualizovali, třeba vám pomůže následující článek http://www.brianlinkletter.com/how-to-use-virtualbox-to-emulate-a-network/.
S blížícím se deadlinem projektu vám bude nabídnuta možnost otestovat si projekt v podmínkách laboratoře C304, kde pro vás bude připravena typizovaná síť s IPv4 konektivitou a několika oběťmi. Tuto nabídku berte jako vsřícné gesto, kdy máte možnost probrat s opravujícím Vaše případné dotazy; vaše (ne)účast na tomto hromadném sedánku není nijak vázána na Vaše výsledné hodnocení, protože nedílnou součástí zadání je, že byste si sami měli být schopni postavit testovací síť k tomuto projektu (ať už virtuální, a nebo reálnou).
Implementované konzolové aplikace budou povinně v jazyce C/C++, využít můžete libovolné v systému dostupné standardní knihovny.
Všechny implementované programy by měly být použitelné a řádně komentované. Pokud už přejímáte zdrojové kódy z různých tutoriálů či příkladů z Internetu (ne mezi sebou pod hrozbou ortelu disciplinární komise), tak je POVINNÉ správně vyznačit tyto sekce a jejich autory dle licenčních podmínek, kterými se distribuce daných zdrojových kódů řídí. V případě nedodržení bude na projekt nahlíženo jako na plagiát!
Pro snadné parsování vstupních argumentů se doporučuje použít funkci getopt().
Aplikace reagují korektním ukončením na SIGINT signál (tedy Ctrl+C).
Počítejte s tím, že při opravování bude projekt testován na PC s Ethernetovým rozhraním vůči topologii s fyzickými zařízeními v laboratoři C304.
Výsledky vaší implementace by měly být co možná nejvíce multiplatformní mezi OS založenými na unixu, ovšem samotné přeložení projektu a funkčnost vaší aplikace budou testovány na refernčním http://nes.fit.vutbr.cz/isa/ISA2015.ova pro předmět ISA, kterýžto bude sloužit jako virtuální mašinka s pravděpodobně jedním síťovým rozhraním.
Do doprovodného souboru readme.txt uveďte případná omezení funkcionality vašeho projektu - na dokumentovanou chybu se rozhodně nahlíží v lepším světle než na nedokumentovanou!
UPŘESNĚNÍ ZADÁNÍ:

Ad [1]

V dobré dokumentaci se očekává následující: titulní strana, obsah, logické strukturování textu, výcuc relevantních informací z nastudované literatury, popis zajímavějších pasáží implementace, demonstrace činnosti implementovaných aplikací, normovaná bibliografie.

Přepisovat Wikipedii do teoretické části dokumentace není ideální. Pokuste se především vlastními slovy vysokoškoláka doplněnými vhodnými obrázky vysvětlit, o čem útoky spojené s DHCP jsou. Než na kvantitu bude se přihlížet hlavně ke kvalitě textu.

Ad [2a]

Cílem aplikace je pomocí vhodně vytvořených DHCP zpráv vyčerpat adresní pool legitimního DHCP serveru tak, aby po spuštění aplikace už žádný nový klient nedostal DHCP výpůjčku.

Konvence jména aplikace a jejích povinných vstupních parametrů:

./pds-dhcpstarve -i interface

interface (řetězec) jméno rozhraní dle OS, na které útočník vygeneruje patřičný provoz s kompromitačními účinky na DHCP server;
Ad [2b]

Cílem aplikace je na daném rozhraní provozovat DHCP server, který bude klientům poskytovat vlastní sadu základních síťových parametrů.

Konvence jména aplikace a jejích povinných vstupních parametrů:

./pds-dhcprogue -i interface -p pool -g gateway -n dns-server -d domain -l lease-time 

interface (řetězec) jméno rozhraní dle OS, na kterém bude falešný DHCP server naslouchat a reagovat na patřičné DHCP zprávy;
pool (řětezec) pool adres reprezentovaný ve formátu <první_IPv4_adresa>-<poslední_IPv4_adresa> (např. 192.168.1.100-192.168.1.199);
gateway (IPv4 adresa) IPv4 adresa výchozí brány pro segment, ve kterém se nachází oběť (např. 192.168.1.1);
dns-server (IPv4 adresa) IPv4 adresa DNS serveru (např. 8.8.8.8);
domain (řetězec) jméno domény, ve které se zařízení nachazí (např. fit.vutbr.cz);
lease-time (číslo) počet sekund reprezentující dobu DHCP výpůjčky (např. 3600);






- PDS - DHCP attacks
- Jan Pawlus (xpawlu00@stud.fit.vutbr.cz)
- submitted files:
	- packet.hpp, packet.cpp - generic class for packet handling, creating and parsing DHCP packets
	- pds-dhcpstarve.cpp - source file for DHCP starvation attack
	- pds-dhcprogue.cpp - source file for DHCP rogue server
	- Makefile - make creates executables, make clean removes them
	- dokumentace.pdf
- executables:
	- pds-dhcpstarve -i [iface]
		- runs DHCP starvation attack on interface [iface]
	- pds-dhcprogue -i [iface] -p [startingIp]-[endingIp] -l [leaseTime] -n [DNS] -g [gateway] -d [domain]
		- runs DHCP rogue server that will provide IPs from pool [startingIp] to [endingIp] for the time of [leaseTime], with DNS server [DNS], gateway [gateway] and domain name [domain]
		- all the arguments are mandatory! If not provided, program will stop with an error
- functionality is described in project's assignment - all the requirements are met in this implementation, no functionality is missing.