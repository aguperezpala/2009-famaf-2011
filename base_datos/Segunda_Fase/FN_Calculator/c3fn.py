# -*- coding: utf-8 -*-
from df import *
import copy
#Vamos a hacer la parte primero del foreach

#Esta funcion va a chequear si algun Ri contiene a la dep. func. a->b
#LRi = Conjuntos de Ri (sets of sets)
#dep = Dependencia funcional a->b (es un tubple (a,b))
# Returns:
#	True 	si encontramos
#	False 	caso contrario
def isDepInRi(LRi, dep):
	cdep = dep.alfa | dep.beta # hacemos una union de a U b = {a,b}
	# ahora vamos a buscar si encontramos en alguno de los Ri cdep
	for c in LRi:
		if cdep <= c:
			# si lo encontramos salimos
			return True
	return False	

# Esta es la la funcion que corresponderia al ciclo foreach
# Requires:
#	Fc (F canonico = set of tuples, donde cada tuple es (set,set) == (a,b)
#	que corresponden al a->b)
#	LRi = Set of Set (Conjunto de Ri's)
# Returns: "nada..."
# En caso de que no exista => agrega automaticamente un Ri al conjunto LRi
def firstLoop (Fc, LRi):
	for depF in Fc:
		if not isDepInRi(LRi, depF):
			#tenemos que agregar el nuevo Ri = {a,b}
			LRi.append(depF.alfa | depF.beta)


# Ahora vamos a la 2º parte, vamos a verificar si existe Ri con clave Candidata
# Una simple comparacion de inclusion.


# Funcion que hace esta comparacion (Asegura la reunion sin perdida).
# Requires:
#	LRi = conjunto de Ri's
#	LCC = Lista de Claves Candidatas :) 
# Retuns:
#	True	si se agrego alguna nueva Ri
#	False	caso contrario (ya existia Clave Ca. € Ri)
# Esta funcion modifica el LRi en caso de que tengamos que generar una nueva Ri
# (osea que clave_candidata(R) !€ algun Ri para todo Ri)
# clavale un nombre a la funcion XD
def reunionWithoutLoss(LRi, LCC):
	for cc in LCC:
		for ri in LRi:
			if cc <= ri:
				# No hay que hacer naranja
				return False
	# si estamos aca es porque tenemos que generar un nuevo Ri, con
	# cualquier Clave Candidata ==> Ri+1 = CCC[0]....
	# Aca podriamos fijarnos si influye en algo tomar cualquiera, la mas
	# chica, la mas grande, etc... Para optimizar en algo..?
	LRi.append(LCC[0])
	return True


################################################################################
#				Algoritmos principal
################################################################################
# Funcion que va a calcular 3FN.
# Requires:
#	Fc 	= Fcanonico
#	LRi 	= Conjunto de Ri inicial (el conj vacío la mayoría de las veces)
#	LCC 	= Lista Claves Candidatas
# Returns:
#	LRi's (Descomposicion 3FN)
# TENER EN CUENTA QUE SE MODIFICAN LOS VALORES, NO SE TRABAJA SOBRE COPIA DE LOS
# MISMOS. (SI NO SE QUIEREN MODIFICAR => trabajar conFc.copy(), LRi.copy()...
def calculate3FN (Fc, LRi, LCC):
	firstLoop(Fc, LRi)
	reunionWithoutLoss(LRi, LCC)
	return LRi
	
	
	
