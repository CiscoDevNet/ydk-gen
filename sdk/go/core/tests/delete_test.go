package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/types/yfilter"
	"github.com/stretchr/testify/suite"
	"strconv"
	"testing"
)

func getE(code int) ysanity.Runner_InbtwList_Ldata {
	e := ysanity.Runner_InbtwList_Ldata{}
	e.Number = code
	e.Name = strconv.Itoa(code)
	e.Subc.Number = code * 10
	e.Subc.Name = strconv.Itoa(code * 10)
	return e
}

func getEE(code int) ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1 {
	ee := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}
	ee.Number = code
	ee.Name = strconv.Itoa(code)
	return ee
}

func getNestedObject() ysanity.Runner {
	runner := ysanity.Runner{}

	e1 := getE(1)
	e2 := getE(2)

	ee11 := getEE(11)
	ee12 := getEE(12)

	e1.Subc.SubcSubl1 = append(e1.Subc.SubcSubl1, ee11)
	e1.Subc.SubcSubl1 = append(e1.Subc.SubcSubl1, ee12)

	ee21 := getEE(21)
	ee22 := getEE(22)

	e2.Subc.SubcSubl1 = append(e2.Subc.SubcSubl1, ee21)
	e2.Subc.SubcSubl1 = append(e2.Subc.SubcSubl1, ee22)

	runner.InbtwList.Ldata = append(runner.InbtwList.Ldata, e1)
	runner.InbtwList.Ldata = append(runner.InbtwList.Ldata, e2)

	return runner
}

type DeleteTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *DeleteTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *DeleteTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *DeleteTestSuite) BeforeTest(suiteName, testName string) {
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *DeleteTestSuite) TestDeleteObjectOnLeaf() {
	runnerCreate := ysanity.Runner{}
	runnerCreate.One.Name = "runner.One.Name"
	runnerCreate.Two.Name = "runner.Two.Name"
	suite.CRUD.Create(&suite.Provider, &runnerCreate)

	// Use YFilter Delete and CRUD update to remove leaf
	runnerUpdate := ysanity.Runner{}
	runnerUpdate.One.Name = yfilter.Delete
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerCmp := ysanity.Runner{}
	runnerCmp.Two.Name = "runner.Two.Name"

	suite.Equal(types.EntityEqual(entityRead, &runnerCmp), true)
}

// TODO: leaf-list encoding error
// func (suite *DeleteTestSuite) TestDeleteOnLeafListSlice() {
//     runnerCreate := ysanity.Runner{}
//     runnerCreate.One.Name = "runner.One.Name"
//     // TODO: leaf-list encoding error
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "1")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "2")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "3")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "4")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "5")
//     suite.CRUD.Create(&suite.Provider, &runnerCreate)

//     runnerUpdate := ysanity.Runner{}
//     runnerUpdate.Ytypes.BuiltInT.Llstring = append(runnerUpdate.Ytypes.BuiltInT.Llstring, "1")
//     runnerUpdate.Ytypes.BuiltInT.Llstring = append(runnerUpdate.Ytypes.BuiltInT.Llstring, "3")
//     // TODO: leaf-list is declared as []interface, how to assign YFilter to them?
//     runnerUpdate.Ytypes.BuiltInT.Llstring = yfilter.Delete
//     suite.CRUD.Update(&suite.Provider, &runnerUpdate)
//     entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})

//     runnerCmp := ysanity.Runner{}
//     runnerCmp.One.Name = "runner.One.Name"
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "2")
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "4")
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "5")
//     suite.Equal(types.EntityEqual(entityRead, &runnerCmp), true)
// }

// TODO: delete leaf from leaf-list
// func (suite *DeleteTestSuite) TestDeleteOnLeafList() {
//     runnerCreate := ysanity.Runner{}
//     runnerCreate.One.Name = "runner.One.Name"
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "0")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "1")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "2")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "3")
//     runnerCreate.Ytypes.BuiltInT.Llstring = append(runnerCreate.Ytypes.BuiltInT.Llstring, "4")

//     suite.CRUD.Create(&suite.Provider, &runnerCreate)

//     runnerUpdate := ysanity.Runner{}
//     // TODO: how to target a particular leaf from leaf-list using YFilter?
//     runnerUpdate.Ytypes.BuiltInT.Llstring = append(runnerUpdate.Ytypes.BuiltInT.Llstring, "3")
//     // runnerUpdate.Ytypes.BuiltInT.Llstring = yfilter.Delete
//     suite.CRUD.Update(&suite.Provider, &runnerUpdate)

//     entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})

//     runnerCmp := ysanity.Runner{}
//     runnerCmp.One.Name = "runner.One.Name"
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "0")
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "1")
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "2")
//     runnerCmp.Ytypes.BuiltInT.Llstring = append(runnerCmp.Ytypes.BuiltInT.Llstring, "4")

//     suite.Equal(types.EntityEqual(entityRead, &runnerCmp), true)
// }

