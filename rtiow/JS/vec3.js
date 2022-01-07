function vsub(v1, v2) {
    v1[0] -= v2[0]; v1[1] -= v2[1]; v1[2] -= v2[2];
    // return [v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]];
    // return [parseFloat(v1[0]) - v2[0], parseFloat(v1[1]) - v2[1], parseFloat(v1[2]) - v2[2]];
    // return [parseFloat(v1[0] - v2[0]), parseFloat(v1[1] - v2[1]), parseFloat(v1[2] - v2[2])];
    // return new Float32Array([v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]]);
}
function vadd(v1, v2) {
    v1[0] += v2[0]; v1[1] += v2[1]; v1[2] += v2[2];
    // return [v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]];
    // return [parseFloat(v1[0]) + v2[0], parseFloat(v1[1]) + v2[1], parseFloat(v1[2]) + v2[2]];
    // return new Float32Array([v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]]);
}
function vmul(f, v) {
    // return [f * v[0], f * v[1], f * v[2]];
    v[0] *= f; v[1] *= f; v[2] *= f;
    // return [parseFloat(f) * v[0], parseFloat(f) * v[1], parseFloat(f) * v[2]];
    // return new Float32Array([f * v[0], f * v[1], f * v[2]]);
}
function vdiv(v, f) {
    // const inv = 1. / f;
    // v[0] *= f, v[1] / f, v[2] / f];
    vmul(1. / f, v);
    // return [v[0] / f, v[1] / f, v[2] / f];
    // return [inv * v[0], inv * v[1], inv * v[2]];
    // return [parseFloat(v[0]) / f, parseFloat(v[1]) / f, parseFloat(v[2]) / f];
    // return new Float32Array([v[0] / f, v[1] / f, v[2] / f]);
}
function dot(v1, v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
    // return parseFloat(v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2]);
    // return parseFloat(v1[0]) * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
function vlensq(v) {
    // return dot(v, v);
    return v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
}
function vlen(v) {
    return Math.sqrt(vlensq(v));
}
function unit_vector(v) {
    //    return vdiv(v, vlen(v));
    vdiv(v, vlen(v));
}
