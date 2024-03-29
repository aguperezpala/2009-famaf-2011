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
from fprima import *


def union_partes_izq(d1,F):
	
	""" Fusiona todas las dependencias funcionales que tenga a alfa
		como parte izquierda de la misma """
	
	trash = []
	for d2 in F:
		if d1.alfa == d2.alfa and not (d1 == d2):
			d1.beta |= (d2.beta)
			trash.append (d2)
	
	for d2 in trash:
		F.remove (d2)
	#print "d1: "+str(d1)+'\n'
 
 
def atrib_raros_der(dep,F):

	""" Elimina y devuelve una lista de los atributos raros de beta """
	
	raros = []
	FNew = F.copy()
	tested = set()
	
	i = len(dep.beta)
	while (0<i and 0<len(dep.beta)):
		
		A = (dep.beta-tested).pop() # obtenemos cualquier elemento aún no evaluado
		tested.add(A)
		
		depNew = copy.deepcopy(dep)
		depNew.beta.remove(A)
		
		assert dep in FNew
		FNew.remove(dep)
		FNew.add(depNew)
		
		b = cierreAtributosAlfa(dep.alfa,FNew)
		
		if A in b: # A era atributo raro
			raros += [A]
			if (len(dep.beta)==1):  # la dependencia quedó vacía
				F.remove(dep)
			else:			# actualizamos los cambios
				F.remove(dep)
				F.add(depNew)
				dep.beta.remove(A)
		else:
			# no era raro => no había que tocar ese atributo
			FNew.remove(depNew)
			FNew.add(dep)
		i-=1
		
	return raros


def atrib_raros_izq(df,R,F):
	
	""" Elimina y devuelve una lista de los atributos raros de alfa
	 FIXME: no sabemos si funciona, nunca fue testeada """
	raros = []
	for A in df.alfa:
		# Chequeamos beta subconjunto del cierre de atributos de alfa-A
		if df.beta.issubset(cierreAtributos[df.alfa-set(A)]):
			F.add(df(df.alfa-set(A),df.beta))
			F.remove(df)
			raros += [A]
	return raros


def calcular_FC(F,R):
	res = F.copy() # Copio F para modificarlo a gusto.
	raros = ["I ALWAYS WANT TO BE A LUMBERJACK"] # Inicialización
	
	while len(raros) > 0 :# Mientras obtengamos atributos raros 
		
		# Unimos las partes izquierdas
		
		i = len(res)
		unidas = []
		
		while (i>0 and 0<len(res)):
			dep = res.pop()
			union_partes_izq(dep,res)
			unidas.append(dep)
			i-=1
		
		res = set()
		for dep in unidas:
			res.add(dep)
		
		# Eliminamos los atributos raros
		
		raros = []
		tested = set()
		i = len(res)
		while (i>0):
			assert 0<len(res)
			untested = res-tested
			if len (untested) == 0:
				i=0
			else:
				dep = untested.pop()
				tested.add(dep)
				raros += atrib_raros_der(dep,res)
				i-=1

	return res
