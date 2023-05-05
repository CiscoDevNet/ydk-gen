package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/CiscoDevNet/ydk-go/ydk/types/ylist"
	"github.com/stretchr/testify/suite"
	"testing"
)

type FiltersTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *FiltersTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *FiltersTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *FiltersTestSuite) BeforeTest(suiteName, testName string) {
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *FiltersTestSuite) TestReadOnRefClass() {
	r1 := ysanity.Runner{}
	r1.One.Number = 1
	r1.One.Name = "r1.One.Name"
	suite.CRUD.Create(&suite.Provider, &r1)

	r2 := ysanity.Runner{}
	r2.One.YFilter = yfilter.Read
	entity := suite.CRUD.Read(&suite.Provider, &r2)

	suite.Equal(types.EntityEqual(entity, &r1), true)
}

// TODO: Encoding error if using YFilter for non-top level leaf
// func (suite *FiltersTestSuite) TestReadOnLeaf() {
//     r1 := ysanity.Runner{}
//     r1.One.Number = 1
//     r1.One.Name = "r1.One.Name"
//     suite.CRUD.Create(&suite.Provider, &r1)

//     r2 := ysanity.Runner{}
//     r2.One.Number = yfilter.Read
//     entity := suite.CRUD.Read(&suite.Provider, &r2)

//     suite.Equal(types.EntityEqual(entity, &r1), true)

//     // r2.One.Number is a context match node:
//     // https://tools.ietf.org/html/rfc6241#section-6.2.5
//     // reads on r2.One.Number returns r2.One.Number and r2.One.Name
//     r2 = ysanity.Runner{}
//     r2.One.Number = 1
//     entity = suite.CRUD.Read(&suite.Provider, &r2)
//     suite.Equal(types.EntityEqual(entity, &r1), true)

//     r2 = ysanity.Runner{}
//     r2.One.Number = 2
//     entity = suite.CRUD.Read(&suite.Provider, &r2)

//     suite.Equal(types.EntityEqual(entity, &r2), true)
// }

func (suite *FiltersTestSuite) TestReadOnRefEnumClass() {
    r1 := ysanity.Runner{}
    r1.Ytypes.BuiltInT.EnumValue = ysanity.YdkEnumTest_local
    suite.CRUD.Create(&suite.Provider, &r1)

    r2 := ysanity.Runner{}
    r2.Ytypes.BuiltInT.EnumValue = yfilter.Read
    entity := suite.CRUD.Read(&suite.Provider, &r1)
    suite.True(types.EntityEqual(entity, &r1))
}

func (suite *FiltersTestSuite) TestReadOnRefList() {
    r1 := ysanity.Runner{}
    l1 := ysanity.Runner_OneList_Ldata{}
    l2 := ysanity.Runner_OneList_Ldata{}
    l1.Number = 1
    l2.Number = 2
    r1.OneList.Ldata = append(r1.OneList.Ldata, &l1)
    r1.OneList.Ldata = append(r1.OneList.Ldata, &l2)
    suite.CRUD.Create(&suite.Provider, &r1)

    r2 := ysanity.Runner{}
    ll := ysanity.Runner_OneList_Ldata{}
    ll.Number = 2
    types.SetEntityFilter(&ll, yfilter.Read)
    r2.OneList.Ldata = append(r2.OneList.Ldata, &ll)

    entity := suite.CRUD.Read(&suite.Provider, &r2)
    runner := entity.(*ysanity.Runner)
    _, ldata := ylist.Get(runner.OneList.Ldata, 2)
    suite.True(types.EntityEqual(&l2, ldata))
}

func (suite *FiltersTestSuite) TestReadOnListWithKey() {
	r1 := ysanity.Runner{}
	l1 := ysanity.Runner_OneList_Ldata{}
	l2 := ysanity.Runner_OneList_Ldata{}
	l1.Number = 1
	l2.Number = 2
	r1.OneList.Ldata = append(r1.OneList.Ldata, &l1)
	r1.OneList.Ldata = append(r1.OneList.Ldata, &l2)
	suite.CRUD.Create(&suite.Provider, &r1)

	r2 := ysanity.Runner{}
	r2.OneList.Ldata = append(r2.OneList.Ldata, &l1)
	entity := suite.CRUD.Read(&suite.Provider, &r2)

	cmpEntity := ysanity.Runner{}
	cmpEntity.OneList.Ldata = append(cmpEntity.OneList.Ldata, &l1)

	suite.Equal(types.EntityEqual(entity, &cmpEntity), true)
}

func (suite *FiltersTestSuite) TestReadOnLeaflist() {
	r1 := ysanity.Runner{}
	r1.Ytypes.BuiltInT.Llstring = append(r1.Ytypes.BuiltInT.Llstring, "1")
	r1.Ytypes.BuiltInT.Llstring = append(r1.Ytypes.BuiltInT.Llstring, "2")
	r1.Ytypes.BuiltInT.Llstring = append(r1.Ytypes.BuiltInT.Llstring, "3")
	suite.CRUD.Create(&suite.Provider, &r1)

	r2 := ysanity.Runner{}
	r2.Ytypes.BuiltInT.Llstring = append(r2.Ytypes.BuiltInT.Llstring, yfilter.Read)
	entity := suite.CRUD.Read(&suite.Provider, &r2)
	suite.True(types.EntityEqual(entity, &r1))
}

// TODO: Encoding error if using YFilter for identity ref
// func (suite *FiltersTestSuite) TestReadOnIdentityRef() {
//     r1 := ysanity.Runner{}
//     r1.Ytypes.BuiltInT.IdentityRefValue = ysanity.ChildIdentity{}
//     suite.CRUD.Create(&suite.Provider, &r1)
//
//     r2 := ysanity.Runner{}
//     r2.Ytypes.BuiltInT.IdentityRefValue = yfilter.Read
//     entity := suite.CRUD.Read(&suite.Provider, &r2)
//     suite.Equal(types.EntityEqual(entity, &r1), true)
// }

// TODO: Encoding error if using YFilter for non-top level leaf
// func (suite *FiltersTestSuite) TestReadOnlyConfig() {
//     r1 := ysanity.Runner{}
//     r1.One.Number = 1
//     r1.One.Name = "r1.One.Name"
//     suite.CRUD.Create(&suite.Provider, &r1)
//
//     r2 := ysanity.Runner{}
//     r2.One.Number = yfilter.Read
//     entity2 := suite.CRUD.ReadConfig(&suite.Provider, &r2)
//     r2Read := entity2.(*ysanity.Runner)
//     suite.Equal(r1.One.Number, r2Read.One.Number)
// }

func (suite *FiltersTestSuite) TestDecoder() {
	r1 := ysanity.Runner{}
	l := ysanity.Runner_OneList_Ldata{}
	l.Number = 1
	l.Name = "l.Name"
	r1.OneList.Ldata = append(r1.OneList.Ldata, &l)
	suite.CRUD.Create(&suite.Provider, &r1)

	r2 := ysanity.Runner{}
	entity := suite.CRUD.Read(&suite.Provider, &r2)

	suite.Equal(types.EntityEqual(entity, &r1), true)
}

func TestFiltersTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(FiltersTestSuite))
}
