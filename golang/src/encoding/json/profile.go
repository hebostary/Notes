package json

type Network struct {
	IPAddress   string `json:"ipaddress" validate:"omitempty,ipv4"`
	IPAddressV6 string `json:"ipaddressv6" validate:"omitempty,ipv6"`
	Hostname    string `json:"hostname" validate:"required,hostname_rfc1123"`
	Interface   string `json:"interface" validate:"omitempty,vlan_name"`
	FQDN        string `json:"fqdn" validate:"omitempty,hostname_rfc1123"`
	DomainName  string `json:"domain-name" validate:"omitempty,hostname_rfc1123"`
	//You can enter up to 3.
	NameServers   []string `json:"nameservers" validate:"omitempty,max=3,dive,omitempty,ip"`
	SearchDomains []string `json:"searchdomains" validate:"omitempty,dive,omitempty,hostname_rfc1123"`
	EtcHostsPath  []string `json:"etc-hosts" validate:"omitempty,dive,omitempty,etc_host_list"`
}

// ServerProfileVolume stores the storage information for an image of an
type ServerProfileVolume struct {
	ID          string `json:"id" validate:"required,uuid_withunderscore"`
	DisplayName string `json:"disp-name" validate:"required,cmd_safe_chars"`
	VolumeType  string `json:"vol_type" validate:"required,uuid_withunderscore"`
	FsType      string `json:"fstype" validate:"omitempty,uuid_withunderscore"`
	FsOpt       string `json:"fsopt" validate:"omitempty,unix_path"`
	MinSize     string `json:"min-size" validate:"omitempty,volume-size"`
	MaxSize     string `json:"max-size" validate:"omitempty,volume-size"`
	VolumeCap   string `json:"volume-cap" validate:"omitempty,volume-size"`
	MountPoint  string `json:"mnt-pnt" validate:"omitempty,unix_path"`
	MountDir    string `json:"mnt-dir" validate:"omitempty,unix_path"`
}

// ServerProfileImage stores the image details for an application in server profile
// There might be multiple images for an application
type ServerProfileImage struct {
	ContainerID    string                 `json:"container_id" validate:"required,uuid_withunderscore"`
	ImageId        string                 `json:"image_id" validate:"omitempty,unix_path"`
	ImageName      string                 `json:"image_name" validate:"required,unix_path"`
	ImageTag       string                 `json:"image_tag" validate:"required,version"`
	ImageTool      string                 `json:"image_tool" validate:"required,unix_path"`
	ImageAddOn     []string               `json:"image_add_on" validate:"omitempty,dive,omitempty,unix_path"`
	HealthCheck    bool                   `json:"health-check" validate:"omitempty"`
	EnvVars        map[string]string      `json:"env_vars" validate:"omitempty,dive,keys,omitempty,uuid_withunderscore,endkeys,omitempty,cmd_safe_chars"`
	Volumes        []ServerProfileVolume  `json:"volumes" validate:"required,dive"`
	NetworkDetails Network                `json:"network" validate:"required"`
	Devices        []string               `json:"devices" validate:"omitempty,dive,omitempty,unix_path"`
	CapAdd         []string               `json:"cap_add" validate:"omitempty,dive,omitempty,uuid_withunderscore"`
	Operations     map[string]interface{} `json:"operations" validate:"omitempty,dive,keys,omitempty,uuid_withunderscore,endkeys,omitempty,datatype_check"`
}

type ServerProfile struct {
	Version                string               `json:"version" validate:"required,version"`
	GUID                   string               `json:"guid" validate:"required,uuid_withunderscore"`
	ID                     string               `json:"id" validate:"required,uuid_withunderscore"`
	Name                   string               `json:"name" validate:"required,uuid_withunderscore"`
	ApplicationName        string               `json:"application_name" validate:"required,uuid_withunderscore"`
	ApplicationDisplayName string               `json:"application_display_name" validate:"required,cmd_safe_chars"`
	ApplicationVersion     string               `json:"application_version" validate:"required,version"`
	ApplicationCategory    string               `json:"application_category" validate:"required,uuid_withunderscore"`
	Tenant                 string               `json:"tenant" validate:"required,tenant_id"`
	ServerImages           []ServerProfileImage `json:"server_images" validate:"required,dive"`
}
