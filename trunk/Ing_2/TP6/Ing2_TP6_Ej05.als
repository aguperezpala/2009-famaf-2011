
module svn_Ing2/TP6/binrelation


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Dominio de trabajo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Universo */
abstract sig Univ {}

/* Elementos aislados en el universo (qué se yo, podría haber...) */
sig iso_elem extends Univ {}

/* Elementos del universo que las relaciones tocan */
some sig elem extends Univ {}  -- universo interesante: hay al menos 1 elem

/* Relación binaria entre elementos */
sig Rel {
	rel: elem -> elem
}

/* Muy bonito eso de los elementos aislados, pero molestan
 * Como no ayudan en nada los sacamos a la mierda */
fact NoMeJodan {
	no iso_elem
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~   Predicados   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Relación reflexiva (forall a | a->a in r.rel) */
pred Reflex  [r:Rel] { (elem <: iden) in r.rel }

/* Relación irreflexiva (forall a | a->a !in r.rel) */
pred IReflex [r:Rel] { no ((elem <: iden) & r.rel) }


/* Relación simétrica (forall a,b | a->b => b->a) */
pred Symm    [r:Rel] { r.rel = ~(r.rel) }

/* Relación asimétrica (forall a,b | a->b => !(b->a)) */
pred ASymm   [r:Rel] { no (r.rel & ~(r.rel)) }

/* Relación antisimétrica (forall a,b | a->b && b->a => a=b) */
pred Antisim [r:Rel] { (r.rel)&(~(r.rel)) in (iden :> elem) }


/* Relación transitiva (forall a,b,c | a->b && b->c => a->c) */
pred Transit [r:Rel] { ^(r.rel) in r.rel }

/* Relación es total (forall a,b | a->b OR b->a) */
pred Total   [r:Rel] { elem->elem = r.rel + ~(r.rel) }


/* Relación es pre-orden */
pred PreOrder [r:Rel] {
	Reflex [r]
	Transit[r]
}

/* Relación es orden parcial */
pred PartialOrder [r:Rel] {
	PreOrder[r]
	Antisim [r]
}

/* Relación es orden total */
pred TotalOrder [r:Rel] {
	PartialOrder[r]
	Total[r]
}

/* Relación es orden (parcial) estricto */
pred StrictOrder [r:Rel] {
	Transit[r]
	IReflex[r]
	ASymm[r]
}

/* Relación con "menor" o "primer" elemento */
pred FirstElem [r:Rel] {
	/* TODO TODO TODO */
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~   Propiedades a satisfacer   ~~~~~~~~~~~~~~~~~~~~~~ */

/* "Si es pre-orden entonces es reflexiva y transitiva" */
assert A_PreOrder {
	all r:Rel | PreOrder[r] => (
		all e1:elem |
			e1->e1 in r.rel &&
			(some e2,e3:elem |
				(e1->e2 in r.rel && e2->e3 in r.rel) => e1->e3 in r.rel)
	)
}

/* "Si es orden parcial entonces es pre-orden y antisimétrica "
 * NOTE: depende de la aserción "A_PreOrder" */
assert A_PartialOrder {
	all r:Rel | PartialOrder[r] => (
		PreOrder[r] &&
		(all a,b:elem | (a->b in r.rel && b->a in r.rel) => a=b)
	)
}

/* "Si es orden total entonces es orden parcial, y es total"
 * NOTE: depende de la aserción "A_PartialOrder" */
assert A_TotalOrder {
	all r:Rel | TotalOrder[r] => (
		PartialOrder[r] &&
		(all a,b:elem | a->b in r.rel || b->a in r.rel)
	)
}


/* "Si es orden estricto entonces es transitiva, irreflexiva y asimétrica " */
assert A_StrictOrder {
	all r:Rel | StrictOrder[r] => (all e1,e2,e3:elem |
		((e1->e2 in r.rel && e2->e3 in r.rel) => e1->e3 in r.rel)
		&& (e1->e1 !in r.rel)
		&& (e1->e2 in r.rel => e2->e1 !in r.rel)
	)
}



/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~   Ejecuciones de prueba   ~~~~~~~~~~~~~~~~~~~~~~~~ */

/* Pre-orden */
run PreOrder for 10 but exactly 1 Rel
check A_PreOrder for 50 but exactly 1 Rel

/* Orden parcial */
run PartialOrder for 10 but exactly 1 Rel
check A_PartialOrder for 50 but exactly 1 Rel

/* Orden total */
run TotalOrder for 10 but exactly 1 Rel
check A_TotalOrder for 50 but exactly 1 Rel

/* Orden estricto */
run StrictOrder for 10 but exactly 1 Rel
check A_StrictOrder for 50 but exactly 1 Rel


