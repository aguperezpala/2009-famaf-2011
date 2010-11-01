LOG=redes_neuronales_practico2.log
EXE=./diluted_Hopfield

# ____________________________________________________________________________ #
############################## EJERCICIO 1 #####################################

echo "________________________________________________________________________________"
echo -e "Ejercicio 1: Hopfield ultra diluído\n\nCompilando las fuentes ..."
make clean &> $LOG
make dil >> $LOG 2>&1
echo -e "Fuentes compiladas\n"

C=20
Pmax=20
Phop=1


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ N = 2500 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
N=2500
FILE="hop_dil_n$N\_c$C\_p$Pmax\_h$Phop.dat"
echo "Ejecutando caso N = $N, C = $C, Pmax = $Pmax, Phop = $Phop"
$EXE $N $C $Pmax $Phop &> $FILE

echo "Graficando resultados"
export DILUTED_HOPFIELD=$FILE
export NNEURONS=$N
export MEAN_CON=$C
gnuplot single_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ N = 5000 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
N=5000
FILE="hop_dil_n$N\_c$C\_p$Pmax\_h$Phop.dat"
echo "Ejecutando caso N = $N, C = $C, Pmax = $Pmax, Phop = $Phop"
$EXE $N $C $Pmax $Phop &> $FILE

echo "Graficando resultados"
export DILUTED_HOPFIELD=$FILE
export NNEURONS=$N
export MEAN_CON=$C
gnuplot single_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ N = 10000 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
N=10000
FILE="hop_dil_n$N\_c$C\_p$Pmax\_h$Phop.dat"
echo "Ejecutando caso N = $N, C = $C, Pmax = $Pmax, Phop = $Phop"
$EXE $N $C $Pmax $Phop &> $FILE

echo "Graficando resultados"
export DILUTED_HOPFIELD=$FILE
export NNEURONS=$N
export MEAN_CON=$C
gnuplot single_result_plotting.gp


# ~~~~~~~~~~~~~~~~~~~~~~~~~ Superposición ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

# TODO  invocar multiple_result_plotting, previa exportación de las variables
#	ambientales pertinentes


echo "Fin de Hopfield diluído."
echo -e "Tablas en los archivos *.dat\nGráficos en los archivos *.png"

make clean >> $LOG 2>&1
echo -e "\nRegistro de las actividades en $LOG\nPráctico 2 finalizado\n"
