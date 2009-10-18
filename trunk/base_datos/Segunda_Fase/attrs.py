# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO

def getEsquemaUniversal():
	return set(['a', 'b', 'c', 'd', 'e', 'f'])
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
	s = set ([df (set("a"),set("c")), \
	df(set("a"),set("b")), \
	df(set("d"),set("a")), \
	df(set("a"),set("d")), \
	df(set("c"),set("e")), \
	df(set("a"),set("f")), \
	df(set("f"),set("a"))])
	return s
