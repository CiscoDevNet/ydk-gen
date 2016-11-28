#define BOOST_TEST_MODULE ValueTest
#include <boost/test/unit_test.hpp>
#include "../../src/types.hpp"
#include "../../src/errors.hpp"
#include <iostream>

using namespace ydk;

class TestIdentity : public Identity
{
  public:
	TestIdentity() : Identity("test-identity")
	{
	}
};

class TestEnum : public Enum
{
  public:
	TestEnum() {}
	~TestEnum(){}

	static const Enum::Value one;
	static const Enum::Value two;
};

const Enum::Value TestEnum::one {1, "one"};
const Enum::Value TestEnum::two {2, "two"};

BOOST_AUTO_TEST_CASE(test_uint8)
{
	ValueList test_value{YType::uint8, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}

BOOST_AUTO_TEST_CASE(test_uint16)
{
	ValueList test_value{YType::uint16, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}


BOOST_AUTO_TEST_CASE(test_uint32)
{
	ValueList test_value{YType::uint32, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}


BOOST_AUTO_TEST_CASE(test_uint64)
{
	ValueList test_value{YType::uint64, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}


BOOST_AUTO_TEST_CASE(test_int8)
{
	ValueList test_value{YType::int8, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}


BOOST_AUTO_TEST_CASE(test_int16)
{
	ValueList test_value{YType::int16, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}


BOOST_AUTO_TEST_CASE(test_int32)
{
	ValueList test_value{YType::int32, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}


BOOST_AUTO_TEST_CASE(test_int64)
{
	ValueList test_value{YType::int64, "name"};
	test_value.append(4);
	BOOST_REQUIRE(test_value[0].get()=="4");
	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="name[.='4']");
}

BOOST_AUTO_TEST_CASE(test_empty)
{
	ValueList test_value{YType::empty, "name"};
	test_value.append(Empty{});
	BOOST_REQUIRE(test_value[0].get()=="");
}

BOOST_AUTO_TEST_CASE(test_identity)
{
	ValueList test_value{YType::identityref, "name"};
	TestIdentity id{};
	test_value.append(id);
	BOOST_REQUIRE(test_value[0].get()=="test-identity");
}

BOOST_AUTO_TEST_CASE(test_enum_)
{
	ValueList test_value{YType::enumeration, "enumval"};
	test_value.append(TestEnum::one);
	BOOST_REQUIRE(test_value[0].get()=="one");
	test_value.append(TestEnum::two);
	BOOST_REQUIRE(test_value[1].get()=="two");

	auto nv = test_value.get_name_leafdata();
	BOOST_REQUIRE(nv[0].first=="enumval[.='one']");
	BOOST_REQUIRE(nv[1].first=="enumval[.='two']");
}

BOOST_AUTO_TEST_CASE(test_str)
{
	ValueList test_value{YType::str, "name"};
	test_value.append("hello");
	BOOST_REQUIRE(test_value[0].get()=="hello");
}

BOOST_AUTO_TEST_CASE(test_bool)
{
	ValueList test_value{YType::boolean, "name"};
	test_value.append(true);
	BOOST_REQUIRE(test_value[0].get()=="true");

	test_value.append(false);
	BOOST_REQUIRE(test_value[1].get()=="false");
}

BOOST_AUTO_TEST_CASE(test_bits)
{
	ValueList test_value{YType::bits, "bits-list"};
	Bits test;
	test["bit1"] = true;
	test["bit2"] = true;
	test["bit3"] = true;
	test["bit4"] = true;
	test_value.append(test);
	BOOST_REQUIRE(test_value[0].get()=="bit1 bit2 bit3 bit4");

	test["bit3"] = false;
	test_value.append(test);
	BOOST_REQUIRE(test_value[1].get()=="bit1 bit2 bit4");
}

BOOST_AUTO_TEST_CASE(test_deci64)
{
	ValueList test_value{YType::decimal64, "value"};
	test_value.append("3.2");
	BOOST_REQUIRE(test_value[0].get()=="3.2");

	test_value.append("1.2");
	BOOST_REQUIRE(test_value[1].get()=="1.2");
}
