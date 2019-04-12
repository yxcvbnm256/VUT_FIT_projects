/**
 * Architektura procesoru (ACH 2018)
 * Projekt c. 1 (nbody)
 * Login: xpawlu00
 */

#include <cmath>
#include <cfloat>
#include "velocity.h"

//step3.1 - spojeni funkci
//step3.2 - zmena rozhrani funkce
void calculate(
  float p1_pos_x, float p1_pos_y, float p1_pos_z,
  float p1_vel_x, float p1_vel_y, float p1_vel_z, float p1_weight,
  float p2_pos_x, float p2_pos_y, float p2_pos_z,
  float p2_vel_x, float p2_vel_y, float p2_vel_z, float p2_weight,
  float& vel_x, float& vel_y, float& vel_z
)
{
	float r, dx, dy, dz;
	float vx, vy, vz;
	vx = vy = vz = 0.0f;
	dx = p2_pos_x - p1_pos_x;
	dy = p2_pos_y - p1_pos_y;
	dz = p2_pos_z - p1_pos_z;

	r = sqrt(dx*dx + dy*dy + dz*dz);
	float weight = p1_weight / p2_weight;
	if (r > COLLISION_DISTANCE) 
	{
		//step0 - vypocet rychlosti diky gravitaci
		vx = (G * p2_weight) / pow(r, 3) * (dx) * DT;
		vy = (G * p2_weight) / pow(r, 3) * (dy) * DT;
		vz = (G * p2_weight) / pow(r, 3) * (dz) * DT;
	} else if (r < COLLISION_DISTANCE && r > 0.0f) 
	{
		//step0 - vypocet rychlosti po kolizi
		vx = (((weight - 1) * p1_vel_x + 2 * p2_vel_x)/(1 + weight) - p1_vel_x);
		vy = (((weight - 1) * p1_vel_y + 2 * p2_vel_y)/(1 + weight) - p1_vel_y);
		vz = (((weight - 1) * p1_vel_z + 2 * p2_vel_z)/(1 + weight) - p1_vel_z);
	}
	vel_x += vx;
	vel_y += vy;
	vel_z += vz;
}


/**
 * @brief   Funkce vypocte rychlost kterou teleso p1 ziska vlivem gravitace p2.
 * @details Viz rovnice 4.4 v souboru zadani.pdf 
 */
void calculate_gravitation_velocity(
  const t_particle &p1,
  const t_particle &p2,
  t_velocity &vel
)
{
	float r, dx, dy, dz;
	float vx, vy, vz;
	
	dx = p2.pos_x - p1.pos_x;
	dy = p2.pos_y - p1.pos_y;
	dz = p2.pos_z - p1.pos_z;

	r = sqrt(dx*dx + dy*dy + dz*dz);

	//step0 - vypocet rychlosti diky gravitaci

	vx = (G * p2.weight) / pow(r, 3) * (dx) * DT;
	vy = (G * p2.weight) / pow(r, 3) * (dy) * DT;
	vz = (G * p2.weight) / pow(r, 3) * (dz) * DT;

	vel.x += (r > COLLISION_DISTANCE) ? vx : 0.0f;
	vel.y += (r > COLLISION_DISTANCE) ? vy : 0.0f;
	vel.z += (r > COLLISION_DISTANCE) ? vz : 0.0f;
}

/**
 * @brief   Funkce vypocte rozdil mezi rychlostmi pred a po kolizi telesa p1 do telesa p2.
 * @details Viz rovnice 4.8 v souboru zadani.pdf
 */
void calculate_collision_velocity(
  const t_particle &p1,
  const t_particle &p2,
  t_velocity &vel
)
{
	float r, dx, dy, dz;
	float vx, vy, vz;

	dx = p2.pos_x - p1.pos_x;
	dy = p2.pos_y - p1.pos_y;
	dz = p2.pos_z - p1.pos_z;

	r = sqrt(dx*dx + dy*dy + dz*dz);

	//step0 - vypocet rychlosti diky gravitaci

	float weight = p1.weight / p2.weight;

	vx = (((weight - 1) * p1.vel_x + 2 * p2.vel_x)/(1 + weight) - p1.vel_x);
	vy = (((weight - 1) * p1.vel_y + 2 * p2.vel_y)/(1 + weight) - p1.vel_y);
	vz = (((weight - 1) * p1.vel_z + 2 * p2.vel_z)/(1 + weight) - p1.vel_z);

	// jedna se o rozdilne ale blizke prvky
	if (r > 0.0f && r < COLLISION_DISTANCE) {
		vel.x += vx;
		vel.y += vy;
		vel.z += vz;
	}
}
