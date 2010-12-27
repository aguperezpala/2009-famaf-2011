
XL = `echo $XLOW`
XH = `echo $XHIGH`

SAMPLE = "`echo $SAMPLE`"
PLOT   = "`echo $PLOT`"

set xrange [XL:XH]
set out PLOT
set term png size 1000, 400 nocrop
plot SAMPLE with lines lt 2 lw 2 t "Mackey-Glass DE"
