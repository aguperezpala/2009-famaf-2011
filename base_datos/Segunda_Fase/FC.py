#!/usr/bin/env python
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

def union_partes_izq(R,alfa,F):
	
	""" Fusiona todas las dependencias funcionales que tenga a alfa
		como parte izquierda de la misma """
	
	
	# KONDRA: se queja de que pedimos 2 valores para iterar sobre
	# R-set(alfa) No le veo nada de malo, incluso añadí esta guarda
	# por si las dudas, pero parece que es un error en tiempo de
	# compilación o algo así. Fijate si lo podés solucionar.
	if R.__len__() - set(alfa).__len__() < 2: return
	
	
	for b1,b2 in R - set(alfa):
		if (alfa,b1) in F and (alfa,b2) in F:
			F.remove((alfa,b1))
			F.remove((alfa,b2))
			F.add((alfa,b1|b2))
				 
def atrib_raros_der(alfa,beta,R,F):
	
	""" Elimina y devuelve una lista de los atributos raros de beta """
	  
	raros = []
	for A in beta:
		# Parecera largo pero mas variables solo complica su escritura.
		if A in cierre_atrib(alfa,(F-set((alfa,beta)))|set((alfa,beta-A)),R):
			F.remove((alfa,beta))
			F.add((alfa,beta-set(A)))
			raros += [A]
	return raros
	
def atrib_raros_izq(alfa,beta,R,F):
	
	""" Elimina y devuelve una lista de los atributos raros de alfa """
	
	raros = []
	for A in alfa:
		# Chequeamos beta subconjunto del cierre de atributos de alfa-A
		if beta.issubset(cierre_atrib(alfa-set(A),F,R)):
			F.remove((alfa,beta))
			F.add((alfa-set(A),beta))
			raros += [A]
	return raros
						
def FC(F,R):
	res = copy.deepcopy(F) # Copio F para modificarlo a gusto.
	raros = ["I ALWAYS WANT TO BE A LUMBERJACK"] # Inicialización
	
	while len(raros) > 0 :# Mientras obtengamos atributos raros 
		raros = []
		for alfa in R: # Realizo todas las uniones para cada alfa
			union_partes_izq(R,alfa,res)
			for beta in R: # Agrego los atributos raros de cada beta
				raros += atrib_raros_izq(alfa,beta,R,res)+atrib_raros_der(alfa,beta,R,res)

	return res
