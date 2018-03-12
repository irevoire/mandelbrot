/* ------------------------------
   $Id: mandel-seq.c,v 1.2 2008/03/04 09:52:55 marquet Exp $
   ------------------------------------------------------------

   Affichage de l'ensemble de Mandelbrot.
   Version sequentielle.

 */

#include <stdio.h>
#include <stdlib.h>

#include "mandel.h"
#include "mpi.h"

#define PROC_NULL 0

int 
main (int argc, char *argv[]) 
{

	int n_iter,			/* degre de nettete  */  
	    x_size, y_size;		/* & dimensions de l'image */  
	double x_min, x_max, y_min, y_max; /* bornes de la representation */
	char *pathname;		/* fichier destination */
	double start_time, end_time;
	picture_t pictlocal, pictglobal;
	MPI_Comm com;	/* communicateur */
	MPI_Status status;

	parse_argv(argc, argv, 
			&n_iter, 
			&x_min, &x_max, &y_min, &y_max, 
			&x_size, &y_size, &pathname);

	com = MPI_COMM_WORLD;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(com, &procs);
	MPI_Comm_rank(com, &self);

	(void)MPI_Wtime();
	start_time = MPI_Wtime();

	init_picture (&pictlocal, x_size, (y_size / procs));
	init_picture (&pictglobal, x_size, y_size);

	compute(&pictlocal, 
			n_iter, 
			x_min, 
			x_max, 
			y_max - ((y_max - y_min) / procs) * (self + 1), // On fait des tranches que chaque processus vont calculer
			y_max - ((y_max - y_min) / procs) * self);

	MPI_Gather(pictlocal.pixels, 
			x_size * (y_size / procs), 
			MPI_CHAR, 
			pictglobal.pixels, 
			x_size * (y_size / procs), 
			MPI_CHAR, 
			PROC_NULL, 
			com);

	end_time = MPI_Wtime();

	if (self == PROC_NULL) {
		save_picture (&pictglobal, pathname);
		fprintf(stderr, "Total compute time : %lf\n", (end_time - start_time));
	}

	MPI_Finalize();

	printf ("%.3d %f\n", self, end_time - start_time);

	exit(EXIT_SUCCESS);
}
