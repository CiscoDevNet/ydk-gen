package path

import (
	"github.com/CiscoDevNet/ydk-go/ydk"
	oc_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/openconfig/bgp"
	"github.com/CiscoDevNet/ydk-go/ydk/path"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"testing"
)

func TestExecuteRpc(t *testing.T) {
	ydk.EnableLogging(ydk.Info)
	provider := providers.NetconfServiceProvider{Address: "127.0.0.1", Username: "admin", Password: "admin", Port: 12022}
	provider.Connect()

	bgp := oc_bgp.Bgp{}
	result := path.ExecuteRpc(&provider, &bgp, "ydk:create", "entity", false)
	if result.Private == nil {
		t.Error("Operation failed")
	}

	provider.Disconnect()
}
