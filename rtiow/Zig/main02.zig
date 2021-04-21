const std = @import("std");

pub fn main() !void {
const stdout = std.io.getStdOut().outStream();
const nx = 200;
const ny = 100;
try stdout.print("P3\n", .{});
try stdout.print("{} {}\n", .{nx, ny});
try stdout.print("{}\n", .{255});
var j: i32 = ny - 1;
while (j >= 0) {
var i: i32 = 0;
while (i < nx) {
var r: f32 = @intToFloat(i) / nx;
try stdout.print("{}\n", .{6});
i += 1;
}
j -= 1;
}
}
