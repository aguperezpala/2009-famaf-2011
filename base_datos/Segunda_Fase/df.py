#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#       df.py
#       
#       Copyright 2009 Drasky Vanderhoff <drasky@drasky-laptop>
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
from sys import *

class df:
	alfa = set()
	beta = set()
	def __hash__(self):
		return 0
	def __repr__(self):
		return str(self.alfa)+","+str(self.beta)
	def __str__(self):
		return str(self.alfa)+","+str(self.beta)
# Kondra: toda instancia 'dep' de 'df' hace que type(dep) == instance
#	def __cmp__(self,otro):
#		assert type(otro) == df
#		return self.alfa ==	otro.alfa and self.beta == otro.beta
	def __init__(self,x,y):
		assert type(x) == set and type(y) == set 
		self.alfa = x.copy()
		self.beta = y.copy()
	def trans (self,df2):
		assert type(df2) == df
		if df2.alfa == self.beta:
			return df(self.alfa,df2.beta)
	def asoc (self,atrib):
		assert type(atrib) == set
		return df (self.alfa|atrib,self.beta|atrib)
	def alfa (self):
		return self.alfa
	def beta (self):
		return self.beta






""" DEBUG 
d1 = df(set("a"), set("abc"))
d2 = df(set("d"), set("a"))

print d2.beta.issubset(d1.beta)

s = set ()
s.add(d1)
s.add(d2)

print s		
"""