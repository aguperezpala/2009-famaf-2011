################################################################################
#                                                                              #
# Print final overlap results of DILUTED HOPFIELD NETWORK of specified size N  #
#                                                                              #
# It takes its input from the specified "FILE", which must be composed of      #
# 3 columns: 1) x axis values: p/K, where K == mean conectivity                #
#	     2) y axis values: mean overlap				       #
#	     3) standard deviations of the former			       #
#                                                                              #
################################################################################
#                                                                              #
#                                                          CONFIG. SECTION     #
#                                                                              #


FILE  = "`echo $DILUTED_HOPFIELD`"
TITLE = "Strongly_diluted_hopfield"

N = `echo $NNEURONS`
K = `echo $MEAN_CON`


#                                                                              #
#                                                      END CONFIG. SECTION     #
#                                                                              #
################################################################################
#                                                                              #
#                                                       BEGIN PLOT SECTION     #
#                                                                              #

reset

set out TITLE."_".N."_plot.png"
set title TITLE offset first -0.36,1.12
set label "N = ".N offset first 0.95,1.115 center
set xlabel "α'   ( p / K )"

set xr [0.0:1.0]
set yr [0.0:1.05]
set xtics 0.05
set mxtics 2
set ytics 0.1
set ytics nomirror

set style line 4 lc rgb "grey"
set grid ls 4
set border 1+2+4

set ylabel "m (overlap)"
set key box
set bar 0.3
set term png size 800, 600 nocrop
plot FILE with lines lt 2 lw 3 t "Values" , \
	FILE u 1:2:3 w errorb lt 8 lw 2 t "Deviation"


#                                                                              #
#                                                         END PLOT SECTION     #
#                                                                              #
################################################################################