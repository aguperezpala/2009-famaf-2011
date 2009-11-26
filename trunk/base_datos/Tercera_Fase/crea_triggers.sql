
-- TABLA: planilla
-- ATRIBUTO/S: "situacion_labural.cta_banco" , "situacion_laboral.tarjeta"
-- ACCIÓN: insertar, actualizar
-- CHECK: si tiene tarjeta => tiene cta_banco
CREATE OR REPLACE FUNCTION cuenta_tarjeta() RETURNS TRIGGER AS $cuenta_tarjeta$
  DECLARE
  BEGIN

	IF NEW."situacion_laboral.tarjeta" AND NOT NEW."situacion_labural.cta_banco" THEN
		UPDATE "planilla"
		SET "situacion_labural.cta_banco" = True
		WHERE "planilla.numero" = NEW."planilla.numero";
	END IF;

  	RETURN NULL;
  END;
$cuenta_tarjeta$ LANGUAGE plpgsql;

CREATE TRIGGER "cuenta_tarjeta" AFTER INSERT OR UPDATE
	ON "planilla" FOR EACH ROW
	EXECUTE PROCEDURE cuenta_tarjeta();

--------------------------------------------------------------------------------

-- TABLA: grupo_familiar
-- ATRIBUTO/S: jefe.fecha_de_nacimiento
-- ACCIÓN: insertar
-- CHECK: el siglo de la fecha sea > 18
CREATE OR REPLACE FUNCTION insert_inmortal() RETURNS TRIGGER AS $insert_inmortal$
  DECLARE
  BEGIN
	CREATE OR REPLACE TEMPORARY VIEW siglo(n) AS
	SELECT EXTRACT(CENTURY FROM (CAST (NEW."jefe.fecha_de_nacimiento" AS TIMESTAMP)));
	IF 18 > siglo.n THEN
		DELETE FROM "grupo_familiar"
		WHERE "encuestado.no_ci" = NEW."encuestado.no_ci";
	END IF;

  	RETURN NULL;
  END;
$insert_inmortal$ LANGUAGE plpgsql;

CREATE TRIGGER "insert_inmortal" AFTER INSERT
	ON "grupo_familiar" FOR EACH ROW
	EXECUTE PROCEDURE insert_inmortal();


-- TABLA: grupo_familiar
-- ATRIBUTO/S: jefe.fecha_de_nacimiento
-- ACCIÓN: actualizar
-- CHECK: el siglo de la fecha sea > 18
CREATE OR REPLACE FUNCTION update_inmortal() RETURNS TRIGGER AS $update_inmortal$
  DECLARE
  BEGIN
	CREATE OR REPLACE TEMPORARY VIEW siglo(n) AS
	SELECT EXTRACT(CENTURY FROM (CAST (NEW."jefe.fecha_de_nacimiento" AS TIMESTAMP)));
	IF 18 > siglo.n THEN
		UPDATE "grupo_familiar"
		SET "jefe.fecha_de_nacimiento" = OLD."jefe.fecha_de_nacimiento"
		WHERE "encuestado.no_ci" = NEW."encuestado.no_ci";
	END IF;

  	RETURN NULL;
  END;
$update_inmortal$ LANGUAGE plpgsql;

CREATE TRIGGER "update_inmortal" AFTER UPDATE
	ON "grupo_familiar" FOR EACH ROW
	EXECUTE PROCEDURE update_inmortal();

--------------------------------------------------------------------------------

-- TABLA: grupo_familar
-- ATRIBUTOS/S: encuestado.jefe_ci
-- ACCIÓN: insertar
-- CHECK: cuando se inserta un encuestado, rellenar su campo encuestado.jefe_ci
--	  Si no hay un encuestado.jefe_ci para esa planilla => removerlo
/*
CREATE OR REPLACE FUNCTION insert_huerfano() RETURNS TRIGGER AS $insert_huerfano$
  DECLARE
  BEGIN
	IF NOT EXISTS ( SELECT "planilla_numero","encuestado.jefe_ci" as jefe_ci
			FROM "grupo_familiar"
			WHERE 
  END;
$insert_huerfano$ LANGUAGE plpgsql;

CREATE TRIGGER "insert_huerfano" AFTER INSERT
	ON "grupo_familar" FOR EACH ROW
	EXECUTE PROCEDURE insert_huerfano();
*/