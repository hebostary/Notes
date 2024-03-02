init:
	go get github.com/docker/go-plugins-helpers/volume
	go get github.com/sirupsen/logrus@v1.9.3
	go get github.com/spf13/afero

	mkdir -p /home/docker/imgvol/data

build:
	go build

run:
	./imgvol &

foobar:
	docker volume create --driver imgvol --opt size=208896 foobar