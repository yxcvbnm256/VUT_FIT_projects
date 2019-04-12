/*
 * Architektura procesoru (ACH 2018)
 * Projekt c.2 (CUDA)
 * Login: xpawlu00
 */

#include <cmath>
#include <cfloat>
#include "nbody.h"

__global__ void calculate_gravitation_velocity(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  int idx = threadIdx.x + (blockIdx.x * blockDim.x);
  if (idx >= N)
    return;
  float r;
  float4 d;
  float4 p1_pos = p.pos[idx];
  float4 vel = {0.0f, 0.0f, 0.0f};
  for (int i = 0; i < N; i++) { 
    if (i == idx)
      continue;
    float4 p2_pos = p.pos[i];
    d.x = p2_pos.x - p1_pos.x;
    d.y = p2_pos.y - p1_pos.y;
    d.z = p2_pos.z - p1_pos.z;
    r = sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    if (r > COLLISION_DISTANCE) {
      float p2_weight = p.vel[i].w;
      vel.x += (G * p2_weight) / pow(r, 3) * (d.x) * dt;
      vel.y += (G * p2_weight) / pow(r, 3) * (d.y) * dt;
      vel.z += (G * p2_weight) / pow(r, 3) * (d.z) * dt;
    }
  }
  tmp_vel.vel[idx] = vel;
}

__global__ void calculate_collision_velocity(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  int idx = threadIdx.x + (blockIdx.x * blockDim.x);
  if (idx >= N)
    return;
  float4 d;
  float r;
  float4 p1_pos = p.pos[idx];
  float4 tmp_vel_local = tmp_vel.vel[idx];
  float4 p1_vel = p.vel[idx];
  float weight1 = p1_vel.w;
  for (int i = 0; i < N; i++) {  
    if (i == idx)
      continue;
    float4 p2_pos = p.pos[i];
    d.x = p2_pos.x - p1_pos.x;
    d.y = p2_pos.y - p1_pos.y;
    d.z = p2_pos.z - p1_pos.z;
    float4 p2_vel = p.vel[i];
    float weight2 = p2_vel.w;
    r = sqrt(d.x*d.x + d.y*d.y + d.z*d.z);
    if (r > 0.0f && r < COLLISION_DISTANCE) {
      float weight = weight1 / weight2;
      tmp_vel_local.x += (((weight - 1) * p1_vel.x + 2 * p2_vel.x)/(1 + weight) - p1_vel.x);
      tmp_vel_local.y += (((weight - 1) * p1_vel.y + 2 * p2_vel.y)/(1 + weight) - p1_vel.y);
      tmp_vel_local.z += (((weight - 1) * p1_vel.z + 2 * p2_vel.z)/(1 + weight) - p1_vel.z);
    }
  }
  tmp_vel.vel[idx] = tmp_vel_local;
}

__global__ void update_particle(t_particles p, t_velocities tmp_vel, int N, float dt)
{
  int idx = threadIdx.x + (blockIdx.x * blockDim.x);
  if (idx >= N)
    return;
  p.vel[idx].x += tmp_vel.vel[idx].x;
  p.vel[idx].y += tmp_vel.vel[idx].y;
  p.vel[idx].z += tmp_vel.vel[idx].z;
  p.pos[idx].x += p.vel[idx].x * dt;
  p.pos[idx].y += p.vel[idx].y * dt;
  p.pos[idx].z += p.vel[idx].z * dt;

}

__host__ void particles_read(FILE *fp, t_particles &p, int N)
{
  for (int i = 0; i < N; i++) {
    fscanf(fp, "%f %f %f %f %f %f %f \n",
    &p.pos[i].x, &p.pos[i].y, &p.pos[i].z,
    &p.vel[i].x, &p.vel[i].y, &p.vel[i].z, &p.vel[i].w);
  }
}

__host__  void particles_write(FILE *fp, t_particles &p, int N)
{
  for (int i = 0; i < N; i++) {
    fprintf(fp, "%f %f %f %f %f %f %f \n",
    p.pos[i].x, p.pos[i].y, p.pos[i].z,
    p.vel[i].x, p.vel[i].y, p.vel[i].z, p.vel[i].w);
  }
}
