set xlabel	"{/*1.5 Number of processors}"
set ylabel	"{/*1.5 Scalability}"
set title	"{/*1.5 Scalability}"
set key font ",14"
set xtics font ",14"
set ytics font ",14"

set style line 1 linecolor 'black' linetype 1 dashtype 2 linewidth 1 pointtype 12 pointsize 2
set style line 2 linecolor 'black' linetype 2 dashtype 3 linewidth 1 pointtype 5 pointsize 0.7
set style line 3 linecolor 'black' linetype 3 dashtype 1 linewidth 1 pointtype 1 pointsize 2.5



plot	'./gnuplot/data_scalability.dat'	index 0 with linespoints linestyle 1 title "2^{30}", \
		''									index 1 with linespoints linestyle 2 title "2^{25}"