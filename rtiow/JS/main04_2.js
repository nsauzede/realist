function main04_2() {
    function ray_color(r) {
        const unit_direction = unit_vector(r.dir);
        const t = 0.5 * (unit_direction[1] + 1.0);
        return [1.0 - t + 0.5 * t, 1.0 - t + 0.7 * t, 1.0];
    }
    const func = arguments.callee.name || "anonymous";
    print(`${func}`);
    var canvas = document.getElementById('canvas');
    const w = canvas.width;
    const h = canvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        const aspect_ratio = 16.0 / 9.0;
        const image_width = w;
        const image_height = h;
        const viewport_height = 2.0;
        const viewport_width = aspect_ratio * viewport_height;
        const focal_length = 1.0;
        const origin = [0, 0, 0];
        const horizontal = [viewport_width, 0, 0];
        const vertical = [0, viewport_height, 0];
        const lower_left_corner = vsub(vsub(vsub(origin, vdiv(horizontal, 2.0)), vdiv(horizontal, 2.0)), [0, 0, focal_length]);
        for (let j = 0; j < h; j++) {
            log(`Scanlines remaining: ${h - j - 1}`);
            for (let i = 0; i < w; i++) {
                const u = parseFloat(i) / (w - 1);
                const v = parseFloat(h - 1 - j) / (h - 1);
                const r = { orig: origin, dir: vsub(vadd(vadd(lower_left_corner, vmul(u, horizontal)), vmul(v, vertical)), origin) };
                const pixel_color = ray_color(r);
                image.data[(j * w + i) * 4 + 0] = 255.999 * pixel_color[0];
                image.data[(j * w + i) * 4 + 1] = 255.999 * pixel_color[1];
                image.data[(j * w + i) * 4 + 2] = 255.999 * pixel_color[2];
            }
        }
        ctx.putImageData(image, 0, 0);
        print(`Done. ${func}`);
    }
}
