package mapTest

import (
	"fmt"
	"strings"
)

type user struct {
	name string
}

func mapTest2() {
	fmt.Println("=========Enter, MapTest.mapTest2()==========")
	defer fmt.Println("=========Exit, MapTest.mapTest2()==========")
	m := map[int]string{
		1: "hunk",
	}
	m[1] = "jack"
	fmt.Println(m)

	m1 := map[int]user{
		1: {"hunk"},
	}
	//m1[1].name = "jack" //compile error:cannot assign to struct field m1[1].name in map, because the m1[1].name only get a temporary copy value
	v := m1[1]
	v.name = "jack"
	m1[1] = v //replace the whole value
	fmt.Println(m1)

	m2 := map[int]*user{ //get the copy of the pointer
		1: &user{"hunk"},
	}
	m2[1].name = "jack"
	fmt.Println(m2[1].name)
}

func wordCount(s string) map[string]int {
	fmt.Println("=========Enter, MapTest.wordCount()==========")
	defer fmt.Println("=========Exit, MapTest.wordCount()==========")
	fmt.Println("Input string: ", s)
	wordlist := strings.Fields(s)
	countMap := make(map[string]int)
	for _, word := range wordlist {
		_, ok := countMap[word]
		if ok {
			countMap[word] += 1
		} else {
			countMap[word] = 1
		}
	}
	fmt.Println("Output map is: ", countMap)
	return countMap
}

type Vertex struct {
	Lat, Long float64
}

func mapTest1() {
	fmt.Println("=========Enter, MapTest.mapTest1()==========")
	defer fmt.Println("=========Exit, MapTest.mapTest1()==========")
	var m map[string]Vertex
	m = make(map[string]Vertex)
	m["Bell Labs"] = Vertex{
		23.2323, -233.34,
	}
	fmt.Println(m["Bell Labs"])

	//map literals
	var m1 = map[string]Vertex{
		"Bell Labs": Vertex{
			10.10, 12.12,
		},
		"Google": Vertex{
			13.13, 14.14,
		},
	}
	fmt.Println(m1)

	delete(m1, "Google")
	elem, ok := m1["Google"]
	if ok {
		fmt.Println("Google is a key of m1, value: ", elem)
	} else {
		fmt.Println("Google is not a key of m1.")
	}
}

func Test() {
	mapTest1()
	mapTest2()
	wordCount("a b c ad dc aewr dc ad  ad sda saaf dsf ds fd dc s ad ")
}
