package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/stretchr/testify/suite"
	"testing"
)

type NetconfProviderTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *NetconfProviderTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *NetconfProviderTestSuite) BeforeTest(suiteName, testName string) {
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *NetconfProviderTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *NetconfProviderTestSuite) TestGetCapabilities() {
	capabilities := suite.Provider.GetCapabilities()
	for _, c := range capabilities {
		msg := fmt.Sprintf("Got capability %v", c)
		ydk.YLogInfo(msg)
	}
}

func TestNetconfProviderTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(NetconfProviderTestSuite))
}
