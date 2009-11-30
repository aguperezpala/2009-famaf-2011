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
	CHECK ( (VALUE LIKE 'Soltero')	  OR
		(VALUE LIKE 'Divorciado') OR
		(VALUE LIKE 'Viudo')	  OR
		(VALUE LIKE 'Concubino')  OR
		(VALUE LIKE 'Casado') );


CREATE DOMAIN nivel_instruccion AS VARCHAR
	CHECK ( (VALUE LIKE 'Sin Instruccion')	OR
		(VALUE LIKE 'Basica')		OR
		(VALUE LIKE 'Bachiller')	OR
		(VALUE LIKE 'Tecnico medio')	OR
		(VALUE LIKE 'Tecnico superior')	OR
		(VALUE LIKE 'Post grado')	OR
		(VALUE LIKE 'Tecnico superior')	OR
		(VALUE LIKE 'Universitario') );


CREATE DOMAIN forma_cobro AS VARCHAR
	CHECK ( (VALUE LIKE 'Diario')	 OR
		(VALUE LIKE 'Semanal')	 OR
		(VALUE LIKE 'Quincenal') OR
		(VALUE LIKE 'Mensual') );


CREATE DOMAIN forma_tenencia AS VARCHAR
	CHECK ( (VALUE LIKE 'Traspasada') OR
		(VALUE LIKE 'Alquilada')  OR
		(VALUE LIKE 'Compartida') OR
		(VALUE LIKE 'Propia')     OR
		(VALUE LIKE 'Invadida')   OR
		(VALUE LIKE 'Otr_') );


CREATE DOMAIN tipo_vivienda AS VARCHAR
	CHECK ( (VALUE LIKE 'Quinta')	   OR
		(VALUE LIKE 'Casa')	   OR
		(VALUE LIKE 'Rancho')	   OR
		(VALUE LIKE 'Apartamento') OR
		(VALUE LIKE 'Pent House') 
		(VALUE LIKE 'Otr_') );


CREATE DOMAIN telefonia AS VARCHAR
	CHECK ( (VALUE LIKE 'Domiciliaria')	OR
		(VALUE LIKE 'Celular')		OR
		(VALUE LIKE 'Prepago') );


CREATE TYPE medios AS ( Television BOOLEAN,
			Radio	   BOOLEAN,
			Prensa	   BOOLEAN,
			Internet   BOOLEAN );


CREATE DOMAIN enfermedades AS VARCHAR
	CHECK ( (VALUE LIKE 'Cancer')		OR
		(VALUE LIKE 'Corazon')		OR
		(VALUE LIKE 'Epilepsia')	OR
		(VALUE LIKE 'Diabetes')		OR
		(VALUE LIKE 'Hepatitis')	OR
		(VALUE LIKE 'Tuberculosis')	OR
		(VALUE LIKE 'Sida')		OR
		(VALUE LIKE 'Leucemia')		OR
		(VALUE LIKE 'Hipertension')	OR
		(VALUE LIKE 'Otra') );
