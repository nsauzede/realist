: r/ ( n n -- r )  swap s>f s>f f/ ;
: .notrail  s>d 0 d.r ;
variable nx 200 nx !
variable ny 100 ny !
: main
." P3" cr
nx ? ny @ .notrail cr
255 .notrail cr
0 ny @ 1 - do
	nx @ 0 do
		( b) 0.2e
		( g) j ny @ r/
		( r) i nx @ r/
		( ir) 255.99e f* f>s .
		( ig) 255.99e f* f>s .
		( ib) 255.99e f* f>s .notrail
		cr
	loop
-1 +loop
; main bye
