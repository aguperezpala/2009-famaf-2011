
module svn_Ing2/TP6/LTS


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Dominio de trabajo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Estados */
abstract sig States {}
sig State extends States {}  -- un alias cómodo

/* Etiquetas */
sig Labels {}
one sig Tau extends Labels {}

/* Sistemas de transiciones */
sig LTS {
	states : set States,
	initial: State,
	trans  : Labels -> states -> states
}

/* Relaciones entre LTS */
sig Relation {
	rel: States -> States
}

/* Todos los estados de un LTS son alcanzables desde su estado inicial */
fact Reachability {
	all s:LTS | let i = s.initial | i->(s.states-i) = ^(Labels.(s.trans))
}

/* Cada LTS tiene sus propios estados. Nada de mezclas raras */
fact Isolation {
	all s,s':LTS | s != s' => no (s.states & s'.states)
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~   Tipos de relaciones entre LTS   ~~~~~~~~~~~~~~~~~~~ */

/* r es una relación de simulación (fuerte) entre s1 y s2
 * ie: s1 simula a s2 a través de r */
pred Sim [r:Relation, s1,s2:LTS] {
	s1.initial->s2.initial in r.rel
	all s,t:s1.states, s':s2.states, l:Labels | (
		(l->s->t in s1.trans && s->s' in r.rel) =>
		(some t':s2.states | l->s'->t' in s2.trans && t->t' in r.rel)
	)
}

/* r es una relación de bi-simulación entre s1 y s2 */
pred BiSim [r:Relation, s1,s2:LTS] {
	Sim[r,s1,s2]
	some r':Relation | r'.rel = ~(r.rel) && Sim[r',s2,s1]
}

/* Hay camino s --l--> s' (comiendo taus) en el LTS s1 */
pred reaches [s,s':States, l:Labels, s1:LTS] {
	l->s->s' in s1.trans ||
	(some s':s1.states | s->s' in ^(Tau.(s1.trans)))
/*   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO
 *   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO
 *   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO   TODO */
}

/* r es una relación de simulación débil entre s1 y s2
 * ie: s1 simula débilmente a s2 a través de r */
pred WeakSim [r:Relation, s1,s2:LTS] {
	s1.initial->s2.initial in r.rel
	all s,t:s1.states, s':s2.states, l:Labels | (
		(l->s->t in s1.trans && s->s' in r.rel) =>
		(some t':s2.states | reaches[s',t',l,s2] && t->t' in r.rel)
	)
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~   Propiedades a satisfacer   ~~~~~~~~~~~~~~~~~~~~~~ */

Conexo: check {
	all s:LTS | (
		#s.states < 2 or
		s.states->s.states = ^(Labels.(s.trans) + ~(Labels.(s.trans)))
	)
} for 8 but exactly 1 LTS, 0 Relation

AllStatesReachable: check {
	all s:LTS | let i = s.initial |
		all st:s.states-i |	i->st in ^(Labels.(s.trans))
} for 50 but exactly 1 LTS, 0 Relation

BiSimImpliesSim: check {
	all s1,s2:LTS, r:Relation |
		BiSim[r,s1,s2] => Sim[r,s1,s2]
} for 10 but exactly 2 LTS, exactly 1 Relation


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~   Ejecuciones de prueba   ~~~~~~~~~~~~~~~~~~~~~~~~ */

pred Show [s:LTS] {
	some s.states
	some s.trans
	no Relation
}

pred ShowSim [s,s':LTS, r:Relation] {
	some s.trans && some s'.trans
	Sim[r,s,s']
}

run Show    for 3 but exactly 1 LTS
run ShowSim for 4 but exactly 2 LTS
