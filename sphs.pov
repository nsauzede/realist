// Pyramid of 35 spheres POV-Ray source by Blotwell
// Updated by Michael Horvath
// Uploaded to Wikimedia Commons and released under GPL
// +KC +KFI0 +KFF23 +ki0 +kf1
// +KC +KFI0 +KFF11 +ki0 +kf0.5
// +KC +KFI12 +KFF23 +ki0.5 +kf1
// Render frames 0 through 23 (24 is identical to 0)

// AXES MACRO AVAILABLE FROM THE POVRAY OBJECT COLLECTION
#include "Axes_macro.inc"
Axes_Macro
(
	1000,	// Axes_axesSize,	The distance from the origin to one of the grid's edges.		(float)
	1,	// Axes_majUnit,	The size of each large-unit square.					(float)
	10,	// Axes_minUnit,	The number of small-unit squares that make up a large-unit square.	(integer)
	0.0001,	// Axes_thickRatio,	The thickness of the grid lines (as a factor of axesSize).		(float)
	off,	// Axes_aBool,		Turns the axes on/off.							(boolian)
	off,	// Axes_mBool,		Turns the minor units on/off. 						(boolian)
	off,	// Axes_xBool,		Turns the plane perpendicular to the x-axis on/off.			(boolian)
	on,	// Axes_yBool,		Turns the plane perpendicular to the y-axis on/off.			(boolian)
	off	// Axes_zBool,		Turns the plane perpendicular to the z-axis on/off.			(boolian)
)
object {Axes_Object}

plane
{
	-y,0.00001
	pigment {color rgbt <0,0,0,3/4>}
}

global_settings
{
	assumed_gamma	1.8
	adc_bailout	0.005
	max_trace_level	50
	radiosity
	{
		pretrace_start	0.08
		pretrace_end	0.01
		count		50
		error_bound	0.1
		recursion_limit	1
		normal		on
		brightness	0.8
		always_sample	yes
		gray_threshold	0.8
		media		on
	}
}

sky_sphere
{
	pigment
	{
		gradient y
		color_map
		{
			[0.0 rgb <0.6,0.7,1.0>*2]
			[0.7 rgb <0.0,0.1,0.8>*4]
//			[0.0 rgb <0.6,0.7,1.0>/2 + 2/4]
//			[0.7 rgb <0.0,0.1,0.8>/2 + 2/4]
		}
	}
}

background {rgb 1}

light_source
{
	<0, 0, 0>
	color rgb <1, 1, 1>
	translate <-30, 30, -30>
	parallel
}

light_source
{
	<0, 0, 0>
	color rgb <1, 1, 1>
	translate <-30, 30, -30>
	rotate y * 90
	parallel
}

#declare sq = 1/sqrt(2);
#declare vx = <2,0,0>;
#declare vy = <1,0,sqrt(3)>;
#declare vz = <1,sqrt(8/3),sqrt(1/3)>;

camera
{
	location	<-15,+11,-12>
	up		y * 1/2
	right		x * 1/2 * image_width/image_height
	look_at		4 * vz - y * 3

	focal_point	4 * vz - y * 3
	aperture	1/2
	blur_samples	20
}

#declare my_sphere_1 = sphere
{
	<0,1,0>, 1
	pigment { color rgbf <0.9,1.0,0.9,0.9> }
	finish { refraction 1 ior 1.2 }
}
#declare my_sphere_2 = sphere
{
	<0,1,0> + vz, 1
	pigment { color rgbf <0.9,0.9,1.0,0.9> }
	finish { refraction 1 ior 1.2 }
}
#declare my_sphere_3 = sphere
{
	<0,1,0> + vz + vz, 1
	pigment { color rgbf <1.0,1.0,0.8,0.9> }
	finish { refraction 1 ior 1.2 }
}
#declare my_sphere_4 = sphere
{
	<0,1,0> + vz + vz + vz, 1
	pigment { color rgbf <1.0,0.9,0.9,0.9> }
	finish { refraction 1 ior 1.2 }
}
#declare my_sphere_5 = sphere
{
	<0,1,0> + vz + vz + vz + vz, 1
	pigment { color rgbf <0.9,0.9,0.9,0.9> }
	finish { refraction 1 ior 1.2 }
}

#declare my_sphere_6 = union
{
	object { my_sphere_1 translate 0 }
	object { my_sphere_1 translate vx }
	object { my_sphere_1 translate vy }
	object { my_sphere_1 translate vx+vx }
	object { my_sphere_1 translate vx+vy }
	object { my_sphere_1 translate vy+vy }
	object { my_sphere_1 translate vx+vx+vx }
	object { my_sphere_1 translate vx+vx+vy }
	object { my_sphere_1 translate vx+vy+vy }
	object { my_sphere_1 translate vy+vy+vy }
	object { my_sphere_1 translate vx+vx+vx+vx }
	object { my_sphere_1 translate vx+vx+vx+vy }
	object { my_sphere_1 translate vx+vx+vy+vy }
	object { my_sphere_1 translate vx+vy+vy+vy }
	object { my_sphere_1 translate vy+vy+vy+vy }

	object { my_sphere_2 translate 0 }
	object { my_sphere_2 translate vx }
	object { my_sphere_2 translate vy }
	object { my_sphere_2 translate vx+vx }
	object { my_sphere_2 translate vx+vy }
	object { my_sphere_2 translate vy+vy }
	object { my_sphere_2 translate vx+vx+vx }
	object { my_sphere_2 translate vx+vx+vy }
	object { my_sphere_2 translate vx+vy+vy }
	object { my_sphere_2 translate vy+vy+vy }

	object { my_sphere_3 translate 0 }
	object { my_sphere_3 translate vx }
	object { my_sphere_3 translate vy }
	object { my_sphere_3 translate vx+vx }
	object { my_sphere_3 translate vx+vy }
	object { my_sphere_3 translate vy+vy }

	object { my_sphere_4 translate 0 }
	object { my_sphere_4 translate vx }
	object { my_sphere_4 translate vy }

	object { my_sphere_5 translate 0 }

	translate	-4 * vz
	rotate		y * clock * 120
	translate	+4 * vz
}

object {my_sphere_6}
