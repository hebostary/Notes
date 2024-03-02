package main

/*
 * This class extends the volume driver helper class.
 */
import (
	"fmt"
	"log/syslog"
	"os"
	"os/exec"
	"path"
	"path/filepath"
	"strings"
	"sync"

	"github.com/docker/go-plugins-helpers/volume"
	"github.com/spf13/afero"
)

var (
	fileSystem                 = afero.NewOsFs()
	fExecCommandCombinedOutput = func(name string, args ...string) ([]byte, error) {
		return exec.Command(name, args...).CombinedOutput()
	}
	fexecCommand = exec.Command
	filepathGlob = filepath.Glob
	fsyslogNew   = syslog.New
)

type imgvolDriver struct {
	vsize  string
	vpath  string
	fstyp  string
	mopts  string
	mhome  string
	mutex  sync.RWMutex
	logger *syslog.Writer
}

func newDriver(vsize, vpath, fstyp, mopts, mhome string) (*imgvolDriver, error) {
	logger, err := fsyslogNew(syslog.LOG_ERR, "containers-imgvol-plugin")
	if err != nil {
		return nil, err
	}

	return &imgvolDriver{
		vsize:  vsize,
		vpath:  vpath,
		fstyp:  fstyp,
		mhome:  mhome,
		mopts:  mopts,
		logger: logger,
	}, nil
}

func getVolumePath(vpath, fstyp, name string) string {
	s := []string{path.Join(vpath, name), "img"}
	return strings.Join(s, ".")
}

func getVolumeRegx(vpath, fstyp string) string {
	return path.Join(vpath, "*.img")
}

func getVolumeName(vpath string) string {
	return strings.TrimSuffix(path.Base(vpath), ".img")
}

func getMountHome(mhome, fstyp string) string {
	return mhome
}

func getMountPath(mhome, fstyp, name string) string {
	return path.Join(getMountHome(mhome, fstyp), name)
}

func (vd *imgvolDriver) Create(req *volume.CreateRequest) error {
	var (
		source  string
		volsize string
	)

	vd.mutex.Lock()
	defer vd.mutex.Unlock()

	// Prepare the path of the target volume.
	vpath := getVolumePath(vd.vpath, vd.fstyp, req.Name)

	// Do nothing if the target volume already exists.
	if _, err := fileSystem.Stat(vpath); !os.IsNotExist(err) {
		vd.logger.Err(fmt.Sprintf("Create: error: target volume exists. %s.", err))
		return err
	}

	// Fetch create volume command line options.
	for key, value := range req.Options {
		if key == "size" {
			volsize = value
		} else if key == "source" {
			source = value
		}
	}

	// Create snapshot volume, if requested.
	if source != "" {
		snapof := getVolumePath(vd.vpath, vd.fstyp, source)

		if out, err := fExecCommandCombinedOutput("cp", "-np", snapof, vpath); err != nil {
			vd.logger.Err(fmt.Sprintf("Create: snapshot error: %s. %s.", err, string(out)))
			return err
		}
	} else {
		if volsize == "" {
			volsize = vd.vsize
		}

		ofvol := fmt.Sprintf("of=%s", vpath)
		count := fmt.Sprintf("count=%s", volsize)
		if out, err := fExecCommandCombinedOutput("dd", "if=/dev/zero", ofvol, count); err != nil {
			vd.logger.Err(fmt.Sprintf("Create: dd error: %s. %s.", err, string(out)))
			return err
		}

		if out, err := fExecCommandCombinedOutput("mkfs", "-t", vd.fstyp, "-F", vpath); err != nil {
			fexecCommand("rm", "-f", vpath)
			vd.logger.Err(fmt.Sprintf("Create: mkfs error: %s. %s.", err, string(out)))
			return err
		}
	}

	return nil
}

func (vd *imgvolDriver) Remove(req *volume.RemoveRequest) error {
	vpath := getVolumePath(vd.vpath, vd.fstyp, req.Name)
	if _, err := fileSystem.Stat(vpath); os.IsNotExist(err) {
		return nil
	}

	if out, err := fExecCommandCombinedOutput("rm", "-f", vpath); err != nil {
		vd.logger.Err(fmt.Sprintf("Remove: rm error: %s. %s.", err, string(out)))
		return err
	}

	return nil
}

func (vd *imgvolDriver) Path(req *volume.PathRequest) (*volume.PathResponse, error) {
	mount := getMountPath(vd.mhome, vd.fstyp, req.Name)
	return &volume.PathResponse{Mountpoint: mount}, nil
}

func (vd *imgvolDriver) Mount(req *volume.MountRequest) (*volume.MountResponse, error) {
	vd.mutex.Lock()
	defer vd.mutex.Unlock()

	mount := getMountPath(vd.mhome, vd.fstyp, req.Name)
	vpath := getVolumePath(vd.vpath, vd.fstyp, req.Name)
	mopts := "loop"
	if vd.mopts != "" {
		mopts = fmt.Sprintf("%s,%s", mopts, vd.mopts)
	}

	err1 := fileSystem.MkdirAll(mount, 0700)
	if err1 != nil {
		vd.logger.Err(fmt.Sprintf("Mount: mkdir error: %s.", err1))
		return &volume.MountResponse{Mountpoint: ""}, err1
	}

	out, err2 := fExecCommandCombinedOutput("mount", "-o", mopts, "-t", vd.fstyp, vpath, mount)
	if err2 != nil {
		vd.logger.Err(fmt.Sprintf("Mount: mount error: %s. %s.", err2, string(out)))
	}

	return &volume.MountResponse{Mountpoint: mount}, err2
}

func (vd *imgvolDriver) Unmount(req *volume.UnmountRequest) error {
	vd.mutex.Lock()
	defer vd.mutex.Unlock()

	mount := getMountPath(vd.mhome, vd.fstyp, req.Name)

	out, err := fExecCommandCombinedOutput("umount", mount)
	if err != nil {
		//  Ignore error, if the imgvol is already unmounted
		if strings.Contains(string(out), "not mounted") {
			vd.logger.Debug(fmt.Sprintf("Unmount: umount error: %s. %s, ignoring", err, string(out)))
			return nil
		}
		vd.logger.Err(fmt.Sprintf("Unmount: umount error: %s. %s.", err, string(out)))
	}

	return err
}

func (vd *imgvolDriver) Get(req *volume.GetRequest) (*volume.GetResponse, error) {
	vd.mutex.RLock()
	defer vd.mutex.RUnlock()

	vpath := getVolumePath(vd.vpath, vd.fstyp, req.Name)
	_, err := fileSystem.Stat(vpath)

	mount := getMountPath(vd.mhome, vd.fstyp, req.Name)

	vol := &volume.Volume{Name: req.Name, Mountpoint: mount}
	res := &volume.GetResponse{Volume: vol}

	return res, err
}

func (vd *imgvolDriver) List() (*volume.ListResponse, error) {
	vd.mutex.RLock()
	defer vd.mutex.RUnlock()

	var vols []*volume.Volume

	vfiles, err := filepathGlob(getVolumeRegx(vd.vpath, vd.fstyp))

	for _, vfile := range vfiles {
		name := getVolumeName(vfile)
		mount := getMountPath(vd.mhome, vd.fstyp, name)
		vol := &volume.Volume{Name: name, Mountpoint: mount}
		vols = append(vols, vol)
	}

	return &volume.ListResponse{Volumes: vols}, err
}

func (vd *imgvolDriver) Capabilities() *volume.CapabilitiesResponse {
	capability := volume.Capability{Scope: "local"}
	return &volume.CapabilitiesResponse{Capabilities: capability}
}
