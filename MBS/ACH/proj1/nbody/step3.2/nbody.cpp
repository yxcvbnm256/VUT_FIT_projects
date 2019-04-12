/**
 * Architektura procesoru (ACH 2018)
 * Projekt c. 1 (nbody)
 * Login: xpawlu00
 */

#include "nbody.h"

void particles_simulate(t_particles *p)
{
	int i, j, k;
	//step2 - zarovnani pameti
	t_velocities v;
	for (k = 0; k < STEPS; k++)
	{
		//vynulovani mezisouctu
		for (i = 0; i < N; i++)
		{
			v.velocities_x[i] = 0.0f;
			v.velocities_y[i] = 0.0f;
			v.velocities_z[i] = 0.0f;		
		}
		//vypocet nove rychlosti
		for (i = 0; i < N; i++)
		{
			//step2 - vytvoreni lokalni struktury castice
			t_particle p_i;
			p_i.pos_x = p->pos_x[i];
			p_i.pos_y = p->pos_y[i];
			p_i.pos_z = p->pos_z[i];
			p_i.vel_x = p->vel_x[i];
			p_i.vel_y = p->vel_y[i];
			p_i.vel_z = p->vel_z[i];
			p_i.weight = p->weight[i];
			//step1 - vynuceni vektorizace smycky
			//step2 - zarovnani pameti
			#pragma omp simd aligned(p:64)
			for (j = 0; j < N; j++)
			{
				//step3.2 - zmena rozhrani
				calculate(p->pos_x[j], p->pos_y[j], p->pos_z[j],
				   p->vel_x[j], p->vel_y[j], p->vel_z[j], p->weight[j],
				   p_i.pos_x, p_i.pos_y, p_i.pos_z,
				   p_i.vel_x, p_i.vel_y, p_i.vel_z, p_i.weight,
				   v.velocities_x[j], v.velocities_y[j], v.velocities_z[j]
				);
			}
		}
		//ulozeni rychlosti a posun castic
		//step2 - pole struktur -> struktura poli
		for (i = 0; i < N; i++)
		{
			p->vel_x[i] += v.velocities_x[i];
			p->vel_y[i] += v.velocities_y[i];
			p->vel_z[i] += v.velocities_z[i];

			p->pos_x[i] += p->vel_x[i] * DT;
			p->pos_y[i] += p->vel_y[i] * DT;
			p->pos_z[i] += p->vel_z[i] * DT;
		}
	}
}


void particles_read(FILE *fp, t_particles *p)
{
	for (int i = 0; i < N; i++)
	{
		//step2 - pole struktur -> struktura poli
		fscanf(fp, "%f %f %f %f %f %f %f \n",
			&p->pos_x[i], &p->pos_y[i], &p->pos_z[i],
			&p->vel_x[i], &p->vel_y[i], &p->vel_z[i],
			&p->weight[i]);
	}
}

void particles_write(FILE *fp, t_particles *p)
{
	for (int i = 0; i < N; i++)
	{
		//step2 - pole struktur -> struktura poli
		fprintf(fp, "%10.10f %10.10f %10.10f %10.10f %10.10f %10.10f %10.10f \n",
			p->pos_x[i], p->pos_y[i], p->pos_z[i],
			p->vel_x[i], p->vel_y[i], p->vel_z[i],
			p->weight[i]);
	}
}
