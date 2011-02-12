#!/bin/sh
# NOTE Depends on test.c

# Following values must reflect those in test.c
T=7
N=5
width=15
decimal_length=4

w=$width
w2=`expr $w / 2`
dl=$decimal_length
TEST=test.exe

gcc -o $TEST ../mzran13.c test.c && \
./$TEST | gawk '
BEGIN {
	w  = '$w'
	w2 = '$w2'
	T  = '$T'
	N  = '$N'
	params[""]=""
	print "+-------------------------------------------------------------------+"
	printf "|%-'$w2's%-'$w's%-'$w's%-'$w's%-'$w's|\n",
		"BRANCH", "INPUT" , "WF[a]", "WF[b]", "WF[c]";
	print "|-------------------------------------------------------------------|"
}
{
	# Fillup array
	
	if (NF == 5) {
		params[$1, $2] = $3 " " $4 " " $5
	} else {
		print "Bad # of fields:", NF
	}
}
END {
	for (i=0 ; i < T ; i++) {
		for (j=0 ; j < N ; j++) {
			printf "|%*d%*d%*'.$dl'f%*'.$dl'f%*'.$dl'f\t    |\n",
			3, i,
			w2, j,
			w+2, params[i, j],
			w, params[i, j],
			w, params[i, j];
		}
		print "|- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -|"
	}
	print " - DONE - "
}
' | less && \
rm $TEST

exit 0
