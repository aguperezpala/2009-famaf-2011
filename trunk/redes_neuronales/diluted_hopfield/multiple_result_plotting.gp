################################################################################
#                                                                              #
# Print results superposition of three DILUTED HOPFIELD NETWORKS               #
#                                                                              #
# It takes its input from the specified files "FILEi", each of which must be   #
# composed of 3 columns: 1) x axis values: p/K , where K == mean connectivity  #
#			 2) y axis values: mean overlap                        #
#			 3) standard deviations of the former                  #
#                                                                              #
################################################################################
#                                                                              #
# FILEi = "i-th file name"                                                     #
# Ni    = network_size                                                         #
#                                                          CONFIG. SECTION     #
#                                                                              #


FILE1  = "`echo $DILUTED_HOPFIELD1`"
FILE2  = "`echo $DILUTED_HOPFIELD2`"
FILE3  = "`echo $DILUTED_HOPFIELD3`"

N1 = `echo $NNEURONS1`
N2 = `echo $NNEURONS2`
N3 = `echo $NNEURONS3`

K = `echo $MEAN_CON`

TITLE="Strongly_diluted_hopfield_comparison"


#                                                                              #
#                                                      END CONFIG. SECTION     #
#                                                                              #
################################################################################
#                                                                              #
#                                                       BEGIN PLOT SECTION     #
#                                                                              #

reset

set out TITLE."_plot.png"
set title TITLE
set xlabel "α'   ( p / K )"
set ylabel "m (overlap)"
set key box

set xr [0.0:1.0]
set yr [0.0:1.05]
set xtics 0.05
set mxtics 2
set ytics 0.1
set ytics nomirror

set style line 1 lc  2 lw 2 pointtype 9
set style line 2 lc  1 lw 2 pointtype 5
set style line 3 lc -1 lw 2 pointtype 7
set style line 4 lc rgb "grey"
set grid ls 4
set border 1+2+4

set term png size 800, 600 nocrop
plot	FILE1 with linespoints ls 1 t "N = ".N1 , \
	FILE2 with linespoints ls 2 t "N = ".N2 , \
	FILE3 with linespoints ls 3 t "N = ".N3

#                                                                              #
#                                                         END PLOT SECTION     #
#                                                                              #
################################################################################