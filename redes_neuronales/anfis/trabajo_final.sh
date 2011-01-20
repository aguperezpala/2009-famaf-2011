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

echo "________________________________________________________________________________"
echo -e "\n\tRedes ANFIS para predicción temporal de series caóticas\n"
echo -e "    Análisis del aprendizaje para la equación diferencial de Mackey-Glass\n"

a=118
b=1118
h=0.1
tau=17
yinit=1.2

echo -e "\nGenerando valores muestrales de la serie Mackey-Glass"
make clean &> $LOG
make mg_sample >> $LOG 2>&1
$GEN_MG $a $b $h $tau $yinit > $MG_DATA

echo -e "\nCompilando programa principal"
make anfis_mg >> $LOG 2>&1
# Lo siguiente guarda en $nlines el # de líneas del archivo MG_DATA
nlines=`wc -l $MG_DATA | tr -c -d [0-9]`
echo "Ejecutando programa principal"
$MAIN $MG_DATA $nlines $MG_ANFIS $LERR_DATA

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

make clean >> $LOG 2>&1
echo -e "\nRegistro de las actividades en $LOG\nFin del programa\n"

exit
