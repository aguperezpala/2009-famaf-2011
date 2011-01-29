#!/bin/bash

# Parámetros para la red ANFIS:
# Número de ramas
T=2
# Dimensión de la entrada
N=4


LOG=anfis.log
MAIN=./anfis_mg

# Serie de aprendizaje
GEN_MG=./Mackey_Glass
PLOT_MG=Mackey_Glass.gp
MG_DATA=Mackey_Glass.dat
MG_ANFIS=anfis.dat
MG_PLOT=Mackey_Glass.png

# Error de aprendizaje del ANFIS
PLOT_LERR=learn_error.gp
LERR_DATA=anfis_err.dat
LERR_PLOT=anfis_err.png

# Funciones de membresía (Membership Functions)
MF_DATA_INITIAL=anfis_mf_initial.dat
MF_DATA_FINAL=anfis_mf_final.dat
MF_PLOT=anfis_mf



echo "________________________________________________________________________________"
echo -e "\n\tRedes ANFIS para predicción temporal de series caóticas\n"
echo -e "    Análisis del aprendizaje para la equación diferencial de Mackey-Glass\n"

# Parámetros para la construcción de la eq. dif. de Mackey-Glass
a=118
b=1618
h=0.1
tau=17
yinit=1.2
# Asíntotas horizontales de la eq.dif.
LB=-0.5 # Lower bound
UB=3.0  # Upper bound

START=$(date +%s)
STARTN=$(date +%N)



echo -e "\nGenerando valores muestrales de la serie Mackey-Glass"
make deep_clean &> $LOG
make mg_sample >> $LOG 2>&1
$GEN_MG $a $b $h $tau $yinit > $MG_DATA



echo -e "\nCompilando programa principal"
make anfis_mg >> $LOG 2>&1
# Lo siguiente guarda en $nlines el # de líneas del archivo MG_DATA
nlines=`wc -l $MG_DATA | tr -c -d [0-9]`
echo "Ejecutando programa principal"
$MAIN $T $N $MG_DATA $nlines $MG_ANFIS $LERR_DATA $MF_DATA_INITIAL \
$MF_DATA_FINAL >> $LOG 2>&1



