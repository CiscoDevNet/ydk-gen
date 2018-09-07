package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	ysanity_types "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity_types"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/ylist"
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
	elems := make([]*ysanity.Runner_OneList_Ldata, 0)
	for i := 0; i < 10; i++ {
		l := ysanity.Runner_OneList_Ldata{}
		l.Number = i
		l.Name = strconv.Itoa(i)
		elems = append(elems, &l)
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
	suite.Regexp("YServiceProviderError:", panicValue)
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

func (suite *SanityTypesTestSuite) TestCascadingTypes() {
	cascadingTypesHelper(suite, ysanity.CompInstType_unknown, ysanity.CompInstType__unknown)
	cascadingTypesHelper(suite, ysanity.CompInstType_phys, ysanity.CompInstType__phys)
	cascadingTypesHelper(suite, ysanity.CompInstType_virt, ysanity.CompInstType__virt)
	cascadingTypesHelper(suite, ysanity.CompInstType_hv, ysanity.CompInstType__hv)
}

func cascadingTypesHelper(suite *SanityTypesTestSuite, enum1 ysanity.CompInstType, enum2 ysanity.CompInstType_){
	ctypes := ysanity.CascadingTypes{}
	ctypes.CompInstType = enum1
	ctypes.CompNicInstType = enum2
	suite.CRUD.Create(&suite.Provider, &ctypes)

	ctypesRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(ctypesRead, &ctypes), true)
}

func (suite *SanityTypesTestSuite) TestCapitalLetters() {
	native := ysanity.Native{}
	native.Interface = ysanity.Native_Interface{}
	ge := ysanity.Native_Interface_GigabitEthernet{Name: "test"}
	native.Interface.GigabitEthernet = append(native.Interface.GigabitEthernet, &ge)
	suite.CRUD.Create(&suite.Provider, &native)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Native{})
	suite.Equal(types.EntityEqual(entityRead, &native), true)
}

func (suite *SanityTypesTestSuite) TestPresence() {
	var runner ysanity.Runner
	var runnerRead ysanity.Runner
	var readEntity types.Entity

	// Setting Presence
	runner = ysanity.Runner{}
	types.SetPresenceFlag(&runner.Outer.Inner)
	suite.Equal(true, runner.Outer.Inner.YPresence)

	suite.CRUD.Create(&suite.Provider, &runner)
	readEntity = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead = *readEntity.(*ysanity.Runner)
	suite.Equal(true, types.EntityEqual(&runnerRead, &runner))
	suite.Equal(runner.Outer.Inner.YPresence, runnerRead.Outer.Inner.YPresence)

	// CRUD Delete
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})

	// Setting Data
	runner = ysanity.Runner{}
	runner.Runner2.SomeLeaf = "test"

	suite.CRUD.Create(&suite.Provider, &runner)
	readEntity = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerRead = *readEntity.(*ysanity.Runner)
	suite.Equal(true, types.EntityEqual(&runnerRead, &runner))
	suite.Equal(runner.Outer.Inner.YPresence, runnerRead.Outer.Inner.YPresence)
}

