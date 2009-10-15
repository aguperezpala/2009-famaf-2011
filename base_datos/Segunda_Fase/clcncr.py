# -*- coding: utf-8 -*-
# Si si, una pijasa el nombre, (Claves Candidatas Creator/dor XD)

# Funcion que toma un conjunto de cierres de atributos y es esquema universal
# y devuelve un NUEVO conjunto de claves candidatas.
# Requires:
#	R 	= esquema universal (Set)
#	LCA	= List. de Cierre atributos (Set of tubples => (attr's, cierre))
# Returns:
#	LCC	= List. de Claves Candidatas
def getCCC(R, LCA):
	LCC = []
	for ca in LCA:
		# verificamos si el cierre (ca[1]) es igual a R
		if ca.am <= R <= ca.am:
			#si son iguales => la agregamos al conjunto
			# agregamos una copia por las dudas...
			LCC.append(ca.a.copy())
	return LCC
