set xrange [150:1118]
# [150:1118]
#set autoscale nocrop
set out "Mackey_Glass.png"
set term png size 900, 400 nocrop
plot "Mackey_Glass.dat" with lines
