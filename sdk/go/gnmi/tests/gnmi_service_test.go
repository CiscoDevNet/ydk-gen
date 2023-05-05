package test

import (
	"fmt"
	"runtime"
	"time"
	"path/filepath"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	ysanity_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp"
	ysanity_int "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_interfaces"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/CiscoDevNet/ydk-go/ydk/types/ylist"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/stretchr/testify/suite"
	"testing"
)

type GnmiServiceTestSuite struct {
	suite.Suite
	Service  services.GnmiService
	Provider providers.GnmiServiceProvider
}

func (suite *GnmiServiceTestSuite) SetupSuite() {
	_, callerFile, _, _ := runtime.Caller(0)
	executablePath := filepath.Dir(callerFile)
	repopath := executablePath + "/../../../cpp/core/tests/models"
	repo := types.Repository{Path: repopath}
	suite.Provider = providers.GnmiServiceProvider{
		Repo: repo,
		Address:  "127.0.0.1",
		Port:     50051}
	suite.Provider.Connect()
}

func (suite *GnmiServiceTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *GnmiServiceTestSuite) BeforeTest(suiteName, testName string) {
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

/* TODO: Failing for unknown reason only in batch run
func (suite *GnmiServiceTestSuite) TestGetCapabilities() {
	caps := suite.Service.Capabilities(&suite.Provider)
	ydk.YLogDebug(fmt.Sprintf("Server capabilities:\n%v\n", caps))
	suite.NotEqual(strings.Index(caps, "supported-encodings"), -1)
}
*/

func (suite *GnmiServiceTestSuite) TestGetSetDeleteMultiple() {
    // Build interface configuration
    ifc := ysanity_int.Interfaces_Interface{}
    ifc.Name = "Loopback10"
    ifc.Config.Name = "Loopback10"
    ifcs := ysanity_int.Interfaces{}
    ifcs.Interface = append(ifcs.Interface, &ifc)
    
    // Build BGP configuration
    bgp := ysanity_bgp.Bgp{}
    bgp.Global.Config.As = 65172
    neighbor := ysanity_bgp.Bgp_Neighbors_Neighbor{}
    neighbor.NeighborAddress = "172.16.255.2"
    neighbor.Config.NeighborAddress = "172.16.255.2"
    bgp.Neighbors.Neighbor = append(bgp.Neighbors.Neighbor, &neighbor)

    // Create congfiguration on device
    configEC := types.NewConfig(&ifcs, &bgp)
    reply := suite.Service.Set(&suite.Provider, configEC, "replace")
    suite.Equal(reply, true)

    // Change and update configuration
    ifc.Config.Description = "Test"
    neighbor.Config.PeerAs = 65172
    reply = suite.Service.Set(&suite.Provider, configEC, "update")
    suite.Equal(reply, true)

    // Read all
    filterInt := ysanity_int.Interfaces{}
    filterBgp := ysanity_bgp.Bgp{}
    filterEc := types.NewFilter(&filterInt, &filterBgp)
    readAll := suite.Service.Get(&suite.Provider, filterEc, "ALL")
    suite.NotNil(readAll)
    suite.Equal(types.IsEntityCollection(readAll), true)
    
    // Validate response
    ec := types.EntityToCollection(readAll)
    readEntity, _ := ec.Get("openconfig-interfaces:interfaces")
    suite.NotNil(readEntity)
    ifcRead := readEntity.(*ysanity_int.Interfaces)
    suite.Equal(types.EntityEqual(ifcRead, &ifcs), true)

    // Read configuration only
    filterInt = ysanity_int.Interfaces{}
    filterBgp = ysanity_bgp.Bgp{}
    filterEc  = types.NewFilter(&filterInt, &filterBgp)
    readConfig := suite.Service.Get(&suite.Provider, filterEc, "CONFIG")
    suite.NotNil(readConfig)
    suite.Equal(types.IsEntityCollection(readConfig), true)

    // Delete configuration
    filterInt = ysanity_int.Interfaces{}
    filterBgp = ysanity_bgp.Bgp{}
    filterEc  = types.NewFilter(&filterInt, &filterBgp)
    reply = suite.Service.Set(&suite.Provider, filterEc, "delete")
    suite.Equal(reply, true)
}

func configInt(provider *providers.GnmiServiceProvider) {
    ifc := ysanity_int.Interfaces_Interface{}
    ifc.Name = "Loopback10"
    ifc.Config.Name = "Loopback10"
    ifc.Config.Description = "Test"

    crud := services.CrudService{}
    crud.Create(provider, &ifc)
}

func deleteInt(provider *providers.GnmiServiceProvider) {
    ifcs := ysanity_int.Interfaces{}    

    crud := services.CrudService{}
    crud.Delete(provider, &ifcs)
}

func configBgp(provider *providers.GnmiServiceProvider) {
    bgp := ysanity_bgp.Bgp{}
    bgp.Global.Config.As = 65172
    neighbor := ysanity_bgp.Bgp_Neighbors_Neighbor{}
    neighbor.NeighborAddress = "172.16.255.2"
    neighbor.Config.NeighborAddress = "172.16.255.2"
    neighbor.Config.PeerAs = 65172
    bgp.Neighbors.Neighbor = append(bgp.Neighbors.Neighbor, &neighbor)

    service := services.CrudService{}
    service.Create(provider, &bgp)
}

func deleteBgp(provider *providers.GnmiServiceProvider) {
    filterBgp := ysanity_bgp.Bgp{}

    service := services.CrudService{}
    service.Delete(provider, &filterBgp)
}

func bgpSubscription(provider *providers.GnmiServiceProvider) {
    previous := ""
    gs := provider.GetSession()
    for true {
        response := gs.GetLastSubscribeResponse(previous)
        if len(response) > 0 && response != previous {
            // Do anything with received response
            ydk.YLogDebug(fmt.Sprintf("%s:  Last received subscribe response:\n%s\n", time.Now().Format(time.RFC850), response))
            previous = response
        }
        if !gs.SubscribeInProgress() {
            break
        }
        time.Sleep(100 * time.Millisecond)
    }
}

func (suite *GnmiServiceTestSuite) TestSubscribeOnce() {
	// Build BGP configuration
	configBgp(&suite.Provider)
	
	// Build subscription request for one BGP neighbor
	neighbor := ysanity_bgp.Bgp_Neighbors_Neighbor{}
	neighbor.NeighborAddress = "172.16.255.2"
	sub := services.GnmiSubscription{}
	sub.Entity = &neighbor
	sub.SubscriptionMode = "ON_CHANGE"
	var subList []services.GnmiSubscription
	subList = append(subList, sub)
	
	// Start subscription and result monitoring
	go suite.Service.Subscribe(&suite.Provider, subList, 10, "ONCE", "JSON_IETF")
	time.Sleep(100 * time.Millisecond)
	bgpSubscription(&suite.Provider)
	
	// Delete BGP configuration
	deleteBgp(&suite.Provider)
}

func (suite *GnmiServiceTestSuite) TestSubscribeStream() {
	// Build BGP configuration
	configBgp(&suite.Provider)
	
	// Build subscription request
	sub := services.GnmiSubscription{}
	bgp := ysanity_bgp.Bgp{}
	sub.Entity = &bgp
	sub.SubscriptionMode = "ON_CHANGE"
	sub.SuppressRedundant = true
	sub.SampleInterval = 5000000000
	sub.HeartbeatInterval = 15000000000

	var subList []services.GnmiSubscription
	subList = append(subList, sub)
	
	// Start subscription and result monitoring
	go suite.Service.Subscribe(&suite.Provider, subList, 10, "STREAM", "JSON_IETF")
	time.Sleep(100 * time.Millisecond)
	bgpSubscription(&suite.Provider)
	
	// Delete BGP configuration
	deleteBgp(&suite.Provider)
}

func (suite *GnmiServiceTestSuite) TestCrudMultiples() {
    var crud services.CrudService

    // Build interface configuration
    ifc := ysanity_int.Interfaces_Interface{}
    ifc.Name = "Loopback10"
    ifc.Config.Name = "Loopback10"
    ifcs := ysanity_int.Interfaces{}
    ifcs.Interface = append(ifcs.Interface, &ifc)
    
    // Build BGP configuration
    bgp := ysanity_bgp.Bgp{}
    bgp.Global.Config.As = 65172
    neighbor := ysanity_bgp.Bgp_Neighbors_Neighbor{}
    neighbor.NeighborAddress = "172.16.255.2"
    neighbor.Config.NeighborAddress = "172.16.255.2"
    bgp.Neighbors.Neighbor = append(bgp.Neighbors.Neighbor, &neighbor)

    // Create congfiguration on device
    configEC := types.NewConfig(&ifcs, &bgp)
    reply := crud.Create(&suite.Provider, configEC)
    suite.Equal(reply, true)

    // Change and update configuration
    ifc.Config.Description = "Test"
    neighbor.Config.PeerAs = 65172
    reply = crud.Update(&suite.Provider, configEC)
    suite.Equal(reply, true)

    // Read all
    filterInt := ysanity_int.Interfaces{}
    filterBgp := ysanity_bgp.Bgp{}
    filterEc := types.NewFilter(&filterInt, &filterBgp)
    readAll := crud.Read(&suite.Provider, filterEc)
    suite.NotNil(readAll)
    suite.Equal(types.IsEntityCollection(readAll), true)
    
    // Validate response
    ec := types.EntityToCollection(readAll)
    readEntity, _ := ec.Get("openconfig-interfaces:interfaces")
    suite.NotNil(readEntity)
    ifcRead := readEntity.(*ysanity_int.Interfaces)
    suite.Equal(types.EntityEqual(ifcRead, &ifcs), true)

    // Read configuration only
    filterInt = ysanity_int.Interfaces{}
    filterBgp = ysanity_bgp.Bgp{}
    filterEc  = types.NewFilter(&filterInt, &filterBgp)
    readConfig := crud.ReadConfig(&suite.Provider, filterEc)
    suite.NotNil(readConfig)
    suite.Equal(types.IsEntityCollection(readConfig), true)

    // Delete configuration
    filterInt = ysanity_int.Interfaces{}
    filterBgp = ysanity_bgp.Bgp{}
    filterEc  = types.NewFilter(&filterInt, &filterBgp)
    reply = crud.Delete(&suite.Provider, filterEc)
    suite.Equal(reply, true)
}

func (suite *GnmiServiceTestSuite) TestCrudSingle() {
    var crud services.CrudService

    // Build interface configuration
    ifc := ysanity_int.Interfaces_Interface{}
    ifc.Name = "Loopback10"
    ifc.Config.Name = "Loopback10"
    
    reply := crud.Create(&suite.Provider, &ifc)
    suite.Equal(reply, true)

    // Change and update configuration
    ifc.Config.Description = "Test"
    reply = crud.Update(&suite.Provider, &ifc)
    suite.Equal(reply, true)

    // Read all
    ifcFilter := ysanity_int.Interfaces_Interface{}
    ifcFilter.Name = "Loopback10"

    readIfc := crud.Read(&suite.Provider, &ifcFilter)
    suite.NotNil(readIfc)
    suite.Equal(types.IsEntityCollection(readIfc), false)
    
    // Validate response
    ifcRead := readIfc.(*ysanity_int.Interfaces_Interface)
    suite.Equal(types.EntityEqual(ifcRead, &ifc), true)

    // Read configuration only
    readConfig := crud.ReadConfig(&suite.Provider, &ifcFilter)
    suite.NotNil(readConfig)

    // Read container/entity with filter
    ifcFilter.Config.YFilter = yfilter.Read
    readConfig = crud.ReadConfig(&suite.Provider, &ifcFilter)
    suite.NotNil(readConfig)

    // Read single leaf
    ifcFilter.Config.YFilter = yfilter.NotSet
    ifcFilter.Config.Description = yfilter.Read
    
    readConfig = crud.ReadConfig(&suite.Provider, &ifcFilter)
    suite.NotNil(readConfig)

    // Delete configuration
    ifcFilter = ysanity_int.Interfaces_Interface{}
    ifcFilter.Name = "Loopback10"
    reply = crud.Delete(&suite.Provider, &ifcFilter)
    suite.Equal(reply, true)
}

func (suite *GnmiServiceTestSuite) TestCrudTwoLeafs() {
    var crud services.CrudService

    // Build interface configuration
    configInt(&suite.Provider)
    configBgp(&suite.Provider)

    ifc := ysanity_int.Interfaces_Interface{}
    ifc.Name = "Loopback10"
    ifc.Config.Description = yfilter.Read
    ifcs := ysanity_int.Interfaces{}
    ifcs.Interface = append(ifcs.Interface, &ifc)
    
    bgp := ysanity_bgp.Bgp{}
    neighbor := ysanity_bgp.Bgp_Neighbors_Neighbor{}
    neighbor.NeighborAddress = "172.16.255.2"
    neighbor.Config.PeerAs = yfilter.Read
    bgp.Neighbors.Neighbor = append(bgp.Neighbors.Neighbor, &neighbor)
 
    filterEc := types.NewFilter(&ifcs, &bgp)
    readConfig := crud.ReadConfig(&suite.Provider, filterEc)
    suite.NotNil(readConfig)
    suite.Equal(types.IsEntityCollection(readConfig), true)

    // Delete configuration
    deleteInt(&suite.Provider)
    deleteBgp(&suite.Provider)
}

func (suite *GnmiServiceTestSuite) TestDeleteContainer() {
	var crud services.CrudService

	// Build loopback configuration
	address := ysanity.Native_Interface_Loopback_Ipv4_Address{}
	address.Ip = "2.2.2.2"
	address.PrefixLength = 32

	loopback := ysanity.Native_Interface_Loopback{}
	loopback.Name = 2222
	loopback.Ipv4.Address = append(loopback.Ipv4.Address, &address)

	native := ysanity.Native{}
        native.Interface.Loopback = append(native.Interface.Loopback, &loopback)

	result := crud.Create(&suite.Provider, &native)
	suite.True(result)

	// Read ipv4 configuration
	native = ysanity.Native{}
	loopback = ysanity.Native_Interface_Loopback{}
	loopback.Name = 2222
	native.Interface.Loopback = append(native.Interface.Loopback, &loopback)
	types.SetAllParents(&native)

	ipv4ConfigEnt := crud.Read(&suite.Provider, &loopback.Ipv4)
	suite.NotNil(ipv4ConfigEnt)
	ipv4Config := ipv4ConfigEnt.(*ysanity.Native_Interface_Loopback_Ipv4)
	_, addressEnt := ylist.Get(ipv4Config.Address, "2.2.2.2")
	suite.NotNil(addressEnt)
	addressPtr := addressEnt.(*ysanity.Native_Interface_Loopback_Ipv4_Address)
	suite.Equal("32", addressPtr.PrefixLength)

	// Remove ipv4 configuration
	native = ysanity.Native{}
	loopback = ysanity.Native_Interface_Loopback{}
	loopback.Name = 2222
	native.Interface.Loopback = append(native.Interface.Loopback, &loopback)
	types.SetAllParents(&native)

	loopback.Ipv4.YFilter = yfilter.Delete
	result = crud.Delete(&suite.Provider, &loopback.Ipv4)
        suite.True(result)

	// Delete configuration
	native = ysanity.Native{}
	result = crud.Delete(&suite.Provider, &native)
	suite.True(result)
}

func TestGnmiServiceTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(GnmiServiceTestSuite))
}
