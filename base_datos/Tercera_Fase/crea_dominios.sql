CREATE DOMAIN uint AS INT
	CONSTRAINT positiveness
	CHECK (VALUE >= 0);

CREATE DOMAIN sexo AS VARCHAR
	CHECK ((VALUE LIKE 'M') OR (VALUE LIKE 'F'));


CREATE DOMAIN parentezco AS VARCHAR
	CHECK ( (VALUE LIKE '')		OR
		(VALUE LIKE 'Padre')	OR
		(VALUE LIKE 'Hijo')	OR
		(VALUE LIKE 'Conyugue')	OR
		(VALUE LIKE 'Abuelo')	OR
		(VALUE LIKE 'Nieto')	OR
		(VALUE LIKE 'Tio') );


CREATE DOMAIN estado_civil AS VARCHAR
	CHECK ( (VALUE LIKE 'Soltero')		OR
		(VALUE LIKE 'Divorciado')	OR
		(VALUE LIKE 'Viudo')		OR
		(VALUE LIKE 'Concubino')	OR
		(VALUE LIKE 'Casado') );


CREATE DOMAIN nivel_instruccion AS VARCHAR
	CHECK ( (VALUE LIKE 'Ninguna')		OR
		(VALUE LIKE 'Basica')		OR
		(VALUE LIKE 'Bachiller')	OR
		(VALUE LIKE 'Tecnico medio')	OR
		(VALUE LIKE 'Tecnico superior')	OR
		(VALUE LIKE 'Post grado')	OR
		(VALUE LIKE 'Tecnico superior')	OR
		(VALUE LIKE 'Universitario') );
