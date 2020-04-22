package word

import "testing"

func TestIsPalindrome(t *testing.T)  {
	var tests  = []struct {
		input string
		want bool
	}{
		{"", true},
		{"a", true},
		{"abba", true},
		{"sdae", false},
		{"中国中", true},
	}
	for _, test := range tests {
		if got := IsPalindrome(test.input); got != test.want {
			t.Errorf("IsPalindrome(%q) = %v", test.input, got)
		}
	}
}

func BenchmarkIsPalindrome(b *testing.B)  {
	for index := 0; index < b.N; index++ {
		IsPalindrome("中国中")
	}
}