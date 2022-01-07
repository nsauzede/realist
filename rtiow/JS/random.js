function random_double0(min = 0.0, max = 1.0) {
    // return parseFloat(min + (max - min) * Math.random());
    // return min + (max - min) * Math.random();
    return Math.random() * (max - min) + min;
}
var pcg_seed = { state: 0, inc: 0 };
function pcg_srand(val) {
    pcg_seed.state = val;
    pcg_seed.inc = 0;
}
function pcg_rand() {
    // const old_state = pcg_seed.state;
    // pcg_seed.state = oldstate * 6364136223846793005 + (seed.inc | 1);
    // const xorshifted = ((oldstate >> 18) ^ oldstate) >> 27;
    // const rot = oldstate >> 59;
    // return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    return 0;
}
function random_double(min = 0.0, max = 1.0) {
    // return parseFloat(min + (max - min) * Math.random());
    // return min + (max - min) * Math.random();
    //return Math.random() * (max - min) + min;
    return pcg_rand() / (1.);
}
function clamp(x, min, max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
function random(p, min = 0., max = 0.) {
    // return [random_double(min, max), random_double(min, max), random_double(min, max)];
    p[0] = random_double(min, max);
    p[1] = random_double(min, max);
    p[2] = random_double(min, max);
    // return new Float32Array([random_double(min, max), random_double(min, max), random_double(min, max)]);
}
function random_in_unit_sphere() {
    var p = [0., 0., 0.];
    while (true) {
        // const p = random(-1., 1.);
        random(p, -1., 1.);
        if (vlensq(p) >= 1.) continue;
        return p;
    }
}