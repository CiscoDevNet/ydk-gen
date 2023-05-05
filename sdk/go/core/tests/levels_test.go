package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/CiscoDevNet/ydk-go/ydk/types"
	"github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/oc_pattern"
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
	runner.One.Number = 1
	runner.One.Name = "runner:one:name"
	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// UPDATE
	runner = ysanity.Runner{}
	runner.One.Number = 10
	runner.One.Name = "runner/one/name"
	suite.CRUD.Update(&suite.Provider, &runner)
	runnerRead = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// // DELETE
	runner = ysanity.Runner{}
	suite.CRUD.Delete(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Nil(entity)
}

func (suite *SanityLevelsTestSuite) TestOneLevelAugPos() {
	// READ
	runner := ysanity.Runner{}
	runner.YdktestSanityAugmOne.TwinNumber = 1000
	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// UPDATE
	runner = ysanity.Runner{}
	runner.YdktestSanityAugmOne.TwinNumber = 100
	suite.CRUD.Update(&suite.Provider, &runner)
	runnerRead = suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	// // DELETE
	runner = ysanity.Runner{}
	suite.CRUD.Delete(&suite.Provider, &runner)
	entity := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Nil(entity)
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
	suite.Nil(entity)
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
	suite.Nil(entity)
}

func (suite *SanityLevelsTestSuite) TestOneListNegDupKeyPos() {
	runner := ysanity.Runner{}
	elem1 := ysanity.Runner_OneList_Ldata{}
	elem2 := ysanity.Runner_OneList_Ldata{}
	elem1.Number = 1
	elem1.Name = "foo"
	elem2.Number = 1
	elem2.Name = "bar"
	runner.OneList.Ldata = append(runner.OneList.Ldata, &elem1)
	runner.OneList.Ldata = append(runner.OneList.Ldata, &elem2)
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
	runner.OneList.Ldata = append(runner.OneList.Ldata, &elem1)
	runner.OneList.Ldata = append(runner.OneList.Ldata, &elem2)
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

	elem1.Subl1 = append(elem1.Subl1, &elem11)
	elem1.Subl1 = append(elem2.Subl1, &elem12)

	runner.TwoList.Ldata = append(runner.TwoList.Ldata, &elem1)

	elem21 := ysanity.Runner_TwoList_Ldata_Subl1{}
	elem22 := ysanity.Runner_TwoList_Ldata_Subl1{}
	elem21.Number = 221
	elem21.Name = "runner:twolist:ldata:22:subl1:221"
	elem22.Number = 222
	elem22.Name = "runner:twolist:ldata:22:subl1:222"

	elem2.Subl1 = append(elem1.Subl1, &elem11)
	elem2.Subl1 = append(elem2.Subl1, &elem12)

	runner.TwoList.Ldata = append(runner.TwoList.Ldata, &elem2)

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
	runner.ThreeList.Ldata = append(runner.ThreeList.Ldata, &elem1)
	runner.ThreeList.Ldata = append(runner.ThreeList.Ldata, &elem2)

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

	elem11.SubSubl1 = append(elem11.SubSubl1, &elem111)
	elem11.SubSubl1 = append(elem11.SubSubl1, &elem112)

	elem121 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}
	elem122 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}

	elem121.Number = 3121
	elem121.Name = "runner:threelist:ldata:31:subl1:311:subsubl1:3121"
	elem122.Number = 3122
	elem122.Name = "runner:threelist:ldata:31:subl1:311:subsubl1:3122"

	elem12.SubSubl1 = append(elem12.SubSubl1, &elem121)
	elem12.SubSubl1 = append(elem12.SubSubl1, &elem122)

	elem1.Subl1 = append(elem1.Subl1, &elem11)
	elem1.Subl1 = append(elem1.Subl1, &elem12)

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

	elem21.SubSubl1 = append(elem21.SubSubl1, &elem211)
	elem21.SubSubl1 = append(elem21.SubSubl1, &elem212)

	elem221 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}
	elem222 := ysanity.Runner_ThreeList_Ldata_Subl1_SubSubl1{}

	elem221.Number = 3221
	elem221.Name = "runner:threelist:ldata:32:subl1:321:subsubl1:3221"
	elem222.Number = 3222
	elem222.Name = "runner:threelist:ldata:32:subl1:321:subsubl1:3222"

	elem22.SubSubl1 = append(elem22.SubSubl1, &elem221)
	elem22.SubSubl1 = append(elem22.SubSubl1, &elem222)

	elem2.Subl1 = append(elem2.Subl1, &elem21)
	elem2.Subl1 = append(elem2.Subl1, &elem22)

	suite.CRUD.Create(&suite.Provider, &runner)
	
	filter := ysanity.Runner{}
	entityRead := suite.CRUD.Read(&suite.Provider, &filter)
	suite.Equal(types.EntityEqual(&runner, entityRead), true)
}

