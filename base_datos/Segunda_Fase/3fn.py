# -*- coding: utf-8 -*-
#Vamos a hacer la parte primero del foreach

#Esta funcion va a chequear si algun Ri contiene a la dep. func. a->b
#CRi = Conjuntos de Ri (sets of sets)
#dep = Dependencia funcional a->b (es un tubple (a,b))
# Returns:
#	True 	si encontramos
#	False 	caso contrario
def isDepInRi(CRi, dep):
	found = False
	cdep = dep[0] | dep[1] # hacemos una union de a U b = {a,b}
	# ahora vamos a buscar si encontramos en alguno de los Ri cdep
	for c in CRi:
		if cdep <= c:
			# si lo encontramos salimos
			found = True
			break
	return found



# Esta es la la funcion que corresponderia al ciclo foreach
# Requires:
#	Fc (F canonico = set of tuples, donde cada tuple es (set,set) == (a,b)
#	que corresponden al a->b)
#	CRi = Set of Set (Conjunto de Ri's)
# Returns: "nada..."
# En caso de que no exista => agrega automaticamente un Ri al conjunto CRi
def firstLoop (Fc, CRi):
	for depF in Fc:
		if isDepInRi(CRi, depF) == False:
			#tenemos que agregar el nuevo Ri = {a,b}
			CRi.add(depF[0] | depF[1])


# Ahora vamos a la 2º parte, vamos a verificar si existe Ri con clave Candidata
# Una simple comparacion de inclusion.


# Funcion que hace esta comparacion (Asegura la reunion sin perdida).
# Requires:
#	CRi = conjunto de Ri's
#	CCC = Conjunto de Claves Candidatas :) 
# Retuns:
#	True	si se agrego alguna nueva Ri
#	False	caso contrario (ya existia Clave Ca. € Ri)
# Esta funcion modifica el CRi en caso de que tengamos que generar una nueva Ri
# (osea que clave_candidata(R) !€ algun Ri para todo Ri)
# clavale un nombre a la funcion XD
def reunionWithoutLoss(CRi, CCC):
	for cc in CCC:
		for ri in CRi:
			if cc <= ri:
				# No hay que hacer naranja
				return False
	# si estamos aca es porque tenemos que generar un nuevo Ri, con
	# cualquier Clave Candidata ==> Ri+1 = CCC[0]....
	# Aca podriamos fijarnos si influye en algo tomar cualquiera, la mas
	# chica, la mas grande, etc... Para optimizar en algo..?
	CRi.add(CCC[0])
	return True


################################################################################
#				Algoritmos principal
################################################################################
# Funcion que va a calcular 3FN.
# Requires:
#	Fc 	= Fcanonico
#	CRi 	= Conjunto de Ri inicial (R universal....)
#	CCC 	= Conjunto Claves Candidatas
# Returns:
#	CRi's (Descomposicion 3FN)
# TENER EN CUENTA QUE SE MODIFICAN LOS VALORES, NO SE TRABAJA SOBRE COPIA DE LOS
# MISMOS. (SI NO SE QUIEREN MODIFICAR => trabajar conFc.copy(), CRi.copy()...
def calculate3FN (Fc, CRi, CCC):
	firstLoop(Fc, CRi)
	reunionWithoutLoss(CRi, CCC)
	return CRi
	
	
	
