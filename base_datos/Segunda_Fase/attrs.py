# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO

def getEsquemaUniversal():
	return set(['Planilla.Numero', 'Planilla.Fecha', 'Encuestado.Edad', \
	'Encuestado.sexo', 'Encuestado.Ingreso_Mensual', \
	'Encuestado.Profesion', 'Encuestado.Instruccion', \
	'Encuestado.No_C.I.', 'Encuestado.Nombre_y_Apellido', \
	'Jefe_de_Grupo_Familiar.Fecha_de_nacimiento', \
	'Jefe_de_Grupo_Familiar.Forma_de_Cobro', \
	'Jefe_de_Grupo_Familiar.¿Trabaja_actualmente?', \
	'Jefe_de_Grupo_Familiar.Dedicacion_actual', \
	'Jefe_de_Grupo_Familiar.E-mail', \
	'Jefe_de_Grupo_Familiar.Telefono.Celular', \
	'Jefe_de_Grupo_Familiar.Telefono.Habitacion', \
	'Jefe_de_Grupo_Familiar.Telefono.Oficina', \
	'Situacion_comunidad.Ventajas', \
	'Situacion_comunidad.Desventajas', \
	'Comunidad.Nombre', \
	'Comunidad.Direccion.Nro', 'Comunidad.Direccion.Calle', \
	'Sector.CP', \
	'Municipio.Nombre', \
	'Parroquia.Nombre', 'Estado.Nombre', \
	'Situacion_Laboral.Tarjeta', \
	'Situacion_Laboral.Ticket', \
	'Situacion_Laboral.Cta_banco', \
	'Situacion_Laboral.Ingreso_familiar', \
	'Situacion_Laboral.Comercio_en_casa', \
	'Situacion_Laboral.Trabajo', 'Vivienda.Mascotas', \
	'Vivienda.Plagas', \
	'Vivienda.Cond_salubridad', 'Vivienda.Enseres', \
	'Vivienda.Techo', \
	'Vivienda.Paredes', 'Vivienda.Terreno_propio', \
	'Vivienda.Forma_tenencia', \
	'Vivienda.Tipo', \
	'Vivienda.OCV', 'Servicios.Servicios_comunales', \
	'Servicios.Recoleccion_basura', \
	'Servicios.Transporte', 'Servicios.Medios', \
	'Servicios.Telefonia', \
	'Servicios.Electricidad', 'Servicios.Gas', \
	'Servicios.Aguas_servidas', \
	'Servicios.Aguas_blancas', 'Salud.Historia_familiar', \
	'Salud.Ayuda_especial', \
	'Situacion_de_Exclusion.Tercera_edad', \
	'Situacion_de_Exclusion.Discapacitados', \
	'Situacion_de_Exclusion.Enfermedades_terminarles', \
	'Situacion_de_Exclusion.Indigentes', \
	'Situacion_de_Exclusion.Niños_calle', \
	'Participacion_Comunitaria.Propia', \
	'Participacion_Comunitaria.Familia', \
	'Participacion_Comunitaria.Org_Comunitarias', \
	'Participacion_Comunitaria.Administracion', \
	'Participacion_Comunitaria.Constitucion', \
	'Participacion_Comunitaria.Sabe', \
	'Participacion_Comunitaria.Apoya', \
	'Participacion_Comunitaria.Área', \
	'Participacion_Comunitaria.Misiones'])
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
	s = set ([df(set(['Planilla.Numero']),set(['Situacion_Laboral.Trabajo','Situacion_Laboral.Comercio_en_casa','Situacion_Laboral.Ingreso_familiar','Situacion_Laboral.Cta_banco','Situacion_Laboral.Tarjeta','Situacion_Laboral.Ticket'])),
		  df(set(['Planilla.Numero']),set(['Vivienda.Tipo','Vivienda.Forma_tenencia','Vivienda.Terreno_propio','Vivienda.OCV','Vivienda.Techo','Vivienda.Paredes','Vivienda.Enseres','Vivienda.Cond_salubridad','Vivienda.Plagas','Vivienda.Mascotas'])), \
	  df(set(['Planilla.Numero']),set(['Servicios.Aguas_blancas','Servicios.Aguas_servidas','Servicios.Gas','Servicios.Electricidad','Servicios.Recoleccion_basura','Servicios.Telefonia','Servicios.Transporte','Servicios.Medios','Servicios.Servicios_comunales'])), \
	  df(set(['Planilla.Numero']),set(['Participacion_Comunitaria.Org_Comunitarias','Participacion_Comunitaria.Administracion','Participacion_Comunitaria.Constitucion','Participacion_Comunitaria.Propia','Participacion_Comunitaria.Familia','Participacion_Comunitaria.Misiones','Participacion_Comunitaria.Sabe','Participacion_Comunitaria.Apoya','Participacion_Comunitaria.Área'])), \
	  df(set(['Planilla.Numero']),set(['Salud.Ayuda_especial','Salud.Historia_familiar'])), \
	  df(set(['Planilla.Numero']),set(['Situacion_de_Exclusion.Niños_calle','Situacion_de_Exclusion.Indigentes','Situacion_de_Exclusion.Tercera_edad','Situacion_de_Exclusion.Discapacitados','Situacion_de_Exclusion.Enfermedades_terminales'])), \
	  df(set(['Encuestado.No_C.I.']),set(['Encuestado.Nombre_y_apellido','Encuestado.Edad','Encuestado.Sexo','Encuestado.Ingreso_mensual','Encuestado.Profesion','Encuestado.Instruccion'])), \
	  df(set(['Encuestado.No_C.I.']),set(['Jefe_de_Grupo_Familiar.E-mail','Jefe_de_Grupo_Familiar.Telefono.Celular','Jefe_de_Grupo_Familiar.Telefono.Habitacion','Jefe_de_Grupo_Familiar.Telefono.Oficina','Jefe_de_Grupo_Familiar.Dedicacion_actual','Jefe_de_Grupo_Familiar.¿Trabaja_actualmente?','Jefe_de_Grupo_Familiar.Forma_de_cobro','Jefe_de_Grupo_Familiar.Fecha_de_nacimiento'])), \
	  df(set(['Encuestado.No_C.I.']),set(['Situacion_comunidad.Ventajas','Situacion_comunidad.Desventajas'])), \
	  df(set(['Planilla.Numero']),set(['Comunidad.Nombre','Comunidad.Direccion.Nro','Comunidad.Direccion.Calle'])), \
	  df(set(['Comunidad.Nombre']),set(['Sector.CP'])), \
	  df(set(['Municipio.Nombre']),set(['Estado.Nombre'])), \
	  df(set(['Parroquia.Nombre']),set(['Municipio.Nombre'])), \
	  df(set(['Planilla.Numero']),set(['Encuestado.No_C.I.'])), \
	  df(set(['Encuestado.No_C.I.']),set(['Planilla.Numero'])), \
	  df(set(['Participacion_Comunitaria.Constitucion']),set(['Participacion_Comunitaria.Sabe'])), \
	  df(set(['Situacion_Laboral.Tarjeta']),set(['Situacion_Laboral.Cta_banco'])) \
	  ])
	
	return s
