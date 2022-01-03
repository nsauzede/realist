function main03_3() {
    const func = arguments.callee.name || "anonymous";
    cls();
    println(`${func}`);
    var canvas = document.getElementById('canvas');
    const w = canvas.width;
    const h = canvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        for (let j = 0; j < h; j++) {
            println(`Scanlines remaining: ${h - j - 1}`);
            for (let i = 0; i < w; i++) {
                const pixel_color = new Float32Array([parseFloat(i) / (w - 1), parseFloat(h - 1 - j) / (h - 1), 0.25]);
                image.data[(j * w + i) * 4 + 0] = 255.999 * pixel_color[0];
                image.data[(j * w + i) * 4 + 1] = 255.999 * pixel_color[1];
                image.data[(j * w + i) * 4 + 2] = 255.999 * pixel_color[2];
            }
        }
        ctx.putImageData(image, 0, 0);
        println(`Done. ${func}`);
    }
}
