package test

import (
	"fmt"
	"strings"
	"strconv"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	ietfNetconf "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/ietf_netconf"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
	"github.com/stretchr/testify/suite"
	"testing"
)

type ExecutorServiceTestSuite struct {
	suite.Suite
	CodecProvider		providers.CodecServiceProvider
	NetconfProvider 	providers.NetconfServiceProvider
	CrudService			services.CrudService
	CodecService		services.CodecService
	ExecutorService		services.ExecutorService
}

func (suite *ExecutorServiceTestSuite) SetupSuite() {
	suite.CodecProvider = providers.CodecServiceProvider{}
	suite.NetconfProvider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.NetconfProvider.Connect()
	suite.CrudService		= services.CrudService{}
	suite.CodecService 		= services.CodecService{}
	suite.ExecutorService	= services.ExecutorService{}
}

func (suite *ExecutorServiceTestSuite) TearDownSuite() {
	suite.NetconfProvider.Disconnect()
}

func (suite *ExecutorServiceTestSuite) BeforeTest(suiteName, testName string) {
	suite.CrudService.Delete(&suite.NetconfProvider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *ExecutorServiceTestSuite) TestEditCommitGet() {
	suite.CodecProvider.Encoding = encoding.XML

	runner := ysanity.Runner{}
	runner.One.Number = 1
	runner.One.Name = "runner:one:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	// Edit Config
	editConfigRpc := ietfNetconf.EditConfig{}
	editConfigRpc.Input.Target.Candidate = types.Empty{}
	editConfigRpc.Input.Config = runnerXML

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &editConfigRpc, nil)
	suite.Equal(readEntity, nil)

	// Get Config
	filterXML := suite.CodecService.Encode(&suite.CodecProvider, &ysanity.Runner{})
	getConfigRpc := ietfNetconf.GetConfig{}
	getConfigRpc.Input.Source.Candidate = types.Empty{}
	getConfigRpc.Input.Filter = filterXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &getConfigRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)

	// Commit
	commitRpc := ietfNetconf.Commit{}

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &commitRpc, nil)
	suite.Equal(readEntity, nil)

	// Get
	filterXML = suite.CodecService.Encode(&suite.CodecProvider, &ysanity.Runner{})
	getRpc := ietfNetconf.Get{}
	getRpc.Input.Filter = filterXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &getRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
}

func (suite *ExecutorServiceTestSuite) TestLockUnlock() {
	lockRpc := ietfNetconf.Lock{}
	lockRpc.Input.Target.Candidate = types.Empty{}

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &lockRpc, nil)
	suite.Equal(readEntity, nil)

	unlockRpc := ietfNetconf.Unlock{}
	unlockRpc.Input.Target.Candidate = types.Empty{}

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &unlockRpc, nil)
	suite.Equal(readEntity, nil)
}

func (suite *ExecutorServiceTestSuite) TestLockUnlockFail() {
	lockRpc := ietfNetconf.Lock{}
	lockRpc.Input.Target.Candidate = types.Empty{}

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &lockRpc, nil)
	suite.Equal(readEntity, nil)

	funcDidPanic, panicValue := didPanic(func() { 
		suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &lockRpc, nil) })
	suite.Equal(funcDidPanic, true)
	suite.Regexp("YGOServiceProviderError:", panicValue)
	suite.Regexp("<error-tag>lock-denied</error-tag>", panicValue)
}

func (suite *ExecutorServiceTestSuite) TestValidate() {
	rpc := ietfNetconf.Validate{}
	rpc.Input.Source.Candidate = types.Empty{}

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &rpc, nil)
	suite.Equal(readEntity, nil)

	suite.CodecProvider.Encoding = encoding.XML
	runner := ysanity.Runner{}
	runner.One.Number = 1
	runner.One.Name = "runner:one:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	rpc = ietfNetconf.Validate{}
	rpc.Input.Source.Config = runnerXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &rpc, nil)
	suite.Equal(readEntity, nil)
}

