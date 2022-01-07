function main08_2() {
    function hit_sphere(d, r, tmin, tmax, rec) {
        const center = d[0], radius = d[1];
        // const oc = vsub(r.orig, center);
        var oc = r.orig.slice(0);
        vsub(oc, center);
        const a = dot(r.dir, r.dir);
        const hb = dot(oc, r.dir);
        const c = dot(oc, oc) - radius * radius;
        const discriminant = hb * hb - a * c;
        if (discriminant < 0.) return false;
        const sqrtd = Math.sqrt(discriminant);
        var root = (-hb - sqrtd) / a;
        if (root < tmin || root > tmax) {
            root = (-hb + sqrtd) / a;
            if (root < tmin || root > tmax) {
                return false;
            }
        }
        rec.t = root;
        rec.p = rat(r, rec.t);
        // rec.normal = vdiv(vsub(rec.p, center), radius);
        rec.normal = rec.p.slice(0);
        vsub(rec.normal, center);
        vdiv(rec.normal, radius);
        return true;
    }
    var quit = false;
    function ray_color(r, world, depth) {
        // if (depth <= 0) return new Float32Array([0, 0, 0]);
        if (depth <= 0) return [0., 0., 0.];
        var ret = 0;
        for (const h of world) {
            if (h.t === 'sphere') {
                var rec = {};
                if (hit_sphere(h.d, r, 0.001, infinity, rec)) {
                    // const target = vadd(rec.p, vadd(rec.normal, random_in_unit_sphere()));
                    var target = rec.p.slice(0);
                    vadd(target, rec.normal)
                    vadd(target, random_in_unit_sphere());
                    // return vmul(0.5, ray_color({ orig: rec.p, dir: vsub(target, rec.p) }, world, depth - 1));
                    vsub(target, rec.p);
                    var col = ray_color({ orig: rec.p, dir: target }, world, depth - 1);
                    vmul(0.5, col);
                    // log(`t=${rec.t} col=${col}`);
                    // quit = true;
                    return col;
                }
            }
        }
        // const unit_direction = unit_vector(r.dir);
        var unit_direction = r.dir.slice(0);
        // log(`r.dir=${r.dir}`);
        unit_vector(unit_direction);
        t = 0.5 * (unit_direction[1] + 1.0);
        return new Float32Array([1.0 - t + 0.5 * t, 1.0 - t + 0.7 * t, 1.0]);
    }
    const func = arguments.callee.name || "anonymous";
    print(`${func}`);
    var canvas = document.getElementById('canvas');
    const w = canvas.width;
    const h = canvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        // image
        const aspect_ratio = 16.0 / 9.0;
        log(`aspect_ratio=${aspect_ratio}`);
        const image_width = w;
        const image_height = h;
        const samples_per_pixel = document.getElementById('spp').value;
        const max_depth = document.getElementById('max_depth').value;
        log(`using ${samples_per_pixel} SPP, ${max_depth} max depth`);
        const sppscale = 1.0 / samples_per_pixel;
        // world
        const world = [{ t: 'sphere', d: [[0., 0., -1.], 0.5] }, { t: 'sphere', d: [[0., -100.5, -1.], 100.] }];
        // camera
        const viewport_height = 2.0;
        const viewport_width = aspect_ratio * viewport_height;
        const focal_length = 1.0;
        const origin = [0., 0., 0.];
        const horizontal = [viewport_width, 0., 0.];
        log(`horizontal=${horizontal}`);
        const vertical = [0., viewport_height, 0.];
        // const lower_left_corner = vsub(vsub(vsub(origin, vdiv(horizontal, 2.0)), vdiv(vertical, 2.0)), [0., 0., focal_length]);
        var lower_left_corner = origin.slice(0);
        var tmp = horizontal.slice(0);
        vdiv(tmp, 2.);
        log(`horizontal=${horizontal}`);
        vsub(lower_left_corner, tmp);
        tmp = vertical.slice(0);
        vdiv(tmp, 2.);
        vsub(lower_left_corner, tmp);
        tmp = [0., 0., focal_length];
        vsub(lower_left_corner, tmp);
        for (let j = 0; j < h; j++) {
            log(`Scanlines remaining: ${h - j - 1}`);
            for (let i = 0; i < w; i++) {
                var pixel_color = new Float32Array([0, 0, 0]);
                // var pixel_color = [0., 0., 0.];
                for (let s = 0; s < samples_per_pixel; s++) {
                    const u = (random_double() + i) / (w - 1.);
                    const v = (random_double() + h - 1. - j) / (h - 1.);
                    // log(`u=${u} v=${v}`);
                    // const r = { orig: origin, dir: vsub(vadd(vadd(lower_left_corner, vmul(u, horizontal)), vmul(v, vertical)), origin) };
                    tmp = lower_left_corner.slice(0);
                    // log(`tmp=${tmp}`);
                    var tmp2 = horizontal.slice(0);
                    // log(`tmp2=${tmp2}`);
                    vmul(u, tmp2);
                    // log(`tmp2=${tmp2}`);
                    vadd(tmp, tmp2);
                    // log(`tmp=${tmp}`);
                    tmp2 = vertical.slice(0);
                    // log(`tmp2=${tmp2}`);
                    vmul(v, tmp2);
                    // log(`tmp2=${tmp2}`);
                    vadd(tmp, tmp2);
                    // log(`tmp=${tmp}`);
                    vsub(tmp, origin);
                    // log(`tmp=${tmp}`);
                    // log(`origin=${origin}`);
                    const r = { orig: origin, dir: tmp };
                    // pixel_color = vadd(pixel_color, ray_color(r, world, max_depth));
                    vadd(pixel_color, ray_color(r, world, max_depth));
                    if (quit) return;
                }
                // return;
                image.data[(j * w + i) * 4 + 0] = 255.999 * pixel_color[0] * sppscale;
                image.data[(j * w + i) * 4 + 1] = 255.999 * pixel_color[1] * sppscale;
                image.data[(j * w + i) * 4 + 2] = 255.999 * pixel_color[2] * sppscale;
            }
            // return;
        }
        ctx.putImageData(image, 0, 0);
        print(`Done. ${func}`);
    }
}
