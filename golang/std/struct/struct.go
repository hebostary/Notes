package structTest

import (
	"fmt"
)

/*
value type, copy data when as param and assignment
*/

type Resource struct {
	id   int
	name string
}

type Classify struct {
	id int
}

type Student struct {
	Resource
	//*Resource //compile error: duplicate field Resource
	Classify
	name string
}

func structTest3() {
	fmt.Println("=========Enter, StructTest.structTest3()==========")
	defer fmt.Println("=========Exit, StructTest.structTest3()==========")
	s := Student{
		Resource{1, "people"},
		Classify{12},
		"hunk",
	}
	fmt.Println(s.name)
	fmt.Println(s.Resource.name)
	//fmt.Println(s.id) //compile error: ambiguous selector s.id
	fmt.Println(s.Classify.id)
}

type User struct {
	name string
}

type Manager struct {
	User
	title string
}

func structTest2() {
	fmt.Println("=========Enter, StructTest.structTest2()==========")
	defer fmt.Println("=========Exit, StructTest.structTest2()==========")
	m := Manager{
		User:  User{"hunk"},
		title: "Administrator",
	}
	fmt.Println(m)
}

type File struct {
	name string
	size int
	attr struct {
		perm  int
		owner int
	}
}

func structTest1() {
	fmt.Println("=========Enter, StructTest.structTest1()==========")
	defer fmt.Println("=========Exit, StructTest.structTest1()==========")

	f := File{
		name: "test.txt",
		size: 1025,
		//attr: {0755, 1}, //compile error:missing type in composite literal
	}
	fmt.Println(f)

	f.attr.owner = 1
	f.attr.perm = 0755
	fmt.Println(f) //output:{test.txt 1025 {493 1}}

	var att = struct {
		perm  int
		owner int
	}{2, 0755}
	f.attr = att
	fmt.Println(f) //output:{test.txt 1025 {2 493}}
}

/*
Test struct
*/
type Employee struct {
	name    string
	badgeId int
}

func structTest0() {
	fmt.Println("=========Enter, StructTest.structTest0()==========")
	defer fmt.Println("=========Exit, StructTest.structTest0()==========")

	fmt.Println(Employee{"hunk", 310144})

	jackObj := Employee{"jack", 129239}
	jackObj.badgeId = 123456
	fmt.Println(jackObj)

	p := &jackObj
	p.name = "jack1"
	fmt.Println(jackObj)

	var (
		bob  = Employee{"bob", 234567}
		hank = Employee{"hank", 345678}
		p1   = &bob
	)
	fmt.Println(bob, hank, p1)

}

func Test() {
	structTest0()
	structTest1()
	structTest2()
	structTest3()
}
