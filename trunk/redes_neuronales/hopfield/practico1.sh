LOG=redes_neuronales_practico1.log

# ____________________________________________________________________________ #
############################## EJERCICIO 1 #####################################

echo -n "________________________________________________________________________________"
echo -e "\nEjercicio 1: Hopfield determinístico\n\nCompilando las fuentes ..."
make clean &> $LOG
make det >> $LOG 2>&1
echo -e "Fuentes compiladas\n"

N=448
P=224
h=2
echo "Ejecutando caso N = $N, Pmax = $P, hop = $h"
./deterministic_Hopfield $N $P $h &> hop_det_n$N\_p$P\_h$h.dat

N=832
P=416
h=4
echo "Ejecutando caso N = $N, Pmax = $P, hop = $h"
./deterministic_Hopfield $N $P $h &> hop_det_n$N\_p$P\_h$h.dat

N=1664
P=832
h=8
echo "Ejecutando caso N = $N, Pmax = $P, hop = $h"
./deterministic_Hopfield $N $P $h &> hop_det_n$N\_p$P\_h$h.dat

echo "Fin del caso determinístico."
echo -e "Salidas en los archivos hop_det_n?_p?_h?.dat\n"


# ____________________________________________________________________________ #
############################## EJERCICIO 2 #####################################

echo -n "________________________________________________________________________________"
echo -e "Ejercicio 2: Hopfield estocástico\n\nCompilando las fuentes ..."
make clean >> $LOG 2>&1
make stoc >> $LOG 2>&1
echo "Fuentes compiladas"
T=1.2
h=0.1
echo "" | awk -v v1=$h -v v2=$T '{ printf ("Se usarán niveles de ruido \
T ∈ { %.1f , %.1f , ... , %.1f }\n\n", v1, v1*2, v2) }'

# ~~~~~~~~~~~~~~~~~~~~~~~ N = 448 ~~~~~~~~~~~~~~~~~~~~~~~~
N=416

P=5
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

P=10
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

P=15
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

# ~~~~~~~~~~~~~~~~~~~~~~~ N = 832 ~~~~~~~~~~~~~~~~~~~~~~~~
N=832

P=10
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

P=20
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

P=30
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

# ~~~~~~~~~~~~~~~~~~~~~~~ N = 1664 ~~~~~~~~~~~~~~~~~~~~~~~
N=1664

P=20
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

P=40
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

P=60
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat


echo "Fin del caso estocástico."
echo "Salidas en los archivos hop_stoc_n?_p?.dat"
make clean >> $LOG 2>&1
echo -e "\nRegistro de las actividades en $LOG\nPráctico 1 finalizado\n\n"
