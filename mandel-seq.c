/* ------------------------------
   $Id: mandel-seq.c,v 1.2 2008/03/04 09:52:55 marquet Exp $
   ------------------------------------------------------------

   Affichage de l'ensemble de Mandelbrot.
   Version sequentielle.

 */

#include <stdio.h>
#include <stdlib.h>

#include "mandel.h"

int 
main (int argc, char *argv[]) 
{

	int n_iter,			/* degre de nettete  */  
	    x_size, y_size;		/* & dimensions de l'image */  
	double x_min, x_max, y_min, y_max; /* bornes de la representation */
	char *pathname;		/* fichier destination */
	picture_t pict;

	parse_argv(argc, argv, 
			&n_iter, 
			&x_min, &x_max, &y_min, &y_max, 
			&x_size, &y_size, &pathname);

	init_picture (& pict, x_size, y_size);
	compute (& pict, n_iter, x_min, x_max, y_min, y_max);
	save_picture (& pict, pathname);

	exit(EXIT_SUCCESS);
}
