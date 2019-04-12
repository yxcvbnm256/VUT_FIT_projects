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
    t_particles particles_gpu[2];
    for (int i = 0; i < 2; i++) {
        cudaMalloc(&particles_gpu[i].pos, N * sizeof(float4));
        cudaMalloc(&particles_gpu[i].vel, N * sizeof(float4));
        cudaMemcpy(particles_gpu[i].pos, particles_cpu.pos, N * sizeof(float4), cudaMemcpyHostToDevice);
        cudaMemcpy(particles_gpu[i].vel, particles_cpu.vel, N * sizeof(float4), cudaMemcpyHostToDevice);
    }
    // vypocet velikosti vypocetniho gridu
    size_t grid = N / thr_blc + (N % thr_blc == 0 ? 0 : 1);
    printf("grid size is %d\n", grid);
    
    // vypocet
    gettimeofday(&t1, 0);

    t_particles *p_in;
    t_particles *p_out;
    for (int s = 0; s < steps; ++s)
    {
        p_in = &particles_gpu[s % 2];
        p_out = &particles_gpu[(s + 1) % 2];
        calculate_velocity<<<grid, thr_blc, thr_blc * 2 * sizeof(float4)>>>(*p_in, *p_out, N, dt);
    }
    cudaDeviceSynchronize();
    gettimeofday(&t2, 0);

    // cas
    double t = (1000000.0 * (t2.tv_sec - t1.tv_sec) + t2.tv_usec - t1.tv_usec) / 1000000.0;
    printf("Time: %f s\n", t);
    // kopirovani castic zpatky na CPU
    cudaMemcpy(particles_cpu.pos, p_out->pos, N * sizeof(float4), cudaMemcpyDeviceToHost);
    cudaMemcpy(particles_cpu.vel, p_out->vel, N * sizeof(float4), cudaMemcpyDeviceToHost);
    // ulozeni castic do souboru
    fp = fopen(argv[6], "w");
    if (fp == NULL)
    {
        printf("Can't open file %s!\n", argv[6]);
        exit(1);
    }
    particles_write(fp, particles_cpu, N);
    fclose(fp);
    for (int i = 0; i < 2; i++) {
        cudaFree(particles_gpu[i].pos);
        cudaFree(particles_gpu[i].vel);
    }
    cudaFreeHost(particles_cpu.pos);   
    cudaFreeHost(particles_cpu.vel);
    return 0;
}
