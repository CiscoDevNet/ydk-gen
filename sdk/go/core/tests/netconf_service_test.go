package test

import (
	"fmt"
	"strconv"
	"strings"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/datastore"
	"github.com/stretchr/testify/suite"
	"testing"
)

type NetconfServiceTestSuite struct {
	suite.Suite
	Provider 	providers.NetconfServiceProvider
	Crud 		services.CrudService
	NS 			services.NetconfService
}

func (suite *NetconfServiceTestSuite) SetupSuite() {
	suite.Crud = services.CrudService{}
	suite.NS = services.NetconfService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *NetconfServiceTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *NetconfServiceTestSuite) BeforeTest(suiteName, testName string) {
	suite.Crud.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *NetconfServiceTestSuite) TestEditCommitGet() {
	runner := ysanity.Runner{}
	runner.One.Number = 1
	runner.One.Name = "runner:one:name"

	// Edit Config
	op := suite.NS.EditConfig(&suite.Provider, datastore.Candidate, &runner, "", "", "")
	suite.Equal(op, true)

	// Get Config
	readEntity := suite.NS.GetConfig(&suite.Provider, datastore.Candidate, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)

	// Commit
	op = suite.NS.Commit(&suite.Provider, false, -1, -1, -1)
	suite.Equal(op, true)

	// Get
	readEntity = suite.NS.Get(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
}

func (suite *NetconfServiceTestSuite) TestLockUnlock() {
	op := suite.NS.Lock(&suite.Provider, datastore.Running)
	suite.Equal(op, true)

	op = suite.NS.Unlock(&suite.Provider, datastore.Running)
	suite.Equal(op, true)
}

func (suite *NetconfServiceTestSuite) TestLockUnlockFail() {
	op := suite.NS.Lock(&suite.Provider, datastore.Candidate)
	suite.Equal(op, true)

	funcDidPanic, panicValue := didPanic(func() { suite.NS.Unlock(&suite.Provider, datastore.Running) })
	suite.Equal(funcDidPanic, true)
	suite.Regexp("YGOServiceProviderError:", panicValue)
	errMsg := `<rpc-error>
    <error-type>application</error-type>
    <error-tag>operation-failed</error-tag>
    <error-severity>error</error-severity>
  </rpc-error>`
	suite.Regexp(errMsg, panicValue)
}

func (suite *NetconfServiceTestSuite) TestValidate() {
	op := suite.NS.Validate(&suite.Provider, datastore.Candidate, nil, "")
	suite.Equal(op, true)

	runner := ysanity.Runner{}
	runner.One.Number = 1
	runner.One.Name = "runner:one:name"
	op = suite.NS.Validate(&suite.Provider, datastore.NotSet, &runner, "")
	suite.Equal(op, true)
}

func (suite *NetconfServiceTestSuite) TestDiscardChanges(){
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"

	// EditConfig
	op := suite.NS.EditConfig(&suite.Provider, datastore.Candidate, &runner, "", "", "")
	suite.Equal(op, true)

	// DiscardChanges
	op = suite.NS.DiscardChanges(&suite.Provider)
	suite.Equal(op, true)

	// GetConfig
	readEntity := suite.NS.GetConfig(&suite.Provider, datastore.Candidate, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &ysanity.Runner{}), true)
}

