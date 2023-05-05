package test

import (
	"fmt"
	ysanity "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/sanity"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	"github.com/stretchr/testify/assert"
	"github.com/stretchr/testify/suite"
	"testing"
)

type ErrorsTestSuite struct {
	suite.Suite
	Provider providers.NetconfServiceProvider
	CRUD     services.CrudService
}

func (suite *ErrorsTestSuite) SetupSuite() {
	suite.CRUD = services.CrudService{}
	suite.Provider = providers.NetconfServiceProvider{
		Address:  "127.0.0.1",
		Username: "admin",
		Password: "admin",
		Port:     12022}
	suite.Provider.Connect()
	suite.CRUD.Delete(&suite.Provider, &ysanity.Runner{})
}

func (suite *ErrorsTestSuite) TearDownSuite() {
	suite.Provider.Disconnect()
}

func (suite *ErrorsTestSuite) BeforeTest(suiteName, testName string) {
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *ErrorsTestSuite) TestInvalidInt8() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number8 = 8.5
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "number8" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/number8`,
		runner.Ytypes.BuiltInT.Number8)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

// TODO: string representation always treated as valid value
// func (suite *ErrorsTestSuite) TestInvalidInt16() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.Number16 = "16"
// 	suite.CRUD.Create(&suite.Provider, &runner)
// }

func (suite *ErrorsTestSuite) TestInvalidInt16() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number16 = true
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "number16" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/number16`,
		runner.Ytypes.BuiltInT.Number16)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

func (suite *ErrorsTestSuite) TestInvalidInt32() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.Number32 = make([]int, 0)
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "number32" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/number32`,
		runner.Ytypes.BuiltInT.Number32)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

// Invalid test case, go int64 range: -9223372036854775808 through 9223372036854775807
// func (suite *ErrorsTestSuite) TestInvalidInt64() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.Number64 = int64(9223372036854775808)
// 	suite.CRUD.Create(&suite.Provider, &runner)
// }

func (suite *ErrorsTestSuite) TestInvalidUint8() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber8 = -1
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "u_number8" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/u_number8`,
		runner.Ytypes.BuiltInT.UNumber8)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

func (suite *ErrorsTestSuite) TestInvalidUint16() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber16 = "non uint16"
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "u_number16" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/u_number16`,
		runner.Ytypes.BuiltInT.UNumber16)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

func (suite *ErrorsTestSuite) TestInvalidUint32() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.UNumber32 = 4294967296
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "u_number32" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/u_number32`,
		runner.Ytypes.BuiltInT.UNumber32)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

// Invalid test case, go int64 range: -9223372036854775808 through 9223372036854775807
// func (suite *ErrorsTestSuite) TestInvalidUint64() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.UNumber64 = 18446744073709551616
// 	suite.CRUD.Create(&suite.Provider, &runner)
// }

// TODO: should raise error
// func (suite *ErrorsTestSuite) TestInvalidString() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.Name = [...]string{"name", "string", "list"}
// 	suite.CRUD.Create(&suite.Provider, &runner)
// }

func (suite *ErrorsTestSuite) TestInvalidBoolean() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.BoolValue = "a string"
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "bool-value" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/bool-value`,
		runner.Ytypes.BuiltInT.BoolValue)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

// TODO: should raise error
// func (suite *ErrorsTestSuite) TestInvalidEmpty() {
// 	runner := ysanity.Runner{}
// 	runner.Ytypes.BuiltInT.Emptee = 123
// 	suite.CRUD.Create(&suite.Provider, &runner)
// }

func (suite *ErrorsTestSuite) TestInvalidEnum() {
	runner := ysanity.Runner{}
	runner.Ytypes.BuiltInT.EnumValue = "non enum"
	errMsg := fmt.Sprintf(`YModelError: Invalid value "%v" in "enum-value" element. Path: /ydktest-sanity:runner/ytypes/built-in-t/enum-value`,
		runner.Ytypes.BuiltInT.EnumValue)
	assert.PanicsWithValue(suite.T(), errMsg, func() { suite.CRUD.Create(&suite.Provider, &runner) })
}

func TestErrorsTestSuite(t *testing.T) {
	if testing.Verbose() {
		ydk.EnableLogging(ydk.Debug)
	}
	suite.Run(t, new(ErrorsTestSuite))
}
