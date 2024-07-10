package json

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"log"
	"os"
	"testing"
)

const (
	demoProfile = "./profile_demo.json"
)

func read_profile(serverProfile *ServerProfile) error {
	fd, err := os.Open(demoProfile)
	if err != nil {
		log.Fatal(err)
		return err
	}
	defer fd.Close()

	jsonParser := json.NewDecoder(fd)
	if err = jsonParser.Decode(serverProfile); err != nil {
		log.Fatal(err)
		return err
	}

	return nil
}

// Update the prof
func update_proflie(prof *ServerProfile) {
	if prof.ApplicationVersion != "10.0" {
		prof.ApplicationVersion = "10.0"
	}

	for i, _ := range prof.ServerImages {
		if prof.ServerImages[i].ImageTag != "10.0" {
			prof.ServerImages[i].ImageTag = "10.0"
		}

		for i, _ := range prof.ServerImages[i].Volumes {
			if prof.ServerImages[i].Volumes[i].ID == "logs" {
				prof.ServerImages[i].Volumes[i].MountPoint = fmt.Sprintf("/mnt/%s/logs", prof.GUID)
			}
		}

		var volume = ServerProfileVolume{
			ID:          "data",
			DisplayName: "persistent data",
			VolumeType:  "ext4",
			FsType:      "ext4",
			FsOpt:       "rw",
			MinSize:     "1G",
			MaxSize:     "10G",
			MountPoint:  fmt.Sprintf("/mnt/%s/data", prof.GUID),
		}

		prof.ServerImages[i].Volumes = append(prof.ServerImages[i].Volumes, volume)
	}
}

func write_profile(prof *ServerProfile) {
	newDemoFile := demoProfile + ".new1"
	fd, err := os.OpenFile(newDemoFile, os.O_RDWR|os.O_CREATE|os.O_TRUNC, 0644)
	if err != nil {
		log.Fatal(err)
	}
	defer fd.Close()

	jsonParser := json.NewEncoder(fd)
	if err = jsonParser.Encode(prof); err != nil {
		log.Fatal(err)
	}

	newDemoFile = demoProfile + ".new2"
	file, _ := json.MarshalIndent(prof, "", " ")
	_ = ioutil.WriteFile(newDemoFile, file, 0644)
}

func manage_profile() {
	var prof ServerProfile
	err := read_profile(&prof)
	if err != nil {
		panic(err)
	}

	fmt.Println(prof)

	// Update the prof
	update_proflie(&prof)

	fmt.Println(prof)

	// Write the prof
	write_profile(&prof)
}

// go test *.go -v
func TestJson(t *testing.T) {
	manage_profile()
}
