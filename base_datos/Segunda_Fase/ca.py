# -*- coding: utf-8 -*-

from sys import *

class ca:
	a  = set()
	am = set()
	def __hash__(self):
		return 0
	def __repr__(self):
		return str(self.a)+"->"+str(self.am)
	def __str__(self):
		return str(self.a)+"->"+str(self.am)
	def __cmp__(self,other):
		assert type(other) == type(self)
		#hay que retornar la negación aunque paresca loco
		return (not(self.a == other.a and self.am == other.am))
			
	def __init__(self,x,y):
		assert type(x) == set and type(y) == set 
		self.a = x.copy()
		self.am = y.copy()
	def trans (self,df2):
		assert type(df2) == type(self)
		if df2.a == self.am:
			return df(self.a,df2.am)
	def asoc (self,atrib):
		assert type(atrib) == set
		return df (self.a|atrib,self.am|atrib)

