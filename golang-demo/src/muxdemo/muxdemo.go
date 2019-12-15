package muxdemo

import (
	"encoding/json"
	"fmt"
	"github.com/gorilla/mux"
	"net/http"
	"log"
	"path/filepath"
	"os"
	"time"
)


type spaHandler struct {
	staticPath string
	indexPath  string
}

func (h spaHandler)ServeHTTP(w http.ResponseWriter, r *http.Request)  {
	fmt.Println("=========Enter, spaHandler()==========")
	defer fmt.Println("=========Exit, spaHandler()==========")
	path, err := filepath.Abs(r.URL.Path)
	if err != nil {
		http.Error(w, err.Error(), http.StatusBadRequest) //400
		return
	}

	path = filepath.Join(h.staticPath, path)

	_, err = os.Stat(path)
	if os.IsNotExist(err) {
		http.ServeFile(w, r,filepath.Join(h.staticPath, h.indexPath))
		return
	} else if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError) //500
		return
	}

	http.FileServer(http.Dir(h.staticPath)).ServeHTTP(w, r)
}

func HomeHandler(w http.ResponseWriter, r *http.Request)  {
	fmt.Println("=========Enter, HomeHandler()==========")
	defer fmt.Println("=========Exit, HomeHandler()==========")

	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "/Home")
}

func ProductsHandler(w http.ResponseWriter, r *http.Request)  {
	fmt.Println("=========Enter, ProductsHandler()==========")
	defer fmt.Println("=========Exit, ProductsHandler()==========")

	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "/Home/products")
}

func ArticleCategoryHandler(w http.ResponseWriter, r *http.Request)  {
	fmt.Println("=========Enter, ArticleCategoryHandler()==========")
	defer fmt.Println("=========Exit, ArticleCategoryHandler()==========")

	vars := mux.Vars(r)
	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "Category: %v\n", vars["category"])
}


func ArticleHandler(w http.ResponseWriter, r *http.Request)  {
	fmt.Println("=========Enter, ArticleHandler()==========")
	defer fmt.Println("=========Exit, ArticleHandler()==========")

	vars := mux.Vars(r)
	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "Category: %v, Id: %v\n", vars["category"], vars["id"])
}

func muxTest1()  {
	fmt.Println("=========Enter, mux.muxTest1()==========")
	defer fmt.Println("=========Exit, mux.muxTest1()==========")

	r := mux.NewRouter()
	r.HandleFunc("/", HomeHandler).Host("localhost").Methods("GET")
	r.HandleFunc("/products", ProductsHandler)
	r.HandleFunc("/articles/{category}", ArticleCategoryHandler)
	//URL路径支持变量，并且被保存在map中，可以用mux.Vars(r)提取map
	r.HandleFunc("/articles/{category}/{id:[0-9]+}", ArticleHandler)
	http.Handle("/", r)

	log.Fatal(http.ListenAndServe(":8080", nil))
}

func muxTest2() {
	fmt.Println("=========Enter, mux.muxTest2()==========")
	defer fmt.Println("=========Exit, mux.muxTest2()==========")

	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println(dir)

	r := mux.NewRouter()
	
	// This will serve files under http://localhost:8000/static/<filename>
	r.PathPrefix("/static/").Handler(http.StripPrefix("/static/", http.FileServer(http.Dir(dir))))

	srv := &http.Server{
		Handler:   r,
		Addr:     "127.0.0.1:8080",
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}

func muxTest3() {
	fmt.Println("=========Enter, mux.muxTest3()==========")
	defer fmt.Println("=========Exit, mux.muxTest3()==========")

	router := mux.NewRouter()

	router.HandleFunc("api/health", func (w http.ResponseWriter, r *http.Request)  {
		json.NewEncoder(w).Encode(map[string]bool{"ok": true})
	})
/*
	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}
*/
	spa := spaHandler{staticPath: "/Users/hunk.he/work/", indexPath: "index.html"}
	router.PathPrefix("/").Handler(spa)

	srv := &http.Server{
		Handler:   router,
		Addr:     "127.0.0.1:8080",
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}

func Test() {
	//muxTest1()
	//muxTest2()
	muxTest3()
}
