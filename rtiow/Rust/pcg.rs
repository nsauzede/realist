/**********************************************
 PCG random implementation - credits to Cieric for original Odin/C version
 */

pub const RAND_MAX: u32 = 4294967295;

struct Pcg32RandomT {
	state: u64,
	inc: u64
}

static mut SEED: Pcg32RandomT = Pcg32RandomT {
	state: 0,
	inc: 0
};

pub fn srand(val: u32) {
unsafe {
	SEED.state = val as u64;
	SEED.inc = 0;
}
}

pub fn rand() -> u32 {
unsafe {
	let oldstate = SEED.state as u64;
	SEED.state = oldstate * 6364136223846793005 as u64 + (SEED.inc | 1);
	let xorshifted = (((oldstate >> 18) ^ oldstate) >> 27) as u32;
	let rot = (oldstate >> 59) as u32;
	return (xorshifted >> rot) | (xorshifted << ((-(rot as i32)) & 31));
}
}

/*
 **********************************************/
