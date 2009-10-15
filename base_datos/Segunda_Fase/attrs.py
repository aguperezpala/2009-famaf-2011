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
	s.add (df(set('AB'),set('E')))
	s.add (df(set('D'),set('C')))
	s.add (df(set('AC'),set('B')))
	return s
