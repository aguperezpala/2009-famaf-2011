CREATE TABLE "encuestador" (
	"planilla.numero" uint NOT NULL,
	"encuestador.no_ci" uint NOT NULL,
	"encuestador.nombre_y_apellido" varchar(50),
	PRIMARY KEY ("encuestador.no_ci"),
	FOREIGN KEY ("planilla.numero") REFERENCES "planilla"
);
