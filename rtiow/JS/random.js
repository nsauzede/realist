function random_double() {
    return Math.random();
}
function clamp(x, min, max) {
    if (x < min) return min;
    if (x > max) return max;
    return x;
}