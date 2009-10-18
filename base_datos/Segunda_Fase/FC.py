# -*- coding: utf-8 -*-
#
#       FC.py
#       
#       Copyright 2009 Drasky Vanderhoff <drasky.vanderhoff@gmail.com>
#       
#       This program is free software; you can redistribute it and/or modify
#       it under the terms of the GNU General Public License as published by
#       the Free Software Foundation; either version 2 of the License, or
#       (at your option) any later version.
#       
#       This program is distributed in the hope that it will be useful,
#       but WITHOUT ANY WARRANTY; without even the implied warranty of
#       MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#       GNU General Public License for more details.
#       
#       You should have received a copy of the GNU General Public License
#       along with this program; if not, write to the Free Software
#       Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#       MA 02110-1301, USA.

#	Utilize las operaciones sobre conj' 
#	{|,&,-} == {unión,intersección,diferencia} las cuales solo devuelven
#	un valor pero no modifican a los conjuntos sobre los que operan.

import copy
from df import *
from fprima import elimTrivial

def union_partes_izq(d1,F):
	
	""" Fusiona todas las dependencias funcionales que tenga a alfa
		como parte izquierda de la misma """
	assert d1.__class__ == df and df == F.pop()
	for d2 in F - d1:
		if d1.alfa == d2.alfa:
			F.remove(d1)
			F.remove(d2)
			F.add(df(d1.alfa,d1.beta|d2.beta))
 
def atrib_raros_der(df,R,F):
	
	""" Elimina y devuelve una lista de los atributos raros de beta """
	  
	raros = []
	for A in df.beta:
		# Parecera largo pero mas variables solo complica su escritura.
		if A in cierreAtributos(df.alfa,(F-df)|set((df.alfa,df.beta - A)),R):
			F.add((df.alfa,df.beta - set(A)))
			F.remove(df)
			raros += [A]
	return raros
	
def atrib_raros_izq(df,R,F):
	
	""" Elimina y devuelve una lista de los atributos raros de alfa """
	
	raros = []
	for A in df.alfa:
		# Chequeamos beta subconjunto del cierre de atributos de alfa-A
		if df.beta.issubset(cierreAtributos(df.alfa-set(A),F,R)):
			F.add(df(df.alfa-set(A),df.beta))
			F.remove(df)
			raros += [A]
	return raros
						
def calcular_FC(F,R):
	res = copy.deepcopy(F) # Copio F para modificarlo a gusto.
	raros = ["I ALWAYS WANT TO BE A LUMBERJACK"] # Inicialización
	
	while len(raros) > 0 :# Mientras obtengamos atributos raros 
		raros = []
		for df in F:
			union_partes_izq(df,res)
			raros += atrib_raros_izq(df,R,res)+atrib_raros_der(df,R,res)
	return res
