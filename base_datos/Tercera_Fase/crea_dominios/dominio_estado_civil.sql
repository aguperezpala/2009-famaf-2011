CREATE DOMAIN estado_civil AS VARCHAR
	CHECK ( (VALUE LIKE 'Soltero')		OR
			(VALUE LIKE 'Divorciado')	OR
			(VALUE LIKE 'Viudo')		OR
			(VALUE LIKE 'Concubino')	OR
			(VALUE LIKE 'Casado');
