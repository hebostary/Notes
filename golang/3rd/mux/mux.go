package mux

import (
	"context"
	"encoding/json"
	"fmt"
	"log"
	"net/http"
	"os"
	"os/signal"
	"path/filepath"
	"syscall"
	"time"

	"github.com/gorilla/mux"
)

type spaHandler struct {
	staticPath string
	indexPath  string
}

func (h spaHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
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
		http.ServeFile(w, r, filepath.Join(h.staticPath, h.indexPath))
		return
	} else if err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError) //500
		return
	}

	http.FileServer(http.Dir(h.staticPath)).ServeHTTP(w, r)
}

func HomeHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("=========Enter, HomeHandler()==========")
	defer fmt.Println("=========Exit, HomeHandler()==========")

	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "/Home")
}

func ProductsHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("=========Enter, ProductsHandler()==========")
	defer fmt.Println("=========Exit, ProductsHandler()==========")

	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "/Home/products")
}

func ArticleCategoryHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("=========Enter, ArticleCategoryHandler()==========")
	defer fmt.Println("=========Exit, ArticleCategoryHandler()==========")

	vars := mux.Vars(r)
	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "Category: %v\n", vars["category"])
}

func ArticleHandler(w http.ResponseWriter, r *http.Request) {
	fmt.Println("=========Enter, ArticleHandler()==========")
	defer fmt.Println("=========Exit, ArticleHandler()==========")

	vars := mux.Vars(r)
	w.WriteHeader(http.StatusOK)
	fmt.Fprintf(w, "Category: %v, Id: %v\n", vars["category"], vars["id"])
}

func muxServer1() {
	fmt.Println("=========Enter, mux.muxServer1()==========")
	defer fmt.Println("=========Exit, mux.muxServer1()==========")

	r := mux.NewRouter()
	r.HandleFunc("/", HomeHandler).Host("localhost").Methods("GET")
	r.HandleFunc("/products", ProductsHandler)
	r.HandleFunc("/articles/{category}", ArticleCategoryHandler)
	//URL路径支持变量，并且被保存在map中，可以用mux.Vars(r)提取map
	r.HandleFunc("/articles/{category}/{id:[0-9]+}", ArticleHandler)
	http.Handle("/", r)

	log.Fatal(http.ListenAndServe(":8080", nil))
}

func muxServer2() {
	fmt.Println("=========Enter, mux.muxServer2()==========")
	defer fmt.Println("=========Exit, mux.muxServer2()==========")

	dir, err := filepath.Abs(filepath.Dir(os.Args[0]))
	if err != nil {
		log.Fatal(err)
	}

	fmt.Println(dir)

	r := mux.NewRouter()

	// This will serve files under http://localhost:8000/static/<filename>
	r.PathPrefix("/static/").Handler(http.StripPrefix("/static/", http.FileServer(http.Dir(dir))))

	srv := &http.Server{
		Handler:      r,
		Addr:         "127.0.0.1:8080",
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}

func muxServer3() {
	fmt.Println("=========Enter, mux.muxServer3()==========")
	defer fmt.Println("=========Exit, mux.muxServer3()==========")

	router := mux.NewRouter()

	router.HandleFunc("api/health", func(w http.ResponseWriter, r *http.Request) {
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
		Handler:      router,
		Addr:         "127.0.0.1:8080",
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}

	log.Fatal(srv.ListenAndServe())
}

func muxServer4() {
	fmt.Println("=========Enter, mux.muxServer4()==========")
	defer fmt.Println("=========Exit, mux.muxServer4()==========")

	//Initialize router
	router := mux.NewRouter()
	router.StrictSlash(true)

	mw := Auth()

	router.Handle("/api/v1/tasks", applyMiddleware(mw, HandleTasksGet)).Methods("GET")

	http.Handle("/", router)

	port := ":8080"
	srv := &http.Server{
		Addr:         port,
		Handler:      nil,
		WriteTimeout: 15 * time.Second,
		ReadTimeout:  15 * time.Second,
	}

	cleanupDone := make(chan struct{})

	go signalHandler(make(chan os.Signal, 1), srv, cleanupDone)

	fmt.Println("Mux server 4 started, listening on: " + port)

	err := srv.ListenAndServe()
	if err != http.ErrServerClosed {
		fmt.Printf("Failed to start server, port=[%s], err=[%v]\n", port, err)
	} else {
		fmt.Printf("Server on port [%s] is shutdown.\n", port)
	}

	fmt.Println("Server shutdown, waiting for cleanup to be done")
	//Wait for cleanup to complete
	<-cleanupDone

	fmt.Println("Mux server 4 stopped")
}

// handles SIGINT and SIGTERM
func signalHandler(signalChannel chan os.Signal, srv *http.Server, done chan struct{}) {
	signal.Notify(signalChannel, syscall.SIGINT, syscall.SIGTERM)
	for {
		s := <-signalChannel
		switch s {
		case syscall.SIGTERM:
			fmt.Println("received SIGTERM")
		case syscall.SIGINT:
			fmt.Println("received SIGINT")
		}

		if err := cleanUp(srv, done); err != nil {
			fmt.Printf("cleanUp failed, err=[%s]\n", err.Error())
		}
	}
}

func cleanUp(srv *http.Server, done chan struct{}) error {
	if err := srv.Shutdown(context.TODO()); err != nil {
		return err
	}

	// close resources
	time.Sleep(3 * time.Second)

	fmt.Println("cleanup done")
	done <- struct{}{}
	return nil
}
