# -*- coding: utf-8 -*-

def calcular_FNBC (conjRi, Fpri):

	"""Descomposición en la forma normal de Boyce-Codd de un
	   conjunto de esquemas relacionales, para un F+ dado"""
	
	if (not (type(conjRi) is set)):
		return "El 1º parametro debe ser un 'set' de 'sets'"
	elif (not (type(Fpri) is set)):
		return "El 2º parametro debe ser un 'set' de 'tuplas'"
	
	# ¿Revisamos los tipos del contenido de conjRi y Fpri?
	
	# conjRi es modificable => se pasa por referencia => lo
	# modificaremos hasta dejarlo en FNBC
	
	stop = False
	F = Fpri.copy()
	while (not stop):
		stop = True
		
		for dep in F:	
			Ri = es_violac_FNBC (conjRi, dep)
			if Ri is not None: # Si hay violación en algun Ri
				stop = False
				convertir_FNBC (conjRi, Ri, dep)
			else:
				F = F.remove(dep) # ya usamos esta dependencia
		
	print "conjRi está ahora en FNBC"




def es_violac_FNBC (conjRi, dep):
	
	""" Indica si dep es violación de la descomposición conjRi.
	    Si así es devuelve el Ri violado. Sino regresa None """
	
	for Ri in conjRi:
		if (viola_FNBC (Ri,dep)):
			return Ri
	# si no encontramos violación se retorna 'None'


def viola_FNBC (Ri, dep):
	
	""" Informa si la d.f. dep es violación FNBC para el esquema Ri """
	
	if (dep[1].issubset(dep[0])):
		# dependencia trivial => no hay violación FNBC
		return False
	
	for t in 'CIERRE_ATRIBUTOS':
		# revisamos si la parte izquierda de la dep es superclave
		if (t[0].issubset(dep[0]) and t[0].issuperset(dep[0]) and
		    Ri.issubset(t[1]) ):
			return False # es superclave => no hay violación FNBC
	
	# si llegamos acá la dep no era trivial, ni superclave de Ri
	# entonces dep es violación FNBC para Ri
	return True




def convertir_FNBC (conjRi, Ri, dep):
	
	""" Descompone Ri según la dependencia dep
	    para que deje de haber violación FNBC """
	
	#assert viola_FNBC (Ri, dep)
	conjRi.remove(Ri)
	
	Rj = dep[0].union(dep[1]) # atributos en la dependencia ({a,b})
	
	Ri = Ri.difference(dep[1]) # quitamos la parte derecha (Ri - {b})
	
	conjRi.add(Ri)
	conjRi.add(Rj)


