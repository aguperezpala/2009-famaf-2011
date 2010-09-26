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

FILE1="hop_det_n448_p224_h4.dat"
N1=448
FILE2="hop_det_n256_p128_h2.dat"
N2=256
FILE3="hop_det_n128_p64_h1.dat"
N3=128

TITLE="Deterministic Hopfield"

# ~~~ END CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set title TITLE

set xtics 0.05
set mxtics 5
set ytics 0.1
set xr [0.0:0.5]
set yr [0.0:1.05]
set style line 4 lc rgb "grey"
set grid ls 4
set border 1+2+4
set ytics nomirror
set xlabel "p/N"
set ylabel "m (overlap)"
set key box
set style line 1 lc 1 lw 2 pointtype 9
set style line 2 lc 2 lw 2 pointtype 5
set style line 3 lc 3 lw 2 pointtype 7
plot	FILE1 with linespoints ls 1 t "N = ".N1 , \
	FILE2 with linespoints ls 2 t "N = ".N2 , \
	FILE3 with linespoints ls 3 t "N = ".N3

