
SAMPLE = "`echo $SAMPLE`"
PLOT   = "`echo $PLOT`"

set out PLOT
set term png size 1000, 400 nocrop
plot SAMPLE with lines lc 1 lw 2 t "ANFIS Learning Error"
