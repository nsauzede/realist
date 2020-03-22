package main

import ("fmt")

func main() {
	var nx = 200
	var ny = 100
	fmt.Printf("P3\n")
	fmt.Printf("%d %d\n", nx, ny)
	fmt.Printf("%d\n", 255)
	for j := ny-1; j >= 0; j-- {
		for i := 0; i < nx; i++ {
			r := float32(i) / float32(nx)
			g := float32(j) / float32(ny)
			b := float32(0.2)
			ir := int(255.99 * r)
			ig := int(255.99 * g)
			ib := int(255.99 * b)
			fmt.Printf("%d %d %d\n", ir, ig, ib)
		}
	}
}
