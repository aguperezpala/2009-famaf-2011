CREATE TABLE "sector" (
	"sector.cp" uint NOT NULL,
	PRIMARY KEY ("sector.cp")
);


CREATE TABLE "comunidad"(
	"comunidad.nombre" VARCHAR(50) NOT NULL,
	"comunidad.direccion.nro" uint NOT NULL,
	"comunidad.direccion.calle" VARCHAR(50) NOT NULL,
	"sector.cp" uint NOT NULL,
	PRIMARY KEY ("comunidad.nombre","sector.cp"),
	FOREIGN KEY ("sector.cp") REFERENCES "sector"
);


CREATE TABLE "estado"(
	"estado.nombre" VARCHAR (50) NOT NULL,
	PRIMARY KEY ("estado.nombre")
);


CREATE TABLE "municipio"(
	"municipio.nombre" VARCHAR(50) NOT NULL,
	"sector.cp" uint NOT NULL,
	"estado.nombre" VARCHAR (50) NOT NULL,
	PRIMARY KEY ("sector.cp","municipio.nombre"),
	FOREIGN KEY ("sector.cp") REFERENCES "sector",
	FOREIGN KEY ("estado.nombre") REFERENCES "estado"
);


CREATE TABLE "planilla" (
	"planilla.numero" uint NOT NULL,
	"planilla.fecha" date,
	"participacion_comunitaria.administracion" boolean DEFAULT False,
	"participacion_comunitaria.sabe" varchar DEFAULT NULL,
	"participacion_comunitaria.constitucion" boolean DEFAULT False,
	"participacion_comunitaria.propia" boolean DEFAULT False,
	"participacion_comunitaria.misiones" varchar(50),
	"participacion_comunitaria.apoya" boolean DEFAULT False,
	"participacion_comunitaria.org_comunitarias" varchar(50),
	"participacion_comunitaria.area" varchar(50),
	"participacion_comunitaria.familia" boolean DEFAULT False,
	"vivienda.terreno_propio" boolean DEFAULT False,
	"vivienda.plagas" varchar(50),
	"vivienda.forma_tenencia" varchar(50),
	"vivienda.ocv" boolean DEFAULT False,
	"vivienda.techo" varchar(50),
	"vivienda.tipo" varchar(50),
	"vivienda.cond_salubridad" varchar(50),
	"vivienda.enseres" varchar(50),
	"vivienda.mascotas" varchar(50),
	"vivienda.paredes" varchar(50),
	"servicios.recoleccion_basura" varchar(50),
	"servicios.gas" varchar(50),
	"servicios.aguas_servidas" varchar(50),
	"servicios.aguas_blancas" varchar(50),
	"servicios.transporte" varchar(50),
	"servicios.servicios_comunales" varchar(50),
	"servicios.electricidad" varchar(50),
	"servicios.telefonia" varchar(50),
	"servicios.medios" varchar(50) ,
	"salud.ayuda_especial" varchar(50) DEFAULT NULL,
	"salud.historia_familiar" varchar(50),
	"situacion_de_exclusion.tercera_edad" uint DEFAULT 0,
	"situacion_de_exclusion.enfermedades_terminales" uint DEFAULT 0,
	"situacion_de_exclusion.indigentes" uint DEFAULT 0,
	"situacion_de_exclusion.niños_de_la_calle" uint DEFAULT 0,
	"situacion_de_exclusion.discapacitado" uint DEFAULT 0,
	"situacion_laboral.tarjeta" boolean DEFAULT True,
	"situacion_labural.cta_banco" boolean DEFAULT True,
	"situacion_laboral.ticket" boolean DEFAULT False,
	"situacion_laboral.ingreso_familiar" uint,
	"situacion_laboral.trabajo" varchar(50),
	"situacion_laboral.comercio_en_casa" varchar(50),
	"comunidad.nombre" varchar(50) NOT NULL,	
	"sector.cp" uint NOT NULL,
	PRIMARY KEY ("planilla.numero"),
	FOREIGN KEY ("comunidad.nombre","sector.cp") REFERENCES "comunidad"
);


CREATE TABLE "grupo_familiar" (
	"encuestado.no_ci" uint NOT NULL ,
	"planilla.numero" uint NOT NULL ,
	"encuestado.jefe_ci" uint NOT NULL ,
	"encuestado.profesion" varchar(20) ,
	"encuestado.nombre" varchar(40) NOT NULL,
	"encuestado.apellido" varchar(40) NOT NULL,
	"encuestado.instruccion" nivel_instruccion NOT NULL,
	"encuestado.edad" uint NOT NULL DEFAULT 0,
	"encuestado.ingreso_mensual" float(10) NOT NULL DEFAULT 0,
	"encuestado.estado_civil" estado_civil NOT NULL,
	"encuestado.sexo" sexo NOT NULL,
	"encuestado.parentezco" parentezco,
	"jefe.fecha_de_nacimiento" date,
	"jefe.dedicacion_actual" varchar(20),
	"jefe.trabaja_actualmente" boolean,
	"jefe.forma_de_cobro" varchar(30),
	"jefe.telefono.celular" uint,
	"jefe.telefono.oficina" uint,
	"jefe.telefono.habitacion" uint,
	"jefe.email" varchar(50),
	"situacion_comunidad.ventajas" varchar(50),
	"situacion_comunitaria.desventajas" varchar(50),
	PRIMARY KEY ("encuestado.no_ci"),
	FOREIGN KEY ("planilla.numero") REFERENCES "planilla",
	FOREIGN KEY ("encuestado.jefe_ci") REFERENCES "grupo_familiar"("encuestado.no_ci")
);


CREATE TABLE "encuestador" (
	"planilla.numero" uint NOT NULL,
	"encuestador.no_ci" uint NOT NULL,
	"encuestador.nombre_y_apellido" varchar(50),
	PRIMARY KEY ("encuestador.no_ci"),
	FOREIGN KEY ("planilla.numero") REFERENCES "planilla"
);


CREATE TABLE "planilla_parroquia" (
	"planilla.numero" uint NOT NULL,
	"parroquia.nombre" varchar(50) NOT NULL,
	PRIMARY KEY ("planilla.numero","parroquia.nombre"),
	FOREIGN KEY ("planilla.numero") REFERENCES "planilla"
);
