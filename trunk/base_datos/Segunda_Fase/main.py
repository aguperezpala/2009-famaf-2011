# -*- coding: utf-8 -*-
from attrs import *
from df import *
from clcncr import getCCC
from fprima import *
from c3fn import calculate3FN
from FC import FC
from FNBC import calcular_FNBC



def mainProg():
	# obtenemos el esquema universal
	EU = getEsquemaUniversal()
	# Obtenemos las dep.func.
	depFun = getDepFunc()
	# calculamos el cierre de atributos
	cierreAttrs = cierreAtributos (depFun, EU)
	# calculamos el cierre minimal
	cierreAttrsMin = elimTrivial(cierreAttrs)
	# obtenemos FPrima
	FPrima = genDep (cierreAttrsMin)
	# calculamos FNBC ahora, que nos pide una lista de Ri, creamos una que
	# contenga simplemente EU
	RiList = list()
	RiList.append (EU)
	descFNBC = calcular_FNBC (RiList, FPrima, cierreAttrs)
	
	# Ahora vamos a obtener el conjunto de claves candidatas
	clavesCandidatas = getCCC (EU, cierreAttrs)
	
	# Calculamos F canonica
	FCanonica = FC(depFun,EU)
	
	# Obtenemos 3FN
	RiList2 = list()
	RiList2.append (EU)
	desc3FN = calculate3FN (FCanonica, RiList, clavesCandidatas)
	
	
	# deberiamos hacer algo aca con todos los datos que tenemos...
	

mainProg()
