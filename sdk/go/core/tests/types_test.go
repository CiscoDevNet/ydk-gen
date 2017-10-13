package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	ysanity_types "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity_types"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/stretchr/testify/suite"
	"strconv"
	"testing"
)

type SanityTypesTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *SanityTypesTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *SanityTypesTestSuite) BeforeTest(suiteName, testName string) {
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *SanityTypesTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *SanityTypesTestSuite) TestInt8() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number8 = 0
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestInt16() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number16 = 126
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestInt32() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number32 = 200000
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestInt64() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number64 = -9223372036854775808
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestUInt8() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber8 = 0
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestUInt16() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber16 = 65535
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestUInt32() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber32 = 5927
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestUInt64() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber64 = 184467440737551615
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestDecimal64() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Deci64 = "3.14"
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestString1() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Name = "name string"
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestEmpty() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Emptee = types.Empty{}
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestBoolean() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.BoolValue = true
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestStatusEnum() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Status = ysanity.Runner_Ytypes_BuiltInT_Status_not_connected
	suite.CRUD.Create(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead := entity.(*ysanity.Runner)
	suite.Equal(runnerRead.Ytypes.BuiltInT.Status, "not connected")
}

func (suite *SanityTypesTestSuite) TestEmbeddedEnum() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.EmbededEnum = ysanity.Runner_Ytypes_BuiltInT_EmbededEnum_zero
	suite.CRUD.Create(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead := entity.(*ysanity.Runner)
	suite.Equal(runnerRead.Ytypes.BuiltInT.EmbededEnum, "zero")
}

func (suite *SanityTypesTestSuite) TestEnum() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.EnumValue = ysanity.YdkEnumTest_not_set
	suite.CRUD.Create(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead := entity.(*ysanity.Runner)
	suite.Equal(runnerRead.Ytypes.BuiltInT.EnumValue, "not-set")
}

func (suite *SanityTypesTestSuite) TestBitsPos1() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.BitsValue = map[string]bool{"disable-nagle": true, "auto-sense-speed": true}
	suite.CRUD.Create(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead := entity.(*ysanity.Runner)
	suite.Equal(runnerRead.Ytypes.BuiltInT.BitsValue, "disable-nagle auto-sense-speed")
}

func (suite *SanityTypesTestSuite) TestBitsPos2() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.BitsValue = map[string]bool{"disable-nagle": true, "auto-sense-speed": false}
	suite.CRUD.Create(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead := entity.(*ysanity.Runner)
	suite.Equal(runnerRead.Ytypes.BuiltInT.BitsValue, "disable-nagle")
}

func (suite *SanityTypesTestSuite) TestUnion() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Younion = ysanity.YdkEnumTest_none
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

// TODO: Missing YdkEnumIntTest_any in generated APIs
// func (suite *SanityTypesTestSuite) TestUnionEnum() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.Younion = ysanity.YdkEnumIntTest_any
// 	suite.CRUD.Create(&suite.Provider, &runner)

// 	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
// 	suite.Equal(types.EntityEqual(entityRead, &runner), true)
// }

func (suite *SanityTypesTestSuite) TestUnionInt() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.EnumIntValue = 2
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestUnionRecursive() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.YounionRecursive = 18
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

// TODO: leaf-list encoding error
// func (suite *SanityTypesTestSuite) TestUnionLeafList() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.Llunion = append(runner.Ytypes.BuiltInT.Llunion, 1)
// 	runner.Ytypes.BuiltInT.Llunion = append(runner.Ytypes.BuiltInT.Llunion, 3)
// 	suite.CRUD.Create(&suite.Provider, &runner)

// 	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
// 	suite.Equal(types.EntityEqual(entityRead, &runner), true)
// }

// TODO: leaf-list encoding error
// func (suite *SanityTypesTestSuite) TestEnumLeafList() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.EnumLlist = append(runner.Ytypes.BuiltInT.EnumLlist, ysanity.YdkEnumTest_local)
// 	runner.Ytypes.BuiltInT.EnumLlist = append(runner.Ytypes.BuiltInT.EnumLlist, ysanity.YdkEnumTest_remote)
// 	suite.CRUD.Create(&suite.Provider, &runner)

// 	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
// 	suite.Equal(types.EntityEqual(entityRead, &runner), true)
// }

// TODO: leaf-list encoding error
// func (suite *SanityTypesTestSuite) TestIdentityLeafList() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.IdentityLlist = append(runner.Ytypes.BuiltInT.IdentityLlist, ysanity.Child_Identity{})
// 	runner.Ytypes.BuiltInT.IdentityLlist = append(runner.Ytypes.BuiltInT.IdentityLlist, ysanity.Child_Child_Identity{})
// 	suite.CRUD.Create(&suite.Provider, &runner)

// 	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
// 	suite.Equal(types.EntityEqual(entityRead, &runner), true)
// }

func (suite *SanityTypesTestSuite) TestIdentityRef() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.IdentityRefValue = ysanity.ChildIdentity{}
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func (suite *SanityTypesTestSuite) TestListMaxElements() {
	runner := ysanity.Runner{}
	elems := make([]ysanity.Runner_OneList_Ldata, 0)
	for i := 0; i < 10; i++ {
		l := ysanity.Runner_OneList_Ldata{}
		l.Number = i
		l.Name = strconv.Itoa(i)
		elems = append(elems, l)
	}
	runner.OneList.Ldata = elems
	// The payload errMsg is hardcoded with message-id of certain value.
	// Please change corresponding message-id if new tests are added/enabled.
	errMsg := `<rpc-error>
    <error-type>application</error-type>
    <error-tag>operation-failed</error-tag>
    <error-severity>error</error-severity>
    <error-app-tag>too-many-elements</error-app-tag>
    <error-path xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity">
    /rpc/edit-config/config/ydkut:runner/ydkut:one-list/ydkut:ldata
  </error-path>
    <error-message xml:lang="en">too many /runner/one-list/ldata, 10 configured, at most 5 must be configured</error-message>
    <error-info xmlns:tailf="http://tail-f.com/ns/netconf/params/1.1" xmlns:ydkut="http://cisco.com/ns/yang/ydktest-sanity">
      <tailf:bad-instance-count>
        <tailf:bad-element>/ydkut:runner/ydkut:one-list/ydkut:ldata</tailf:bad-element>
        <tailf:instances>10</tailf:instances>
        <tailf:max-instances>5</tailf:max-instances>
      </tailf:bad-instance-count>
    </error-info>
  </rpc-error>
</rpc-reply>
`
	funcDidPanic, panicValue := didPanic(func() { suite.CRUD.Create(&suite.Provider, &runner) })
	suite.Equal(funcDidPanic, true)
	suite.Regexp("YGOServiceProviderError:", panicValue)
	suite.Regexp(errMsg, panicValue)
}

func (suite *SanityTypesTestSuite) TestSubmodule() {
	subtest := ysanity.SubTest{}
	subtest.OneAug.Name = "test"
	subtest.OneAug.Number = 3
	suite.CRUD.Create(&suite.Provider, &subtest)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.SubTest{})
	suite.Equal(types.EntityEqual(entityRead, &subtest), true)
}

func (suite *SanityTypesTestSuite) TestIdentityFromOtherModule() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.IdentityRefValue = ysanity_types.YdktestType{}
	suite.CRUD.Create(&suite.Provider, &runner)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(entityRead, &runner), true)
}

func TestSanityTypesTestSuite(t *testing.T) {
	suite.Run(t, new(SanityTypesTestSuite))
}
