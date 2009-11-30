--- 1er consulta : Parientes
CREATE OR REPLACE TEMPORARY VIEW v_parientes(ci,nombre,apellido,parentesco,pariente_ci) AS
SELECT encuestado.no_ci AS ci,encuestado.nombre AS nombre,encuestado.apellido AS apellido,
	   encuestado.parentezco AS parentesco, ,jefe.no_ci AS jefe_ci  
FROM grupo_familiar;

CREATE OR REPLACE TEMPORARY VIEW parientes(jefe,profesion,dedicacion,conyuge,hijos,padres,vivenda) AS
SELECT encuestado.nombre_y_apellido AS jefe , encuestado.profesion AS profesion,
	   jefe.dedicacion_actual AS dedicacion, conyugue, 
	   count(hijos_ci) AS hijos,count(padres_ci) AS padres,
	   vivienda.tipo AS vivienda
FROM grupo_familiar , planilla , 
	 SELECT nombre_apellido AS conyugue FROM v_parientes 
	 WHERE pariente_ci = jefe.ci AND WHERE parentesco = "conyugue" ,
	 SELECT ci AS hijos_ci FROM v_parientes
	 WHERE pariente_ci = jefe.ci AND WHERE parentesco = "hijo" ,
	 SELECT ci AS padres_ci FROM v_parientes 
	 WHERE pariente_ci = jefe.ci AND WHERE parentesco = "padre" ,
WHERE 	grupo_familiar.planilla.numero = planilla.planilla.numero AND
		grupo_familiar.encuestado.no_ci = grupo_familiar.encuestado.jefe_ci -- Es JEFE!
GROUP BY jefe

--- 3ra consulta : ingreso_viviendas

CREATE OR REPLACE FUNCTION f_ingreso_familiar(IN ingreso uint) RETURNS VARCHAR
  DECLARE
  BEGIN
   	IF ingreso = 0 THEN 
		RETURN "Ninguno"; 
	ELSE IF ingreso < 200000 THEN 
		RETURN "Menos de 200000";
	ELSE IF ingreso < 500000 THEN 
		RETURN "200001 a 500000";
	ELSE IF ingreso < 2000000 THEN 
		RETURN "500001 a 2000000";
	ELSE 
		RETURN "Mas de 2000000";
	END IF;
  END;
$f_ingreso_familiar$ LANGUAGE plpgsql;

CREATE OR REPLACE TEMPORARY VIEW total_viviendas(viviendas) AS
SELECT count(vivienda.tipo) as viviendas FROM planilla;

CREATE OR REPLACE TEMPORARY VIEW ingreso_familiar_viviendas(ingreso,viviendas) AS
SELECT DISTINCT f_ingreso_familiar(situacion_laboral.ingreso_familiar) AS ingreso, 
				(count(vivienda.tipo)/total_viviendas.viviendas) as viviendas
FROM planilla , total_viviendas
GROUP BY ingreso;   

--- 4ta consulta: instruccion_personas

CREATE OR REPLACE TEMPORARY VIEW total_personas(personas) AS
SELECT count(encuestado.no_ci) as personas FROM grupo_familiar;

CREATE OR REPLACE TEMPORARY VIEW intruccion_personas(instruccion,personas) AS
SELECT DISTINCT encuestado.instruccion AS instruccion, 
				(count(encuestado.no_ci)/total_personas.personas) as viviendas
FROM grupo_familiar , total_personas
GROUP BY instruccion; 

--- 5ta consulta: poblacion_viviendas-en-estado_municipio

CREATE OR REPLACE TEMPORARY VIEW poblacion(estado,municipio,personas) AS
SELECT estado.nombre AS estado , municipio.nombre AS municipio , count(encuestado.no_ci) AS personas
FROM municipio , planilla , grupo_familiar
WHERE 	municipio.sector.cp == planilla.sector.cp AND  
		grupo_familiar.planilla.numero == planilla.planilla.numero
GROUP BY estado , municipio 

CREATE OR REPLACE TEMPORARY VIEW v_domiciliarias(estado,municipio,domiciliarias) AS
SELECT estado.nombre AS estado , municipio.nombre AS municipio , count(planilla.numero) AS domiciliarias
FROM municipio , planilla
WHERE 	municipio.sector.cp == planilla.sector.cp
GROUP BY estado , municipio 

CREATE OR REPLACE TEMPORARY VIEW v_internet(estado,municipio,internetarias) AS
SELECT estado.nombre AS estado , municipio.nombre AS municipio , count(planilla.numero) AS internetarias
WHERE 	municipio.sector.cp == planilla.sector.cp and servicios.medios == "internet" --- Esto ultimo puede cambiar!
GROUP BY estado , municipio 

CREATE OR REPLACE TEMPORARY VIEW v_internet AS
SELECT estado , municipio , personas , domiciliarias , internetiarias
FROM (personas JOIN v_domiciliarias USING(estado,municipio)) JOIN v_internet

--- 6ta consulta: Enfermedades
CREATE OR REPLACE TEMPORARY VIEW p_total AS 
SELECT count(encuestado.no_ci) as personas
FROM planilla WHERE salud.historial_familiar IS NULL

CREATE OR REPLACE TEMPORARY VIEW p_enfermas AS 
SELECT 	salud.historia_familiar AS enfermedad , 
		(count(encuestado.no_ci)/p_total.personas) as personas
FROM planilla , p_total WHERE salud.historial_familiar IS NOT NULL
group by salud.historia_familiar

CREATE OR REPLACE TEMPORARY VIEW p_curadas AS 
SELECT (count(encuestado.no_ci)/p_total.personas) as personas
FROM planilla , p_total WHERE salud.historial_familiar IS NULL

--- 8va consulta: Encuestadores

CREATE OR REPLACE TEMPORARY VIEW e_municipio AS
SELECT 	municipio.nombre AS municipio , 
		count(encuestador.no_ci) AS encuestadores ,
		count(planilla.numero)/count(encuestador.no_ci) AS porcentaje_casas
FROM planilla , encuestador , municipio
WHERE 	planilla.encuestador.nro_ci == encuestador.encuestador.no_ci AND
		municipio.sector.cp == planilla.sector.cp
GROUP BY municipio

