package vec

import (
	"math"
	"testing"
)

func float64Eq(x, y float64) bool { return math.Abs(x-y) < 1e-14}

func TestDot(t *testing.T) {
	tests := []struct {
		v1, v2 Vector
		want float64
	}{
		{Vector{1, 1, 1}, Vector{-1, -1, -1}, -3},
	}
	for _, test := range tests {
		v1 := test.v1
		v2 := test.v2
		res := v1.Dot(v2)
		if !float64Eq(res, test.want) {
			t.Errorf("%v . %v = %v, want %v", v1, v2, res, test.want)
		}
	}
}

func TestMult(t *testing.T) {
	tests := []struct {
		v1 Vector
		f1 float64
		want Vector
	}{
		{Vector{1, 1, 1}, -3, Vector{-3, -3, -3}},
	}
	for _, test := range tests {
		v1 := test.v1
		f1 := test.f1
		res := v1.Mult(f1)
		if !res.IsEqual(test.want) {
			t.Errorf("%v * %v = %v, want %v", v1, f1, res, test.want)
		}
	}
}

func TestAdd(t *testing.T) {
	tests := []struct {
		v1 Vector
		v2 Vector
		want Vector
	}{
		{Vector{1, 1, 1}, Vector{ -1, -2, -3}, Vector{0, -1, -2}},
	}
	for _, test := range tests {
		v1 := test.v1
		v2 := test.v2
		res := v1.Add(v2)
		if !res.IsEqual(test.want) {
			t.Errorf("%v * %v = %v, want %v", v1, v2, res, test.want)
		}
	}
}
