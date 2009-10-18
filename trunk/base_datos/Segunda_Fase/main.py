# -*- coding: utf-8 -*-
from attrs import *
from df import *
from clcncr import getCCC
from fprima import *
from c3fn import calculate3FN
from FNBC import calcular_FNBC
from FC import *



def mainProg():
	print "\nObteniendo el Esquema Uiversal"
	EU = getEsquemaUniversal()
	print "Esquema Universal obtenido\n"
	
	print "Obteniendo conjunto de depFunc inicial"
	depFun = getDepFunc()
	print "Conjunto depFun obtenido\n"
	
	print "Calculando cierre de atributos"
	cierreAttrs = cierreAtributos (depFun, EU)
	print "Cierre de atributos calculado\n"
	
	print "Eliminando dependencias triviales del cierre de atributos"
	cierreAttrsMin = elimTrivial(cierreAttrs)
	print "Dependencias triviales eliminadas\n"
	
	print "Calculando F Canonica"
	FCanonica = calcular_FC(depFun,EU)
	print "Conseguimos F Canonica\n"
	
	print "Calculando F+ sin las dependencias triviales (F prima)"
	FPrima = genDep (cierreAttrsMin)
	print "Conseguimos FPrima\n"
	
	# calculamos FNBC ahora, que nos pide una lista de Ri, creamos una que
	# contenga simplemente EU
	RiList = list()
	RiList.append (EU)
	print "Calculando FNBC del esquema universal"
	descFNBC = calcular_FNBC (RiList, FPrima, cierreAttrsMin)
	print "Conseguimos FNBC\n"
	
	print "Calcualndo claves candidatas"
	clavesCandidatas = getCCC (EU, cierreAttrs)
	print "Conseguimos clavesCandidatas\n"
	
	# Obtenemos 3FN
	RiList2 = list()
	RiList2.append (EU)
	print "Calculando 3FN del esquema universal"
	desc3FN = calculate3FN (FCanonica, RiList2, clavesCandidatas)
	print "Conseguimos 3FN\n"

mainProg()
