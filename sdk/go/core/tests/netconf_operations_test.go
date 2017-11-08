package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/stretchr/testify/suite"
	"testing"
)

type NETCONFOperationsTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *NETCONFOperationsTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *NETCONFOperationsTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *NETCONFOperationsTestSuite) BeforeTest(suiteName, testName string) {
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *NETCONFOperationsTestSuite) TestReplace() {
	runner := ysanity.Runner{}
	runner.YFilter = yfilter.Delete
	suite.CRUD.Update(&suite.Provider, &runner)

	runnerCreate := ysanity.Runner{}
	runnerCreate.Ytypes.BuiltInT.Number8 = 10
	suite.CRUD.Create(&suite.Provider, &runnerCreate)

	runnerUpdate := ysanity.Runner{}
	runnerUpdate.Ytypes.BuiltInT.Number8 = 25
	runnerUpdate.YFilter = yfilter.Replace
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entity, &runnerUpdate), true)
}

func (suite *NETCONFOperationsTestSuite) TestCreate() {
	runnerCreate := ysanity.Runner{}
	e1 := ysanity.Runner_OneList_Ldata{}
	e2 := ysanity.Runner_OneList_Ldata{}
	e1.Number = 1
	e1.Name = "foo"
	e1.YFilter = yfilter.Create
	e2.Number = 2
	e2.Name = "bar"
	e2.YFilter = yfilter.Create

	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, e1)
	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, e2)

	suite.CRUD.Update(&suite.Provider, &runnerCreate)
	// create duplicate value raises error
	// The payload errMsg is hardcoded with message-id of certain value.
	// Please change corresponding message-id if new tests are added/enabled.
	errMsg := `<rpc-error>
    <error-type>application</error-type>
    <error-tag>data-exists</error-tag>
    <error-severity>error</error-severity>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0">
    /nc:rpc/nc:edit-config/nc:config/ydkut:runner/ydkut:one-list/ydkut:ldata\[ydkut:number='[1-2]']
  </error-path>
    <error-info>
      <bad-element>ldata</bad-element>
    </error-info>
  </rpc-error>
</rpc-reply>
`
	funcDidPanic, panicValue := didPanic(func() { suite.CRUD.Update(&suite.Provider, &runnerCreate) })
	suite.Equal(funcDidPanic, true)
	suite.Regexp("YGOServiceProviderError:", panicValue)
	suite.Regexp(errMsg, panicValue)
}

func (suite *NETCONFOperationsTestSuite) TestDelete() {
	runnerCreate := ysanity.Runner{}
	e1 := ysanity.Runner_OneList_Ldata{}
	e2 := ysanity.Runner_OneList_Ldata{}
	e1.Number = 1
	e1.Name = "foo"
	e1.YFilter = yfilter.Create
	e2.Number = 2
	e2.Name = "bar"
	e2.YFilter = yfilter.Create
	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, e1)
	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, e2)
	suite.CRUD.Update(&suite.Provider, &runnerCreate)
	fmt.Println("TestDelete finished Create")

	runnerUpdate := ysanity.Runner{}
	eU1 := ysanity.Runner_OneList_Ldata{}
	eU1.Number = 1
	eU1.YFilter = yfilter.Delete
	runnerUpdate.OneList.Ldata = append(runnerUpdate.OneList.Ldata, eU1)
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)
	fmt.Println("TestDelete finished Update")

	// delete again raises error
	// The payload errMsg is hardcoded with message-id of certain value.
	// Please change corresponding message-id if new tests are added/enabled.
	errMsg := `<rpc-error>
    <error-type>application</error-type>
    <error-tag>data-missing</error-tag>
    <error-severity>error</error-severity>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity" xmlns:nc="urn:ietf:params:xml:ns:netconf:base:1.0">
    /nc:rpc/nc:edit-config/nc:config/ydkut:runner/ydkut:one-list/ydkut:ldata\[ydkut:number='[1-2]']
  </error-path>
    <error-info>
      <bad-element>ldata</bad-element>
    </error-info>
  </rpc-error>
</rpc-reply>
`
	funcDidPanic, panicValue := didPanic(func() { suite.CRUD.Update(&suite.Provider, &runnerUpdate) })
	suite.Equal(funcDidPanic, true)
	suite.Regexp("YGOServiceProviderError:", panicValue)
	suite.Regexp(errMsg, panicValue)
}

func (suite *NETCONFOperationsTestSuite) TestRemove() {
	runnerCreate := ysanity.Runner{}
	runnerCreate.Ytypes.BuiltInT.Number8 = 25
	runnerCreate.YFilter = yfilter.Merge
	suite.CRUD.Update(&suite.Provider, &runnerCreate)

	runnerUpdate := ysanity.Runner{}
	runnerUpdate.YFilter = yfilter.Remove
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	// remove again without any error
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)
}

func (suite *NETCONFOperationsTestSuite) TestMerge() {
	runnerCreate := ysanity.Runner{}
	runnerCreate.Ytypes.BuiltInT.Number8 = 25
	suite.CRUD.Create(&suite.Provider, &runnerCreate)

	runnerUpdate := ysanity.Runner{}
	runnerUpdate.Ytypes.BuiltInT.Number8 = 32
	runnerUpdate.YFilter = yfilter.Merge
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead := entity.(*ysanity.Runner)
	suite.Equal(types.EntityEqual(entity, runnerRead), true)
}

// TODO: Encoding error using YFilter for non-top level leaves
// need to use subtree filter
// func (suite *NETCONFOperationsTestSuite) TestDeleteLeaf() {
//     runnerCreate := ysanity.Runner{}
//     runnerCreate.Ytypes.BuiltInT.Number8 = 10
//     suite.CRUD.Create(&suite.Provider, &runnerCreate)

//     runnerUpdate := ysanity.Runner{}
//     runnerUpdate.Ytypes.BuiltInT.Number8 = yfilter.Delete
//     suite.CRUD.Update(&suite.Provider, &runnerUpdate)

//     // delete leaf again raises error
//     suite.CRUD.Update(&suite.Provider, &runnerUpdate)
// }

// TODO: Encoding error using YFilter as leaf-list value
// func (suite *NETCONFOperationsTestSuite) TestDeleteLeafList() {
//     runnerCreate := ysanity.Runner{}
//     runnerCreate.Ytypes.BuiltInT.EnumLlist = append(runnerCreate.Ytypes.BuiltInT.EnumLlist, ysanity.YdkEnumTest_local)
//     suite.CRUD.Create(&suite.Provider, &runnerCreate)

//     runnerUpdate := ysanity.Runner{}
//     // TODO: verify that appending YFilter.Delete to leaf-list delete whole leaf list or a particular leaf
//     runnerUpdate.Ytypes.BuiltInT.EnumLlist = append(runnerUpdate.Ytypes.BuiltInT.EnumLlist, yfilter.Delete)
//     suite.CRUD.Update(&suite.Provider, &runnerUpdate)

//     // delete again with error
//     suite.CRUD.Update(&suite.Provider, &runnerUpdate)
// }

func TestNETCONFOperationsTestSuite(t *testing.T) {
	suite.Run(t, new(NETCONFOperationsTestSuite))
}
