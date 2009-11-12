CREATE TABLE "parroquia_sector" (
	"parroquia.nombre" varchar(25) NOT NULL PRIMARY KEY, 
	"sector.nombre" varchar(25) NOT NULL,
	FOREIGN KEY ("parroquia.nombre") REFERENCES comunidad_sector
);