/*
 * Copyright(c) 2016-2019 Nicolas Sauzede (nsauzede@laposte.net)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
module vec

import math

struct Vector {

        x f64
        y f64
        z f64

//        x, y, z f64
}

pub fn (v Vector) str() string {
        return '[${v.x}, ${v.y}, ${v.z}]'
}

pub fn (v Vector) dot(ov Vector) f64 {
        return v.x * ov.x + v.y * ov.y + v.z * ov.z
}

pub fn (v Vector) cross(ov Vector) Vector {
        return Vector {
                v.y * ov.z - v.z * ov.y,
                v.z * ov.x - v.x * ov.z,
                v.x * ov.y - v.y * ov.x
        }
}

pub fn (v Vector) norm() f64 {
        return math.sqrt(v.dot(v))
}

pub fn (v Vector) mult(f f64) Vector {
        return Vector {
                v.x * f,
                v.y * f,
                v.z * f
        }
}

pub fn (v Vector) div(f f64) Vector {
        return Vector {
                v.x / f,
                v.y / f,
                v.z / f
        }
}

pub fn (v mut Vector) normalize() {
        *v = v.div(v.norm())
}

pub fn (v Vector) add(ov Vector) Vector {
        return Vector {
                v.x + ov.x,
                v.y + ov.y,
                v.z + ov.z
        }
}

pub fn (v Vector) sub(ov Vector) Vector {
        return Vector {
                v.x - ov.x,
                v.y - ov.y,
                v.z - ov.z
        }
}
