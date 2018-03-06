## TODO Makefile
$ mpicc mandel-par.c -o mandel-par
$ mpirun -np 208 --hostfile mpi_hosts mandel-par -d 4096 3072 -n 4096
