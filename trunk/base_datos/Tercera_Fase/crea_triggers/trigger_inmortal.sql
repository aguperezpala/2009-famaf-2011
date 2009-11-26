-- TABLA: grupo_familiar
-- ATRIBUTO/S: jefe.fecha_de_nacimiento
-- ACCIÓN: insertar
-- CHECK: el siglo de la fecha sea > 18
CREATE OR REPLACE FUNCTION insert_inmortal() RETURNS TRIGGER AS $insert_inmortal$
  DECLARE
  BEGIN
   	IF 18 > EXTRACT(CENTURY FROM (CAST (NEW."jefe.fecha_de_nacimiento" AS TIMESTAMP)));
		DELETE FROM "grupo_familiar"
		WHERE "encuestado.no_ci" = NEW."encuestado.no_ci";
	END IF

  	RETURN NULL;
  END;
$insert_inmortal$ LANGUAGE plpgsql;

CREATE TRIGGER "update_inmortal" AFTER INSERT
	ON "grupo_familiar" FOR EACH ROW
	EXECUTE PROCEDURE update_inmortal();




-- TABLA: grupo_familiar
-- ATRIBUTO/S: jefe.fecha_de_nacimiento
-- ACCIÓN: actualizar
-- CHECK: el siglo de la fecha sea > 18
CREATE OR REPLACE FUNCTION update_inmortal() RETURNS TRIGGER AS $update_inmortal$
  DECLARE
  BEGIN
   	IF 18 > EXTRACT(CENTURY FROM (CAST (NEW."jefe.fecha_de_nacimiento" AS TIMESTAMP)));
		UPDATE "grupo_familiar"
		SET "jefe.fecha_de_nacimiento" = OLD."jefe.fecha_de_nacimiento"
		WHERE "encuestado.no_ci" = NEW."encuestado.no_ci";
	END IF

  	RETURN NULL;
  END;
$update_inmortal$ LANGUAGE plpgsql;

CREATE TRIGGER "update_inmortal" AFTER UPDATE
	ON "grupo_familiar" FOR EACH ROW
	EXECUTE PROCEDURE update_inmortal();
