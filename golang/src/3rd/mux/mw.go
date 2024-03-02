package mux

import (
	"fmt"
	"net/http"
)

// auth middleware
func Auth() func(h http.Handler) http.Handler {
	return func(h http.Handler) http.Handler {
		return http.HandlerFunc(func(w http.ResponseWriter, r *http.Request) {
			subjectstr := r.Header.Get("Authenticated-Subject")
			if subjectstr == "" {
				fmt.Printf("[Error] Missing Authenticated-Subject in request header\n")
				http.Error(w, http.StatusText(http.StatusUnauthorized), http.StatusUnauthorized)
				return
			}
			h.ServeHTTP(w, r)
		})
	}
}

func applyMiddleware(mw func(http.Handler) http.Handler, handler func(http.ResponseWriter, *http.Request) error) http.Handler {
	return mw(AppHandler(handler))
}
