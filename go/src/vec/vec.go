package vec

type V3 struct {
	X, Y, Z float32
}

func (v V3) Dot(ov V3) float32 {
	return v.X * ov.X + v.Y * ov.Y + v.Z * ov.Z
}