func (suite *NetconfServiceTestSuite) TestConfirmedCommit() {
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"

	op := suite.NS.EditConfig(&suite.Provider, datastore.Candidate, &runner, "", "", "")
	suite.Equal(op, true)

	op = suite.NS.Commit(&suite.Provider, true, 120, -1, -1)
	suite.Equal(op, true)

	readEntity := suite.NS.Get(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
}

// skip
func (suite *NetconfServiceTestSuite) TestCancelCommit() {
	suite.T().Skip("No message id in cancel commit payload")

	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"

	op := suite.NS.EditConfig(&suite.Provider, datastore.Candidate, &runner, "", "", "")
	suite.Equal(op, true)

	op = suite.NS.Commit(&suite.Provider, true, 120, -1, -1)
	suite.Equal(op, true)

	op = suite.NS.CancelCommit(&suite.Provider, -1)
	suite.Equal(op, true)

	readEntity := suite.NS.Get(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
}

// TODO: Problems found with path functions:
//			readEntity changes with getFilter
//			EntityEqual(readEntity, &runner) returns true, even after the above
func (suite *NetconfServiceTestSuite) TestCopyConfig() {
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	getFilter := ysanity.Runner{}

	var op bool
	var readEntity types.Entity

	// Modify Candidate via CopyConfig from runner
	op = suite.NS.CopyConfig(
		&suite.Provider, datastore.Candidate, datastore.NotSet, &runner, "")
	suite.Equal(op, true)

	readEntity = suite.NS.GetConfig(&suite.Provider, datastore.Candidate, &getFilter)
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
	getFilter = ysanity.Runner{}

	// Modify Candidate via CopyConfig from runner
	runner.Two.Name = fmt.Sprintf("%s_modified", runner.Two.Name)

	op = suite.NS.CopyConfig(
		&suite.Provider, datastore.Candidate, datastore.NotSet, &runner, "")
	suite.Equal(op, true)

	readEntity = suite.NS.GetConfig(&suite.Provider, datastore.Candidate, &getFilter)
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
	getFilter = ysanity.Runner{}

	// Modify Candidate via CopyConfig from Running
	op = suite.NS.CopyConfig(&suite.Provider, datastore.Candidate, datastore.Running, nil, "")
	suite.Equal(op, true)

	readEntity = suite.NS.GetConfig(&suite.Provider, datastore.Candidate, &getFilter)
	suite.Equal(types.EntityEqual(readEntity, &ysanity.Runner{}), true)

	// DiscardChanges
	op = suite.NS.DiscardChanges(&suite.Provider)
	suite.Equal(op, true)
}

// skip
func (suite *NetconfServiceTestSuite) TestDeleteConfig() {
	suite.T().Skip("startup not enabled in ConfD")

	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"

	op := suite.NS.CopyConfig(
		&suite.Provider, datastore.Startup, datastore.NotSet, &runner, "")
	suite.Equal(op, true)

	op = suite.NS.DeleteConfig(&suite.Provider, datastore.Startup, "")
	suite.Equal(op, true)
}

func (suite *NetconfServiceTestSuite) TestCloseSession() {
	op := suite.NS.CloseSession(&suite.Provider)
	suite.Equal(op, true)

	funcDidPanic, panicValue := didPanic(func() { suite.NS.Lock(&suite.Provider, datastore.Running) })
	suite.Equal(funcDidPanic, true)
	suite.Equal(panicValue, "YGOClientError: Could not send payload")

	suite.Provider.Connect()

	op = suite.NS.Lock(&suite.Provider, datastore.Running)
	op = suite.NS.Unlock(&suite.Provider, datastore.Running)
	suite.Equal(op, true)
}

// skip
func (suite *NetconfServiceTestSuite) TestKillSession() {
	suite.T().Skip("session-id not recognized")

	op := suite.NS.Lock(&suite.Provider, datastore.Candidate)
	suite.Equal(op, true)

	funcDidPanic, panicValue := didPanic(func() { suite.NS.Lock(&suite.Provider, datastore.Candidate) })
	suite.Equal(funcDidPanic, true)
	suite.Regexp("<session-id>", panicValue)

	sessionIdStr := strings.Split(panicValue, "<session-id>")[1]
	sessionIdStr = strings.Split(sessionIdStr, "</session-id>")[0]
	sessionId, err := strconv.Atoi(sessionIdStr)
	suite.Equal(err, nil)

	op = suite.NS.KillSession(&suite.Provider, sessionId)
	suite.Equal(op, true)
}

func TestNetconfServiceTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(NetconfServiceTestSuite))
}