func (suite *SanityTypesTestSuite) TestEntityCollection() {
    // Create Data entities and access values
    runner := ysanity.Runner{}
    native := ysanity.Native{}
    runnerPath := types.GetSegmentPath(&runner)
    nativePath := types.GetSegmentPath(&native)
    suite.Equal(types.EntityToString(&runner), "Type: *sanity.Runner, Path: ydktest-sanity:runner")
    suite.Equal(types.EntityToString(&native), "Type: *sanity.Native, Path: ydktest-sanity:native")

    // Initialization
    config := types.NewEntityCollection()
    suite.Equal(config.Len(), 0)
    suite.Equal(config.String(), "EntityCollection is empty")

    config = types.NewEntityCollection(&runner)
    suite.Equal(config.Len(), 1)

    config = types.NewEntityCollection(&runner, &native)
    suite.Equal(config.Len(), 2)
    suite.Equal(config.String(), "EntityCollection [Type: *sanity.Runner, Path: ydktest-sanity:runner; Type: *sanity.Native, Path: ydktest-sanity:native]")

    // Add
    config = types.NewEntityCollection()
    config.Append([]types.Entity{&runner, &native})
    suite.Equal(config.Len(), 2)

    config.Add(&runner)
    suite.Equal(config.Len(), 2)

    // Get
    e, _ := config.Get(runnerPath)
    suite.NotNil(e)
    suite.IsType(&runner, e)
    
    // HasKey
    suite.Equal(config.HasKey(runnerPath), true)
    suite.Equal(config.HasKey(nativePath), true)
    suite.Equal(config.HasKey("oc_bgp"), false)

    // Get all keys
    suite.Equal(config.Keys(), []string{runnerPath, nativePath})

    // Get all entities
    ydk.YLogDebug("All entities:")
    for _, entity := range config.Entities() {
    	ydk.YLogDebug(types.EntityToString(entity))
    }

    // Delete entity
    e, _ = config.Pop(runnerPath)
    suite.NotNil(e)
    suite.IsType(&runner, e)
    suite.Equal(config.Keys(), []string{nativePath})

    ydk.YLogDebug("All entities after Runner deleted:")
    for _, key := range config.Keys() {
        en, exist := config.Get(key)
        if exist {
	        ydk.YLogDebug(fmt.Sprintf("%s", types.EntityToString(en)))
        }
    }
    
    // Add back and test order
    config.Add(&runner)
    suite.Equal(config.Keys(), []string{nativePath, runnerPath})

	// Getting enities by item number
    ydk.YLogDebug("Getting enities by item number:")
	for i:=0; i<3; i++ {
		e = config.GetItem(i)
		if e != nil {
			ydk.YLogDebug(fmt.Sprintf("%d:  %s", i, types.EntityToString(e)))
		} else {
			ydk.YLogDebug(fmt.Sprintf("%d:  nil\n", i))
		}
	}
    // Clear collection
    config.Clear()
    suite.Equal(config.Len(), 0)
    
    // Testing passing parameters and return values
    ret1 := testParams("test1", config)
    col1 := types.EntityToCollection(ret1)
    suite.Equal(col1.Len(), 0)

    ret2 := testParams("test2", &runner)
    suite.Equal(types.EntityEqual(ret2, &runner), true)

    config.Add(&runner, &native)
    ret3 := testParams("test3", config)
    col3 := types.EntityToCollection(ret3)
    suite.Equal(col3.Len(), 2)
    
    // Testing Config and Filter aliases
    cfg := types.NewConfig()
	cfg.Add(&runner, &native)
	suite.Equal(types.IsEntityCollection(cfg), true)
	filter := types.NewFilter(&native)
	suite.Equal(types.IsEntityCollection(filter), true)
	suite.Equal(filter.Len(), 1)
}

func (suite *SanityTypesTestSuite) TestListTwoKeys() {
	runner := ysanity.Runner{}
	l1 := ysanity.Runner_TwoKeyList{First: "f1", Second: 11}
	l2 := ysanity.Runner_TwoKeyList{First: "f2", Second: 22}
	runner.TwoKeyList = []*ysanity.Runner_TwoKeyList {&l1, &l2}

	ldataKeys := ylist.Keys(runner.TwoKeyList)
	suite.Equal(fmt.Sprintf("%v", ldataKeys), "[[f1 11] [f2 22]]")

	for _, lkey := range ldataKeys {
		_, ldata := ylist.Get(runner.TwoKeyList, lkey);
		suite.NotNil(ldata)
	}
	_, ldata := ylist.Get(runner.TwoKeyList, "f1", 11)
	suite.Equal(types.EntityEqual(ldata, &l1), true)
	_, ldata = ylist.Get(runner.TwoKeyList, "f2", 22)
	suite.Equal(types.EntityEqual(ldata, &l2), true)

	i, ldata := ylist.Get(runner.TwoKeyList, "f1", 11)
	suite.Equal(i, 0)
	suite.NotNil(ldata)
	suite.Equal(types.EntityEqual(ldata, &l1), true)
	if ldata != nil {
		runner.TwoKeyList = append(runner.TwoKeyList[:i], runner.TwoKeyList[i+1:]...)
		i, ldata = ylist.Get(runner.TwoKeyList, "f1", 11)
		suite.Nil(ldata)
	}
}

