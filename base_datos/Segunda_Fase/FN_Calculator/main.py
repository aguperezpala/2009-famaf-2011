# -*- coding: utf-8 -*-
from attrs import *
from df import *
from clcncr import getCCC
from fprima import *
from c3fn import calculate3FN
from FNBC import calcular_FNBC, chequear_FNBC , chequear_FNBC_df
from FC import *



def mainProg():
	# Para saber si ya se ha calculado la descomposición en FNBC
	calcFNBC = False
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
	
	print "Calculando F+ sin las dependencias triviales (F prima)"
	FPrima = genDep (cierreAttrsMin)
	print "Conseguimos FPrima\n"
	
	print "Calculando claves candidatas"
	clavesCandidatas = getCCC (EU, cierreAttrs)
	print "Conseguimos clavesCandidatas\n"
	
	
	print "Calculando F Canonica"
	FCanonica = calcular_FC(depFun,EU)
	print "Conseguimos F Canonica\n"
	
	
	op = ""
	while not (op == "0"):
		print "\n\nSeleccione una opcion\n"
		print "\t1) Cierre de atributos sin trivialidades"
		print "\t2) F Canonica"
		print "\t3) F Prima"
		print "\t4) Claves Canidatas"
		print "\t5) Descomposicion FNBC"
		print "\t6) Descomposicion 3FN"
		print "\t7) Chequear que la descomposición FNBC realmente",
		print "respeta\n\t   la Forma Normal de Boyce-Codd"
		print "\t8) Chequear que la descomposición FNBC preserva"
		print "\t   las dependencias funcionales"
		print "\t0) Para salir"
		op = raw_input()
		if op == "1":
			print str(cierreAttrsMin),
		elif op == "2":
			print str (FCanonica)
		elif op == "3":
			print str (FPrima)
		elif op == "4":
			print str (clavesCandidatas)
		elif op == "5":
			# calculamos FNBC ahora, que nos pide una lista de Ri, creamos una que 
			# contenga simplemente EU
			RiList = list() 
			RiList.append (EU) 
			print "Calculando FNBC del esquema universal" 
			descFNBC = calcular_FNBC (RiList, FPrima, cierreAttrsMin)
			calcFNBC = True 
			#print str (descFNBC) 
			print "\nConseguimos FNBC\n"
		elif op == "6":
			print "Calculando 3FN del esquema universal"
			desc3FN = calculate3FN (FCanonica, [], clavesCandidatas)
			print str (desc3FN)
			print "\nConseguimos 3FN\n"
		elif op == "7":
			if calcFNBC == True:
				if chequear_FNBC (FPrima, descFNBC, cierreAttrsMin):
					print "\nRealmente está en FNBC"
				else:
					print "\n¡¡¡Todo como el chori!!!"
			else:
				print "\nTodavía no se ha calculado la descomposición en FNBC"
		elif op == "8":
			if calcFNBC == True:
				if chequear_FNBC_df (depFun, descFNBC):
					print "\nLas dependencias son preservadas"
				else:
					print "\nNo se preservaron las dependencias"
			else:
				print "\nTodavía no se ha calculado la descomposición en FNBC"		
		elif op == "0":
			print "\nChau chau\n"
		else:
			print "Opcion incorrecta. Por favor señor, ",
			print "aprenda a teclear antes de pedirnos algo"
	
	
mainProg()
