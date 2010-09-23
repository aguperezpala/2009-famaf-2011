LOG=RedesN_Practico1.log

# ____________________________________________________________________________ #
############################## EJERCICIO 1 #####################################

echo -e "\nEjercicio 1: Hopfield determinístico\nCompilando las fuentes ..."
make clean &> $LOG
make det &>> $LOG
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
echo "Ejecutando caso N = $N, Pmax = $P, hop = $h\n"
./deterministic_Hopfield $N $P $h &> hop_det_n$N\_p$P\_h$h.dat

echo "Fin del caso determinístico."
echo "Salidas en los archivos hop_det_n?_p?_h?.dat"


# ____________________________________________________________________________ #
############################## EJERCICIO 2 #####################################

echo -e "\n\nEjercicio 2: Hopfield estocástico\nCompilando las fuentes ..."
make clean &>> $LOG
make stoc &>> $LOG
echo -e "Fuentes compiladas\n"
T=1.2
h=0.1
echo "" | gawk -v v1=$h -v v2=$T '{ printf ("Se usarán niveles de ruido \
T ∈ { %.1f , %.1f , ... , %.1f }\n", v1, v1*2, v2) }'

# ~~~~~~~~~~~~~~~~~~~~~~~ N = 448 ~~~~~~~~~~~~~~~~~~~~~~~~
N=448
P=20
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

N=448
P=40
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

N=448
P=60
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

# ~~~~~~~~~~~~~~~~~~~~~~~ N = 816 ~~~~~~~~~~~~~~~~~~~~~~~~
N=816
P=20
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

N=816
P=40
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

N=816
P=60
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

# ~~~~~~~~~~~~~~~~~~~~~~~ N = 1664 ~~~~~~~~~~~~~~~~~~~~~~~
N=1664
P=20
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

N=1664
P=40
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat

N=1664
P=60
echo "Ejecutando caso N = $N, P = $P"
./stochastic_Hopfield $N $P $T $h &> hop_stoc_n$N\_p$P.dat


echo "Fin del caso estocástico."
echo "Salidas en los archivos hop_stoc_n?_p?.dat"

echo -e "\nRegistro de las actividades en $LOG\nPráctico 1 finalizado\n\n"
