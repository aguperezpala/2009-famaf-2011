# -*- coding: utf-8 -*-

from df import *

# Aca vamos a definir todos los atributos, y una funcion nos va a devolver un
# set, que vendria a ser el Esquema universal

# TODO

def getEsquemaUniversal():
	return set(
	'Planilla.Número', 'Planilla.Fecha', 'Encuestado.Edad',
	'Encuestado.sexo', 'Encuestado.Ingreso_Mensual',
	'Encuestado.Profesión', 'Encuestado.Instrucción',
	'Encuestado.Nº_C.I.', 'Encuestado.Nombre_y_Apellido',
	'Jefe_de_Grupo_Familiar.Fecha_de_nacimiento',
	'Jefe_de_Grupo_Familiar.Forma_de_Cobro',
	'Jefe_de_Grupo_Familiar.¿Trabaja_actualmente?',
	'Jefe_de_Grupo_Familiar.Dedicación_actual',
	'Jefe_de_Grupo_Familiar.E-mail',
	'Jefe_de_Grupo_Familiar.Teléfono.Celular',
	'Jefe_de_Grupo_Familiar.Teléfono.Habitación',
	'Jefe_de_Grupo_Familiar.Teléfono.Oficina',
	'Situación_comunidad.Ventajas',
	'Situación_comunidad.Desventajas',
	'Comunidad.Nombre',
	'Comunidad.Dirección.Nro', 'Comunidad.Dirección.Calle',
	'Sector.CP',
	'Municipio.Nombre',
	'Parroquia.Nombre', 'Estado.Nombre',
	'Situación_Laboral.Tarjeta',
	'Situación_Laboral.Ticket',
	'Situación_Laboral.Cta_banco',
	'Situación_Laboral.Ingreso_familiar',
	'Situación_Laboral.Comercio_en_casa',
	'Situación_Laboral.Trabajo', 'Vivienda.Mascotas',
	'Vivienda.Plagas',
	'Vivienda.cond-Cond_salubridad', 'Vivienda.Enseres',
	'Vivienda.Techo',
	'Vivienda.Paredes', 'Vivienda.Terreno_propio',
	'Vivienda.Forma_tenencia',
	'Vivienda.Tipo',
	'Vivienda.OCV', 'Servicios.Servicios_comunales',
	'Servicios.Recolección_basura',
	'Servicios.Transporte', 'Servicios.Medios',
	'Servicios.Telefonía',
	'Servicios.Electricidad', 'Servicios.Gas',
	'Servicios.Aguas_servidas',
	'Servicios.Aguas_blancas', 'Salud.Historia_familiar',
	'Salud.Ayuda_especial',
	'Situación_de_Exclusión.Tercera_edad',
	'Situación_de_Exclusión.Discapacitados',
	'Situación_de_Exclusión.Enfermedades_terminarles',
	'Situación_de_Exclusión.Indigentes',
	'Situación_de_Exclusión.Niños_calle',
	'Participación_Comunitaria.Propia',
	'Participación_Comunitaria.Familia',
	'Participación_Comunitaria.Org_comunitarias',
	'Participación_Comunitaria.Administración',
	'Participación_Comunitaria.Constitución',
	'Participación_Comunitaria.Sabe',
	'Participación_Comunitaria.Apoya',
	'Participación_Comunitaria.Área',
	'Participación_Comunitaria.Misiones'
	)
	
	
# Funcion que devuelve el conjunto de dependencias funcionales
def getDepFunc ():
s = set ([df(set(['Planilla.Número']),set(['Situación_Laboral.Trabajo','Situación_Laboral.Comercio_en_casa','Situación_Laboral.Ingreso_familiar','Situación_Laboral.Cta._banco','Situación_Laboral.Tarjeta','Situación_Laboral.Ticket'])),
	df(set(['Planilla.Número']),set(['Vivienda.Tipo','Vivienda.Forma_tenecia','Vivienda.Terreno_propio','Vivienda.OCV','Vivienda.Techo','Vivienda.Paredes','Vivienda.Enseres','Vivienda.Cond._salubridad','Vivienda.Plagas','Vivienda.Mascotas'])),
	df(set(['Planilla.Número']),set(['Servicios.Aguas_blancas','Servicios.Aguas_servidas','Servicios.Gas','Servicios.Electricidad','Servicios.Recolección_basura','Servicios.Telefonía','Servicios.Transporte','Servicios.Mecanismo_de_información','Servicios.Servicios_comunales'])),
	df(set(['Planilla.Número']),set(['Participación_comunitaria.Org_comunitarias','Participación_comunitaria.Administración','Participación_comunitaria.Constitución','Participación_comunitaria.Propia','Participación_comunitaria.Familia','Participación_comunitaria.Misiones','Participación_comunitaria.Sabe','Participación_comunitaria.Apoya','Participación_comunitaria.Área'])),
	df(set(['Planilla.Número']),set(['Salud.Ayuda_especial','Salud.Historia_familiar'])),
	df(set(['Planilla.Número']),set(['Situación_de_exclusión.Niños_calle','Situación_de_exclusión.Indigentes','Situación_de_exclusión.Tercera_edad','Situación_de_exclusión.Discapacitados','Situación_de_exclusión.Enfermedades_terminales'])),
	df(set(['Encuestado.No_C.I.']),set(['Encuestado.Nombre_y_apellido','Encuestado.Edad','Encuestado.Sexo','Encuestado.Ingreso_mensual','Encuestado.Profesión','Encuestado.Instrucción'])),
	df(set(['Encuestado.No_C.I.']),set(['Jefe_de_Grupo_Familiar.E-mail','Jefe_de_Grupo_Familiar.Teléfono.Celular','Jefe_de_Grupo_Familiar.Teléfono.Habitación','Jefe_de_Grupo_Familiar.Teléfono.Oficina','Jefe_de_Grupo_Familiar.Dedicación_actual','Jefe_de_Grupo_Familiar.¿Trabaja_actualmente?','Jefe_de_Grupo_Familiar.Forma_de_cobro','Jefe_de_Grupo_Familiar.Fecha_de_nacimiento'])),
	df(set(['Encuestado.No_C.I.']),set(['Situación_comunidad.Ventajas','Situación_comunidad.Desventajas'])),
	df(set(['Planilla.Número']),set(['Comunidad.Nombre','Comunidad.Dirección.Nro','Comunidad.Dirección.Calle'])),
	df(set(['Comunidad.Nombre']),set(['Sector.CP'])),
	df(set(['Municipio.Nombre']),set(['Estado.Nombre'])),
	df(set(['Parroquia.Nombre']),set(['Municipio.Nombre'])),
	df(set(['Planilla.Número']),set(['Encuestado.No_C.I.'])),
	df(set(['Encuestado.No_C.I.']),set(['Planilla.Número'])),
	df(set(['Participación_comunitaria']),set(['Constitución'])),
	df(set(['Situación_laboral.Tarjeta']),set(['Situación_laboral.Cta._banco']))
	])
	
	return s
