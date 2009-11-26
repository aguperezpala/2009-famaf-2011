CREATE TABLE "municipio"(
	"municipio.nombre" VARCHAR(50) NOT NULL,
	"sector.cp" uint NOT NULL,
	"estado.nombre" VARCHAR (50) NOT NULL,
	PRIMARY KEY ("sector.cp","municipio.nombre"),
	FOREIGN KEY ("sector.cp") REFERENCES "sector",
	FOREIGN KEY ("estado.nombre") REFERENCES "estado"
);