func (suite *ExecutorServiceTestSuite) TestDiscardChanges(){
	suite.CodecProvider.Encoding = encoding.XML
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	// EditConfig
	editConfigRpc := ietfNetconf.EditConfig{}
	editConfigRpc.Input.Target.Candidate = types.Empty{}
	editConfigRpc.Input.Config = runnerXML

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &editConfigRpc, nil)
	suite.Equal(readEntity, nil)
	
	// DiscardChanges
	readEntity = suite.ExecutorService.ExecuteRpc(
		&suite.NetconfProvider, &ietfNetconf.DiscardChanges{}, nil)
	suite.Equal(readEntity, nil)

	// GetConfig
	filterXML := suite.CodecService.Encode(&suite.CodecProvider, &ysanity.Runner{})
	getConfigRpc := ietfNetconf.GetConfig{}
	getConfigRpc.Input.Source.Candidate = types.Empty{}
	getConfigRpc.Input.Filter = filterXML

	readEntity = suite.ExecutorService.ExecuteRpc(
		&suite.NetconfProvider, &getConfigRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &ysanity.Runner{}), true)
}

func (suite *ExecutorServiceTestSuite) TestConfirmedCommit() {
	suite.CodecProvider.Encoding = encoding.XML
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	// EditConfig
	editConfigRpc := ietfNetconf.EditConfig{}
	editConfigRpc.Input.Target.Candidate = types.Empty{}
	editConfigRpc.Input.Config = runnerXML

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &editConfigRpc, nil)
	suite.Equal(readEntity, nil)

	// Commit
	commitRpc := ietfNetconf.Commit{}
	commitRpc.Input.Confirmed = types.Empty{}
	commitRpc.Input.ConfirmTimeout = 5

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &commitRpc, nil)
	suite.Equal(readEntity, nil)

	// Get
	filterXML := suite.CodecService.Encode(&suite.CodecProvider, &ysanity.Runner{})
	getRpc := ietfNetconf.Get{}
	getRpc.Input.Filter = filterXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &getRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)
}

// skip
func (suite *ExecutorServiceTestSuite) TestCancelCommit() {
	suite.T().Skip("No message id in cancel commit payload")

	suite.CodecProvider.Encoding = encoding.XML
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	// EditConfig
	editConfigRpc := ietfNetconf.EditConfig{}
	editConfigRpc.Input.Target.Candidate = types.Empty{}
	editConfigRpc.Input.Config = runnerXML

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &editConfigRpc, nil)
	suite.Equal(readEntity, nil)

	// Commit
	commitRpc := ietfNetconf.Commit{}
	commitRpc.Input.Confirmed = types.Empty{}
	// commitRpc.Input.ConfirmTimeout = 5
	// commitRpc.Input.Persist = 2
	// commitRpc.Input.PersistId = 2

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &commitRpc, nil)
	suite.Equal(readEntity, nil)

	// CancelCommit
	cancelRpc := ietfNetconf.CancelCommit{}
	// cancelRpc.Input.PersistId = 2

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &cancelRpc, nil)
	suite.Equal(readEntity, nil)
}

