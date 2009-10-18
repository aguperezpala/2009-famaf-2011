# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO

def getEsquemaUniversal():
	return set(['a', 'b', 'c', 'd'])
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
	s = set ([df (set("ab"),set("c")), df(set("ad"),set("b")),df(set("d"),set("a"))])
	
	return s
