
//open util/ordering [ State ] 

// El dominio del problema ---------

sig Addr, Data { }

sig Memory{
    data: Addr -> lone Data
   }

pred write [ m, m': Memory, a: Addr, d: Data ] {
    m'.data = m.data ++ a -> d
   }

pred read [ m: Memory, a: Addr, d: Data ] {
    let d' = m.data[a] | (some d') implies d = d'
   }




// MainMemory y Cache

sig MainMemory extends Memory {}


sig CacheMemory extends Memory {
   dirty: set data.Data	// solo direcciones de memoria que esten escritas
   }


sig SistemaDeMemoriaDeJuli{
	mem: MainMemory,
	cache: CacheMemory
   }



run write for 3 but exactly 1 SistemaDeMemoriaDeJuli


/*
// La dinámica --------------------
// Espacio de estado

sig State {
    mem:Memory
   }

// Estado inicial (predicado auxiliar)

pred init [ m: Memory ] { no m.data }

// Traza



fact Traces {
    init[first[].mem]
    all s:State, s':next[s] |
      (some a:Addr, d:Data | read[s.mem,a,d] and s = s')
      or
      (some a:Addr, d:Data | write[s.mem,s'.mem,a,d])
   }




// Propiedad requerida

pred freshDir [ m: Memory ] {
    some a: Addr | no (m.data.Data & a) 
   }

pred freshDirInLast {
    freshDir [last[].mem]
   }

run freshDirInLast for 5*/
