function vsub(v1, v2) {
    return new Float32Array([v1[0] - v2[0], v1[1] - v2[1], v1[2] - v2[2]]);
}
function vadd(v1, v2) {
    return new Float32Array([v1[0] + v2[0], v1[1] + v2[1], v1[2] + v2[2]]);
}
function vdiv(v, f) {
    return new Float32Array([v[0] / f, v[1] / f, v[2] / f]);
}
function vmul(f, v) {
    return new Float32Array([f * v[0], f * v[1], f * v[2]]);
}
function dot(v1, v2) {
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}
function vlensq(v) {
    return dot(v, v);
}
function vlen(v) {
    return Math.sqrt(vlensq(v));
}
function unit_vector(v) {
    return vdiv(v, vlen(v));
}
