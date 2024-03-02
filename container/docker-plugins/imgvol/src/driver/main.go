package main

import (
	"flag"
	"fmt"
	"os"

	"github.com/docker/go-plugins-helpers/volume"
	"github.com/sirupsen/logrus"
)

const (
	imgvolHome = "/home/docker/imgvol/mp"
)

var (
	flVersion *bool
	flDebug   *bool

	tfSize  = "208896"
	tfFStyp = "ext4"
	tfPath  = "/home/docker/imgvol/data"
	tmOpts  = ""
)

func init() {
	flVersion = flag.Bool("version", false, "Print version information and quit.")
	flDebug = flag.Bool("debug", false, "Enable debug logging.")
}

func main() {
	flag.Parse()

	if *flVersion {
		fmt.Fprint(os.Stdout, "Docker imgvol plugin version: 1.0.0\n")
		return
	}

	if *flDebug {
		logrus.SetLevel(logrus.DebugLevel)
	}

	if _, err := os.Stat(imgvolHome); err != nil {
		if !os.IsNotExist(err) {
			logrus.Fatal(err)
		}

		logrus.Debugf("Created home dir at %s.", imgvolHome)
		if err := os.MkdirAll(imgvolHome, 0700); err != nil {
			logrus.Fatal(err)
		}
	}

	imgvol, err := newDriver(tfSize, tfPath, tfFStyp, tmOpts, imgvolHome)
	if err != nil {
		logrus.Fatalf("Error initializing imgvolDriver %v.", err)
	}

	h := volume.NewHandler(imgvol)
	if err := h.ServeUnix("imgvol", 0); err != nil {
		logrus.Fatal(err)
	}
}