echo -e "Graficando los valores generados vs. la serie original"
set -- $(cat $MG_ANFIS | gawk '
BEGIN {
	# Notar que los valores iniciales son "invertidos"
	first_x_val = 99999999999
	last_x_val = -99999999999
}
{
	if ($1 < first_x_val) {
		first_x_val = $1;
	}
	if ($1 > last_x_val) {
		last_x_val = $1;
	}
}
END {
	print first_x_val;
	print last_x_val;
}
')
export XLOW=$1
export XHIGH=$2
export ORIGINAL=$MG_DATA
export LEARNING=$MG_ANFIS
export PLOT=$MG_PLOT
gnuplot $PLOT_MG
echo -e "\nDatos originales de la serie de Mackey-Glass en $MG_DATA"
echo -e "Datos del aprendizaje de la red en $MG_ANFIS\nGráfico en $MG_PLOT"



echo -e "\nGraficando errores de aprendizaje obtenidos"
set -- $(cat $LERR_DATA | gawk '
BEGIN {
	# Notar que los valores iniciales son "invertidos"
	first_x_val = 99999999999
	last_x_val = -99999999999
}
{
	if ($1 < first_x_val) {
		first_x_val = $1;
	}
	if ($1 > last_x_val) {
		last_x_val = $1;
	}
}
END {
	print first_x_val;
	print last_x_val;
}
')
export XLOW=$1
export XHIGH=$2
export SAMPLE=$LERR_DATA
export PLOT=$LERR_PLOT
gnuplot $PLOT_LERR
echo -e "Datos en $LERR_DATA\nGráfico en $LERR_PLOT"



echo -e "\nGraficando las funciones membresía iniciales (antes del aprendizaje)"
TMP_FILE=THISAINTGONNAMATCH.tmp
# Primero establecemos el supremo y el ínfimo de los datos muestrales
cat $MG_DATA | gawk '
BEGIN {
# Notar que los valores iniciales son "invertidos"
	inf = '$UB'
	sup = '$LB'
}
{
	if ($2 < inf)
		inf = $2
	if ($2 > sup)
		sup = $2
}
END {
	print "sup " sup >  "'$TMP_FILE'"
	print "inf " inf >> "'$TMP_FILE'"
}
'
# Después generamos los scripts de gnuplot para ploteo
cat $MF_DATA_INITIAL | gawk '
BEGIN {
	T = '$T'
	N = '$N'
	plot = "initial_mf_"
	mf[""] = ""
}
{
	# Guardamos las funciones del archivo en nuestro arreglo interno
	if (NF == 5) {
		a = $3 ; b = $4 ; c = $5;
		mf[$1, $2] = "1 / (1 + (abs ((x - " c ") / " a ") ** (2 * " b ")) )"
	} else {
		print "Bad # of fields in file line", FNR, ":", NF
	}
}
END {
	# Recuperamos los valores ínfimo y supremo de los datos muestrales
	for (i=0 ; i<2 ; i++) {
		getline < "'$TMP_FILE'"
		if ($0 ~ /sup/) {
			sup = $2;
		} else if ($0 ~ /inf/) {
			inf = $2;
		}
	}
	# Por cada rama generamos un script de gnuplot para impresión de las MF
	for (i=0 ; i < T ; i++) {
		
		fout = "branch_" i "_initial.gp"
		
		title = "Initial membership functions of branch # " i
		print "set title \"" title "\" font \"giant\""		  > fout
		print "set arrow from " inf ",0 to " inf ",1 nohead lt 0" >>fout
		print "set arrow from " sup ",0 to " sup ",1 nohead lt 0" >>fout
		print "set xrange ['$LB':'$UB']"	   >> fout
		print "set out \"" plot i ".png\""	   >> fout
		print "set term png size 1000, 400 nocrop" >> fout
		
		for (j=0 ; j < N ; j++) {
			print "mf_" j "(x) = " mf[i, j]    >> fout
		}
		
		printf "plot"				   >> fout
		
		for (j=0 ; j < N-1 ; j++) {
			printf "\tmf_%d(x) with lines lt %d lw 2, \\\n",
				j, j+1 >> fout
		}
		printf "\tmf_%d(x) with lines lt %d lw 2\n", N-1, N	 >> fout
	}
}
'
# Por último ejecutamos todos estos scripts de gnuplot para generar los gráficos
for ((i=0 ; i < $T ; i++))
do
	plotter="branch_"$i"_initial.gp"
	gnuplot $plotter && \
	rm $plotter
done



echo "Graficando las funciones membresía resultantes del aprendizaje"
# Generamos los scripts de gnuplot para ploteo, igual que antes
cat $MF_DATA_FINAL | gawk '
BEGIN {
	T = '$T'
	N = '$N'
	plot = "final_mf_"
	mf[""] = ""
}
{
	# Guardamos las funciones del archivo en nuestro arreglo interno
	if (NF == 5) {
		a = $3 ; b = $4 ; c = $5;
		mf[$1, $2] = "1 / (1 + (abs ((x - " c ") / " a ") ** (2 * " b ")) )"
	} else {
		print "Bad # of fields in file line", FNR, ":", NF
	}
}
END {
	# Recuperamos los valores ínfimo y supremo de los datos muestrales
	for (i=0 ; i<2 ; i++) {
		getline < "'$TMP_FILE'"
		if ($0 ~ /sup/) {
			sup = $2;
		} else if ($0 ~ /inf/) {
			inf = $2;
		}
	}
	# Por cada rama generamos un script de gnuplot para impresión de las MF
	for (i=0 ; i < T ; i++) {
		
		fout = "branch_" i "_final.gp"
		
		title = "Final membership functions of branch # " i
		print "set title \"" title "\" font \"giant\""		  > fout
		print "set arrow from " inf ",0 to " inf ",1 nohead lt 0" >>fout
		print "set arrow from " sup ",0 to " sup ",1 nohead lt 0" >>fout
		print "set xrange ['$LB':'$UB']"	   >> fout
		print "set out \"" plot i ".png\""	   >> fout
		print "set term png size 1000, 400 nocrop" >> fout
		
		for (j=0 ; j < N ; j++) {
			print "mf_" j "(x) = " mf[i, j]    >> fout
		}
		
		printf "plot"				   >> fout
		
		for (j=0 ; j < N-1 ; j++) {
			printf "\tmf_%d(x) with lines lt %d lw 2, \\\n",
				j, j+1 >> fout
		}
		printf "\tmf_%d(x) with lines lt %d lw 2\n", N-1, N	 >> fout
	}
}
'
# Por último ejecutamos todos estos scripts de gnuplot para generar los gráficos
for ((i=0 ; i < $T ; i++))
do
	plotter="branch_"$i"_final.gp"
	gnuplot $plotter && \
	rm $plotter
done
rm $TMP_FILE



END=$(date +%s)
ENDN=$(date +%N)
gawk ' BEGIN { 
	start = '$START' + ('$STARTN' / (10.0**9));
	end   = '$END'   + ('$ENDN'   / (10.0**9));
	print "\nTiempo total de ejecución: " end-start " s"
}'
eog ./ &
make clean >> $LOG 2>&1
echo -e "Registro de las actividades en $LOG\nFin del programa\n"

exit 0
