package mux

import (
	"encoding/json"
	"fmt"
	"net/http"
)

// AppHandler handles error response of API handlers
type AppHandler func(http.ResponseWriter, *http.Request) error

func (fn AppHandler) ServeHTTP(w http.ResponseWriter, r *http.Request) {
	// Set the content-type response header to application/json
	w.Header().Set("Content-Type", "application/json")

	// Handle handler errors
	if err := fn(w, r); err != nil {
		http.Error(w, err.Error(), http.StatusInternalServerError)
	}
}

func HandleTasksGet(w http.ResponseWriter, r *http.Request) error {
	fmt.Printf("Entering HandleTasksGet...\n")
	defer fmt.Printf("Exiting HandleTasksGet: method=%s\n", r.Method)

	m := make(map[string]interface{})
	m["tasks"] = "abcdefg"
	return json.NewEncoder(w).Encode(m)
}
