function main02_3() {
    var canvas = document.getElementById('canvas');
    const w = canvas.width;
    const h = canvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        cls();
        for (let j = 0; j < h; j++) {
            println(`Scanlines remaining: ${h - j}`);
            for (let i = 0; i < w; i++) {
                const r = parseFloat(i) / (w - 1);
                const g = parseFloat(h - 1 - j) / (h - 1);
                const b = 0.25;
                image.data[(j * w + i) * 4 + 0] = 255.999 * r;
                image.data[(j * w + i) * 4 + 1] = 255.999 * g;
                image.data[(j * w + i) * 4 + 2] = 255.999 * b;
            }
        }
        ctx.putImageData(image, 0, 0);
        println(`Done.`);
    }
}
