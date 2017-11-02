use std::ops::Add;

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
    }

    #[test]
    fn test_add() {
        let a =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let b =  V3 {x: 2f64, y: 2f64, z: 2f64};
        let sum = a + b;
        assert_eq!(sum.x, 4f64);
        assert_eq!(sum.y, 4f64);
        assert_eq!(sum.z, 4f64);
    }
}
