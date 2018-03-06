CC := mpicc
RUN := mpirun
HOSTFILE := mpi_hosts

EXE := mandel-par mandel-seq

.PHONY: all run clean

all: $(EXE)

## MPI ####################

mandel-par: mandel-par.o mandel.o
	$(CC) $^ -o $@

mandel-par.o: mandel-par.c
	$(CC) $< -c

run:
	$(RUN) -np 208 --hostfile $(HOSTFILE) mandel-par -d 4096 3072 -n 4096

## Seq ####################

mandel-seq: mandel-seq.o mandel.o
	gcc $^ -o $@

mandel-seq.o: mandel-seq.c
	gcc $< -c

## Lib ####################

mandel.o: mandel.c mandel.h
	gcc $< -c

clean:
	@rm -f $(EXE) *.o mandel.ppm
