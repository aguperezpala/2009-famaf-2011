module chapter6/memory/cacheMemory [Addr, Data] ----- the model from page 219

/* ------------------------------------------------------------------------- */
/* --------------------------   Dominios   --------------------------------- */

sig Addr, Data {}

sig Memory {
		/* Tengo un subconj. de todas las direcciones */
		addrs: set Addr
		/* Cada dirección tiene a lo sumo un dato */
		map : addrs -> lone Data
	}

/* MainMem es un singulete de Memory
 * Es decir: es una memoria */
sig MainMem extends Memory {}

/* Otra memoria, pero extendida con bits de "dirty" */
sig CacheMem extends Memory {
	dirty: set addrs
	/* "dirty" ⊆ (direcciones de la caché)
   * Si a ∈ "dirty" => la dirección 'a' está sucia */
	}

sig CacheSystem {
	main, cache: Addr -> lone Data
	}


/* ------------------------------------------------------------------------- */
/* -------------------------   Operaciones   ------------------------------- */


/*
pred init [c: CacheSystem] {
	no c.main + c.cache
	}

pred write [c, c': CacheSystem, a: Addr, d: Data] {
	c'.main = c.main
	c'.cache = c.cache ++ a -> d
	}

pred read [c: CacheSystem, a: Addr, d: Data] {
	some d
	d = c.cache [a]
	}

pred flush [c, c': CacheSystem] {
		c'.cache = c.cache
		c'.main  = c.main ++ c.cache
		some c.cache[Addr]  /* Para que tenga algo que mostrar 
	}

run flush for 8 but exactly 2 CacheSystem

*/
