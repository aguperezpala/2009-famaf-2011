-- Datos de Sector
INSERT INTO "sector" VALUES (5016);
INSERT INTO "sector" VALUES (5099);
INSERT INTO "sector" VALUES (1235);
INSERT INTO "sector" VALUES (6489);
INSERT INTO "sector" VALUES (1277);
INSERT INTO "sector" VALUES (963);
INSERT INTO "sector" VALUES (111);
INSERT INTO "sector" VALUES (0149);
INSERT INTO "sector" VALUES (6489);
INSERT INTO "sector" VALUES (2548);
INSERT INTO "sector" VALUES (2154);
INSERT INTO "sector" VALUES (6233);
INSERT INTO "sector" VALUES (3314);
INSERT INTO "sector" VALUES (0666);
-- Datos de Estado
INSERT INTO "estado" VALUES ('Argentina');
INSERT INTO "estado" VALUES ('Brazil');
INSERT INTO "estado" VALUES ('Espana');
INSERT INTO "estado" VALUES ('Pais Vazco');
INSERT INTO "estado" VALUES ('Escocia');
INSERT INTO "estado" VALUES ('Dinamarca');
INSERT INTO "estado" VALUES ('Alemania');
INSERT INTO "estado" VALUES ('Tailandia');
INSERT INTO "estado" VALUES ('Groenlandia');
INSERT INTO "estado" VALUES ('Camboya');
INSERT INTO "estado" VALUES ('Papua Nueva Guinea');
INSERT INTO "estado" VALUES ('Guayana francesa');
INSERT INTO "estado" VALUES ('Uzbekistan');
INSERT INTO "estado" VALUES ('Taiwan');
INSERT INTO "estado" VALUES ('Laos');
INSERT INTO "estado" VALUES ('Villa los 40 guasos');
-- Datos de Municipio
INSERT INTO "municipio" VALUES ('Cordoba',5016,'Argentina');
INSERT INTO "municipio" VALUES ('Green Lands',111,'Escocia');
INSERT INTO "municipio" VALUES ('Porto Alegre',0149,'Brazil');
INSERT INTO "municipio" VALUES ('Carlos Paz',5099,'Argentina');
INSERT INTO "municipio" VALUES ('Madrid',6489,'Espana');
INSERT INTO "municipio" VALUES ('Barcelona',6489,'Espana');
INSERT INTO "municipio" VALUES ('Camboyita',0149,'Camboya');
INSERT INTO "municipio" VALUES ('Vazcolandia',6233,'Pais Vazco');
INSERT INTO "municipio" VALUES ('Copenhagen',1235,'Dinamarca');
INSERT INTO "municipio" VALUES ('Munich',6489,'Alemania');
INSERT INTO "municipio" VALUES ('Berlin',963,'Alemania');
INSERT INTO "municipio" VALUES ('Rotterdam',6489,'Alemania');
INSERT INTO "municipio" VALUES ('Tazmania',1277,'Papua Nueva Guinea');
INSERT INTO "municipio" VALUES ('Le magnifique waikiki',2548,'Guayana francesa');
INSERT INTO "municipio" VALUES ('Abalakazabom',2154,'Uzbekistan');
INSERT INTO "municipio" VALUES ('Chie chie',3314,'Taiwan');
INSERT INTO "municipio" VALUES ('Pu ka chi',3314,'Taiwan');
INSERT INTO "municipio" VALUES ('Che culiau',0666,'Villa los 40 guasos');
INSERT INTO "municipio" VALUES ('Te via rompe la caripela',0666,'Villa los 40 guasos');
-- Datos de Comunidad
INSERT INTO "comunidad" VALUES ('Albuquerque',423,'Los Alamos',5016);
INSERT INTO "comunidad" VALUES ('Mi hogar 3',1302,'Belgrano',5016);
INSERT INTO "comunidad" VALUES ('Los abuelos de la nada',012,'Belardinelli',111);
INSERT INTO "comunidad" VALUES ('Centro vecinal de Santa Isabel',2300,'Republica de Siria',5099);
INSERT INTO "comunidad" VALUES ('Centro de Jubilados dona Eume',233,'General Artigas',6489);
INSERT INTO "comunidad" VALUES ('Mi hogar -1',1234,'Av. Velez Sarsfield',1235);
INSERT INTO "comunidad" VALUES ('Cosmic Bowling',865,'Arguello',0149);
INSERT INTO "comunidad" VALUES ('Un antro mal',2248,'Los Boulevares',0149);
-- Datos de Planilla
INSERT INTO "planilla" VALUES (01,'Feb-15-2008',false,'',false,false,'',true,'','limpieza',true,true,'ratas','dueno',false,'cemento','casa','razonables','mesa, sillas','puma','ladrillos','camion','bombona','cloaca','tanque','automovil','recoleccion de basura','tendido','celular','periodico','','cancer',2,0,0,0,0,true,false,true,500000,'dependencia','kiosco','Centro vecinal de Santa Isabel',5099);
INSERT INTO "planilla" VALUES (02,'Feb-15-2008',false,'',false,false,'',true,'','limpieza',true,true,'cucarachas','dueno',false,'chapa','ranchito','malas','banquito','raton','madera','camion','','rio','rio','bicicleta','policia','ganchos','celular carisimo','periodico','','',0,1,9,10,0,true,false,true,10000,'','kiosco','Mi hogar -1',1235);
-- Datos de Grupo_familiar
INSERT INTO "grupo_familiar" VALUES (10266984,01,10266984,'Carpintero','Don','Ramon','Ninguna',61,300000,'Casado','M','','Oct-20-1965','',False,'Destajo',NULL,NULL,0364556,'','Ninugna, esto es una mierda','Y tiene mal olor');
