

def calcular_FNBC (conjRi, Fpri):

	"""Descomposición en la forma normal de Boyce-Codd de un
	   conjunto de esquemas relacionales, para un F+ dado"""
	
	if (not (type(conjRi) is set)):
		return "El 1º parametro debe ser un 'set' de 'sets'"
	elif (not (type(Fpri) is set)):
		return "El 2º parametro debe ser un 'set' de 'sets'"
	
	Ri = conjRi.pop
	
	if (not (type(Ri) is set)):
		return "El 1º set debe tener sets dentro suyo"
	else:
		conjRi.add(Ri)
	
	# conjRi es modificable => se pasa por referencia => lo
	# modificaremos hasta dejarlo en FNBC
	
	stop = False
	while (not stop):
		stop = True
		
		for dep in Fpri:	
			Ri,depv = es_violación_FNBC (conjRi, dep)
			if Ri is not None: # Si hay violación en algún Ri
				stop = False
				convertir_FNBC (conjRi, Ri, depv)



def es_violación_FNBC (conjRi, dep):
	