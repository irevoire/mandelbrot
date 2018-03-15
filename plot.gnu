set autoscale


set title "Temps d'execution passé dans chaque processus"
set grid

set xlabel "Identifiant du processus"
set ylabel "Temps d'execution en secondes"

set style data linespoints

plot "save-mandel-par.dat" using 1:2 title "MPI_Gather", \
	"save-mandel-par-charge.dat" using 1:2 title "Slave-Master"

pause -1
