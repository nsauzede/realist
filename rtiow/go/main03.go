package main

import ("fmt")

type Vec3 [3]float32

func main() {
	var nx = 200
	var ny = 100
	fmt.Printf("P3\n")
	fmt.Printf("%d %d\n", nx, ny)
	fmt.Printf("%d\n", 255)
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			col := Vec3{float32(i) / float32(nx), float32(j) / float32(ny), float32(0.2)}
			ir := int(255.99 * col[0])
			ig := int(255.99 * col[1])
			ib := int(255.99 * col[2])
			fmt.Printf("%d %d %d\n", ir, ig, ib)
		}
	}
}
