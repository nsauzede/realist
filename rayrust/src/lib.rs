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
}
