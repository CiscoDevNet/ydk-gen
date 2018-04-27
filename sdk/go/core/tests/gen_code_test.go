package test

import (
	"fmt"
	ysanity_bgp "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp"
	ysanity_bgp_types "github.com/CiscoDevNet/ydk-go/ydk/models/ydktest/openconfig_bgp_types"
	"github.com/CiscoDevNet/ydk-go/ydk"
	"github.com/CiscoDevNet/ydk-go/ydk/providers"
	"github.com/CiscoDevNet/ydk-go/ydk/services"
	encoding "github.com/CiscoDevNet/ydk-go/ydk/types/encoding_format"
	"github.com/stretchr/testify/suite"
	"testing"
)

const (
	xmlPayload = `<bgp xmlns="http://openconfig.net/yang/bgp">
  <global>
    <afi-safis>
      <afi-safi>
        <afi-safi-name xmlns:oc-bgp-types="http://openconfig.net/yang/bgp-types">oc-bgp-types:IPV4_UNICAST</afi-safi-name>
        <config>
          <afi-safi-name xmlns:oc-bgp-types="http://openconfig.net/yang/bgp-types">oc-bgp-types:IPV4_UNICAST</afi-safi-name>
          <enabled>true</enabled>
        </config>
      </afi-safi>
    </afi-safis>
    <config>
      <as>65172</as>
      <router-id>1.2.3.4</router-id>
    </config>
  </global>
</bgp>
`
)

type GenCodeTestSuite struct {
	suite.Suite
	Provider providers.CodecServiceProvider
	Codec    services.CodecService
}

func (suite *GenCodeTestSuite) SetupSuite() {
	suite.Codec = services.CodecService{}
	suite.Provider = providers.CodecServiceProvider{}
}

func (suite *GenCodeTestSuite) BeforeTest(suiteName, testName string) {
	fmt.Printf("%v: %v ...\n", suiteName, testName)
}

func (suite *GenCodeTestSuite) TearDownSuite() {
}

func configBgp(bgp *ysanity_bgp.Bgp) {
	bgp.Global.Config.As = 65172 // yfilter.Delete
	bgp.Global.Config.RouterId = "1.2.3.4"

	ipv6Afisafi := ysanity_bgp.Bgp_Global_AfiSafis_AfiSafi{}
	ipv6Afisafi.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	ipv6Afisafi.Config.AfiSafiName = &ysanity_bgp_types.IPV6UNICAST{}
	ipv6Afisafi.Config.Enabled = true
	bgp.Global.AfiSafis.AfiSafi = append(bgp.Global.AfiSafis.AfiSafi, ipv6Afisafi)

	ipv4Afisafi := ysanity_bgp.Bgp_Global_AfiSafis_AfiSafi{}
	ipv4Afisafi.AfiSafiName = &ysanity_bgp_types.IPV4UNICAST{}
	ipv4Afisafi.Config.AfiSafiName = &ysanity_bgp_types.IPV4UNICAST{}
	ipv4Afisafi.Config.Enabled = true
	bgp.Global.AfiSafis.AfiSafi = append(bgp.Global.AfiSafis.AfiSafi, ipv4Afisafi)
}

func (suite *GenCodeTestSuite) TestGenCodeXMLEncoding() {
	bgp := ysanity_bgp.Bgp{}
	configBgp(&bgp)

	suite.Provider.Encoding = encoding.XML
	payload := suite.Codec.Encode(&suite.Provider, &bgp)

	ydk.YLogDebug(fmt.Sprintf(
		"In TestGenCodeXMLEncoding, payload:\n%v", payload))
}

func (suite *GenCodeTestSuite) TestGenCodeXMLDecoding() {
	entity := suite.Codec.Decode(&suite.Provider, xmlPayload)
	bgpDecoded := entity.(*ysanity_bgp.Bgp)

	p := suite.Codec.Encode(&suite.Provider, bgpDecoded)
	ydk.YLogDebug(fmt.Sprintf("In TestGenCodeXMlDecoding, payload:\n%v", p))
}

func TestGenCodeTestSuite(t *testing.T) {
	suite.Run(t, new(GenCodeTestSuite))
}
