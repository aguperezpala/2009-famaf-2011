LOG=anfis.log
EXE=./anfis_mg
GEN_MG=./Mackey_Glass
PLOT_MG=Mackey_Glass.gp
MG_DATA=Mackey_Glass.dat
MG_PLOT=Mackey_Glass.png

# ____________________________________________________________________________ #
############################## EJERCICIO 1 #####################################

echo "________________________________________________________________________________"
echo "\tRedes ANFIS para predicción temporal de series caóticas\n"
echo "\tAnálisis del aprendizaje para la equación diferencial de Mackey-Glass"

a=118
b=1118
margin=50
h=0.1
tau=17
yinit=1.2

echo "\nGenerando valores muestrales de la serie"
make clean &> $LOG
make mg_sample >> $LOG 2>&1
$GEN_MG $a $b $h $tau $yinit > $MG_DATA

echo "Imprimiendo gráfico con los valores generados"
export XLOW=`expr $a + $margin`
export XHIGH=`expr $b + $margin`
export SAMPLE=$MG_DATA
export PLOT=$MG_PLOT
gnuplot $PLOT_MG













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
