# -*- coding: utf-8 -*-
def cierreAtributos (f, eu):
	"""Usando el conjunto de d.f. y el esquema universal construimos el
       cierre de atributos de cada atributo. Lo representaremos como un
       par ordenado donde la primer componente es el atributo y la segunda
       componente es el cierre mismo."""
	global cierreAtr #¡acá tenés tu variable global!...
	
	""" Dadas las observaciones que hicimos sobre nuestras d.f., tomamos
		cada atributo del esquema universal y calculamos su cierre, en vez
		de considerar cada subconjunto de atributos del esquema. """

	for atr in eu:
		setAtr = set(atr)
		cierreSetAtr = setAtr
		for (a,b) in f:
			if a.issubset(cierreSetAtr):
				cierreSetAtr.add(b)
		"""Tenemos el cierre del conjunto de atributos "setAtr". Lo guardamos
       	   en la dichosa variable global "cierreAtr" """
		cierreAtr.add((setAtr,cierreSetAtr))

def elimTrivial (cierreAtr):
	""" Eliminamos las depedencias triviales """
	
	for (a,b) in cierreAtr:
		""" Como no se si el tipo "tupla" de python permite modificación alguna, 
			apelo al recurso de borrar el par (a,b) para luego agregar el
			par (a,b-a).
				Luego, si el cierre de un atributo es un singletón cuyo único
			elemento es el atributo mismo => es uno de los atributos que 
			aparecen del lado derecho en nuestras d.f. => podemos descartar su 
			cierre.  """
		cierreAtr.delete((a,b))
		if b-a != set([]):
			cierreAtr.add((a,b-a))

	return cierreAtr

def genDep (cierreAtr):
	""" Ya se acaba. Con el conjunto de cierres de atributos construimos
		las d.f. """
	global Fprima #¡acá está tu otra variable global!
	""" Como queremos tener a->b,c en vez de a->b y a->c => no hace falta 
		obtener todos los subconjuntos del cierre de un atributo. Podemos ver 
		las tuplas del conjunto cierreAtr como las d.f. del conjunto F'. """
	Fprima = cierreAtr
	
	
				
			
