# -*- coding: utf-8 -*-

from df import *
from ca import *

def cierreAtributos (f, eu):
	"""Usando el conjunto de d.f. y el esquema universal construimos el
	   cierre de atributos de cada atributo. Lo representaremos con un TAD
	   que es un par ordenado, donde la primer componente ('a') es el
	   atributo considerado, y la segunda ('am') es el cierre del mismo """
	cierreAtr = set ()
	
	""" Como nuestras dependencias funcionales tienen todas un único
	    atributo a la izquierda, tomamos cada atributo del esquema universal
	    y calculamos su cierre, en lugar de considerar cada posible
	    subconjunto de atributos del esquema. """
	
	for atr in eu:
		stop = False
		cierreSetAtr = set([atr])
		while (not stop):
			stop = True
			for d in f:
				if d.alfa.issubset(cierreSetAtr):
					temp = cierreSetAtr | d.beta
					if not (cierreSetAtr == temp):
						stop = False
						cierreSetAtr = temp
						
			"""Tenemos el cierre del conjunto de atributos set(atr).
				Lo guardamos en cierreAtr """
			cierreAtr.add(ca(set([atr]),cierreSetAtr))
		
	return cierreAtr


def cierreAtributosAlfa (alfa,f):
	""" Calcula el cierre del atributo 'alfa' dado un conj. 'f' de d.f."""
	cierreAtr = alfa.copy()
	stop = False
	while (not stop):
		stop = True
		for d in f:
			if d.alfa.issubset(cierreAtr):
				temp = cierreAtr | d.beta
				if cierreAtr != temp:
					stop = False
					cierreAtr |= d.beta
	return cierreAtr


def elimTrivial (cierreAtributos):
	""" Elimina las trivialidades de cada cierre """
	global cierresDic
	cierresDic = {}
	for cierre in cierreAtributos:
		if cierre.am-cierre.a != set([]): # si no es puramente trivial
			cierresDic.setdefault(frozenset(cierre.a.copy()),cierre.am-cierre.a)
		# si era puramente trivial ni siquiera lo agregamos
	return cierresDic


def genDep (cierreAtr):
	""" Con el conjunto de cierres de atributos construimos las d.f. """
	Fprima = set()
	""" Como queremos tener a->b,c en vez de a->b y a->c => no hace falta 
	    obtener todos los subconjuntos del cierre de un atributo. Podemos ver 
	    las tuplas del conjunto cierreAtr como las d.f. del conjunto F'. """
	for cierre in cierreAtr:
		Fprima.add(df(cierre,cierreAtr[cierre]))
	
	printFprima(Fprima)
	
	return Fprima


""" Funciones de impresión """

def printCierre (cierreAtr):
	print "Cierre de atributos:\n{"
	for cierre in cierreAtr:
		print "(" + str(cierre.a) + " , " + str(cierre.am) + "),\n"
	print "}"


def printElimTrivial (cierresDic):
	print "Cierre de atributos sin trivialidades:\n{"
	for cierre in cierresDic:
		print "(" + str(cierre) + " , " + str(cierresDic[cierre]) + "), \n"
	print "}"


def printFprima (Fprima):
	print "F':\n{"
	for dep in Fprima:
		print "(",
		for char in dep.alfa:
			print str(char),
		print " , ",
		for char in dep.beta:
			print str(char),
		print "), \n"
	print "}"

