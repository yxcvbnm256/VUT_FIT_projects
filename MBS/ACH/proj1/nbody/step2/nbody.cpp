/**
 * Architektura procesoru (ACH 2018)
 * Projekt c. 1 (nbody)
 * Login: xpawlu00
 */

#include "nbody.h"
#include <stdio.h>

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
				//step2 - vytvoreni lokalni struktury castice
				t_particle p_j;
				p_j.pos_x = p->pos_x[j];
				p_j.pos_y = p->pos_y[j];
				p_j.pos_z = p->pos_z[j];
				p_j.vel_x = p->vel_x[j];
				p_j.vel_y = p->vel_y[j];
				p_j.vel_z = p->vel_z[j];
				p_j.weight = p->weight[j];
				//step2 - vytvoreni lokalni struktury rychlosti
				t_velocity velocity_j;
				velocity_j.x = v.velocities_x[j];
				velocity_j.y = v.velocities_y[j];
				velocity_j.z = v.velocities_z[j];
				calculate_gravitation_velocity(p_j, p_i, velocity_j);
				calculate_collision_velocity(p_j, p_i, velocity_j);
				v.velocities_x[j] = velocity_j.x;
				v.velocities_y[j] = velocity_j.y;
				v.velocities_z[j] = velocity_j.z;
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
