#ifndef _MANDEL_H_
#define _MANDEL_H_

/* Valeur par defaut des parametres */
#define N_ITER  255		/* nombre d'iterations */

#define X_MIN   -1.78		/* ensemble de Mandelbrot */
#define X_MAX   0.78
#define Y_MIN   -0.961
#define Y_MAX   0.961

#define X_SIZE  2048		/* dimension image */
#define Y_SIZE  1536
#define FILENAME "mandel.ppm"	/* image resultat */
#define PROC_NULL 0

typedef struct {  
	int x_size, y_size;		/* dimensions */
	char *pixels;		/* matrice linearisee de pixels */
} picture_t;

extern int self;
extern int procs;
extern int rnbr, lnbr;

void usage();

void parse_argv (int argc, char *argv[], 
		int *n_iter, 
		double *x_min, double *x_max, double *y_min, double *y_max, 
		int *x_size, int *y_size, 
		char **path);

void 
init_picture (picture_t *pict, int x_size, int y_size);

void 
save_picture (const picture_t *pict, const char *pathname);

void
compute (picture_t *pict,
		int nb_iter,
		double x_min, double x_max, double y_min, double y_max);



#endif /* _MANDEL_H_ */