/*func (suite *SanityLevelsTestSuite) TestNestedNaming() {
	n1 := ysanity.Runner_NestedNaming_NestedNaming_{}
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

	elem1.Subc.SubcSubl1 = append(elem1.Subc.SubcSubl1, &elem11)
	elem1.Subc.SubcSubl1 = append(elem1.Subc.SubcSubl1, &elem12)

	elem21 := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}
	elem22 := ysanity.Runner_InbtwList_Ldata_Subc_SubcSubl1{}

	elem21.Number = 121
	elem21.Name = "runner:inbtwlist:21:subc::subcsubl1:121"

	elem22.Number = 122
	elem22.Name = "runner:inbtwlist:22:subc:subcsubl1:122"

	elem2.Subc.SubcSubl1 = append(elem2.Subc.SubcSubl1, &elem21)
	elem2.Subc.SubcSubl1 = append(elem2.Subc.SubcSubl1, &elem22)

	runner.InbtwList.Ldata = append(runner.InbtwList.Ldata, &elem1)
	runner.InbtwList.Ldata = append(runner.InbtwList.Ldata, &elem2)

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
	oneAug := ysanity.Runner_One{}
	oneAug.Number = 1
	oneAug.Name = "runner:one:one_aug"

	suite.CRUD.Create(&suite.Provider, &oneAug)

	oneRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner_One{})
	suite.Equal(types.EntityEqual(&oneAug, oneRead), true)
}

func (suite *SanityLevelsTestSuite) TestAugOneListPos() {
	oneAugList := ysanity.Runner_OneList_OneAugList{}
	elem1 := ysanity.Runner_OneList_OneAugList_Ldata{}
	elem2 := ysanity.Runner_OneList_OneAugList_Ldata{}

	elem1.Number = 1
	elem1.Name = "elem1"

	elem2.Number = 2
	elem2.Name = "elem2"

	oneAugList.Ldata = append(oneAugList.Ldata, &elem1)
	oneAugList.Ldata = append(oneAugList.Ldata, &elem2)
	oneAugList.Enabled = true

	suite.CRUD.Create(&suite.Provider, &oneAugList)

	listRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner_OneList_OneAugList{})
	suite.Equal(types.EntityEqual(&oneAugList, listRead), true)
}

func (suite *SanityLevelsTestSuite) TestParentEmpty() {
	runner := ysanity.Runner{}
	runner.Ytypes.Enabled = types.Empty{}
	runner.Ytypes.BuiltInT.Emptee = types.Empty{}

	suite.CRUD.Create(&suite.Provider, &runner)
	runnerRead := suite.CRUD.Read(&suite.Provider, &ysanity.Runner{})
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)
}

func (suite *SanityLevelsTestSuite) TestMtus() {
	// CREATE
	mtu := ysanity.Runner_Mtus_Mtu{}
	mtu.Owner = "test"
	mtu.Mtu = 12
	suite.CRUD.Create(&suite.Provider, &mtu)

	// READ and VALIDATE
	mtuFilter := ysanity.Runner_Mtus_Mtu{}
	mtuFilter.Owner = "test"
	mtuReadEntity := suite.CRUD.Read(&suite.Provider, &mtuFilter)
	suite.NotNil(mtuReadEntity)
	mtuRead := mtuReadEntity.(*ysanity.Runner_Mtus_Mtu)
	suite.Equal(types.EntityEqual(&mtu, mtuRead), true)
	
	// DELETE and VALIDATE
	suite.CRUD.Delete(&suite.Provider, &mtuFilter)
	mtuReadEntity = suite.CRUD.Read(&suite.Provider, &mtuFilter)
	suite.Nil(mtuReadEntity)
}

func (suite *SanityLevelsTestSuite) TestOcPattern() {
	ocA := oc_pattern.OcA{}
	ocA.A = "xyz"
	ocA.B.B = "xyz"

	suite.CRUD.Create(&suite.Provider, &ocA)

	ocAFilter := oc_pattern.OcA{}
	ocAFilter.A = "xyz"
	ocARead := suite.CRUD.Read(&suite.Provider, &ocAFilter)
	suite.Equal(types.EntityEqual(&ocA, ocARead), true)

	ocADel := oc_pattern.OcA{}
	ocADel.A = "xyz"
	suite.CRUD.Delete(&suite.Provider, &ocADel)
}

func (suite *SanityLevelsTestSuite) TestEmbeddedQuotes() {
	runner := ysanity.Runner{}
	l1 := ysanity.Runner_TwoKeyList{First: "ab'cd",  Second: 11}
	l2 := ysanity.Runner_TwoKeyList{First: "ab\"cd", Second: 22}
	runner.TwoKeyList = []*ysanity.Runner_TwoKeyList {&l1, &l2}

	suite.CRUD.Create(&suite.Provider, &runner)

	runnerFilter := ysanity.Runner{}
	runnerRead := suite.CRUD.Read(&suite.Provider, &runnerFilter)
	suite.Equal(types.EntityEqual(&runner, runnerRead), true)

	suite.CRUD.Delete(&suite.Provider, &runner)
}

func TestSanityLevelsTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(SanityLevelsTestSuite))
}
