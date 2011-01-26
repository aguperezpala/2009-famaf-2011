#!/bin/bash

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

# Funciones de membresía tras el aprendizaje
PLOT_MF=final_mf_
MF_DATA=anfis_mf.dat
MF_PLOT=anfis_mf



echo "________________________________________________________________________________"
echo -e "\n\tRedes ANFIS para predicción temporal de series caóticas\n"
echo -e "    Análisis del aprendizaje para la equación diferencial de Mackey-Glass\n"

# Parámetros para la construcción de la ecuac. dif. de Mackey-Glass
a=118
b=1618
h=0.1
tau=17
yinit=1.2
# Parámetros para la red ANFIS
T=2
N=4


echo -e "\nGenerando valores muestrales de la serie Mackey-Glass"
make clean &> $LOG
make mg_sample >> $LOG 2>&1
$GEN_MG $a $b $h $tau $yinit > $MG_DATA


echo -e "\nCompilando programa principal"
make anfis_mg >> $LOG 2>&1
# Lo siguiente guarda en $nlines el # de líneas del archivo MG_DATA
nlines=`wc -l $MG_DATA | tr -c -d [0-9]`
echo "Ejecutando programa principal"
$MAIN $T $N $MG_DATA $nlines $MG_ANFIS $LERR_DATA $MF_DATA >> $LOG 2>&1


echo -e "Graficando los valores generados vs. la serie original"
export XLOW=`expr $nlines / 2`
export XHIGH=$nlines
export ORIGINAL=$MG_DATA
export LEARNING=$MG_ANFIS
export PLOT=$MG_PLOT
gnuplot $PLOT_MG
eog $MG_PLOT &
echo -e "\nDatos originales de la serie de Mackey-Glass en $MG_DATA"
echo -e "Datos del aprendizaje de la red en $MG_ANFIS\nGráfico en $MG_PLOT"


echo -e "\nGraficando errores de aprendizaje obtenidos"
export SAMPLE=$LERR_DATA
export PLOT=$LERR_PLOT
gnuplot $PLOT_LERR
eog $LERR_PLOT &
echo -e "Datos en $LERR_DATA\nGráfico en $LERR_PLOT"


echo -e "\nGraficando las funciones membresía resultantes del aprendizaje"
cat $MF_DATA | gawk '
BEGIN {
	T = '$T'
	N = '$N'
	plot = "'$PLOT_MF'"
	mf[""] = ""
}
{
	# Guardamos las funciones del archivo en nuestro arreglo interno
	
	if (NF == 5) {
		a = $3 ; b = $4 ; c = $5;
		mf[$1, $2] = "1 / (1 + (((x - " c ") / " a ") ** 2 ** " b ") )"
	} else {
		print "Bad # of fields in file line", FNR, ":", NF
	}
}
END {
	# Por cada rama generamos un script de gnuplot para impresión de las MF
	
	for (i=0 ; i < T ; i++) {
		
		fout = "branch_" i ".gp"
		
		print "set title \"Membership functions of branch # " i   > fout
		print "set out \"" plot i ".png\""	   >> fout
		print "set term png size 1000, 400 nocrop" >> fout
		
		for (j=0 ; j < N ; j++) {
			print "mf_" j "(x) = " mf[i, j]    >> fout
		}
		
		printf "plot"				   >> fout
		
		for (j=0 ; j < N-1 ; j++) {
			printf "\tmf_%d(x) with lines lt %d, \\\n", j, j+1 >> fout
		}
		printf "\tmf_%d(x) with lines lt %d\n", N-1, N		 >> fout
	}
}
'
# Por último ejecutamos todos estos scripts de gnuplot para generar los gráficos
for ((i=0 ; i < $T ; i++))
do
	plotter="branch_"$i".gp"
	gnuplot $plotter && \
	rm $plotter
done

make clean >> $LOG 2>&1
echo -e "\nRegistro de las actividades en $LOG\nFin del programa\n"

exit 0
