
module svn_Ing2/TP6/catalogomusical


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Dominio de trabajo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

sig Interprete {}

sig Cancion {}

sig Catalogo {
	canciones: set Cancion,
	interpretes: set Interprete,
	interpretaciones: canciones -> interpretes
	}{
	/* todas las canciones están registradas bajo algún intérprete */
	canciones   = interpretaciones.interpretes
	/* todo intérprete tiene registrada alguna canción */
	interpretes = canciones.interpretaciones
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~   Predicados   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Añade una canción con su intérprete al catálogo */
pred AddSong [c,c':Catalogo, can:Cancion, i:Interprete] {
	c'.canciones   = c.canciones + can
	c'.interpretes = c.interpretes + i
	c'.interpretaciones = c.interpretaciones ++ can->i
}

/* Quita una canción con su intérprete del catálogo */
pred DelSong [c,c':Catalogo, can:Cancion, i:Interprete] {
	c'.canciones   = c.canciones - can
	c'.interpretes = c.interpretes - i
	c'.interpretaciones = c.interpretaciones - can->i
}

/* Intérpretes de canciones compartidas */
fun Bands [c:Catalogo] : set Interprete->Interprete {
	~(c.interpretaciones).(c.interpretaciones)
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~   Propiedades a satisfacer   ~~~~~~~~~~~~~~~~~~~~~~ */

/* No hay canciones sin intérprete */
NoAnonymousSong: check {
	all c:Catalogo | all can:c.canciones |
		some i:c.interpretes | can->i in c.interpretaciones
} for 50 but exactly 1 Catalogo

/* No hay intérpretes sin canciones */
NoFakeArtist: check {
	all c:Catalogo | all i:c.interpretes |
		some can:c.canciones | can->i in c.interpretaciones
} for 50 but exactly 1 Catalogo

/* Las canciones añadidas... son añadidas */
CorrectAdding: check {
	all c,c':Catalogo, can:Cancion, i:Interprete |
		AddSong[c,c',can,i] => can->i in c'.interpretaciones
} for 50 but exactly 1 Catalogo

/* Las canciones eliminadas... son eliminadas*/
CorrectDeleting: check {
	all c,c':Catalogo, can:Cancion, i:Interprete |
		DelSong[c,c',can,i] => can->i !in c'.interpretaciones
} for 50 but exactly 1 Catalogo


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~   Ejecuciones de prueba   ~~~~~~~~~~~~~~~~~~~~~~~~ */

pred SomeCatalogo [c:Catalogo] { #c.interpretaciones > 6 }
run SomeCatalogo for 6 but exactly 1 Catalogo

pred AddNewSong [c,c':Catalogo, can:Cancion, i:Interprete] {
	#c.interpretaciones > 4
	can !in c.canciones
	AddSong[c,c',can,i]
}
run AddNewSong for 6 but exactly 2 Catalogo

pred AddExistingSong [c,c':Catalogo, can:Cancion, i:Interprete] {
	#c.interpretaciones > 4
	can in c.canciones
	AddSong[c,c',can,i]
}
run AddExistingSong for 6 but 2 Catalogo

pred DelExistingSong [c,c':Catalogo, can:Cancion, i:Interprete] {
	#c.interpretaciones > 4
	can in c.canciones
	DelSong[c,c',can,i]
}
run DelExistingSong for 6 but exactly 2 Catalogo

pred DelNonExistingSong [c,c':Catalogo, can:Cancion, i:Interprete] {
	#c.interpretaciones > 4
	can !in c.canciones
	DelSong[c,c',can,i]
}
run DelNonExistingSong for 6 but 2 Catalogo

