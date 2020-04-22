dir = $(shell pwd)

export GOPATH:=$(dir)

# build example
# make module=list

all: build run

build:
	go build -o ./bin/exe exe

run:
	./bin/exe --module=$(module)
