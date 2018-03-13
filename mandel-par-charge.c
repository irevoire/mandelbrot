#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mandel.h"
#include "mpi.h"

#define PROC_MASTER 0

/* On a besoin de ces deux tags pour ne pas mélanger les messages
 * envoyant des bouts d'images calculé et les messages envoyant les
 * prochaines bouts d'images à calculer */
#define NEW_JOB 0xcaca
#define FINISHED_JOB 0xbabe

#define FINISH_VALUE -21.111996

int n_iter,			/* degre de nettete  */  
	x_size, y_size;		/* & dimensions de l'image */  
double x_min, x_max, y_min, y_max; /* bornes de la representation */
char *pathname;		/* fichier destination */
double start_time, end_time;


void slave(MPI_Comm com, MPI_Status status, const double real_size_bands, const unsigned pixel_size_bands)
{
	double y_min_tmp;
	picture_t tmp_pic;

	init_picture(&tmp_pic, x_size, pixel_size_bands);

	while (1)
	{
		MPI_Recv(&y_min_tmp, 1, MPI_DOUBLE, PROC_MASTER, NEW_JOB, com, &status);
		if (y_min_tmp == FINISH_VALUE)
			break;

		compute(&tmp_pic, n_iter, x_min, x_max, y_min_tmp, y_min_tmp + real_size_bands);

		MPI_Send(tmp_pic.pixels, x_size * pixel_size_bands, MPI_CHAR, PROC_MASTER, FINISHED_JOB, com);
	}
}

	int
main(int argc, char * argv[])
{
	/* Mandelbrot */
	picture_t final_pic,
			  tmp_pic;

	parse_argv(argc, argv, 
			&n_iter, 
			&x_min, &x_max, &y_min, &y_max, 
			&x_size, &y_size, &pathname);

	/* MPI */
	MPI_Comm com;	/* communicateur */
	MPI_Status status;

	com = MPI_COMM_WORLD;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(com, &procs);
	MPI_Comm_rank(com, &self);
	MPI_Request request[procs];

	double start_time, end_time;


	if (self != PROC_MASTER)
	{
		(void)MPI_Wtime();
		start_time = MPI_Wtime();
	}

	init_picture (&final_pic, x_size, y_size);

	unsigned nb_bands = procs * 5;
	unsigned current_band = 0;
	unsigned tab_procs[procs];
	double y_min_tmp;

	double real_size_bands = (y_max - y_min) / nb_bands;
	unsigned pixel_size_bands = y_size / nb_bands;

	if (self != PROC_MASTER)
	{
		slave(com, status, real_size_bands, pixel_size_bands);
		MPI_Finalize();
		end_time = MPI_Wtime();
		printf ("%.3d %f\n", self, end_time - start_time);
		exit(EXIT_SUCCESS);
	}

	
	/* On démarre chaque processus sur une première bande */
	for (int i = 1; i < procs; i++)
	{
		y_min_tmp = y_min + real_size_bands * current_band;

		tab_procs[i] = current_band++;

		MPI_Send(&y_min_tmp, 1, MPI_DOUBLE, i, NEW_JOB, com);
	}

	init_picture(&tmp_pic, x_size, pixel_size_bands);


	/* Tant qu'il y a encore des bandes on les files aux processus */
	while (current_band < nb_bands)
	{
		MPI_Recv(tmp_pic.pixels,
				x_size * pixel_size_bands,
				MPI_CHAR, MPI_ANY_SOURCE, FINISHED_JOB, com, &status);

		memcpy(final_pic.pixels + (pixel_size_bands * x_size * (nb_bands - tab_procs[status.MPI_SOURCE] - 1)),
				tmp_pic.pixels, x_size * pixel_size_bands);

		y_min_tmp = y_min + real_size_bands * current_band;

		tab_procs[status.MPI_SOURCE] = current_band++;

		/* On envoie le nouveau y_min au même processus */
		MPI_Send(&y_min_tmp, 1, MPI_DOUBLE, status.MPI_SOURCE, NEW_JOB, com);
	}


	for (int i = 1; i < procs; i++)
	{
		MPI_Recv(tmp_pic.pixels, x_size * pixel_size_bands,
				MPI_CHAR, MPI_ANY_SOURCE, FINISHED_JOB, com, & status);

		memcpy(final_pic.pixels + ((nb_bands - tab_procs[status.MPI_SOURCE] - 1) * x_size * pixel_size_bands),
				tmp_pic.pixels, (x_size * pixel_size_bands));
	}


	y_min_tmp = FINISH_VALUE;

	for (int i = 1; i < procs; i++)
		MPI_Send(&y_min_tmp, 1, MPI_DOUBLE, i, NEW_JOB, com);

	end_time = MPI_Wtime();

	save_picture(&final_pic, pathname);
	fprintf(stderr, "\nTotal compute time : %lf\n", (end_time - start_time));

	MPI_Finalize();
	exit(EXIT_SUCCESS);
}
