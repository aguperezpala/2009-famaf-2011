# -*- coding: utf-8 -*-
from attrs import *
from df import *
from clcncr import getCCC
from fprima import *
from c3fn import calculate3FN
from FNBC import calcular_FNBC
from FC import *



def mainProg():
	# obtenemos el esquema universal
	EU = getEsquemaUniversal()
	# Obtenemos las dep.func.
	depFun = getDepFunc()
	print "conseguimos depFun"
	# calculamos el cierre de atributos
	cierreAttrs = cierreAtributos (depFun, EU)
	# calculamos el cierre minimal
	cierreAttrsMin = elimTrivial(cierreAttrs)
	print "conseguimos cierreAttrsMin"
	# obtenemos FPrima
	FPrima = genDep (cierreAttrsMin)
	print "conseguimos FPrima"
	# calculamos FNBC ahora, que nos pide una lista de Ri, creamos una que
	# contenga simplemente EU
	RiList = list()
	RiList.append (EU)
	#descFNBC = calcular_FNBC (RiList, FPrima, cierreAttrsMin)
	#print "conseguimos descFNBC"
	# Ahora vamos a obtener el conjunto de claves candidatas
	clavesCandidatas = getCCC (EU, cierreAttrs)
	print "conseguimos clavesCandidatas"
	# Calculamos F canonica
	FCanonica = calcular_FC(cierreAttrsMin,depFun,EU)
	print "conseguimos FCanonica"
	# Obtenemos 3FN
	RiList2 = list()
	RiList2.append (EU)
	desc3FN = calculate3FN (FCanonica, RiList, clavesCandidatas)
	print "conseguimos desc3FN"
	
	# deberiamos hacer algo aca con todos los datos que tenemos...
	

mainProg()
