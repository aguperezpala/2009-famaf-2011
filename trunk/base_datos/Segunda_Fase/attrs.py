# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO
def getEsquemaUniversal():
	return set("ABC")
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
	s = set(df(set('A'),set('')))
	s.add ()
	return set(
		df(set(),set())
		)