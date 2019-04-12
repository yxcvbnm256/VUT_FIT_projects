#include "hasici.h"

Histogram skody("Skody", 0, 100000, 100);
Histogram histintenzita("Intenzita", 0, 10, 80);


void GenerovatStanice() {
	stanice.push_back(new Stanice(11250, 5625, "Morkovice-Slížany", true));
	stanice.push_back(new Stanice(11250, 22500, "Uherské Hradiště1", true));
	stanice.push_back(new Stanice(11250, 22500, "Uherské Hradiště2", false));
	stanice.push_back(new Stanice(11250, 33750, "Uherský Brod", true));
	stanice.push_back(new Stanice(22500, 16875, "Otrokovice", true));	
	stanice.push_back(new Stanice(33750, 5625, "Kroměříž1", true));
	stanice.push_back(new Stanice(33750, 5625, "Kroměříž2", false));
	stanice.push_back(new Stanice(45000, 11250, "Holešov", true));
	stanice.push_back(new Stanice(56250, 3375, "Bystřice pod Hostýnem", true));
	stanice.push_back(new Stanice(73125, 11250, "Valašské Meziříčí", true));
	stanice.push_back(new Stanice(39375, 22500, "Zlín1", true));
	stanice.push_back(new Stanice(39375, 22500, "Zlín2", false));
	stanice.push_back(new Stanice(39375, 22500, "Zlín3", false));
	stanice.push_back(new Stanice(33750, 33750, "Luhačovice", true));
	stanice.push_back(new Stanice(45000, 39375, "Slavičín", true));
	stanice.push_back(new Stanice(56250, 39375, "Valašské Klobouky", true));
	stanice.push_back(new Stanice(67500, 33750, "Vsetín1", true));
	stanice.push_back(new Stanice(67500, 33750, "Vsetín2", false));

	// potencialni stanice
	//stanice.push_back(new Stanice(56250, 22500, "Slušovice"));
	//stanice.push_back(new Stanice(56250, 28125, "Vizovice"));
	//stanice.push_back(new Stanice(78750, 22500, "Rožnov pod Radhoštěm"));

	
	// Nahodne rozmisteni 13 stanic
	/*for (int i = 0; i < 13; i++)
	{
		stanice.push_back(new Stanice(Uniform(0, X_MAX), (Uniform(0, Y_MAX)), "Stanice " + i));
	}*/
}

void SmazatStanice()
{
	Stanice *temp = NULL;
	while (!stanice.empty()) {
		temp = stanice.back();
		delete temp;
		stanice.pop_back();
	}
}

/* Funkce, ktera se zavola po uvolneni jednoho auta od pozaru. Probudi nejprioritnejsi ohen, ktery si tak auto muze vzit pro sebe */
void UvolneniAuta() {
	// Serazeni ohnu dle priority ve frontw
	OrderQueue();
	// konec haseni, primarne vytahnout pozar s intenzitou 3
	if (Q3.Length()>0) {
		//std::cout << "Q3 length " << Q3.Length() << "\n";
		(Q3.GetFirst())->Activate();
	}		
	else if (Q2.Length()>0) {
		//std::cout << "Q2 length " << Q3.Length() << "\n";
		(Q2.GetFirst())->Activate();
	}		
	else if (Q1.Length()>0) {
		//std::cout << "Q1 length " << Q3.Length() << "\n";
		(Q1.GetFirst())->Activate();
	}		
}

