use std::ops::{Add,Sub,Mul};

#[derive(Clone)]
pub struct V3 {
    x: f64,
    y: f64,
    z: f64,
}

impl V3 {
    pub fn cross(&self, other: &V3) -> V3 {
        V3 {
            x: self.y * other.z - self.z * other.y,
            y: self.z * other.x - self.x * other.z,
            z: self.x * other.y - self.y * other.x,
        }
    }

    pub fn dot(&self, other: &V3) -> f64 {
        self.x * other.x + self.y * other.y + self.z * other.z
    }

    pub fn normalize(&mut self) {
        let norm = f64::sqrt(V3::dot(&self, &self));
        self.x /= norm;
        self.y /= norm;
        self.z /= norm;
    }
}

impl Add for V3 {
    type Output = V3;
    fn add(self, other: V3) -> V3 {
        V3 {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
        }
    }
}

impl<'a,'b> Add<&'b V3> for &'a V3 {
    type Output = V3;
    fn add(self, other: &V3) -> V3 {
        V3 {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
        }
    }
}


impl Sub for V3 {
    type Output = V3;
    fn sub(self, other: V3) -> V3 {
        V3 {
            x: self.x - other.x,
            y: self.y - other.y,
            z: self.z - other.z,
        }
    }
}

impl<'a, 'b> Sub<&'b V3> for &'a V3 {
    type Output = V3;
    fn sub(self, other: &V3) -> V3 {
        V3 {
            x: self.x - other.x,
            y: self.y - other.y,
            z: self.z - other.z,
        }
    }
}

impl Mul<f64> for V3 {
    type Output = V3;
    fn mul(self, other: f64) -> V3 {
        V3 {
            x: self.x * other,
            y: self.y * other,
            z: self.z * other,
        }
    }
}

impl<'b> Mul<f64> for &'b V3 {
    type Output = V3;
    fn mul(self, other: f64) -> V3 {
        V3 {
            x: self.x * other,
            y: self.y * other,
            z: self.z * other,
        }
    }
}

impl Mul<V3> for f64 {
    type Output = V3;
    fn mul(self, other: V3) -> V3 {
        V3 {
            x: self * other.x,
            y: self * other.y,
            z: self * other.z,
        }
    }
}

impl<'a> Mul<&'a V3> for f64 {
    type Output = V3;
    fn mul(self, other: &V3) -> V3 {
        V3 {
            x: self * other.x,
            y: self * other.y,
            z: self * other.z,
        }
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_cross() {
        let a =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let b =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let cross = V3::cross(&a,&b);
        assert_eq!(cross.x, 0f64);
        assert_eq!(cross.y, 0f64);
        assert_eq!(cross.z, 0f64);
        //test if input moves
        drop(a);
        drop(b);
    }

    #[test]
    fn test_dot() {
        let a =  V3 {x: 1f64, y: 2f64, z: 2f64};
        let b =  V3 {x: 2f64, y: 2f64, z: 3f64};
        let res = V3::dot(&a,&b);
        assert_eq!(res, 12f64);
        //test if input moves
        drop(a);
        drop(b);
    }

    #[test]
    fn test_normalize() {
        let mut a =  V3 {x: 16f64, y: 0f64, z: 0f64};
        a.normalize();
        assert_eq!(a.x, 1f64);
        //test if input moves
        drop(a);
    }

    #[test]
    fn test_add_func() {
        let a =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let b =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let sum = <&V3 as Add<&V3>>::add(&a,&b);
        assert_eq!(sum.x, 4f64);
        assert_eq!(sum.y, 4f64);
        assert_eq!(sum.z, 4f64);
    }

    #[test]
    fn test_add_val() {
        let a =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let b =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let sum = a + b;
        assert_eq!(sum.x, 4f64);
        assert_eq!(sum.y, 4f64);
        assert_eq!(sum.z, 4f64);
    }

    #[test]
    fn test_add_ref() {
        let a =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let b =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let sum = &a + &b;
        assert_eq!(sum.x, 4f64);
        assert_eq!(sum.y, 4f64);
        assert_eq!(sum.z, 4f64);
    }

    #[test]
    fn test_sub_val() {
        let a =  V3 {x: 4f64, y: 4f64, z: 4f64};
        let b =  V3 {x: 3f64, y: 2f64, z: 1f64};
        let res = a - b;
        assert_eq!(res.x, 1f64);
        assert_eq!(res.y, 2f64);
        assert_eq!(res.z, 3f64);
    }

    #[test]
    fn test_sub_ref() {
        let a =  V3 {x: 4f64, y: 4f64, z: 4f64};
        let b =  V3 {x: 3f64, y: 2f64, z: 1f64};
        let res = &a - &b;
        assert_eq!(res.x, 1f64);
        assert_eq!(res.y, 2f64);
        assert_eq!(res.z, 3f64);
    }

    #[test]
    fn test_mul_val1() {
        let a =  V3 {x: 1f64, y: 2f64, z: 3f64};
        let res = a * 5f64;
        assert_eq!(res.x, 5f64);
        assert_eq!(res.y, 10f64);
        assert_eq!(res.z, 15f64);
    }

    #[test]
    fn test_mul_ref1() {
        let a =  V3 {x: 1f64, y: 2f64, z: 3f64};
        let res = &a * 5f64;
        assert_eq!(res.x, 5f64);
        assert_eq!(res.y, 10f64);
        assert_eq!(res.z, 15f64);
    }

    #[test]
    fn test_mul_val2() {
        let a =  V3 {x: 1f64, y: 2f64, z: 3f64};
        let res = 5f64 * a;
        assert_eq!(res.x, 5f64);
        assert_eq!(res.y, 10f64);
        assert_eq!(res.z, 15f64);
    }

    #[test]
    fn test_mul_ref2() {
        let a =  V3 {x: 1f64, y: 2f64, z: 3f64};
        let res = 5f64 * &a;
        assert_eq!(res.x, 5f64);
        assert_eq!(res.y, 10f64);
        assert_eq!(res.z, 15f64);
    }

    #[test]
    fn test_clone() {
        let a =  V3 {x: 1f64, y: 2f64, z: 3f64};
        let res = a.clone();
        drop(a);
        assert_eq!(res.x, 1f64);
        assert_eq!(res.y, 2f64);
        assert_eq!(res.z, 3f64);
    }
}
