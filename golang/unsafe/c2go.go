package main

import "C"

func main() {}

//export Hello
func Hello() string {
	return "Hello"
}

//export Test
func Test() {
	println("export Test")
}

// build .so
// go build -buildmode=c-shared -o libhello.so c2go.go

// build .a
// go build -buildmode=c-archive -o libhello.a c2go.go
