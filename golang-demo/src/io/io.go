package io

import (
		"fmt"
	"io"
	"os"
	"strings"
)

type rot13Reader struct {
	r io.Reader
}

func rot13(p byte) byte {
	switch {
	case p >= 'A' && p <= 'M':
		p += 13
	case p >= 'N' && p <= 'Z':
		p -= 13
	case p >= 'a' && p <= 'm':
		p += 13
	case p >= 'n' && p <= 'z':
		p -= 13
	}
	return p
}

func (reader rot13Reader) Read(p []byte) (int, error) {
	buffer := make([]byte, len(p))

	n, err := reader.r.Read(buffer)
	for index, byte := range buffer {
		p[index] = rot13(byte)
	}

	return n, err
}

func ioTest2() {
	fmt.Println("=========Enter, IoTest.ioTest2()==========")
	defer fmt.Println("=========Exit, IoTest.ioTest2()==========")
	s := strings.NewReader("Lbh penpxrq gur pbqr!\n")
	r := rot13Reader{s}
	io.Copy(os.Stdout, &r) //You cracked the code!
}

/*
Note:
The io package specifies the io.Reader interface, which represents the read end of a stream of data.
The io.Reader interface has a Read method:

func (T) Read(b []byte) (n int, err error)
Read populates the given byte slice with data and returns the number of bytes populated and an error value. It returns an io.EOF error when the stream ends.

The example code creates a strings.Reader and consumes its output 8 bytes at a time.
*/

func ioTest1() {
	fmt.Println("=========Enter, IoTest.ioTest1()==========")
	defer fmt.Println("=========Exit, IoTest.ioTest1()==========")

	r := strings.NewReader("Hello, hunk!")
	b := make([]byte, 8)

	for {
		n, err := r.Read(b)
		fmt.Printf("n = %v, err = %v b = %v", n, err, b)
		fmt.Printf("b[:n] = %q\n", b[:n])
		if err == io.EOF {
			break
		}
	}
}

func Test() {
	ioTest1()
	ioTest2()
}
