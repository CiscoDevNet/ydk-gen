package test

import (
	"fmt"
	filterread "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/filterread"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/stretchr/testify/suite"
	"testing"
)

func getInitEntity() filterread.A {
	a := filterread.A{}
	a.A1 = "a.A1"
	a.A2 = "a.A2"
	a.A3 = "a.A3"
	a.B.B1 = "a.B.B1"
	a.B.B2 = "a.B.B2"
	a.B.B3 = "a.B.B3"
	a.B.F = filterread.A_B_F{}
	a.B.F.F1 = "a.B.F.F1"
	a.B.C = filterread.A_B_C{}
	a.B.D.D1 = "a.B.D.D1"
	a.B.D.D2 = "a.B.D.D2"
	a.B.D.D3 = "a.B.D.D3"
	a.B.D.E.E1 = "a.B.D.E.E1"
	a.B.D.E.E2 = "a.B.D.E.E2"

	l1 := filterread.A_Lst{}
	l2 := filterread.A_Lst{}
	l3 := filterread.A_Lst{}

	l1.Number = 1
	l1.Value = "l1.Value"

	l2.Number = 2
	l2.Value = "l2.Value"

	l3.Number = 3
	l3.Value = "l3.Value"

	a.Lst = append(a.Lst, &l1)
	a.Lst = append(a.Lst, &l2)
	a.Lst = append(a.Lst, &l3)

	return a
}

type FilterReadTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *FilterReadTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()

	suite.CRUD.Delete(&suite.Provider, &filterread.A{})
	initEntity := getInitEntity()
	suite.CRUD.Create(&suite.Provider, &initEntity)
}

func (suite *FilterReadTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *FilterReadTestSuite) TearDownTest() {
}

func (suite *FilterReadTestSuite) Test1() {
	// Read on top container returns all data
	a := filterread.A{}
	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := getInitEntity()
	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

func (suite *FilterReadTestSuite) Test2() {
	// According to https://tools.ietf.org/html/rfc6241#section-6.2.5,
	// `a.A1` is a content match node. Reads on `a` returns all data.
	a := filterread.A{}
	a.A1 = "a.A1"
	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := getInitEntity()
	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

func (suite *FilterReadTestSuite) Test3() {
	// Read on leaf
	a := filterread.A{}
	a.A1 = yfilter.Read
	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := filterread.A{}
	initEntity.A1 = "a.A1"
	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

func (suite *FilterReadTestSuite) Test4() {
	// According to https://tools.ietf.org/html/rfc6241#section-6.2.5,
	// `a.B.B1` is a content match node.
	a := filterread.A{}
	a.B.B1 = "a.B.B1"
	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := filterread.A{}
	initEntity.B.B1 = "a.B.B1"
	initEntity.B.B2 = "a.B.B2"
	initEntity.B.B3 = "a.B.B3"

	initEntity.B.F = filterread.A_B_F{}
	initEntity.B.F.F1 = "a.B.F.F1"
	initEntity.B.C = filterread.A_B_C{}
	initEntity.B.D.D1 = "a.B.D.D1"
	initEntity.B.D.D2 = "a.B.D.D2"
	initEntity.B.D.D3 = "a.B.D.D3"
	initEntity.B.D.E.E1 = "a.B.D.E.E1"
	initEntity.B.D.E.E2 = "a.B.D.E.E2"

	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

func (suite *FilterReadTestSuite) Test5() {
	// According to https://tools.ietf.org/html/rfc6241#section-6.2.5,
	// `a.B.D.E.E1` is a content match node.
	a := filterread.A{}
	a.B.D.E.E1 = "a.B.D.E.E1"
	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := filterread.A{}
	initEntity.B.D.E.E1 = "a.B.D.E.E1"
	initEntity.B.D.E.E2 = "a.B.D.E.E2"

	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

// // empty presence container encoding error
// // func (suite *FilterReadTestSuite) Test6() {
// //     // `a.B.C` is an empty presence container
// //     a := filterread.A{}
// //     a.B.C = filterread.A_B_C{}
// //     entity := suite.CRUD.Read(&suite.Provider, &a)
//
// //     initEntity := filterread.A{}
// //     initEntity.B.C = filterread.A_B_C{}
//
// //     suite.Equal(types.EntityEqual(entity, &initEntity), true)
// // }

func (suite *FilterReadTestSuite) Test7() {
	// According to https://tools.ietf.org/html/rfc6241#section-6.2.5,
	// `item1.Number`, `item2.Number` is a content match node.
	a := filterread.A{}
	item1 := filterread.A_Lst{}
	item2 := filterread.A_Lst{}
	item1.Number = 1
	item2.Number = 2
	a.Lst = append(a.Lst, &item1)
	a.Lst = append(a.Lst, &item2)

	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := filterread.A{}
	item1.Value = "l1.Value"
	item2.Value = "l2.Value"
	initEntity.Lst = append(initEntity.Lst, &item1)
	initEntity.Lst = append(initEntity.Lst, &item2)

	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

func (suite *FilterReadTestSuite) Test8() {
	a := filterread.A{}
	a.B.F = filterread.A_B_F{}
	a.B.F.F1 = "a.B.F.F1"
	entity := suite.CRUD.Read(&suite.Provider, &a)

	initEntity := a

	suite.Equal(types.EntityEqual(entity, &initEntity), true)
}

func (suite *FilterReadTestSuite) BeforeTest(suiteName, testName string) {
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func TestFilterReadTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(FilterReadTestSuite))
}
