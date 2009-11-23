CREATE TABLE "planilla_parroquia" (
	"planilla.numero" integer NOT NULL,
	"parroquia.nombre" varchar(50) NOT NULL,
	PRIMARY KEY ("planilla.numero","parroquia.nombre"),
	FOREIGN KEY ("planilla.numero") REFERENCES "planilla"
);
