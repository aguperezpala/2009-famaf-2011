
module svn_Ing2/TP6/hanoi

open util/ordering [ Disc ] as ordDisc
/* El orden impone el tamaño de los discos,
 * y por ende cómo están ordenados en los palitos */

open util/ordering [ Game ] as ordGame
/* Usamos un orden para avanzar en el juego */


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Dominio de trabajo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

sig Disc {}

sig Stick { s : set Disc }

sig Game {
	stick1: set Disc,
	stick2: set Disc,
	stick3: set Disc
  /* El "buen" orden de los discos en los palitos
   * está dado implícitamente por ordDisc */
	}{
	Disc = stick1 + stick2 + stick3
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~   Dinámica del modelo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Disco en la cima de un palito */
fun top [stick : set Disc] : lone Disc {
	{ d:stick | all d':stick | d'!= d => d' !in ordDisc/nexts[d] }
}

/* Los palitos A y B son palitos (distintos) de este juego */
pred SticksOf [g,g':Game, sA,sB,sA',sB': set Disc] {
	((sA = g.stick1 && sA' = g'.stick1)  ||
	 (sA = g.stick2 && sA' = g'.stick2)  ||
	 (sA = g.stick2 && sA' = g'.stick2))
	&&
	((sB = g.stick1 && sB' = g'.stick1)  ||
	 (sB = g.stick2 && sB' = g'.stick2)  ||
	 (sB = g.stick2 && sB' = g'.stick2))
	&&
	no (sA & sB) && no (sA' & sB')
}

/* Mover legalmente un disco entre dos palitos */
pred Move [s1,s2,s1',s2': set Disc] {
	some d:s1 | d = top[s1]  &&  /* movemos el disco en la cima de s1 */
				s1' = s1 - d &&  /* quitamos d de s1 */
				s2' = s2 + d &&  /* metemos  d en s2 */
				(no s2 || ordDisc/lt [d, top[s2]]) /* s2={} or d<top(s2) */
} /* FIXME: pasa de s1{d0} ; s2{d1,d2} ; s3{}
   *        hacia   s1{d0} ; s2{d1,d2} ; s3{d1,d2} */

/* En un comienzo todo está en el primer palito */
pred Initial [ g:Game ] { no (g.stick2 + g.stick3) }
fact Initialized { Initial[ ordGame/first[] ] }

/* Avanzar un paso en el juego
 * ie: mover un disco entre palitos, respetando las reglas */
fact Step {
	all g:Game, g':ordGame/next[g] | (
		some disj ssrc,sdest,ssrc',sdest': Stick  | (
			SticksOf[g, g', ssrc.s, sdest.s, ssrc'.s, sdest'.s] &&
			Move[ssrc.s,sdest.s,ssrc'.s,sdest'.s]
		)
	)
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~   "Ejecución" del juego   ~~~~~~~~~~~~~~~~~~~~~~~~ */

pred SolveGame {
	(ordGame/last[]).stick3 = Disc
}

run SolveGame for 12 but 3 Disc
