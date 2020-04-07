package pcg

/**********************************************
 PCG random implementation - credits to Cieric for original Odin/C version
 */
const RAND_MAX = uint64(4294967295)

type pcg32_random_t struct {
	state, inc uint64
}

var seed = pcg32_random_t {0, 0}

func Srand(val uint32) {
	seed.state = uint64(val)
	seed.inc = 0
}

func Rand() uint32 {
	oldstate := seed.state
	seed.state = oldstate * uint64(6364136223846793005) + (seed.inc | 1)
	xorshifted := uint32(((oldstate >> 18) ^ oldstate) >> 27)
	rot := uint32(oldstate >> 59)
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31))
}
/*
 **********************************************/
