package slice

import (
	"fmt"
)

func arrayAsParamTest() {
	fmt.Println("=========SliceTest.sliceAsParamTest()==========")
	arrayA := [2]int{1, 2}
	var arrayB [2]int
	arrayB = arrayA
	fmt.Printf("Address: %p.\n", &arrayA)
	fmt.Printf("Address: %p.\n", &arrayB)
	arrayAsParam(arrayA)
}

func arrayAsParam(x [2]int) {
	fmt.Printf("Address: %p.\n", &x)
}

type Employee struct {
	name    string
	badgeId int
}

/*
Test slice
*/
func sliceCopy() {
	fmt.Println("=========SliceTest.sliceCopy()==========")
	s := []string{"hunk", "jack", "bob"}
	d := make([]string, 3, 3)
	copy(d, s)
	fmt.Printf("s len=%d cap=%d adress=%p value=%v\n", len(s), cap(s), &s, s)
	fmt.Printf("d len=%d cap=%d adress=%p value=%v\n", len(d), cap(d), &d, d)
	for i, v := range d {
		fmt.Printf("d[%d] is %s\n", i, v)
	}
}

func printSlice(s string, x []int) {
	fmt.Printf("%s len=%d cap=%d adress=%p value=%v\n", s, len(x), cap(x), &x, x)
}

/*
When ranging over a slice, two values are returned for each iteration. The first is the index, and the second is a copy of the element at that index.
*/
func sliceRange() {
	fmt.Println("=========SliceTest.sliceRange()=========")
	var pow = []int{1, 2, 4, 8}

	for i, v := range pow {
		fmt.Printf("value = %d, value.address = %p, slice-address = %p.\n", v, &v, &pow[i])
		pow[i] = 100
	}

	for i := range pow {
		fmt.Printf("%d\n", i)
	}

	for _, v := range pow {
		fmt.Printf("%d\n", v)
	}
}

func sliceAppend() {
	fmt.Println("=========SliceTest.sliceAppend()=========")
	var s []int
	printSlice("s", s)

	// append works on nil slices.
	s = append(s, 0)
	printSlice("s", s)

	// The slice grows as needed.
	//s = append(s, 1)
	//printSlice("s", s)

	// We can add more than one element at a time.
	s = append(s, 2, 3, 4, 5, 6, 7)
	printSlice("s", s)

	s = append(s, 5, 6)
	printSlice("s", s)
}

func slice() {
	fmt.Println("=========SliceTest.slice()========")
	a := make([]int, 5) //len = 5, cap = 5
	printSlice("a", a)

	b := make([]int, 0, 5) //len = 0, cap = 5
	printSlice("b", b)

	c := b[:2] //len = 2 , cap = 5
	printSlice("c", c)

	d := c[2:5] //len = 3, cap = 3
	printSlice("d", d)
}

/*
Test array and slice
*/
func arraySlice() {
	fmt.Println("=========SliceTest.arraySlice()=========")
	var a5 [5]int
	fmt.Println(a5)

	s3 := [3]string{"hunk", "bob", "hank"}
	fmt.Println(s3)

	eArray3 := [3]Employee{{"hunk", 310144}, {"bob", 123456}, {"hank", 234567}}
	fmt.Println("Base array value: ", eArray3)

	//slice not store any data, it just descirbles a section of an underlying array
	var eSlice []Employee = eArray3[0:2]
	fmt.Println("len of slice: ", len(eSlice), ", cap:", cap(eSlice))
	fmt.Println("Slice of array: ", eSlice)
	eSlice[0].name = "Nunk"
	fmt.Println("Base array value:", eArray3)
	eSlice[1].name = "Dunk"
	fmt.Println("Slice of array: ", eSlice)
	fmt.Println("Base array value:", eArray3)
	eSlice = eSlice[1:2]
	fmt.Println("len of slice: ", len(eSlice), ", cap:", cap(eSlice))

	//creat a array literal
	bArray3 := [3]bool{true, false, true}
	//create a same array literal as above, then build a slice references it
	sArray3 := []bool{true, false, true}
	fmt.Println("Bool array: ", bArray3)
	fmt.Println("Bool slice: ", sArray3)

	//nil slice
	var s []int
	fmt.Println(s, len(s), cap(s))
	if s == nil {
		fmt.Println("nil!")
	}
}
func Test() {
	arraySlice()
	slice()
	sliceAppend()
	sliceRange()
	sliceCopy()
	arrayAsParamTest()
}
