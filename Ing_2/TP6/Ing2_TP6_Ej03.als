
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Dominio de trabajo   ~~~~~~~~~~~~~~~~~~~~~~~~~~ */

sig Addr {}
sig Data {}

sig Mem {
	addrs : set Addr,
	map : addrs -> lone Data
	}{
	/* addrs lista todas las memorias de map, con o sin datos */
	addrs in map.Data
}

sig MainMem extends Mem {}

sig Cache extends Mem {
	mugre: set addrs
}

sig System {
	cache: Cache,
	main: MainMem
	}{
	cache.addrs in main.addrs   // toda dirección de cache existe en mainMem
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~   Predicados   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */

pred Read [s,s':System, a:Addr, d:Data] {
	/* Dirección hallada en caché */
	a in s.cache.addrs =>
		(s'= s and (let d'= s.cache.map[a] | some d' implies d = d'))
	/* Dirección hallada en memoria principal */
	(a in s.main.addrs - s.cache.addrs) =>
		(s'.main.map = s.main.map and s'.cache.mugre = s.cache.mugre and
		(let d' = s.main.map[a] |
			some d' =>
				/* Tenía un dato mapeado */
				(d = d' and s'.cache.map = s.cache.map ++ a->d)
			else
				/* No tenía dato */
				(s'.cache.map = s.cache.map)
	))
	/* Dirección inexistente */
	a !in s.cache.addrs + s.main.addrs => s'= s
}

pred Write [s,s':System, a:Addr, d:Data] {
	/* Dirección existe en caché */
	a in s.cache.addrs => (
		s'.cache.map = s.cache.map ++ a->d	and
		s'.cache.mugre = s.cache.mugre ++ a and
		s'.main = s.main )
	/* Dirección sólo existe en mainMem */
	a in s.main.addrs - s.cache.addrs => (
		s'.cache = s.cache	and
		s'.main.map = s.main.map ++ a->d )
	/* Dirección no existe */
	a !in s.main.addrs + s.cache.addrs =>
		s' = s
}

pred Flush [s,s':System] {
	s'.main.addrs = s.main.addrs /* Mismo espacio de direcciones   */
	s'.cache.map  = s.cache.map  /* No tocamos el mapeo de caché   */
	no s'.cache.mugre			 /* Borramos todos los bits sucios */
	all a:s.main.addrs |
	a in s.cache.mugre =>
		s'.main.map[a] = s.cache.map[a]
	else
		s'.main.map[a] = s.main.map[a]
}

pred Load [s,s':System] {
	s'.cache.addrs = s.cache.addrs	/* Mismo espacio de direcciones    */
	s'.main = s.main				/* No tocamos la memoria principal */
	no s'.cache.mugre				/* Borramos todos los bits sucios  */
	all a:s.cache.addrs | s'.cache.map[a] = s.main.map[a]
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~   Propiedades a satisfacer   ~~~~~~~~~~~~~~~~~~~~~~ */

assert DirtyInv {
	all s: System |
	no s.cache.mugre => s.cache.map in s.main.map
}

assert DirtyInv2 [s:System] {
	no s.cache.mugre => s.cache.map in s.main.map
}

assert DirtyInvRead [s,s':System] {
	all a:Addr | all d:Data |
	(DirtyInv2[s] and Read[s,s',a,d]) => DirtyInv2[s']
}


/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
/* ~~~~~~~~~~~~~~~~~~~~~~~   Ejecución de prueba   ~~~~~~~~~~~~~~~~~~~~~~~~~ */

pred ReadAUX [s,s':System, a:Addr, d:Data] {
	/* Forzamos un Read que meta algo en caché */
	Read [s, s', a, d]
	not s.cache = s'.cache
}

pred WriteAUX [s,s':System, a:Addr, d:Data] {
	/* Forzamos un Write sobre una dirección 'a'
	 * de un dato distinto al que previamente estaba ahí */
	Write[s,s',a,d]
	a in s.cache.addrs and a !in s.cache.mugre
}

pred FlushAUX [s,s':System] {
	/* Forzamos un sistema con una dirección 'a' sucia
	 * tal que: s.main.map[a] != s.cache.map[a] */
	some a:s.cache.mugre |
		let d = s.cache.map[a] | a->d !in s.main.map
	Flush[s,s']
}

pred LoadAUX [s,s':System] {
	/* Forzamos un sistema con una dirección 'a'
	 * tal que: s.cache.map[a] != s.main.map[a] */
	some a:s.cache.addrs |
		let d = s.main.map[a] | a->d !in s.cache.map
	Load[s,s']
}


run ReadAUX for 4 but exactly 2 System

check DirtyInvRead
