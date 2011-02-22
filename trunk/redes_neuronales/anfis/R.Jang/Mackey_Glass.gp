
XL = `echo $XLOW`
XH = `echo $XHIGH`

ORIG   = "`echo $ORIGINAL`"
LEARN  = "`echo $LEARNING`"
PLOT   = "`echo $PLOT`"

set xrange [XL:XH]
set tics out
set xtics nomirror
set ytics nomirror
set ytics scale 1, 1
set ytics 0.2
set mytics 2
set out PLOT
set term png size 1100, 400 nocrop
plot ORIG  with lines lt 1 lc 8 lw 3 t "Mackey-Glass DE", \
     LEARN with lines lt 1 lc 1 lw 2 t "ANFIS Learning"
