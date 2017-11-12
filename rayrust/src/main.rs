extern crate rayrust;
use rayrust::{V3, solvetri, Sol};

use std::f64;

struct Sphere {
    center: V3,
    radius: f64,
    color: [f64;3],
}

const SPH:[Sphere;9] = [
    Sphere { center: V3 {x: 0.0 , y:-0.1 , z: 0.0} , radius: 0.05, color: [0.8, 0.8, 0.8]},
    Sphere { center: V3 {x: 0.0 , y: 0.0 , z: 0.0} , radius: 0.05, color: [0.8, 0.8, 0.8]},
    Sphere { center: V3 {x: 0.0 , y: 0.1 , z: 0.0} , radius: 0.05, color: [0.8, 0.8, 0.8]},
    Sphere { center: V3 {x: 0.1 , y:-0.05, z: 0.0} , radius: 0.05, color: [0.8, 0.0, 0.0]},
    Sphere { center: V3 {x: 0.1 , y: 0.05, z: 0.0} , radius: 0.05, color: [0.0, 0.0, 0.8]},
    Sphere { center: V3 {x: 0.2 , y: 0.0 , z: 0.0} , radius: 0.05, color: [0.0, 0.8, 0.0]},
    Sphere { center: V3 {x: 0.05, y:-0.05, z: 0.1} , radius: 0.05, color: [0.8, 0.8, 0.8]},
    Sphere { center: V3 {x: 0.05, y: 0.05, z: 0.1} , radius: 0.05, color: [0.8, 0.8, 0.8]},
    Sphere { center: V3 {x: 0.0 , y:-0.5 , z: 0.5} , radius: 0.02, color: [1.0, 1.0, 0.0]},
];

fn Intersec( sph1: &Sphere,  o: &V3,  v: &V3) -> f64 {
    let m_r = sph1.radius;
    let m_c = &sph1.center;
    let sr2 = m_r * m_r;
    let vt =  o - m_c;//V3::sub(o, m_c);
    let a = V3::dot( v, v);
    let b = 2f64 * V3::dot( v, &vt);
    let c = V3::dot( &vt, &vt) - sr2;
    let sol = solvetri( a, b, c);
    match sol {
        Sol::S2(t1, t2) => {return f64::max(t1, t2);},
        Sol::S1(t) => {return t;},
        Sol::None => {return f64::INFINITY;},
    }
}

fn Trace( o: &V3, v: &V3) -> [f64; 3] {
    const TMAX:f64 = 1E10f64;
	let mut tmin = f64::INFINITY;
	let mut omin = [0f64;3];
	//unsigned imin = 0;
	for elem in SPH.iter() {
        let tres = Intersec( elem, o, v);
        if (tres > 0f64) && (tres < tmin) {
            tmin = tres;
            omin = elem.color;
        }
    }
    if tmin < TMAX {
        omin = [0f64;3];
    }
    return omin;
}

fn main() {
    println!("Hello, world!");
}
