CREATE DOMAIN instruccion AS VARCHAR
	CHECK ( (VALUE LIKE 'Ninguna')		OR
		(VALUE LIKE 'Basica')		OR
		(VALUE LIKE 'Bachiller')	OR
		(VALUE LIKE 'Tecnico medio')	OR
		(VALUE LIKE 'Tecnico Superior')	OR
		(VALUE LIKE 'Post grado')	OR
		(VALUE LIKE 'Tecnico Superior')	OR
		(VALUE LIKE 'Universitario') );
