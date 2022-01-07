const infinity = Number.POSITIVE_INFINITY;
function log(s) {
    console.log(s);
}
function print(s) {
    document.getElementById('status').textContent = s;
}
function setup() {
    print("Beware, some tests take some time to finish (main07+)");
    const c = 0.;
    print(`c=${c} ${typeof (c)}`);
    // const a = [0];
    // const a = new Float32Array([0]);
    const a = new Float32Array([1]);
    function foo(v) {
        v[0] = 12;
    }
    foo(a)
    print(`a=${a[0]} ${typeof (a[0])}`);
    var canvas = document.getElementById('canvas');
    var zcanvas = document.getElementById('zcanvas');
    const w = canvas.width;
    const h = canvas.height;
    const zw = zcanvas.width;
    const zh = zcanvas.height;
    if (canvas.getContext) {
        var ctx = canvas.getContext('2d', { alpha: false });
        var zctx = zcanvas.getContext('2d', { alpha: false });
        zctx.imageSmoothingEnabled = false;
        image = ctx.getImageData(0, 0, w, h);
        for (let j = 0; j < h; j++) {
            for (let i = 0; i < w; i++) {
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
            var x = event.layerX - rect.left, cx = x;
            var y = event.layerY - rect.top, cy = y;
            const ww = w / 5;
            const hh = h / 5;
            if (x - ww / 2 < 0) {
                x = ww / 2;
            } else if (x + ww / 2 > zw) {
                cx = parseInt(ww - (w - x));
                x = zw - ww / 2;
            } else {
                cx = parseInt(ww / 2);
            }
            if (y - hh / 2 < 0) {
                y = hh / 2;
            } else if (y + hh / 2 > zh) {
                cy = parseInt(hh - (zh - y));
                y = zh - hh / 2;
            } else {
                cy = parseInt(hh / 2);
            }

            var imageData = ctx.getImageData(x - ww / 2, y - hh / 2, ww, hh);
            var newCanvas = document.createElement("canvas");
            newCanvas.id = "cursor";
            newCanvas.width = imageData.width;
            newCanvas.height = imageData.height;
            imageData.data[(cy * imageData.width + cx) * 4 + 0] = 255.999 * 1;
            imageData.data[(cy * imageData.width + cx) * 4 + 1] = 255.999 * 1;
            imageData.data[(cy * imageData.width + cx) * 4 + 2] = 255.999 * 1;
            newCanvas.getContext("2d").putImageData(imageData, 0, 0);

            zctx.drawImage(newCanvas, 0, 0, ww, hh, 0, 0, zw, zh);
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
