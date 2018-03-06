CC := mpicc
RUN := mpirun
HOSTFILE := mpi_hosts

.PHONY: all run clean

all: mandel-par

mandel-par: mandel-par.c
	$(CC) $^ -o $@
run:
	$(RUN) -np 208 --hostfile $(HOSTFILE) mandel-par -d 4096 3072 -n 4096

clean:
	@rm -f mandel-par *.o mandel.ppm
