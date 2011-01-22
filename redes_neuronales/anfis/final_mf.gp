a1_1 = 0.25
b1_1 = 3.0
c1_1 = 0.25
a2_1 = 0.25
b2_1 = 3.0
c2_1 = 0.75
a3_1 = 0.25
b3_1 = 3.0
c3_1 = 1.25
a4_1 = 0.25
b4_1 = 3.0
c4_1 = 1.75

#a2_1 = `echo $A1_1`
#b2_1 = `echo $B1_1`
#c2_1 = `echo $C1_1`
#a2_2 = `echo $A1_2`
#b2_2 = `echo $B1_2`
#c2_2 = `echo $C1_2`
#a2_3 = `echo $A1_3`
#b2_3 = `echo $B2_3`
#c2_3 = `echo $C2_3`
#a2_4 = `echo $A2_4`
#b2_4 = `echo $B2_4`
#c2_4 = `echo $C2_4`


mf_1_1(x) = 1 / (1 + (((x - c1_1) / a1_1) ** 2 ** b1_1) )
mf_2_1(x) = 1 / (1 + (((x - c2_1) / a2_1) ** 2 ** b2_1) )
mf_3_1(x) = 1 / (1 + (((x - c3_1) / a3_1) ** 2 ** b3_1) )
mf_4_1(x) = 1 / (1 + (((x - c4_1) / a4_1) ** 2 ** b4_1) )


set out "final_mf.png"
set term png size 1000, 400 nocrop
set xrange [-0.5:2.5]

plot	mf_1_1(x) with lines lt 1, \
	mf_2_1(x) with lines lt 2, \
	mf_3_1(x) with lines lt 3, \
	mf_4_1(x) with lines lt 4