/* Funkce, ktera urci polohu ohne na zaklade dat o vyskytu pozaru */
Point GenerovatOhen()
{
	int x = 1000 * Random();
	Point pozice;
	switch (x) {
		//Luhacovice - 2,9 %
		case 0 ... 29:
			pozice.X = Uniform(28125,45000);
			pozice.Y = Uniform(33750,45000);
			break;
		//Valasske klobouky - 3,15 %
		case 30 ... 61:
			pozice.X = Uniform(45000,67500);
			pozice.Y = Uniform(33750,45000);
			break;
		//Bystrice pod Hostynem - 2,8 %
		case 62 ... 90:
			pozice.X = Uniform(50625,67500);
			pozice.Y = Uniform(0,11250);
			break;
		//Holesov - 3 %
		case 91 ... 121:
			pozice.X = Uniform(33750,50625);
			pozice.Y = Uniform(0,15750);
			break;
		//Vizovice - 2,8 %
		case 122 ... 150:
			pozice.X = Uniform(50625,67500);
			pozice.Y = Uniform(11250,28125);
			break;
		//Roznov pod Radhostem - 5,9 %
		case 151 ... 210:
			pozice.X = Uniform(67500,90000);
			pozice.Y = Uniform(16875,28125);
			break;
		//Valasske Mezirici - 7,68 %
		case 211 ... 288:
			pozice.X = Uniform(67500,90000);
			pozice.Y = Uniform(0,16875);
			break;
		//Otrokovice - 7,57 %
		case 289 ... 365:
			pozice.X = Uniform(16875,33750);
			pozice.Y = Uniform(11250,28125);
			break;
		//Uhersky Brod - 9,2 %
		case 366 ... 458:
			pozice.X = Uniform(0,28125);
			pozice.Y = Uniform(28125,45000);
			break;
		//Kromeriz - 13,05 %
		case 459 ... 589:
			pozice.X = Uniform(0,33750);
			pozice.Y = Uniform(0,14060);
			break;
		//Vsetin - 10,7 %
		case 590 ... 697:
			pozice.X = Uniform(61875,90000);
			pozice.Y = Uniform(28125,45000);
			break;
		//Uherske Hradiste - 15,15 %
		case 698 ... 849:
			pozice.X = Uniform(0,16875);
			pozice.Y = Uniform(11250,28125);
			break;
		//Zlin - 15,1 %
		default:
			pozice.X = Uniform(37500,50625);
			pozice.Y = Uniform(16875,33750);
			break;
	}
	return pozice;
}

/* Funkce, ktera seradi stanice podle vzdalenosti k danemu bodu. */
void SeraditStanice(Point pozar) {
	std::sort(stanice.begin(),
		stanice.end(),
		[pozar](Stanice* lhs, Stanice* rhs){ return Vzdalenost(pozar, lhs->Poloha) < Vzdalenost(pozar, rhs->Poloha); });
}

/* Funkce, ktera vypocte eukleidicky vzdalenost mezi dvema body. */
double Vzdalenost(Point pozar, Point stanice) {	
	double dist = pow(pozar.X - stanice.X,2)+pow(pozar.Y - stanice.Y,2);
	dist = sqrt(dist);
    return dist;
}

/* Funkce, ktera usporada fronty. */
void OrderQueue() {
	while (Q1.IntensityOverflow())
	{
		Q1.GetFirst()->Into(Q2);
	}
	while (Q2.IntensityOverflow())
	{
		Q2.GetFirst()->Into(Q3);
	}
	while (Q3.IntensityOverflow());
}

/* Konstruktor stanice */
Stanice::Stanice(int x, int y, const char* name, bool budova) : Facility(name) {
	Poloha.X = x;
	Poloha.Y = y;
	Budova = budova;
}

/* Funkce, ktera zjisti, zda prvni pozar ve fronte nepretekl do vyssi kategorie intenzity. */
bool QueuePozar::IntensityOverflow() {
	if (Length() < 1)
		return false;
	Pozar *temp = ((Pozar*)(*begin()));
	if (temp == NULL)
		return false;
	if (round(temp->StartTime) == round(Time)) {
		//std::cout << "Tento fajr je jiz vyresen.\n";
		return false;
	}
	// Cas horeni je aktualni cas minus cas vzniku pozaru
	double burnTime = (Time - temp->StartTime);
	// Uchovani puvodni intenzity
	double intenzita = temp->Intenzita;
	// 1 bod intenzity == 1 minuta
	temp->Skoda += temp->VypocetSkod(burnTime, intenzita);
	if ((burnTime >= 100 || fmod(temp->Intenzita, 100)) <= (fmod(intenzita, 100))) {
		// Priorita je doplnek stovky (0 je nejvetsi, 100 nejmensi priorita) modula aktualni intenzity
		temp->Priority = 100 - fmod(temp->Intenzita, 100);
		return true;
	}
	// Pokud pozar zustava ve stejne kategorii intenzity, zvysi se priorita
	else {
		temp->Priority -= burnTime;
		return false;
	}
		
}

