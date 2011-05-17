module chapter6/memory/cacheMemory [Addr, Data] ----- the model from page 219

sig CacheSystem {
	main, cache: Addr -> lone Data
	}

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
		some c.cache[Addr]  /* Para que tenga algo que mostrar */
	}

run flush for 8 but exactly 2 CacheSystem

