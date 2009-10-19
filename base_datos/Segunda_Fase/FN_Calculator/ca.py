# -*- coding: utf-8 -*-

from sys import *

class ca:
	a  = set()
	am = set()
	def __hash__(self):
		return 0
	def __repr__(self):
		rep = "(ATRIBUTO: "
		for elem in self.a:
			rep += elem+' '
		rep += ", CIERRE:"
		for elem in self.am:
			rep += ' '+elem
		rep += ')'
		return rep
	def __str__(self):
		rep = "(ATRIBUTO: "
		for elem in self.a:
			rep += elem+' '
		rep += ", CIERRE:"
		for elem in self.am:
			rep += ' '+elem
		rep += ')'
		return rep
	def __cmp__(self,other):
		assert type(other) == type(self)
		#hay que retornar la negación aunque paresca loco
		return (not(self.a == other.a and self.am == other.am))
			
	def __init__(self,x,y):
		assert type(x) == set and type(y) == set 
		self.a = x.copy()
		self.am = y.copy()

