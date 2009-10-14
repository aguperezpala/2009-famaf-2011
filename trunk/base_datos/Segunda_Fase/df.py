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

class df:
	alfa = set()
	beta = set()
	def __init__(self,x,y):
		assert type(x) == set and type(y) == set 
		alfa.union(x)
		beta.union(y)
	def trans (self,df2):
		assert type(df2) == df
		if df2.alfa == self.beta:
			return df(self.alfa,df2.beta)
	def asoc (self,atrib):
		assert type(atrib) == set
		return df (self.alfa|atrib,self.beta|atrib)
		
		
