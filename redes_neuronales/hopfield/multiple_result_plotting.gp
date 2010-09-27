# Print final overlap results of a HOPFIELD NETWORK of a specified size N
#
# It takes its input from the specified files "FILEi", each of which must be
# composed of 3 columns: 1) p/N values
#			 2) mean overlap values
#			 3) standard deviations
#
# ~~~ CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
# FILEi="i-th file name"
# Ni=network_size

FILE1="hop_stoc_n448_p60.dat"
N1=448
FILE2="hop_stoc_n832_p60.dat"
N2=832
FILE3="hop_stoc_n1664_p60.dat"
N3=1664

TITLE="Stochastic_hopfield"

P="60"

# ~~~ END CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

reset

# set title TITLE
set title TITLE offset first -0.37,0.02
set label "P = ".P offset first 1.05,1.13 center

# set xr [0.0:0.5]
# set yr [0.0:1.05]
set xr [0.1:1.2]
set yr [-0.05:1.05]
set xtics 0.1
set mxtics 5
set ytics 0.1
set ytics nomirror
set style line 4 lc rgb "grey"
set grid ls 4
set border 1+2+4

set xlabel "T (noise level)"
set ylabel "m (overlap)"
set key box
set style line 1 lc  1 lw 2 pointtype 9
set style line 2 lc  8 lw 2 pointtype 5
set style line 3 lc -1 lw 2 pointtype 7

set term png size 800, 600 nocrop
set out TITLE."_P".P."_comparison_plot.png"
plot	FILE1 with linespoints ls 1 t "N = ".N1 , \
	FILE2 with linespoints ls 2 t "N = ".N2 , \
	FILE3 with linespoints ls 3 t "N = ".N3

