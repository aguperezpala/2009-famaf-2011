

CREATE OR REPLACE FUNCTION comunidad.direccion.nro() RETURNS TRIGGER
    AS $pos_int_comunidad.direccion.nro$
    BEGIN
	IF NEW."comunidad.direccion.nro"<0 THEN
	   BEGIN
		DELETE FROM "comunidad"
		WHERE "comunidad.direccion.nro"<0;
	   END;
	END IF;
    END;
    $pos_int_comunidad.direccion.nro$ LANGUAGE plpgsql;


CREATE TRIGGER "pos_int_comunidad.direccion.nro"
BEFORE INSERT OR UPDATE ON "comunidad"
    FOR EACH ROW EXECUTE PROCEDURE comunidad.direccion.nro();

