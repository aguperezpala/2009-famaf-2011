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
SELECT  , 
	   