func (suite *SanityTypesTestSuite) TestIdentityList() {
	runner := ysanity.Runner{}
	i1 := ysanity.Runner_IdentityList{Name: "first"}
	i2 := ysanity.Runner_IdentityList{Name: "second"}
	i3 := ysanity.Runner_IdentityList{Name: "third"}
	runner.IdentityList = []*ysanity.Runner_IdentityList {&i1, &i2, &i3}

	ldataKeys := ylist.Keys(runner.IdentityList)
	suite.Equal(fmt.Sprintf("%v", ldataKeys), "[first second third]")

	var ldata types.Entity
	for _, lkey := range ldataKeys {
		_, ldata = ylist.Get(runner.IdentityList, lkey);
		suite.NotNil(ldata)
	}
	_, ldata = ylist.Get(runner.IdentityList, "first")
	suite.Equal(types.EntityEqual(ldata, &i1), true)
	_, ldata = ylist.Get(runner.IdentityList, "third")
	suite.Equal(types.EntityEqual(ldata, &i3), true)
}

func (suite *SanityTypesTestSuite) TestEnumList() {
	runner := ysanity.Runner{}
	i1 := ysanity.Runner_EnumList{Name: ysanity.YdkEnumTest_none}
	i2 := ysanity.Runner_EnumList{Name: ysanity.YdkEnumTest_local}
	i3 := ysanity.Runner_EnumList{Name: ysanity.YdkEnumTest_remote}
	runner.EnumList = []*ysanity.Runner_EnumList{&i1, &i2, &i3}

	ldataKeys := ylist.Keys(runner.EnumList)
	suite.Equal(fmt.Sprintf("%v", ldataKeys), "[none local remote]")

	var ldata types.Entity
	for _, lkey := range ldataKeys {
		_, ldata = ylist.Get(runner.EnumList, lkey);
		suite.NotNil(ldata)
	}
	_, ldata = ylist.Get(runner.EnumList, "none")
	suite.Equal(types.EntityEqual(ldata, &i1), true)
	_, ldata = ylist.Get(runner.EnumList, "remote")
	suite.Equal(types.EntityEqual(ldata, &i3), true)
}

func (suite *SanityTypesTestSuite) TestListNoKeys() {
	runner := ysanity.Runner{}
	t1 := ysanity.Runner_NoKeyList{Test: "t1"}
	t2 := ysanity.Runner_NoKeyList{Test: "t2"}
	t3 := ysanity.Runner_NoKeyList{Test: "t3"}
	runner.NoKeyList = []*ysanity.Runner_NoKeyList {&t1, &t2, &t3}

	suite.Equal(len(ylist.Keys(runner.NoKeyList)), 0)
	var count string
	for _, elem := range runner.NoKeyList {
		count = count + elem.Test.(string)
	}
	suite.Equal(count, "t1t2t3")
}

func testParams(testName string, entity types.Entity) types.Entity {
	ec := types.EntityToCollection(entity)
	if ec == nil {
		ydk.YLogDebug(fmt.Sprintf("%s: %s\n", testName, types.EntityToString(entity)))
		return entity
	}
	ydk.YLogDebug(fmt.Sprintf("%s: %s\n", testName, ec.String()))
	return ec
}

func TestSanityTypesTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(SanityTypesTestSuite))
}
