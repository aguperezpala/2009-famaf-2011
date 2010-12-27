LOG=anfis.log

# Generación de muestra
GEN_MG=./Mackey_Glass
PLOT_MG=Mackey_Glass.gp
MG_DATA=Mackey_Glass.dat
MG_PLOT=Mackey_Glass.png

# Aprendizaje ANFIS
MAIN=./anfis_mg
LERR_PLOT=anfis.png
LERR_DATA=anfis.dat
PLOT_LERR=learn_error.gp

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
echo -e "Datos en $MG_DATA\tGráfico en $MG_PLOT"

echo -e "\nCompilando programa principal"
make anfis_mg >> $LOG 2>&1
nlines=`wc -l $MG_DATA | tr -c -d [0-9]`
echo "Ejecutando programa principal"
$MAIN $MG_DATA $nlines $LERR_DATA

echo "Graficando errores de aprendizaje obtenidos"
export SAMPLE=$LERR_DATA
export PLOT=$LERR_PLOT
gnuplot $PLOT_LERR
eog $LERR_PLOT &
echo -e "Datos en $LERR_DATA\tGráfico en $LERR_PLOT"

echo "Limpieza final"
make clean >> $LOG 2>&1

exit








echo -e "Ejercicio 1: Hopfield ultra diluído\n\nCompilando las fuentes ..."
make clean &> $LOG
make dil >> $LOG 2>&1
echo -e "Fuentes compiladas\n"

N1=2500
N2=5000
N3=10000
C=20
Pmax=20
Phop=1


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ N = 2500 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE1=hop_dil_n$N1\_c$C\_p$Pmax\_h$Phop.dat
echo "Ejecutando caso N = $N1, C = $C, Pmax = $Pmax, Phop = $Phop"
$EXE $N1 $C $Pmax $Phop &> $FILE1

echo "Graficando resultados"
export DILUTED_HOPFIELD=$FILE1
export NNEURONS=$N1
export MEAN_CON=$C
gnuplot single_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ N = 5000 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FILE2=hop_dil_n$N2\_c$C\_p$Pmax\_h$Phop.dat
echo "Ejecutando caso N = $N2, C = $C, Pmax = $Pmax, Phop = $Phop"
$EXE $N2 $C $Pmax $Phop &> $FILE2

echo "Graficando resultados"
export DILUTED_HOPFIELD=$FILE2
export NNEURONS=$N2
export MEAN_CON=$C
gnuplot single_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ N = 10000 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
FILE3=hop_dil_n$N3\_c$C\_p$Pmax\_h$Phop.dat
echo "Ejecutando caso N = $N3, C = $C, Pmax = $Pmax, Phop = $Phop"
$EXE $N3 $C $Pmax $Phop &> $FILE3

echo "Graficando resultados"
export DILUTED_HOPFIELD=$FILE3
export NNEURONS=$N3
export MEAN_CON=$C
gnuplot single_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~ Superposición ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

echo -e "\nFin sección de cálculo."
echo "Graficando superposición de resultados obtenidos"

export DILUTED_HOPFIELD1=$FILE1
export DILUTED_HOPFIELD2=$FILE2
export DILUTED_HOPFIELD3=$FILE3

export NNEURONS1=$N1
export NNEURONS2=$N2
export NNEURONS3=$N3

export MEAN_CON=$C

gnuplot multiple_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~ Shut down ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

echo -e "\nFin de Hopfield diluído."
echo -e "Tablas en los archivos *.dat\nGráficos en los archivos *.png"

make clean >> $LOG 2>&1
echo -e "Registro de las actividades en $LOG\n\nPráctico 2 finalizado\n"
