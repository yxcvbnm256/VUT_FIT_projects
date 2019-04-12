/**
 * Architektura procesoru (ACH 2018)
 * Projekt c. 1 (nbody)
 * Login: xpawlu00
 */

#ifndef __NBODY_H__
#define __NBODY_H__

#include <cstdlib>
#include <cstdio>
#include "velocity.h"

//step2 - zarovnani pameti, prevedeni pole struktur -> struktura poli
typedef struct particles {
	__declspec(align(64)) float pos_x[N];
	__declspec(align(64)) float pos_y[N];
	__declspec(align(64)) float pos_z[N];
	__declspec(align(64)) float vel_x[N];
	__declspec(align(64)) float vel_y[N];
	__declspec(align(64)) float vel_z[N];
	__declspec(align(64)) float weight[N];
} t_particles;

typedef struct velocities {
	__declspec(align(64)) float velocities_x[N];
	__declspec(align(64)) float velocities_y[N];
	__declspec(align(64)) float velocities_z[N];
} t_velocities;

void particles_simulate(t_particles *p);

void particles_read(FILE *fp, t_particles *p);

void particles_write(FILE *fp, t_particles *p);

#endif /* __NBODY_H__ */
