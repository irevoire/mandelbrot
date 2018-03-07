CC := mpicc
RUN := mpirun
HOSTFILE := mpi_hosts

EXE := mandel-seq \
	mandel-par \
	mandel-par-charge

.PHONY: all run clean

all: $(EXE)

## MPI ####################

mandel-par: mandel-par.o mandel.o
	$(CC) $^ -o $@

mandel-par.o: mandel-par.c
	$(CC) $< -c

mandel-par-charge: mandel-par-charge.o mandel.o
	$(CC) $^ -o $@

mandel-par-charge.o: mandel-par-charge.c
	$(CC) $< -c

## Seq ####################

mandel-seq: mandel-seq.o mandel.o
	gcc $^ -o $@

mandel-seq.o: mandel-seq.c
	gcc $< -c

## Lib ####################

mandel.o: mandel.c mandel.h
	gcc $< -c

## Misc ###################

run:
	$(RUN) -np 208 --hostfile $(HOSTFILE) mandel-par -d 4096 3072 -n 4096

clean:
	@rm -f $(EXE) *.o mandel.ppm
