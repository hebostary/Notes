package word

import "unicode"
import "fmt"

func IsPalindrome(s string) bool {
	var letters []rune
	//range在遍历字符串时，会自动隐式的解析Unicode字符
	for _, r := range s {
		if unicode.IsLetter(r) {
			letters = append(letters, unicode.ToLower(r))
		}
	}

	for i := range letters {
		if letters[i] != letters[len(letters)-1-i] {
			return false
		}
	}

	return true
}

func Test()  {
	fmt.Println(IsPalindrome("aass"))
}

