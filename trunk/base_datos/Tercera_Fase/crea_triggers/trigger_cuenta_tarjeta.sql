
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


