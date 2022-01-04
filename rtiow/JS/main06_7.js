function main06_7() {
    function hit_sphere(d, r) {
        const center = d[0], radius = d[1];
        const oc = vsub(r.orig, center);
        const a = dot(r.dir, r.dir);
        const hb = dot(oc, r.dir);
        const c = dot(oc, oc) - radius * radius;
        const discriminant = hb * hb - a * c;
        if (discriminant < 0) {
            return -1.0;
        } else {
            return (-hb - Math.sqrt(discriminant)) / a;
        }
    }
    function ray_color(r, world) {
        var ret = 0;
        for (const h of world) {
            if (h.t === 'sphere') {
                // println(`hitting sphere ${h}`)
                var t = hit_sphere(h.d, r);
                if (t > 0.0) {
                    const N = unit_vector(vsub(rat(r, t), new Float32Array([0, 0, -1])));
                    return vmul(0.5, vadd(N, new Float32Array([1, 1, 1])));
                }
            }
        }
        const unit_direction = unit_vector(r.dir);
        t = 0.5 * (unit_direction[1] + 1.0);
        return new Float32Array([1.0 - t + 0.5 * t, 1.0 - t + 0.7 * t, 1.0]);
    }
    const func = arguments.callee.name || "anonymous";
    cls();
    println(`${func}`);
    var canvas = document.getElementById('canvas');
    const w = canvas.width;
    const h = canvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        // image
        const aspect_ratio = 16.0 / 9.0;
        const image_width = w;
        const image_height = h;
        // world
        const world = [{ t: 'sphere', d: [[0, 0, -1], 0.5] }, { t: 'sphere', d: [[0, -100.5, -1], 100] }];
        // camera
        const viewport_height = 2.0;
        const viewport_width = aspect_ratio * viewport_height;
        const focal_length = 1.0;
        const origin = new Float32Array([0, 0, 0]);
        const horizontal = new Float32Array([viewport_width, 0, 0]);
        const vertical = new Float32Array([0, viewport_height, 0]);
        const lower_left_corner = vsub(vsub(vsub(origin, vdiv(horizontal, 2.0)), vdiv(vertical, 2.0)), new Float32Array([0, 0, focal_length]));
        for (let j = 0; j < h; j++) {
            println(`Scanlines remaining: ${h - j - 1}`);
            for (let i = 0; i < w; i++) {
                const u = parseFloat(i) / (w - 1);
                const v = parseFloat(h - 1 - j) / (h - 1);
                const r = { orig: origin, dir: vsub(vadd(vadd(lower_left_corner, vmul(u, horizontal)), vmul(v, vertical)), origin) };
                const pixel_color = ray_color(r, world);
                image.data[(j * w + i) * 4 + 0] = 255.999 * pixel_color[0];
                image.data[(j * w + i) * 4 + 1] = 255.999 * pixel_color[1];
                image.data[(j * w + i) * 4 + 2] = 255.999 * pixel_color[2];
            }
        }
        ctx.putImageData(image, 0, 0);
        println(`Done. ${func}`);
    }
}
