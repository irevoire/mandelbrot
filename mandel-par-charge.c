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

	printf("Je suis le slave %d\n", self);

	while (1)
	{
		MPI_Recv(&y_min_tmp, 1, MPI_DOUBLE, PROC_MASTER, NEW_JOB, com, &status);
		printf("[%d] Doit calculer la bande %f\n", self, y_min_tmp);
		if (y_min_tmp == FINISH_VALUE)
			break;
		printf("[%d] Va calculer une bande\n", self);

		compute(&tmp_pic, n_iter, x_min, x_max, y_min_tmp, y_min_tmp + real_size_bands);

		printf("[%d] envoie ses données\n", self);
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

	
	if (self == PROC_MASTER)
		start_time = MPI_Wtime();

	init_picture (&final_pic, x_size, y_size);

	unsigned nb_bands = procs * 5;
	unsigned current_band = 0;
	unsigned tab_procs[procs];
	double y_min_tmp;

	printf("y_max %f, y_min %f", y_max, y_min);
	double real_size_bands = (y_max - y_min) / nb_bands;
	unsigned pixel_size_bands = y_size / nb_bands;

	if (self != PROC_MASTER)
	{
		slave(com, status, real_size_bands, pixel_size_bands);
		printf("[%d] fini\n", self);
		MPI_Finalize();
		exit(EXIT_SUCCESS);
	}

	printf("sizex %d, sizey %d\n", x_size, y_size);
	printf("nb_procs %d, nb_bands %d\n", procs, nb_bands);


	printf("[M] Avant envoie des bandes à tous les processus...\n");
	
	
	/* On démarre chaque processus sur une première bande */
	for (int i = 1; i < procs; i++)
	{
		printf("ymin = %f\n", y_min);
		printf("size_bands = %f\n", real_size_bands);
		printf("current_band = %d\n", current_band);
		y_min_tmp = y_min + real_size_bands * current_band;

		tab_procs[i] = current_band++;

		MPI_Send(&y_min_tmp, 1, MPI_DOUBLE, i, NEW_JOB, com);
	}

	printf("[M] Après avoir distribuée toutes les %d bandes...\n", current_band);
	init_picture(&tmp_pic, x_size, pixel_size_bands);


	/* Tant qu'il y a encore des bandes on les files aux processus */
	while (current_band < nb_bands)
	{
		printf("[M] Entrain de récupérer une bande...\n");
		MPI_Recv(tmp_pic.pixels,
				x_size * pixel_size_bands,
				MPI_CHAR, MPI_ANY_SOURCE, FINISHED_JOB, com, &status);
		printf("[M] A récupéré la bande %d...\n", tab_procs[status.MPI_SOURCE]);

		memcpy(final_pic.pixels + (pixel_size_bands * x_size * (nb_bands - tab_procs[status.MPI_SOURCE] - 1)),
				tmp_pic.pixels, x_size * pixel_size_bands);

		y_min_tmp = y_min + real_size_bands * current_band;

		tab_procs[status.MPI_SOURCE] = current_band++;

		printf("[M] Envoie la bande %d en %f...\n", tab_procs[status.MPI_SOURCE], y_min_tmp);
		/* On envoie le nouveau y_min au même processus */
		MPI_Send(&y_min_tmp, 1, MPI_DOUBLE, status.MPI_SOURCE, NEW_JOB, com);
		printf("#");
	}

	printf("\n[M] Calcul des dernières bandes...\n");


	for (int i = 1; i < procs; i++)
	{
		MPI_Recv(tmp_pic.pixels, x_size * pixel_size_bands,
				MPI_CHAR, MPI_ANY_SOURCE, FINISHED_JOB, com, & status);

		memcpy(final_pic.pixels + ((nb_bands - tab_procs[status.MPI_SOURCE] - 1) * x_size * pixel_size_bands),
				tmp_pic.pixels, (x_size * pixel_size_bands));
	}


	y_min_tmp = FINISH_VALUE;

	printf("[M] On dit aux autres processus de se termienr...\n");
	for (int i = 1; i < procs; i++)
		MPI_Send(&y_min_tmp, 1, MPI_DOUBLE, i, NEW_JOB, com);

	end_time = MPI_Wtime();

	save_picture(&final_pic, pathname);
	printf("Total compute time : %lf\n", (end_time - start_time));

	MPI_Finalize();
	exit(EXIT_SUCCESS);
}
