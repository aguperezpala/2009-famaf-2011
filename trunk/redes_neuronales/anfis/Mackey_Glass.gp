
XL = `echo $XLOW`
XH = `echo $XHIGH`

ORIG   = "`echo $ORIGINAL`"
LEARN  = "`echo $LEARNING`"
PLOT   = "`echo $PLOT`"

set xrange [XL:XH]
set out PLOT
set term png size 1100, 400 nocrop
plot ORIG  with lines lt 2 lw 2 t "Mackey-Glass DE", \
     LEARN with lines lt 3 lw 1 t "ANFIS Learning"
