function rat(r, t) {
    var at = r.dir.slice(0);
    vadd(at, r.orig);
    vmul(t, at);
    return at;
}
