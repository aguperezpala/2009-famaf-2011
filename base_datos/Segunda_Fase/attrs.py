# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO
def getEsquemaUniversal():
	return set("ABCDE")
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
	s = set()
	for a in "ABCDE":
		for b in "ABCDE":
			s.add(df(set(a),set(b)))

	return s
