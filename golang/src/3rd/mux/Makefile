build:
	go get github.com/gorilla/mux
	go build
server1:
	go test *.go -v -run TestMuxServer1

server2:
	go test *.go -v -run TestMuxServer2

server3:
	go test *.go -v -run TestMuxServer3

server4:
	go test *.go -v -run TestMuxServer4

client4:
	curl -X GET localhost:8080/api/v1/tasks
	curl -X GET localhost:8080/api/v1/tasks -H "Authenticated-Subject":"clientId"