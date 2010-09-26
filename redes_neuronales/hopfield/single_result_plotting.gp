# Print final overlap results of a HOPFIELD NETWORK of a specified size N
#
# It takes its input from the specified "FILE", which must be composed of
# 3 columns: 1) p/N values
#	     2) mean overlap values
#	     3) standard deviations
#
# ~~~ CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE="hop_det_n448_p224_h2.dat"
TITLE="Deterministic Hopfield"
N=448

# ~~~ END CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

set title TITLE offset first -0.16,0.05 font "serif,16"
set label "N = ".N offset first 0.45,1.16 center font "sans,14"
set xtics 0.05
set mxtics 5
set ytics 0.1
set xr [0.0:0.5]
set yr [0.0:1.05]
set style line 4 lc rgb "grey"
set grid ls 4
set border 1+2+4
set ytics nomirror
set xlabel "α (p/N)" 
set ylabel "m (overlap)"
set key box
set bar 0.3
plot FILE with lines lt 3 lw 3 t "Values" , \
	FILE u 1:2:3 w errorb lt 2 t "Deviations"

