#define BOOST_TEST_MODULE YLeafTest
#include <boost/test/unit_test.hpp>
#include "../src/types.hpp"
#include "../src/errors.hpp"
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

	static const Enum::YLeaf one;
	static const Enum::YLeaf two;
};

const Enum::YLeaf TestEnum::one {1, "one"};
const Enum::YLeaf TestEnum::two {2, "two"};

BOOST_AUTO_TEST_CASE(test_uint8)
{
	YLeaf test_value{YType::uint8, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}

BOOST_AUTO_TEST_CASE(test_uint16)
{
	YLeaf test_value{YType::uint16, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}


BOOST_AUTO_TEST_CASE(test_uint32)
{
	YLeaf test_value{YType::uint32, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}


BOOST_AUTO_TEST_CASE(test_uint64)
{
	YLeaf test_value{YType::uint64, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}


BOOST_AUTO_TEST_CASE(test_int8)
{
	YLeaf test_value{YType::int8, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}


BOOST_AUTO_TEST_CASE(test_int16)
{
	YLeaf test_value{YType::int16, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}


BOOST_AUTO_TEST_CASE(test_int32)
{
	YLeaf test_value{YType::int32, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}


BOOST_AUTO_TEST_CASE(test_int64)
{
	YLeaf test_value{YType::int64, "name"};
	test_value = 4;
	BOOST_REQUIRE(test_value.get()=="4");
}

BOOST_AUTO_TEST_CASE(test_empty)
{
	YLeaf test_value{YType::empty, "name"};
	test_value = Empty{};
	BOOST_REQUIRE(test_value.get()=="");
}

BOOST_AUTO_TEST_CASE(test_identity)
{
	YLeaf test_value{YType::identityref, "name"};
	test_value = TestIdentity{};
	BOOST_TEST_MESSAGE(test_value.get());
	BOOST_REQUIRE(test_value.get()=="test-identity");
}

BOOST_AUTO_TEST_CASE(test_enum_)
{
	YLeaf test_value{YType::enumeration, "enumval"};
	test_value = TestEnum::one;
	BOOST_TEST_MESSAGE(test_value.get());
	BOOST_REQUIRE(test_value.get()=="one");
}

BOOST_AUTO_TEST_CASE(test_str)
{
	YLeaf test_value{YType::str, "name"};
	test_value = "hello";
	BOOST_REQUIRE(test_value.get()=="hello");
}

BOOST_AUTO_TEST_CASE(test_bool)
{
	YLeaf test_value{YType::boolean, "name"};
	test_value = true;
	BOOST_REQUIRE(test_value.get()=="true");

	test_value = false;
	BOOST_REQUIRE(test_value.get()=="false");
}

BOOST_AUTO_TEST_CASE(test_bits)
{
	YLeaf test_value{YType::bits, "bits-field"};
	test_value["bit1"] = true;
	test_value["bit2"] = true;
	test_value["bit3"] = true;
	test_value["bit4"] = true;
	BOOST_REQUIRE(test_value.get()=="bit1 bit2 bit3 bit4");

	test_value["bit3"] = false;
	BOOST_REQUIRE(test_value.get()=="bit1 bit2 bit4");
}

BOOST_AUTO_TEST_CASE(test_bits_assign)
{
	YLeaf test_value{YType::bits, "bits-field"};
	Bits test{};
	test["bit1"] = true;
	test["bit2"] = true;
	test["bit3"] = true;
	test["bit4"] = true;
	test_value = test;
	BOOST_REQUIRE(test_value.get()=="bit1 bit2 bit3 bit4");

	test["bit3"] = false;
	test_value = test;
	BOOST_REQUIRE(test_value.get()=="bit1 bit2 bit4");

	std::vector<YLeaf> vs;
	vs.push_back(test_value);
	BOOST_REQUIRE(vs[0].get()=="bit1 bit2 bit4");
	BOOST_REQUIRE(test_value.get()=="bit1 bit2 bit4");
}

BOOST_AUTO_TEST_CASE(test_deci64)
{
	YLeaf test_value{YType::decimal64, "value"};
	test_value = Decimal64("3.2");
	BOOST_REQUIRE(test_value.get()=="3.2");

	test_value = Decimal64("1.2");
	BOOST_REQUIRE(test_value.get()=="1.2");
}
