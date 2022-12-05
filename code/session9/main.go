package main

import (
	"fmt"
	"math/rand"
	"time"
)

func main() {
	rand.Seed(int64(time.Now().Nanosecond()))
	i := rand.Intn(10)
	a := callbackTP(i)
	fmt.Println(a)
}

// go:noinline
func callbackTP(i int) int {
	if i < 5 {
		return i
	}

	for a := 0; a < i; a++ {
		fmt.Printf("xxxxx %d", a)
	}

	return 99
}
