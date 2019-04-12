#ifndef _HASICI_H
#define _HASICI_H
#include "simlib.h"
#include <time.h>
#include <vector>
#include <math.h>
#include <algorithm>
#include <iostream>

#define MINUTES_TO_FIRE 733
#define POCET_STANIC 5
#define AVERAGE_CAR_SPEED 750 // 45 km/h == 750 m/min
#define FIREMEN_GET_READY 2 // 2 minuty, nez hasici vyrazi
#define X_MAX 90000
#define Y_MAX 45000
#define SIMULATE_YEARS 1
#define POSADKA_VYDAJE 6753600
#define BUDOVA_VYDAJE 2000000

typedef struct Point {
	int X;
	int Y;
} Point;

void GenerovatStanice();
void DeleteStanice();
void SeraditStanice(Point pozar);
void OrderQueue();
double Vzdalenost(Point pozar, Point stanice);

class QueuePozar : public Queue {
public:
	bool IntensityOverflow();
};

class Stanice : public Facility {
public:
	Stanice(int x, int y, const char* name, bool budova);	
	Point Poloha;
	Point Auto;
	bool Budova;
	double AutoCasVyjezdu;
	double AutoDobaJizdy;
	int Vzdalenost;	
};

class Pozar : public Process {
public:
	double Intenzita;
	double StartTime;
	int Skoda;
	std::vector<Stanice*> Auta;
	Point Poloha;
	Pozar(int intenzita);	
	void Behavior();
	int VypocetSkod(double burnTime, double pocatecniIntenzita);
};

class Generator : public Event {
public:
	Generator();	
	void Behavior();
};

std::vector<Stanice*> stanice;
QueuePozar Q1;
QueuePozar Q2;
QueuePozar Q3;
double CelkoveSkody;
double NakladyStanice;

#endif // _HASICI_H