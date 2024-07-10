package crypto

import (
	"fmt"
	"math/rand"
)

func generate_data_with_size(size int) []byte {
	fmt.Println("\nGenerating data of size [" + convert_blcok_size(size) + "].")

	buf := make([]byte, size)
	charArr := []byte{'a', 'b', 'c', 'd', 'e', 'f', '0', '1', '2', '3', '4', '5', '6', '7'}
	charArrSize := len(charArr)
	for i := 0; i < size; i++ {
		buf[i] = charArr[rand.Intn(charArrSize)]
	}
	//fmt.Println("Generated data as string:" + string(buf))
	return buf
}

func convert_blcok_size(size int) string {
	size_str := fmt.Sprint(size) + " Bytes"
	if size >= MB {
		size_str = fmt.Sprint(size/MB) + " MB"
	} else if size >= KB {
		size_str = fmt.Sprint(size/KB) + " KB"
	}
	return size_str
}
