
# Sólo para debugging
# Para impresión manual de las funciones membresía resultantes del aprendizaje
# La impresión ya fue automatizada en "trabajo_final.sh"

a1_1 = 0.125
b1_1 = 2.29
c1_1 = 0.25
a2_1 = 0.01
b2_1 = 2.18
c2_1 = 0.75
a3_1 = 0.13
b3_1 = 2.52
c3_1 = 1.25
a4_1 = 0.16
b4_1 = 2.05
c4_1 = 1.75

mf_1_1(x) = 1 / (1 + (abs ((x - c1_1) / a1_1) ** (2 * b1_1)) )
mf_2_1(x) = 1 / (1 + (abs ((x - c2_1) / a2_1) ** (2 * b2_1)) )
mf_3_1(x) = 1 / (1 + (abs ((x - c3_1) / a3_1) ** (2 * b3_1)) )
mf_4_1(x) = 1 / (1 + (abs ((x - c4_1) / a4_1) ** (2 * b4_1)) )


set out "final_mf.png"
set term png size 1000, 400 nocrop
set xrange [-0.5:2.5]

plot	mf_1_1(x) with lines lt 1 lw 2, \
	mf_2_1(x) with lines lt 2 lw 2, \
	mf_3_1(x) with lines lt 3 lw 2, \
	mf_4_1(x) with lines lt 4 lw 2