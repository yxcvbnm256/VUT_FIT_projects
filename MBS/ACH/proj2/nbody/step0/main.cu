/*
 * Architektura procesoru (ACH 2018)
 * Projekt c.2 (CUDA)
 * Login: xpawlu00
 */

#include <sys/time.h>
#include <cstdio>
#include <cmath>
#include <vector_types.h>
#include "nbody.h"

int main(int argc, char **argv)
{
    FILE *fp;
    struct timeval t1, t2;
    int N;
    float dt;
    int steps;
    int thr_blc;

    // parametry
    if (argc != 7)
    {
        printf("Usage: nbody <N> <dt> <steps> <thr/blc> <input> <output>\n");
        exit(1);
    }
    N = atoi(argv[1]);
    dt = atof(argv[2]);
    steps = atoi(argv[3]);
    thr_blc = atoi(argv[4]);

    printf("N: %d\n", N);
    printf("dt: %f\n", dt);
    printf("steps: %d\n", steps);
    printf("threads/block: %d\n", thr_blc);
    // alokace pameti na CPU
    t_particles particles_cpu;
    cudaHostAlloc(&particles_cpu.pos, N * sizeof(float4), cudaHostAllocDefault);
    cudaHostAlloc(&particles_cpu.vel, N * sizeof(float4), cudaHostAllocDefault);
    // nacteni castic ze souboru
    fp = fopen(argv[5], "r");
    if (fp == NULL)
    {
        printf("Can't open file %s!\n", argv[5]);
        exit(1);
    }
    particles_read(fp, particles_cpu, N);
    fclose(fp);
    t_particles particles_gpu;
    t_velocities tmp_velocities_gpu;
    // alokace pameti na GPU
    cudaMalloc(&particles_gpu.pos, N * sizeof(float4));
    cudaMalloc(&particles_gpu.vel, N * sizeof(float4));
    // kopie dat z cpu na gpu
    cudaMemcpy(particles_gpu.pos, particles_cpu.pos, N * sizeof(float4), cudaMemcpyHostToDevice);
    cudaMemcpy(particles_gpu.vel, particles_cpu.vel, N * sizeof(float4), cudaMemcpyHostToDevice);
    // alokace pomocne struktury na gpu
    cudaMalloc(&tmp_velocities_gpu.vel, N * sizeof(float4));
    // vypocet gridu
    size_t grid = N / thr_blc + (N % thr_blc == 0 ? 0 : 1);    
    // vypocet
    gettimeofday(&t1, 0);
    for (int s = 0; s < steps; ++s)
    {
        calculate_gravitation_velocity<<<grid, thr_blc>>>(particles_gpu,tmp_velocities_gpu, N, dt);
        calculate_collision_velocity<<<grid, thr_blc>>>(particles_gpu,tmp_velocities_gpu, N, dt);
        update_particle<<<grid, thr_blc>>>(particles_gpu,tmp_velocities_gpu, N, dt);
    }
    cudaDeviceSynchronize();
    gettimeofday(&t2, 0);
    // cas
    double t = (1000000.0 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec) / 1000000.0;
    printf("Time: %f s\n", t);
    // kopirovani castic zpatky na CPU */
    cudaMemcpy(particles_cpu.pos, particles_gpu.pos, N * sizeof(float4), cudaMemcpyDeviceToHost);
    cudaMemcpy(particles_cpu.vel, particles_gpu.vel, N * sizeof(float4), cudaMemcpyDeviceToHost);
    // ulozeni castic do souboru
    fp = fopen(argv[6], "w");
    if (fp == NULL)
    {
        printf("Can't open file %s!\n", argv[6]);
        exit(1);
    }
    particles_write(fp, particles_cpu, N);
    fclose(fp);
    cudaFreeHost(particles_cpu.pos);   
    cudaFreeHost(particles_cpu.vel);
    cudaFree(particles_gpu.pos);
    cudaFree(particles_gpu.vel);
    cudaFree(tmp_velocities_gpu.vel);
    return 0;
}