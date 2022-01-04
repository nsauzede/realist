const infinity = Number.POSITIVE_INFINITY;
function cls(s) {
    document.getElementById('log').textContent = "";
}
function println(s) {
    // document.getElementById('log').textContent += s + `\r\n`;
    document.getElementById('log').textContent = s + `\r\n` + document.getElementById('log').textContent;
}
function print(s) {
    document.getElementById('log').textContent += s;
}
function setup() {
    document.getElementById('log').textContent = "Ready.";
    var canvas = document.getElementById('canvas');
    var zcanvas = document.getElementById('zcanvas');
    const w = canvas.width;
    const h = canvas.height;
    const zw = zcanvas.width;
    const zh = zcanvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        var zctx = zcanvas.getContext('2d', { alpha: false });
        image = ctx.getImageData(0, 0, w, h);
        for (let j = 0; j < (h - 1); j++) {
            for (let i = 0; i < (w - 1); i++) {
                const r = parseFloat(i) / (w - 1);
                const g = parseFloat(h - 1 - j) / (h - 1);
                const b = 0.25;
                image.data[(j * w + i) * 4 + 0] = 255.999 * r;
                image.data[(j * w + i) * 4 + 1] = 255.999 * 0;
                image.data[(j * w + i) * 4 + 2] = 255.999 * 0;
            }
        }
        ctx.putImageData(image, 0, 0);

        var hoveredColor = document.getElementById('hovered-color');
        var selectedColor = document.getElementById('selected-color');
        function pick(event, destination) {
            const rect = event.target.getBoundingClientRect();
            const x = event.layerX - rect.left;
            const y = event.layerY - rect.top;
            const pixel = ctx.getImageData(x, y, 1, 1);
            const data = pixel.data;
            const rgba = `rgba(${data[0]}, ${data[1]}, ${data[2]}, ${data[3] / 255})`;
            destination.style.background = rgba;
            destination.textContent = `${x} ${y} ${rgba}`;
        }
        function zoom(event) {
            const rect = event.target.getBoundingClientRect();
            var x = event.layerX - rect.left;
            var y = event.layerY - rect.top;
            const ww = w / 4;
            const hh = h / 4;
            if (x + ww > zw) x = w - ww;
            if (y + hh > zh) y = h - hh;
            zctx.drawImage(canvas, x, y, ww, hh, 0, 0, zw, zh);
        }
        canvas.addEventListener('mousemove', function (event) {
            pick(event, hoveredColor);
            zoom(event);
        });
        canvas.addEventListener('click', function (event) {
            pick(event, selectedColor);
        });
    }
}
