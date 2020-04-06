package pcg

/**********************************************
 PCG random implementation - credits to Cieric
 */
RAND_MAX : u32 = 4294967295;
pcg32_random_t :: struct { state, inc : u64, };
seed : pcg32_random_t = {0, 0};
srand :: proc(val : u32) {
	seed.state = cast(u64)(val);
	seed.inc = 0;
}

rand :: proc() -> u32 {
	oldstate : u64 = seed.state;
	seed.state = oldstate * cast(u64)(6364136223846793005) + (seed.inc | 1);
	xorshifted : u32 = cast(u32)(((oldstate >> 18) ~ oldstate) >> 27);
	rot : u32 = cast(u32)(oldstate >> 59);
	return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
/*
 **********************************************/
