CREATE DOMAIN parentezco AS VARCHAR
	CHECK ( (VALUE LIKE 'Padre')	OR
			(VALUE LIKE 'Hijo')		OR
			(VALUE LIKE 'Conyugue')	OR
			(VALUE LIKE 'Abuelo')	OR
			(VALUE LIKE 'Nieto')	OR
			(VALUE LIKE 'Tio') );

