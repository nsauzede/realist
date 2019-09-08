extern crate rayrust;
use rayrust::{V3, solvetri, Sol};

use std::f64;
use std::env;
use std::fs::File;
use std::io::prelude::*;

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

fn intersec( sph1: &Sphere,  o: &V3,  v: &V3) -> f64 {
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

fn trace( o: &V3, v: &V3) -> [f64; 3] {
    let mut tmin = f64::INFINITY;
    let mut omin = [0f64;3];
    //unsigned imin = 0;
    for elem in SPH.iter() {
        let tres = intersec( elem, o, v);
        if (tres > 0f64) && (tres < tmin) {
            tmin = tres;
            omin = elem.color;
        }
    }
    if tmin < f64::INFINITY {
        omin = [0f64;3];
    }
    return omin;
}

fn render( w: u32, h: u32, fnameout: String) {
// screen
    let m_ww = 1f64;
    let m_hh = m_ww * (h as f64) / (w as f64);
    // camera
    let cam = [
        V3 { x: 0.4, y: 0.0, z: 0.4},               // eye
        V3 { x: -1.0, y: 0.0, z: -1.0},             // front towards screen
        V3 { x: -0.707107, y: 0.0, z: 0.707107},    // up along screen
    ];
    let m_e = cam[0].clone();   // eye position
    let m_f = cam[1].clone();   // front towards screen
    let mut m_u = cam[2].clone();   // up along screen
    //v3 m_r;   // right along screen (computed)

    m_u.normalize();
    let mut m_r = V3::cross(&m_f, &m_u);      // compute right
    m_u = V3::cross(&m_r, &m_f);      // re-compute up
    m_u.normalize();
    m_r.normalize();
    let mut fout = File::create(fnameout).expect("file not found");
    let _ = write!(fout, "P3\n");
    let _ = write!(fout,"{} {}\n", w, h);
    let _ = write!(fout,"{}\n", 100);

    //let mut v = V3 {x: 0f64, y: 0f64, z: 0f64};
    for jj in 0..h {
        let vu = &m_u * (((h as f64) - (jj as f64) - 1f64 - (h as f64)/ 2.0) / 
            (h as f64) * m_hh);
//      vprintn( "rayc vu", vu);
//      getchar();
        for ii in 0..w {
            let vr = &m_r * (((ii as f64) - (w as f64) / 2.0) / (w as f64) 
                * m_ww);
            let mut v = &vu + &vr;
            v = &v + &m_f;
            v.normalize();
            let color = trace( &m_e, &v);
            let _ = write!(fout, "{:2.0} {:2.0} {:2.0}   ", 100.0 * color[0],
                100.0 * color[1], 100.0 * color[2]);
        }
        let _ = write!(fout, "\n");
    }
    // need to flush here ?
}


fn main() {
    let mut args = env::args();
    args.next();
    let w: u32 = args.next()
                     .unwrap()
                     .parse()
                     .expect("width missing or incorrect");
    let h: u32 = args.next()
                     .unwrap()
                     .parse()
                     .expect("height missing or incorrect");
    let file_name = args.next().expect("require file output");
    render(w, h, file_name);
}
