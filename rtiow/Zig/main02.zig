const std = @import("std");

pub fn main() !void {
    const nx = 200;
    const ny = 100;
    const stdout = std.io.getStdOut().writer();
    try stdout.print("P3\n", .{});
    try stdout.print("{} {}\n", .{ nx, ny });
    try stdout.print("{}\n", .{255});
    var j: i32 = ny - 1;
    while (j >= 0) {
        var i: i32 = 0;
        while (i < nx) {
            var r: f32 = @intToFloat(f32, i) / nx;
            var g: f32 = @intToFloat(f32, j) / ny;
            var b: f32 = 0.2;
            var ir: i32 = @floatToInt(i32, 255.99 * r);
            var ig: i32 = @floatToInt(i32, 255.99 * g);
            var ib: i32 = @floatToInt(i32, 255.99 * b);
            // printf("%d %d %d\n", ir, ig, ib);
            try stdout.print("{} {} {}\n", .{ ir, ig, ib });
            i += 1;
        }
        j -= 1;
    }
}
