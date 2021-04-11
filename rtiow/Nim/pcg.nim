#  PCG random implementation - credits to Cieric for original Odin/C version
const RAND_MAX* = uint32(4294967295)
type Pcg32RandomT* = object
    state: uint64
    inc: uint64

var SEED = Pcg32RandomT(state: 0, inc: 0)

proc srand*(val: uint32) {.inline.} =
    SEED.state = val
    SEED.inc = 0

proc rand*(): uint32 {.inline.} =
    var oldstate = SEED.state
    SEED.state = oldstate * uint64(6364136223846793005) + (SEED.inc or 1)
    var xorshifted = uint32(((oldstate shr 18) xor oldstate) shr 27)
    var rot = uint32(oldstate shr 59)
    return (xorshifted shr rot) or (xorshifted shl ((-int32(rot)) and 31))

proc random_double*(): float32 {.inline.} =
    float32(rand()) / (float32(RAND_MAX) + 1)
