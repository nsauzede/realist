function random_double(min = 0.0, max = 1.0) {
    return min + (max - min) * Math.random();
}
function clamp(x, min, max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}
function random(min = 0, max) {
    return new Float32Array([random_double(min, max), random_double(min, max), random_double(min, max)]);
}
function random_in_unit_sphere() {
    while (true) {
        const p = random(-1, 1);
        if (vlensq(p) >= 1) continue;
        return p;
    }
}