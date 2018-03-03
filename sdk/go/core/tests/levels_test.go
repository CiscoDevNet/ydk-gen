package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/stretchr/testify/suite"
	"testing"
)

type SanityLevelsTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *SanityLevelsTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
}

func (suite *SanityLevelsTestSuite) BeforeTest(suiteName, testName string) {
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *SanityLevelsTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *SanityLevelsTestSuite) TestOneLevelPos() {
	// READ
	runner := ysanity.Runner{}
	runner.YdktestSanityOne.Number = 1
	runner.YdktestSanityOne.Name = "runner:one:name"
	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// UPDATE
	runner = ysanity.Runner{}
	runner.YdktestSanityOne.Number = 10
	runner.YdktestSanityOne.Name = "runner/one/name"
	suite.CRUD.Update(&suite.Provider, &runner)
	runnerRead = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// // DELETE
	runner = ysanity.Runner{}
	suite.CRUD.Delete(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	entityRead := entity.(*ysanity.Runner)
	suite.Equal(types.HasDataOrFilter(entityRead), false)
}

func (suite *SanityLevelsTestSuite) TestTwoLevelPos() {
	// READ
	runner := ysanity.Runner{}
	runner.Two.Number = 2
	runner.Two.Name = "runner:two:name"
	runner.Two.Sub1.Number = 21
	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// UPDATE
	runner = ysanity.Runner{}
	runner.Two.Number = 210
	runner.Two.Name = "runner/two/name"
	runner.Two.Sub1.Number = 210
	suite.CRUD.Update(&suite.Provider, &runner)
	runnerRead = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// DELETE
	runner = ysanity.Runner{}
	suite.CRUD.Delete(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	entityRead := entity.(*ysanity.Runner)
	suite.Equal(types.HasDataOrFilter(entityRead), false)
}

func (suite *SanityLevelsTestSuite) TestThreeLevelPos() {
	// READ
	runner := ysanity.Runner{}
	runner.Three.Number = 3
	runner.Three.Name = "runner:three:name"
	runner.Three.Sub1.Number = 31
	runner.Three.Sub1.Sub2.Number = 311
	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// UPDATE
	runner = ysanity.Runner{}
	runner.Three.Number = 30
	runner.Three.Name = "runner/three/name"
	runner.Three.Sub1.Number = 310
	runner.Three.Sub1.Sub2.Number = 3110

	suite.CRUD.Update(&suite.Provider, &runner)
	runnerRead = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// DELETE
	runner = ysanity.Runner{}
	suite.CRUD.Delete(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	entityRead := entity.(*ysanity.Runner)
	suite.Equal(types.HasDataOrFilter(entityRead), false)
}

func (suite *SanityLevelsTestSuite) TestOneListNegDupKeyPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_OneList_Ldata{}
	elem2 := ysanity.Runner_OneList_Ldata{}
	elem1.Number = 1
	elem1.Name = "foo"
	elem2.Number = 1
	elem2.Name = "bar"
	runner.OneList.Ldata = append(runner.OneList.Ldata, elem1)
	runner.OneList.Ldata = append(runner.OneList.Ldata, elem2)
	suite.CRUD.Create(&suite.Provider, &runner)
	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, entityRead), true)
}

func (suite *SanityLevelsTestSuite) TestOneListPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_OneList_Ldata{}
	elem2 := ysanity.Runner_OneList_Ldata{}
	elem1.Number = 1
	elem1.Name = "name1"
	elem2.Number = 2
	elem2.Name = "name2"
	runner.OneList.Ldata = append(runner.OneList.Ldata, elem1)
	runner.OneList.Ldata = append(runner.OneList.Ldata, elem2)
	suite.CRUD.Create(&suite.Provider, &runner)
	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, entityRead), true)
}