func (suite *ExecutorServiceTestSuite) TestCopyConfig() {
	suite.CodecProvider.Encoding = encoding.XML
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	var readEntity types.Entity

	// Modify Candidate via CopyConfig from runner
	copyRpc := ietfNetconf.CopyConfig{}
	copyRpc.Input.Target.Candidate = types.Empty{}
	copyRpc.Input.Source.Config = runnerXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &copyRpc, nil)
	suite.Equal(readEntity, nil)

	filterXML := suite.CodecService.Encode(&suite.CodecProvider, &ysanity.Runner{})
	getConfigRpc := ietfNetconf.GetConfig{}
	getConfigRpc.Input.Source.Candidate = types.Empty{}
	getConfigRpc.Input.Filter = filterXML

	readEntity = suite.ExecutorService.ExecuteRpc(
		&suite.NetconfProvider, &getConfigRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)

	// Modify Candidate via CopyConfig from runner
	runner.Two.Name = fmt.Sprintf("%s_modified", runner.Two.Name)
	runnerXML = suite.CodecService.Encode(&suite.CodecProvider, &runner)

	copyRpc.Input.Source.Config = runnerXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &copyRpc, nil)
	suite.Equal(readEntity, nil)

	readEntity = suite.ExecutorService.ExecuteRpc(
		&suite.NetconfProvider, &getConfigRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)

	// Modify Candidate via CopyConfig from Running
	copyRpc = ietfNetconf.CopyConfig{}
	copyRpc.Input.Target.Candidate = types.Empty{}
	copyRpc.Input.Source.Running = types.Empty{}

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &copyRpc, nil)
	suite.Equal(readEntity, nil)

	getConfigRpc = ietfNetconf.GetConfig{}
	getConfigRpc.Input.Source.Candidate = types.Empty{}
	getConfigRpc.Input.Filter = filterXML

	readEntity = suite.ExecutorService.ExecuteRpc(
		&suite.NetconfProvider, &getConfigRpc, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(readEntity, &runner), true)

	// DiscardChanges
	readEntity = suite.ExecutorService.ExecuteRpc(
		&suite.NetconfProvider, &ietfNetconf.DiscardChanges{}, nil)
	suite.Equal(readEntity, nil)
}

// skip
func (suite *ExecutorServiceTestSuite) TestDeleteConfig() {
	suite.T().Skip("startup/url not enabled in ConfD")

	suite.CodecProvider.Encoding = encoding.XML
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	runnerXML := suite.CodecService.Encode(&suite.CodecProvider, &runner)

	// CopyConfig
	copyRpc := ietfNetconf.CopyConfig{}
	copyRpc.Input.Target.Startup = types.Empty{}
	copyRpc.Input.Source.Config = runnerXML

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &copyRpc, nil)
	suite.Equal(readEntity, nil)

	// DeleteConfig
	deleteRpc := ietfNetconf.DeleteConfig{}
	deleteRpc.Input.Target.Startup = types.Empty{}

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &deleteRpc, nil)
	suite.Equal(readEntity, nil)

	// CopyConfig
	copyRpc = ietfNetconf.CopyConfig{}
	copyRpc.Input.Target.Url = "http://test"
	copyRpc.Input.Source.Config = runnerXML

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &copyRpc, nil)
	suite.Equal(readEntity, nil)

	// DeleteConfig
	deleteRpc = ietfNetconf.DeleteConfig{}
	deleteRpc.Input.Target.Url = "http://test"

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &deleteRpc, nil)
	suite.Equal(readEntity, nil)
}

func (suite *ExecutorServiceTestSuite) TestCloseSession() {
	closeRpc := ietfNetconf.CloseSession{}
	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &closeRpc, nil)
	suite.Equal(readEntity, nil)

	suite.NetconfProvider.Connect()

	validateRpc := ietfNetconf.Validate{}
	validateRpc.Input.Source.Candidate = types.Empty{}

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &validateRpc, nil)
	suite.Equal(readEntity, nil)
}

// skip
func (suite *ExecutorServiceTestSuite) TestKillSession() {
	suite.T().Skip("session-id not recognized")

	lockRpc := ietfNetconf.Lock{}
	lockRpc.Input.Target.Candidate = types.Empty{}

	readEntity := suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &lockRpc, nil)
	suite.Equal(readEntity, nil)

	funcDidPanic, panicValue := didPanic(func() { 
		suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &lockRpc, nil) })

	suite.Equal(funcDidPanic, true)
	suite.Regexp("<session-id>", panicValue)

	sessionIdStr := strings.Split(panicValue, "<session-id>")[1]
	sessionIdStr = strings.Split(sessionIdStr, "</session-id>")[0]
	sessionId, err := strconv.Atoi(sessionIdStr)
	suite.Equal(err, nil)

	fmt.Println(sessionId)

	killRpc := ietfNetconf.KillSession{}
	killRpc.Input.SessionId = sessionId

	readEntity = suite.ExecutorService.ExecuteRpc(&suite.NetconfProvider, &killRpc, nil)
	suite.Equal(readEntity, nil)
}

func TestExecutorServiceTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(ExecutorServiceTestSuite))
}
