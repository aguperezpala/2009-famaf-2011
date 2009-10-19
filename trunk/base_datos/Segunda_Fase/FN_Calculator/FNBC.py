# -*- coding: utf-8 -*-
from fprima import cierreAtributosAlfa
import copy

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
	elif (not (type(cierreAtr) is dict)):
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
				print "\nSe halló que dep viola un Ri..."
				print "dep = "+str(dep)+'\n'
				convertir_FNBC (FNBC, Ri, dep)
	
	return FNBC



def es_violac_FNBC (conjRi, dep, cierreAtr):
	
	""" Indica si dep es violación de la descomposición conjRi.
	    Si así es devuelve el Ri violado. Sino regresa None """
	
	for Ri in conjRi:
		if (viola_FNBC (Ri, dep, cierreAtr)):
			"""  DEBUG
			print ("Violación encontrada\n")
			print ("Ri: ") + str(Ri) + '\n'
			print ("dep: ") + str(dep.alfa) + "->" + str(dep.beta) + '\n' """
			return Ri
	# si no encontramos violación se retorna 'None'


def viola_FNBC (Ri, dep, cierreAtr):
	
	""" Informa si la d.f. dep es violación FNBC para el esquema Ri """
	if (not dep.alfa.issubset(Ri) or not dep.beta.issubset(Ri)):
		return False
	
	if (dep.beta.issubset(dep.alfa)):
		# dependencia trivial => no hay violación FNBC
		return False
	
	for atr in cierreAtr:
		# revisamos si la parte izquierda de la dep es superclave
		if (dep.alfa == atr and Ri.issubset(cierreAtr[atr].union(atr)) ):
			return False # es superclave => no hay violación FNBC
	
	# si llegamos acá la dep no era trivial, ni superclave de Ri
	# entonces dep es violación FNBC para Ri
	return True




def convertir_FNBC (conjRi, Ri, dep):
	
	""" Descompone Ri según la dependencia dep
	    para que deje de haber violación FNBC """
	
	print "Descomponiendo Ri: " + str (conjRi) + '\n'
	
	conjRi.remove(Ri)
	
	Rj = dep.alfa.union(dep.beta) #  {a} U {b}
	
	Ri = Ri.difference(dep.beta) # Ri - {b}
	
	print "Descomposición obtenida:"
	print "\tRj1 = "+str(Rj)+'\n\tRj2 = '+str(Ri)+'\n'
	
	conjRi.append(Ri)
	conjRi.append(Rj)
	print "\n"+str(conjRi)

def chequear_FNBC (fPrima, conjRi, cierreAtr):
	""" Determina si la descomposición conjRi respeta FNBC, según fPrima """
	
	for dep in fPrima:
		#Calculamos el cierre de atributos de dep.alfa
		"""cierre = cierreAtributosAlfa (dep.alfa,fPrima)
		print "\nD.f.: "+str(dep.alfa)+"->"+str(dep.beta)
		print "\nCierre: "+str(cierre)"""
		ri = es_violac_FNBC (conjRi, dep, cierreAtr)
		if ri is not None:
			#print "\nEl esquema "+str(ri)+" no respeta FNBC y "+str(dep.alfa)+"->"+str(dep.beta)+" es un testigo..."
			return False # ri es una violación FNBC

	# Si llegamos hasta acá, conjRi respeta FNBC
	return True

def chequear_FNBC_df(F,conjR):
	""" Determina si la descomposición conjR preserva las df de F """
	
	for df in F: # Chequeamos que la descompoción conserve cada df
		res = copy.deepcopy(df.alfa)
		c = len(res) - 1
		print "beta =" , df.beta
		while c < len(res): 
			print "Res = " , res
			c = len(res)
			for Ri in conjR: 
				cierre = cierreAtributosAlfa(res.intersection(Ri),F)
				res.update(cierre.intersection(Ri))
				print "cierre = " , cierre
				print "Res = " , res
		if  not df.beta.issubset(res):# beta no esta en res
			return False
	# Todos los beta estaban contenidos en res
	return True				
			
