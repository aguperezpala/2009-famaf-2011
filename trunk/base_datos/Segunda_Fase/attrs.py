# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO
def getEsquemaUniversal():
	return set("ABCDE")
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
<<<<<<< .mine
	s = set()
	for a in "ABCDE":
		for b in "ABCDE":
			s.add(df(set(a),set(b)))
=======
	s = set()
	s.add (df(set('AB'),set('E')))
	s.add (df(set('D'),set('C')))
	s.add (df(set('AC'),set('B')))
>>>>>>> .r124
	return s