/* Metoda pro vypocet skody a intenzity za urcitou dobu horeni (burnTime). Rozdeluje dobu horeni na useky oddelene prijezdy hasicskych aut.*/
int Pozar::VypocetSkod(double burnTime, double pocatecniIntenzita) {
	int skoda = 0;
	// Narust intenzity - defaultne 1
	double intenzitaRust = 1;
	do {
		// Pocet pritomnych aut pri danem useku
		int autoCount = 0;
		// Usek, pro ktery se pocitaji skody. Pokud je to usek, ve kterem se potencialne prehoupne intenzita do vyssi kategorie, usek se zkrati presne na hranici prechodu do vyssi kategorie.
		double temp = ((burnTime) >= 100 || fmod(pocatecniIntenzita + (burnTime), 100) <= fmod(pocatecniIntenzita,100)) ? (100 - (fmod(pocatecniIntenzita, 100)))/intenzitaRust : (burnTime);
		for (int i = 0; i < Auta.size(); i++) {
			// Pokud je cas prijezdu auta vetsi, nez pocatecni cas pozaru, zasahuje do vypoctu. Pokud je mensi nebo roven, auto uz pro pocitany usek dorazilo
			if (round(Auta[i]->AutoCasVyjezdu + Auta[i]->AutoDobaJizdy) > round(StartTime)) {				
				// Pokud auto dorazi v prubehu pocitaneho useku, je tento usek treba zkratit na cas prijezdu auta, aby se mohly vypocitat skody pro usek, kdy auto jeste nedorazilo
				if (round(Auta[i]->AutoCasVyjezdu + Auta[i]->AutoDobaJizdy) < round(StartTime + temp)) {
					temp = (Auta[i]->AutoCasVyjezdu + Auta[i]->AutoDobaJizdy) - StartTime;
				}
			}
			else {
				autoCount++;
			}			
		}
		// Nastavuje rust intenzity a skod podle poctu chybejicich hasicskych aut v pozaru
		double koefi = 0;
		switch ((int)pocatecniIntenzita - (autoCount * 100)) {
			case 0 ... 99:
				koefi = 1;
				intenzitaRust = 0;
				break;					
			case 100 ... 199:
				intenzitaRust = 0.3;
				koefi = 2;
				break;
			case 200 ... 299:
				intenzitaRust = 0.6;
				koefi = 4;
				break;
			default:
				intenzitaRust = 1;
				koefi = 8;
				break;
		}
		Intenzita += temp * intenzitaRust;
		// Prumer intenzity, se kterou pocitany usek horel
		double average = (pocatecniIntenzita + (Intenzita))/2;
		skoda += pow(average/84.3,(average/100))*100*koefi*temp;
		// Posun pocatecni intenzity o vypocteny usek - chceme pocitat dalsi usek
		pocatecniIntenzita += intenzitaRust * temp;
		// Posun casu pozaru o vypocteny usek - aktualni je jiz vypocteny, StartTime se tedy posune na dalsi nevypocitany usek
		StartTime += temp;
		burnTime -= temp;
	// Cyklus pokracuje, pokud jeste nejsou vypocteny skody od zacatku pozaru (resp. posledniho vypocitaneho useku) az po aktualni stav pozaru
	} while (burnTime > 0);
	return skoda;	
}

/* Kontruktor pozaru */
Pozar::Pozar(int intenzita) : Process(intenzita) {
	Intenzita = intenzita;
	Priority = 100 - (int)fmod(intenzita, 100);
	Skoda = 0;
}