func (suite *DeleteTestSuite) TestDeleteOnListWithIdentitykey() {
	runner := ysanity.Runner{}
	il := ysanity.Runner_OneList_IdentityList{}
	il.Config.Id = ysanity.Child_Identity{}
	il.IdRef = ysanity.Child_Identity{}

	runner.OneList.IdentityList = append(runner.OneList.IdentityList, il)
	suite.CRUD.Create(&suite.Provider, &runner)

	runnerUpdate := ysanity.Runner{}
	k := ysanity.Runner_OneList_IdentityList{}
	k.Config.Id = ysanity.Child_Identity{}
	k.IdRef = ysanity.Child_Identity{}
	k.YFilter = yfilter.Delete
	runnerUpdate.OneList.IdentityList = append(runnerUpdate.OneList.IdentityList, k)
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})

	suite.Equal(types.EntityEqual(entityRead, &ysanity.Runner{}), true)
}

func (suite *DeleteTestSuite) TestDeleteOnContainer() {
	runnerCreate := ysanity.Runner{}
	runnerCreate.One.Name = "runner.One.Name"
	runnerCreate.Two.Name = "runner.Two.Name"
	suite.CRUD.Create(&suite.Provider, &runnerCreate)

	runnerUpdate := ysanity.Runner{}
	runnerUpdate.Two.YFilter = yfilter.Delete
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	runnerCmp := ysanity.Runner{}
	runnerCmp.One.Name = "runner.One.Name"

	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})

	suite.Equal(types.EntityEqual(entityRead, &runnerCmp), true)
}

// TODO: Delete whole list using YFilter
// func (suite *DeleteTestSuite) TestDeleteOnNestedList() {
// 	runnerCreate := getNestedObject()
// 	fmt.Println(ee22)
// 	suite.CRUD.Create(&suite.Provider, &runnerCreate)

//     entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
//     runnerUpdate := entity.(*ysanity.Runner)

//     // TODO: YANG list is printed as []interface, not able to assign YFilter Delete to YANG list
//     runnerUpdate.InbtwList.Ldata[1].Subc.SubcSubl1 = yfilter.Delete
//     suite.CRUD.Update(&suite.Provider, runnerUpdate)

//     entity = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
//     runnerCmp := runnerCreate
//     runnerCmp.InbtwList.Ldata[1].Subc.SubcSubl1 = runnerCmp.InbtwList.Ldata[1].Subc.SubcSubl1[:0]

//     suite.Equal(types.EntityEqual(entity, &runnerCmp), true)
// }

func (suite *DeleteTestSuite) TestDeleteOnListElement() {
	runnerCreate := getNestedObject()
	suite.CRUD.Create(&suite.Provider, &runnerCreate)

	runnerUpdate := runnerCreate
	runnerUpdate.InbtwList.Ldata[1].YFilter = yfilter.Delete
	suite.CRUD.Update(&suite.Provider, &runnerUpdate)

	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})

	runnerCmp := runnerCreate
	runnerCmp.InbtwList.Ldata = runnerCmp.InbtwList.Ldata[:1]

	suite.Equal(types.EntityEqual(entity, &runnerCmp), true)
}

func (suite *DeleteTestSuite) TestDeleteOnListElements() {
	runnerCreate := ysanity.Runner{}
	runnerCreate.One.Name = "one"
	foo := ysanity.Runner_OneList_Ldata{}
	bar := ysanity.Runner_OneList_Ldata{}
	baz := ysanity.Runner_OneList_Ldata{}
	foo.Number = 1
	foo.Name = "foo"
	bar.Number = 2
	bar.Name = "bar"
	baz.Number = 3
	baz.Name = "baz"

	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, foo)
	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, bar)
	runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, baz)

	suite.CRUD.Create(&suite.Provider, &runnerCreate)

	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	runnerUpdate := entity.(*ysanity.Runner)
	runnerUpdate.OneList.Ldata[1].YFilter = yfilter.Delete
	runnerUpdate.OneList.Ldata[2].YFilter = yfilter.Delete

	suite.CRUD.Update(&suite.Provider, runnerUpdate)

	entity = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})

	runnerCmp := runnerCreate
	runnerCmp.OneList.Ldata = runnerCmp.OneList.Ldata[:1]

	suite.Equal(types.EntityEqual(entity, &runnerCmp), true)
}

// TODO: Delete list using YFilter
// func (suite *DeleteTestSuite) TestDeleteOnList() {
//     runnerCreate := ysanity.Runner{}
//     runnerCreate.One.Name = "one"
//     foo := ysanity.Runner_OneList_Ldata{}
//     bar := ysanity.Runner_OneList_Ldata{}
//     foo.Number = 1
//     foo.Name = "foo"
//     bar.Number = 2
//     bar.Name = "bar"

//     runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, foo)
//     runnerCreate.OneList.Ldata = append(runnerCreate.OneList.Ldata, bar)

//     suite.CRUD.Create(&suite.Provider, &runnerCreate)

//     entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
//     runnerUpdate := entity.(*ysanity.Runner)

//     // TODO: Delete whole list using YFilter
//     // runnerUpdate.OneList.Ldata = yfilter.Delete
//     // suite.CRUD.Update(&suite.Provider, runnerUpdate)

//     runnerCmp := runnerCreate
//     runnerCmp.OneList.Ldata = runnerCmp.OneList.Ldata[:0]

//     suite.Equal(types.EntityEqual(entity, &runnerCmp), true)
// }

func TestDeleteTestSuite(t *testing.T) {
	suite.Run(t, new(DeleteTestSuite))
}
