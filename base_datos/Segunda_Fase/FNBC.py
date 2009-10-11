

def calcular_FNBC (conjRi, Fpri):

	"""Cálculo de la forma normal de Boyce Codd de un
	   conjunto de esquemas relacionales, para un F+ dado"""
	
	if (not (type(conjRi) is set)):
		return "1º parameter must be a set of sets!"
	elif (not (type(Fpri) is set)):
		return "2º parameter must be a set of sets!"




def es_violación_FNBC (conjRi, dep):
	