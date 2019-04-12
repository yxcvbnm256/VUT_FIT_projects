/**
 * Architektura procesoru (ACH 2018)
 * Projekt c. 1 (nbody)
 * Login: xpawlu00
 */

#include "nbody.h"
#include <cmath>

void particles_simulate(t_particles *p)
{
	int i, j, k;
	float vel_x,vel_y,vel_z;
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
			vel_x = v.velocities_x[i];
			vel_y = v.velocities_y[i];
			vel_z = v.velocities_z[i];
			//step2 - vytvoreni lokalni struktury castice
			t_particle p_i;
			p_i.pos_x = p->pos_x[i];
			p_i.pos_y = p->pos_y[i];
			p_i.pos_z = p->pos_z[i];
			p_i.vel_x = p->vel_x[i];
			p_i.vel_y = p->vel_y[i];
			p_i.vel_z = p->vel_z[i];
			p_i.weight = p->weight[i];
			j = i+1;
		   	//step1 - vynuceni vektorizace smycky
			//step2 - zarovnani pameti
			//step4 - osetreni zapisu na jedno misto redukci
			#pragma omp simd aligned(p:64) reduction(+:vel_x,vel_y,vel_z)
			for (j; j < N; j++)
			{
				float r, dx, dy, dz;
				float vx1, vy1, vz1, vx2, vy2, vz2;
				vx1 = vy1 = vz1 = vx2 = vy2 = vz2 = 0.0f;
				dx = p_i.pos_x - p->pos_x[j];
				dy = p_i.pos_y - p->pos_y[j];
				dz = p_i.pos_z - p->pos_z[j];
				r = sqrt(dx*dx + dy*dy + dz*dz);
				float weight = p->weight[j] / p_i.weight;
				if (r > COLLISION_DISTANCE) 
				{
					vx1 = (G * p_i.weight) / pow(r, 3) * (dx) * DT;
					vy1 = (G * p_i.weight) / pow(r, 3) * (dy) * DT;
					vz1 = (G * p_i.weight) / pow(r, 3) * (dz) * DT;
					//step4 - vypocet rychlosti vznikajici gravitaci pro druhou castici
					vx2 = -vx1;
					vy2 = -vy1;
					vz2 = -vz1;
				} else if (r < COLLISION_DISTANCE && r > 0.0f) 
				{
					vx1 = (((weight - 1) * p->vel_x[j] + 2 * p_i.vel_x)/(1 + weight)) - p->vel_x[j];
					vy1 = (((weight - 1) * p->vel_y[j] + 2 * p_i.vel_y)/(1 + weight)) - p->vel_y[j];
					vz1 = (((weight - 1) * p->vel_z[j] + 2 * p_i.vel_z)/(1 + weight)) - p->vel_z[j];
					//step4 - vypocet rychlosti po kolizi pro druhou castici
					vx2 = p->vel_x[j] * weight + p_i.vel_x - weight * (vx1 + p->vel_x[j]) - p_i.vel_x;
					vy2 = p->vel_y[j] * weight + p_i.vel_y - weight * (vy1 + p->vel_y[j]) - p_i.vel_y;
					vz2 = p->vel_z[j] * weight + p_i.vel_z - weight * (vz1 + p->vel_z[j]) - p_i.vel_z;
				}
				v.velocities_x[j] += vx1;
				v.velocities_y[j] += vy1;
				v.velocities_z[j] += vz1;
				//step4 - vypocet rychlosti pro druhou castici
				vel_x += vx2;
				vel_y += vy2;
				vel_z += vz2;
			}
			v.velocities_x[i] = vel_x;
			v.velocities_y[i] = vel_y;
			v.velocities_z[i] = vel_z;
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
