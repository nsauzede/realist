/*
 * Copyright(c) 2016-2019 Nicolas Sauzede (nsauzede@laposte.net)
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
package vec

import "math"

type Vector struct {
	X, Y, Z float64
}

func (v Vector) IsEqual(ov Vector) bool {
	return v.X == ov.X && v.Y == ov.Y && v.Z == ov.Z
}

func (v Vector) Dot(ov Vector) float64 {
	return v.X * ov.X + v.Y * ov.Y + v.Z * ov.Z
}

func (v Vector) Cross(ov Vector) Vector {
	var rv Vector
	rv.X = v.Y * ov.Z - v.Z * ov.Y
	rv.Y = v.Z * ov.X - v.X * ov.Z
	rv.Z = v.X * ov.Y - v.Y * ov.X
	return rv
}

func (v Vector) Norm() float64 {
	return math.Sqrt( v.Dot( v))
}

func (v Vector) Mult(f float64) Vector {
	var rv Vector
	rv.X = v.X * f
	rv.Y = v.Y * f
	rv.Z = v.Z * f
	return rv
}

func (v Vector) Div(f float64) Vector {
	var rv Vector
	rv.X = v.X / f
	rv.Y = v.Y / f
	rv.Z = v.Z / f
	return rv
}

func (v *Vector) Normalize() {
	*v = v.Div( v.Norm())
}

func (v Vector) Add(ov Vector) Vector {
	var rv Vector
	rv.X = v.X + ov.X
	rv.Y = v.Y + ov.Y
	rv.Z = v.Z + ov.Z
	return rv
}

func (v Vector) Sub(ov Vector) Vector {
	var rv Vector
	rv.X = v.X - ov.X
	rv.Y = v.Y - ov.Y
	rv.Z = v.Z - ov.Z
	return rv
}
