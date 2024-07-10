package crawl

import (
	"bufio"
	"fmt"
	"net/http"
	"os"
	"strings"

	"golang.org/x/net/html"
)

func extract(url string) ([]string, error) {
	resp, err := http.Get(url)
	if err != nil {
		return nil, err
	}
	if resp.StatusCode != http.StatusOK {
		resp.Body.Close()
		return nil, fmt.Errorf("getting %s: %s", url, resp.Status)
	}
	doc, err := html.Parse(resp.Body)
	resp.Body.Close()
	if err != nil {
		return nil, fmt.Errorf("Parsing %s as HTML: %v", url, err)
	}

	var links []string
	visitNode := func(n *html.Node) {
		if n.Type == html.ElementNode && n.Data == "a" {
			for _, a := range n.Attr {
				if a.Key != "href" {
					continue
				}
				link, err := resp.Request.URL.Parse(a.Val)
				if err != nil {
					continue
				}
				links = append(links, link.String())
			}
		}
	}
	forEachNode(doc, visitNode, nil)
	return links, nil
}

func forEachNode(n *html.Node, pre, post func(n *html.Node)) {
	if pre != nil {
		pre(n)
	}
	for c := n.FirstChild; c != nil; c = c.NextSibling {
		forEachNode(c, pre, post)
	}
	if post != nil {
		post(n)
	}
}

var tokens = make(chan struct{}, 20)

func crawl(url string) []string {
	fmt.Println(url)
	tokens <- struct{}{}
	list, err := extract(url)
	<-tokens
	if err != nil {
		fmt.Println(err)
	}
	return list
}

var (
	inputReader *bufio.Reader
	url         string
	err         error
)

func Test() {
	fmt.Println("=========Enter, crawl.Test.()==========")
	defer fmt.Println("=========Exit, crawl.Test.()==========")
	worklist := make(chan []string)
	var n int

	n++
	go func() {
		//worklist <- os.Args[1:]
		inputReader = bufio.NewReader(os.Stdin)
		fmt.Printf("Please input a url:")
		url, err = inputReader.ReadString('\n')
		if err != nil {
			os.Exit(1)
		}
		url = strings.Replace(url, "\n", "", -1)
		list := []string{""}
		list[0] = url
		worklist <- list
	}()

	seen := make(map[string]bool)

	for ; n > 0; n-- {
		list := <-worklist
		for _, link := range list {
			if !seen[link] {
				seen[link] = true
				n++
				go func(link string) {
					worklist <- crawl(link)
				}(link)
			}
		}
	}
}
