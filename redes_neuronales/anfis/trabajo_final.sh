#!/bin/bash

LOG=anfis.log

# Generación de muestra
GEN_MG=./Mackey_Glass
PLOT_MG=Mackey_Glass.gp
MG_DATA=Mackey_Glass.dat
MG_PLOT=Mackey_Glass.png

# Aprendizaje ANFIS
MAIN=./anfis_mg
PLOT_LERR=learn_error.gp
LERR_DATA=anfis.dat
LERR_PLOT=anfis.png

echo "________________________________________________________________________________"
echo -e "\n\tRedes ANFIS para predicción temporal de series caóticas\n"
echo -e "    Análisis del aprendizaje para la equación diferencial de Mackey-Glass\n"

a=118
b=1118
margin=50
h=0.1
tau=17
yinit=1.2

echo -e "\nGenerando valores muestrales de la serie"
make clean &> $LOG
make mg_sample >> $LOG 2>&1
$GEN_MG $a $b $h $tau $yinit > $MG_DATA

echo -e "Graficando los valores generados"
export XLOW=`expr $a + $margin`
export XHIGH=`expr $b + $margin`
export SAMPLE=$MG_DATA
export PLOT=$MG_PLOT
gnuplot $PLOT_MG
eog $MG_PLOT &
echo -e "Datos en $MG_DATA\nGráfico en $MG_PLOT"

echo -e "\nCompilando programa principal"
make anfis_mg >> $LOG 2>&1
# Lo siguiente guarda en $nlines el # de líneas del archivo MG_DATA
nlines=`wc -l $MG_DATA | tr -c -d [0-9]`
echo "Ejecutando programa principal"
$MAIN $MG_DATA $nlines $LERR_DATA

echo "Graficando errores de aprendizaje obtenidos"
export SAMPLE=$LERR_DATA
export PLOT=$LERR_PLOT
gnuplot $PLOT_LERR
eog $LERR_PLOT &
echo -e "Datos en $LERR_DATA\nGráfico en $LERR_PLOT"

make clean >> $LOG 2>&1
echo -e "\nRegistro de las actividades en $LOG\nFin del programa\n"

exit