func (suite *SanityLevelsTestSuite) TestTwoListPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_TwoList_Ldata{}
	elem2 := ysanity.Runner_TwoList_Ldata{}
	elem1.Number = 21
	elem1.Name = "runner:twolist:ldata:21"
	elem2.Number = 22
	elem2.Name = "runner:twolist:ldata:22"

	elem11 := ysanity.Runner_TwoList_Ldata_Subl1{}
	elem12 := ysanity.Runner_TwoList_Ldata_Subl1{}
	elem11.Number = 211
	elem11.Name = "runner:twolist:ldata:21:subl1:211"
	elem12.Number = 212
	elem12.Name = "runner:twolist:ldata:21:subl1:212"

	elem1.Subl1 = append(elem1.Subl1, elem11)
	elem1.Subl1 = append(elem2.Subl1, elem12)

	runner.TwoList.Ldata = append(runner.TwoList.Ldata, elem1)

	elem21 := ysanity.Runner_TwoList_Ldata_Subl1{}
	elem22 := ysanity.Runner_TwoList_Ldata_Subl1{}
	elem21.Number = 221
	elem21.Name = "runner:twolist:ldata:22:subl1:221"
	elem22.Number = 222
	elem22.Name = "runner:twolist:ldata:22:subl1:222"

	elem2.Subl1 = append(elem1.Subl1, elem11)
	elem2.Subl1 = append(elem2.Subl1, elem12)

	runner.TwoList.Ldata = append(runner.TwoList.Ldata, elem2)

	suite.CRUD.Create(&suite.Provider, &runner)
	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, entityRead), true)
}

func (suite *SanityLevelsTestSuite) TestThreeListPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_ThreeList_Ldata{}
	elem2 := ysanity.Runner_ThreeList_Ldata{}
	elem1.Number = 31
	elem2.Number = 32
	elem2.Name = "runner:threelist:ldata:31"
	elem2.Name = "runner:threelist:ldata:32"

	elem11 := ysanity.Runner_ThreeList_Ldata_Subl1{}
	elem12 := ysanity.Runner_ThreeList_Ldata_Subl1{}

	elem11.Number = 311
	elem12.Number = 312
	elem11.Name = "runner:threelist:ldata:31:subl1:311"
	elem12.Name = "runner:threelist:ldata:31:subl1:312"

	elem111 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}
	elem112 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}

	elem111.Number = 3111
	elem111.Name = "runner:threelist:ldata:31:subl1:311:subsubl1:3111"
	elem112.Number = 3112
	elem112.Name = "runner:threelist:ldata:31:subl1:311:subsubl1:3112"

	elem11.SubSubl1 = append(elem11.SubSubl1, elem111)
	elem11.SubSubl1 = append(elem11.SubSubl1, elem112)

	elem121 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}
	elem122 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}

	elem121.Number = 3121
	elem121.Name = "runner:threelist:ldata:31:subl1:311:subsubl1:3121"
	elem122.Number = 3122
	elem122.Name = "runner:threelist:ldata:31:subl1:311:subsubl1:3122"

	elem12.SubSubl1 = append(elem12.SubSubl1, elem121)
	elem12.SubSubl1 = append(elem12.SubSubl1, elem122)

	elem1.Subl1 = append(elem1.Subl1, elem11)
	elem1.Subl1 = append(elem1.Subl1, elem12)

	elem21 := ysanity.Runner_ThreeList_Ldata_Subl1{}
	elem22 := ysanity.Runner_ThreeList_Ldata_Subl1{}

	elem21.Number = 321
	elem22.Number = 322
	elem21.Name = "runner:threelist:ldata:32:subl1:321"
	elem22.Name = "runner:threelist:ldata:32:subl1:322"

	elem211 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}
	elem212 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}

	elem211.Number = 3211
	elem211.Name = "runner:threelist:ldata:32:subl1:321:subsubl1:3211"
	elem212.Number = 3212
	elem212.Name = "runner:threelist:ldata:32:subl1:321:subsubl1:3212"

	elem21.SubSubl1 = append(elem21.SubSubl1, elem211)
	elem21.SubSubl1 = append(elem21.SubSubl1, elem212)

	elem221 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}
	elem222 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}

	elem221.Number = 3221
	elem221.Name = "runner:threelist:ldata:32:subl1:321:subsubl1:3221"
	elem222.Number = 3222
	elem222.Name = "runner:threelist:ldata:32:subl1:321:subsubl1:3222"

	elem22.SubSubl1 = append(elem22.SubSubl1, elem221)
	elem22.SubSubl1 = append(elem22.SubSubl1, elem222)

	elem2.Subl1 = append(elem2.Subl1, elem21)
	elem2.Subl1 = append(elem2.Subl1, elem22)

	suite.CRUD.Create(&suite.Provider, &runner)
	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, entityRead), true)
}

