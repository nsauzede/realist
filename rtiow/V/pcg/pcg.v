module pcg

struct PCG32Random {
mut:
	state u64
	inc   u64
}

__global (
	rfcnt   int 
	riuscnt int 
	riudcnt int 
	seed    PCG32Random 
)

const (
	pcg_rand_max = 4294967295
)

pub fn pcg_srand(val u32) {
	seed.state = val
	seed.inc = 0
}

pub fn pcg_rand() u32 {
	oldstate := seed.state
	seed.state = oldstate * u64(6364136223846793005) + (seed.inc | 1)
	xorshifted := u32(((oldstate >> 18) ^ oldstate) >> 27)
	rot := u32(oldstate >> 59)
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31))
}
