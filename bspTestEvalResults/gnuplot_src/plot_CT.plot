set xlabel	"Number of processors"
set ylabel	"Completion Time (ms)"
set title	"Completion Time"

set style line 1 linecolor 'black' linetype 1 dashtype 2 linewidth 1 pointtype 12 pointsize 2
set style line 2 linecolor 'black' linetype 2 dashtype 3 linewidth 1 pointtype 5 pointsize 0.7
set style line 3 linecolor 'black' linetype 3 dashtype 1 linewidth 1 pointtype 1 pointsize 2.5


plot	'./gnuplot_src/data_completion_time.dat'	index 0 with linespoints linestyle 1,\
		''											index 1 with linespoints linestyle 2,\
		''											index 2 with linespoints linestyle 3