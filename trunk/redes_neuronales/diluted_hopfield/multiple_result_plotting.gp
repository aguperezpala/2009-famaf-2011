#
# Print final overlap results of a HOPFIELD NETWORK of a specified size N
#
# It takes its input from the specified files "FILEi", each of which must be
# composed of 3 columns: 1) x axis values: p/N or T levels
#			 2) y axis values: mean overlap
#			 3) standard deviations of the former
#
DET=0
STOC=1
#
#
# ~~~ CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
#
# FILEi = "i-th file name"
# Ni    = network_size


FILE1 = "hop_stoc_n416_p5.dat"
FILE2 = "hop_stoc_n832_p10.dat"
FILE3 = "hop_stoc_n1664_p20.dat"

N1 = 416
N2 = 832
N3 = 1664

TITLE="Stochastic_hopfield_small_α"

# Options for COND: { DET , STOC }
COND = STOC

# If STOC chosen then must give # of memories for N1:
P1 = 5



# ~~~ END CONFIG. SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#
################################################################################
#
# ~~~ BEGIN PLOT SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

reset

alpha = (P1+0.0)/(N1+0.0)
a = sprintf ("%.8f",alpha)

if (COND==DET)								\
	set title TITLE   ;						\
	set xlabel "α   ( p / N )" ;					\
	set xr [0.0:0.5]  ;						\
	set yr [0.0:1.05] ;						\
	set xtics 0.05    ;						\
	set mxtics 5	  ;						\
									\
else if (COND==STOC)							\
	set out TITLE."_comparison_plot.png" ;				\
	set title TITLE offset first -0.37,0.02 ;			\
	set label "α = ".a offset first 1.05,1.12 center ;		\
	set xlabel "T (noise level)" ;					\
	set xr [0.1:1.2]    ;						\
	set yr [-0.05:1.05] ;						\
	set xtics 0.1       ;						\
	set mxtics 2        ;						\
									\
else									\
	print "\nChoose a valid option for COND\n" ;			\
	exit() ;


set ytics 0.1
set ytics nomirror
set style line 4 lc rgb "grey"
set grid ls 4
set border 1+2+4

set ylabel "m (overlap)"
set key box
set style line 1 lc  2 lw 2 pointtype 9
set style line 2 lc  1 lw 2 pointtype 5
set style line 3 lc -1 lw 2 pointtype 7

set term png size 800, 600 nocrop
plot	FILE1 with linespoints ls 1 t "N = ".N1 , \
	FILE2 with linespoints ls 2 t "N = ".N2 , \
	FILE3 with linespoints ls 3 t "N = ".N3

#
# ~~~ END PLOT SECTION ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
