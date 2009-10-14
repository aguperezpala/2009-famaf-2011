# -*- coding: utf-8 -*-

def calcular_FNBC (conjRi, Fpri, cierreAtr):

	"""Descomposición en la forma normal de Boyce-Codd de un
	   conjunto de esquemas relacionales, para un F+ dado.
	   1º parámetro => conjunto de esquemas relacionales
	   2º parámetro => cierre del conj de dependencias funcionales
	   3º parametro => conj de cierres de los atributos"""
	
	if (not (type(conjRi) is list)):
		return "El 1º parametro debe ser una 'list' de 'sets'"
	elif (not (type(Fpri) is set)):
		return "El 2º parametro debe ser un 'set' de 'df'"
	elif (not (type(cierreAtr) is set)):
		return "El 3º parametro debe ser un 'set' de 'tuplas'"
	
	# ¿Revisamos los tipos del contenido de conjRi y Fpri?
	
	stop = False
	# copiamos los parámetros para modificarlos a gusto
	F = Fpri.copy()
	FNBC = []
	FNBC.extend(conjRi)
	
	while (not stop):
		stop = True
		for dep in F:	
			Ri = es_violac_FNBC (FNBC, dep, cierreAtr)
			if Ri is not None: # Si hay violación en algun Ri
				stop = False
				convertir_FNBC (FNBC, Ri, dep)
			else:
				F = F.remove(dep) # ya usamos esta dependencia
	return FNBC




def es_violac_FNBC (conjRi, dep, cierreAtr):
	
	""" Indica si dep es violación de la descomposición conjRi.
	    Si así es devuelve el Ri violado. Sino regresa None """
	
	for Ri in conjRi:
		if (viola_FNBC (Ri, dep, cierreAtr)):
			return Ri
	# si no encontramos violación se retorna 'None'


def viola_FNBC (Ri, dep, cierreAtr):
	
	""" Informa si la d.f. dep es violación FNBC para el esquema Ri """
	
	if (dep.beta.issubset(dep.alfa)):
		# dependencia trivial => no hay violación FNBC
		return False
	
	for atr in cierreAtr:
		# revisamos si la parte izquierda de la dep es superclave
		if (dep.alfa == atr[0] and Ri.issubset(atr[1]) ):
			return False # es superclave => no hay violación FNBC
	
	# si llegamos acá la dep no era trivial, ni superclave de Ri
	# entonces dep es violación FNBC para Ri
	return True




def convertir_FNBC (conjRi, Ri, dep):
	
	""" Descompone Ri según la dependencia dep
	    para que deje de haber violación FNBC """
	
	#assert viola_FNBC (Ri, dep)
	conjRi.remove(Ri)
	
	Rj = dep.alfa.union(dep.beta) # atributos en la dependencia ({a,b})
	
	Ri = Ri.difference(dep.beta) # quitamos la parte derecha (Ri - {b})
	
	conjRi.add(Ri)
	conjRi.add(Rj)


