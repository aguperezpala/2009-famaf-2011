CREATE TABLE "comunidad"(
	"comunidad.nombre" VARCHAR(50) NOT NULL,
	"comunidad.direccion.nro" uint NOT NULL,
	"comunidad.direccion.calle" VARCHAR(50) NOT NULL,
	"sector.cp" uint NOT NULL,
	PRIMARY KEY ("comunidad.nombre","sector.cp"),
	FOREIGN KEY ("sector.cp") REFERENCES "sector"
);
