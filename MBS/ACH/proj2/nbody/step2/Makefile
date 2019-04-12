# Architektura procesoru (ACH 2018)
# Projekt c.2 (CUDA)
# Login: xpawlu00

N=8192
DT=0.001f
STEPS=1000
THREADS_PER_BLOCK=128

INPUT=../input.dat
OUTPUT=../step2.dat

flags=-Xptxas="-v"

all:
	nvcc $(flags) nbody.cu main.cu -o nbody
	icpc gen.cpp -o gen

clean:
	rm -f *.o nbody

run:
	./nbody $(N) $(DT) $(STEPS) $(THREADS_PER_BLOCK) $(INPUT) $(OUTPUT)

profile:
	nvprof \
		--devices 0 \
 		--metrics gld_throughput \
		--metrics gst_throughput \
		--metrics flops_sp \
		./nbody $(N) $(DT) 1 $(THREADS_PER_BLOCK) $(INPUT) $(OUTPUT)
