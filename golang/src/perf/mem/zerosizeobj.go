package mem

import "fmt"

//有一些对象所需的内存大小是0，比如 [0]int, struct{}，
//这种类型的数据根本就不需要内存，所以没必要走复杂地内存分配逻辑。
//系统会直接返回一个固定的内存地址
func outZeroSizeObj() {
	var (
		a struct{}
		b [0]int
		c [100]struct{}
		d = make([]struct{}, 1024)
	)
	fmt.Printf("%p\n", &a)
	fmt.Printf("%p\n", &b)
	fmt.Printf("%p\n", &c)
	fmt.Printf("%p\n", &(d[0]))
	fmt.Printf("%p\n", &(d[1]))
	fmt.Printf("%p\n", &(d[1000]))
}
