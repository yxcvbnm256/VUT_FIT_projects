# Architektura procesoru (ACH 2018)
# Projekt c. 1 (nbody)
# Login: xpawlu00

CC=icpc
CFLAGS=-std=c++11 -lpapi -ansi-alias
OPT=-O2 -Wall -xavx -qopenmp-simd
REPORT=-qopt-report=5

N=1500
DT=0.001f
STEPS=750

PARAMS=-DN=$(N) -DDT=$(DT) -DSTEPS=$(STEPS)

INPUT=../input.dat
OUTPUT=../step4.dat

PAPI_EVENTS=PAPI_FP_OPS|PAPI_SP_OPS
PAPI_EVENTS_L1=PAPI_LD_INS|PAPI_SR_INS|PAPI_L1_TCM|PAPI_L2_TCA|PAPI_L2_TCM|PAPI_L3_TCA|PAPI_L3_TCM
all:
	$(CC) $(CFLAGS) $(OPT) $(REPORT) $(PARAMS) -c velocity.cpp
	$(CC) $(CFLAGS) $(OPT) $(REPORT) $(PARAMS) -c nbody.cpp
	$(CC) $(CFLAGS) $(OPT) $(PARAMS) velocity.o nbody.o main.cpp -o nbody
	$(CC) $(CFLAGS) gen.cpp -o gen

clean:
	rm -f *.o *.optrpt *.s gen nbody

run:
	PAPI_EVENTS='$(PAPI_EVENTS)' ./nbody $(INPUT) $(OUTPUT)
