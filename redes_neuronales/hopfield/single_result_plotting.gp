# Print final overlap results of a HOPFIELD NETWORK of a specified size N
#
# It takes its input from the specified "FILE", which must be composed of
# 3 columns: 1) p/N values
#	     2) mean overlap values
#	     3) standard deviations
#
# ~~~ CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE="hop_stoc_n1664_p20.dat"
TITLE="Stochastic_hopfield"
N="1664"
P="20"

# ~~~ END CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

reset

# set title TITLE offset first -0.16,1.12
# set label "N = ".N offset first 0.45,1.115 center
set title TITLE offset first -0.4,1.12
set label "N = ".N offset first 0.85,1.115 center
set label "P = ".P offset first 1.05,1.115 center

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
set bar 0.3

set term png size 800, 600 nocrop
set out TITLE."_".N."_".P."_plot.png"
plot FILE with lines lt -1 lw 3 t "Values" , \
	FILE u 1:2:3 w errorb lt 2 lw 2 t "Deviations"