/* Chovani pozaru */
void Pozar::Behavior() {
	StartTime = Time;
	double dobaCekani = 0;
	Poloha = GenerovatOhen();
	int auta = 0;	
	zpet:
	Stanice *aktualniStanice = NULL;
	SeraditStanice(Poloha);
	for (int i = 0; i < stanice.size(); i++) {
		if (!stanice[i]->Busy()) {
			aktualniStanice = stanice[i];
			break;
		}
	}
	if (aktualniStanice == NULL) {
		switch ((int)Intenzita) {
			case 0 ... 199:
				Into(Q1);
				break;
			case 200 ... 299:
				Into(Q2);
				break;			
			default:				
				Into(Q3);
				break;
		}
		Passivate();
		goto zpet;
	}		
	Seize(*aktualniStanice);

	aktualniStanice->AutoCasVyjezdu = Time;
	// auto prijede za tolik minut (vzdalenost / rychlost v metrech za minutu) plus vyrazi 1 - 2 minuty od vyvolaneho poplachu
	aktualniStanice->AutoDobaJizdy = Vzdalenost(Poloha, aktualniStanice->Poloha)/AVERAGE_CAR_SPEED + FIREMEN_GET_READY;
	Auta.push_back(aktualniStanice);
	if (Intenzita - (Auta.size()*100) >= 100)
		goto zpet;
	dobaCekani = (aktualniStanice->AutoDobaJizdy + aktualniStanice->AutoCasVyjezdu) - Time;
	if (aktualniStanice->AutoDobaJizdy + aktualniStanice->AutoCasVyjezdu > Time)
	{
		Wait(dobaCekani);
	}	
	Skoda += VypocetSkod(dobaCekani, Intenzita);
	// vzorec pro vypocet doby haseni
	dobaCekani = 8.7715*exp(0.0122962*Intenzita);
	Wait(dobaCekani);
	double intenzitaPrumer = Intenzita/2;
	// vzorec pro vypocet skod
	Skoda += pow(intenzitaPrumer/84.3,(intenzitaPrumer/100))*100*dobaCekani;
	Skoda = Exponential(Skoda);
	skody(Skoda);
	histintenzita(Intenzita);
	CelkoveSkody += Skoda;	
	std::sort(Auta.begin(), Auta.end(), 
		[](Stanice *a, Stanice *b)
		{ 
			return a->AutoDobaJizdy < b->AutoDobaJizdy; 
		});
	// Autu po prijezdu od pozaru trva 25 - 35 minut doplneni vody, kysliku v pristrojich atd.
	Wait(Uniform(25,35));
	// Simuluje trvani cesty zpet jednotlivych vozu a uvolneni (cekani ve stanici na naplneni aut je simulovano pro vsechny vozy v predchozim kroku)
	dobaCekani = 0;
	for (int i = 0; i < Auta.size(); i++) {
		dobaCekani = Auta[i]->AutoDobaJizdy - dobaCekani - FIREMEN_GET_READY;
		Wait(dobaCekani);		
		Auta[i]->AutoCasVyjezdu = 0;
		Auta[i]->AutoDobaJizdy = 0;
		Release(*Auta[i]);
		UvolneniAuta();
	}
}


Generator::Generator() : Event() {
}
	
void Generator::Behavior() {
	double k = Random();
	// pravdepodobnostni funkce na generovani ruznych kategorii intenzit
	if (k < 0.083)
		(new Pozar(300 + Random()*90))->Activate();		
	else if (k < 0.363)		
		(new Pozar(200 + Random()*90))->Activate();
	else
		(new Pozar(100 + Random()*90))->Activate();
	Activate(Time+Exponential(MINUTES_TO_FIRE));
}

/* Funkce pro vypocet nakladu stanic za rok */
double Naklady() {
	double naklady = 0;
	for (int i = 0; i < stanice.size(); i++) {
		naklady += POSADKA_VYDAJE;
		if (stanice[i]->Budova)
			naklady += BUDOVA_VYDAJE;
	}
	return naklady;
}

int	main()
{
	CelkoveSkody = 0;
	GenerovatStanice();
	SetOutput("pozar.dat");
	// 1 rok v minutach 525600
	Init(0, 525600*SIMULATE_YEARS);
	RandomSeed(time(NULL));
	(new Generator())->Activate();
	Run();
	for (int i = 0; i < stanice.size(); i++)
		stanice[i]->Output();
	Q1.Output();
	Q2.Output();
	Q3.Output();
	skody.Output();
	histintenzita.Output();	
	std::cout << "Naklady na stanice za dobu simulace: " << Naklady()*SIMULATE_YEARS << " korun.\n";
	std::cout << "Celkove skody: " << CelkoveSkody << " korun." << std::endl;
	SmazatStanice();
}