/*func (suite *SanityLevelsTestSuite) TestNestedNaming() {
	n1 := ysanity.Runner_NestedNaming_NestedNaming{}
	n2 := ysanity.Runner_NestedNaming{}
	n1.NestedNaming.NestedNaming = 1
	n2.NestedNaming.NestedNaming.NestedNaming = 1
	suite.Equal(n1.NestedNaming.NestedNaming, n2.NestedNaming.NestedNaming.NestedNaming)
}*/

func (suite *SanityLevelsTestSuite) TestInbtwListPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_InbtwList_Ldata{}
	elem2 := ysanity.Runner_InbtwList_Ldata{}

	elem1.Number = 11
	elem1.Name = "runner:inbtwlist:11"
	elem1.Subc.Number = 121
	elem2.Number = 12
	elem2.Name = "runner:inbtwlist:12"
	elem2.Subc.Number = 121

	elem11 := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}
	elem12 := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}

	elem11.Number = 111
	elem11.Name = "runner:inbtwlist:11:subc::subcsubl1:111"

	elem12.Number = 112
	elem12.Name = "runner:inbtwlist:12:subc:subcsubl1:112"

	elem1.Subc.SubcSubl1 = append(elem1.Subc.SubcSubl1, elem11)
	elem1.Subc.SubcSubl1 = append(elem1.Subc.SubcSubl1, elem12)

	elem21 := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}
	elem22 := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}

	elem21.Number = 121
	elem21.Name = "runner:inbtwlist:21:subc::subcsubl1:121"

	elem22.Number = 122
	elem22.Name = "runner:inbtwlist:22:subc:subcsubl1:122"

	elem2.Subc.SubcSubl1 = append(elem2.Subc.SubcSubl1, elem21)
	elem2.Subc.SubcSubl1 = append(elem2.Subc.SubcSubl1, elem22)

	runner.InbtwList.Ldata = append(runner.InbtwList.Ldata, elem1)
	runner.InbtwList.Ldata = append(runner.InbtwList.Ldata, elem2)

	suite.CRUD.Create(&suite.Provider, &runner)
	entityRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, entityRead), true)
}

func (suite *SanityLevelsTestSuite) TestLeafrefSimplePos() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number8 = 100
	runner.Ytypes.BuiltInT.LeafRef = runner.Ytypes.BuiltInT.Number8

	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)
}

func (suite *SanityLevelsTestSuite) TestAugOnePos() {
	runner := ysanity.Runner{}
	runner.YdktestSanityOne.OneAug.Number = 1
	runner.YdktestSanityOne.OneAug.Name = "runner:one:one_aug"

	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)
}

func (suite *SanityLevelsTestSuite) TestAugOneListPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_OneList_OneAugList_Ldata{}
	elem2 := ysanity.Runner_OneList_OneAugList_Ldata{}

	elem1.Number = 1
	elem1.Name = "elem1"

	elem2.Number = 2
	elem2.Name = "elem2"

	runner.OneList.OneAugList.Ldata = append(runner.OneList.OneAugList.Ldata, elem1)
	runner.OneList.OneAugList.Ldata = append(runner.OneList.OneAugList.Ldata, elem2)
	runner.OneList.OneAugList.Enabled = true

	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)
}

func (suite *SanityLevelsTestSuite) TestParentEmpty() {
	runner := ysanity.Runner{}
	runner.Ytypes.Enabled = types.Empty{}
	runner.Ytypes.BuiltInT.Emptee = types.Empty{}

	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)
}

func TestSanityLevelsTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(SanityLevelsTestSuite))
}
