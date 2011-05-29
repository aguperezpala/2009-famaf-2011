
module svn_Ing2/TP6/grafos


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Dominio de trabajo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

sig Node {}

sig Graph {
	edges: Node -> Node
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~   Predicados   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

pred Acyclic [g:Graph] {
	no ((iden :> Node) & ^(g.edges))
	/* "n0 -> n0" no es considerado un ciclo */
}

pred Undirected [g:Graph] {
	/* El grafo es no-dirigido */
	g.edges = ~(g.edges)
}

pred Connected [g:Graph] { /* "g es conexo" */
	(Node -> Node) in ^(g.edges + ~(g.edges))
	/* no se exige "n->n" para los nodos */
}

pred StronglyConnected [g:Graph] {
	/* "g es conexo respetando direcciones de los vértices" */
	(Node -> Node) in ^(g.edges)
	/* no se exige "n->n" para los nodos */
}

pred HasConnectedComp [g:Graph] {
	some g':Graph | g'.edges in g.edges and Connected[g']
}

pred HasStrongConnectedComp [g:Graph] {
	some g':Graph | g'.edges in g.edges and StronglyConnected[g']
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~   Ejecuciones de prueba   ~~~~~~~~~~~~~~~~~~~~~~~~ */

pred AcyclicAUX [g:Graph]    { some g.edges and Acyclic[g] }

pred UndirectedAUX [g:Graph] { some g.edges and Undirected[g] }

pred ConnectedAUX [g:Graph]  { some g.edges and Connected[g] }

pred StronglyConnectedAUX [g:Graph] {
	some e:g.edges | some n,n':Node |
		n->n' = e and n'->n !in g.edges
	StronglyConnected[g]
}

pred HasConnectedCompAUX [g:Graph] {
	#(g.edges) > 4
	HasConnectedComp[g]
}

pred HasStrongConnectedCompAUX [g:Graph] {
	#(g.edges) > 4
	HasStrongConnectedComp[g]
}

run HasStrongConnectedCompAUX for 5 but 1 Graph
