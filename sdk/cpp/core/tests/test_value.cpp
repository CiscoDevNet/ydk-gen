#include "../src/types.hpp"
#include "../src/errors.hpp"
#include "catch.hpp"
#include <spdlog/spdlog.h>

#include <iostream>

using namespace ydk;

class TestIdentity1 : public Identity
{
  public:
	TestIdentity1() : Identity("test-identity")
	{
	}
};

class TestEnum1 : public Enum
{
  public:
	TestEnum1() {}
	~TestEnum1(){}

	static const Enum::YLeaf one;
	static const Enum::YLeaf two;
};

const Enum::YLeaf TestEnum1::one {1, "one"};
const Enum::YLeaf TestEnum1::two {2, "two"};

TEST_CASE("test_uint8")
{
	YLeaf test_value{YType::uint8, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}

TEST_CASE("test_uint16")
{
	YLeaf test_value{YType::uint16, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}


TEST_CASE("test_uint32")
{
	YLeaf test_value{YType::uint32, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}


TEST_CASE("test_uint64")
{
	YLeaf test_value{YType::uint64, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}


TEST_CASE("test_int8")
{
	YLeaf test_value{YType::int8, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}


TEST_CASE("test_int16")
{
	YLeaf test_value{YType::int16, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}


TEST_CASE("test_int32")
{
	YLeaf test_value{YType::int32, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}


TEST_CASE("test_int64")
{
	YLeaf test_value{YType::int64, "name"};
	test_value = 4;
	REQUIRE(test_value.get()=="4");
}

TEST_CASE("test_empty")
{
	YLeaf test_value{YType::empty, "name"};
	test_value = Empty{};
	REQUIRE(test_value.get()=="");
}

TEST_CASE("test_identity")
{
	YLeaf test_value{YType::identityref, "name"};
	test_value = TestIdentity1{};
	INFO(test_value.get());
	REQUIRE(test_value.get()=="test-identity");
}

TEST_CASE("test_enum_")
{
	YLeaf test_value{YType::enumeration, "enumval"};
	test_value = TestEnum1::one;
	INFO(test_value.get());
	REQUIRE(test_value.get()=="one");
}

TEST_CASE("test_str")
{
	YLeaf test_value{YType::str, "name"};
	test_value = "hello";
	REQUIRE(test_value.get()=="hello");
}

TEST_CASE("test_bool")
{
	YLeaf test_value{YType::boolean, "name"};
	test_value = true;
	REQUIRE(test_value.get()=="true");

	test_value = false;
	REQUIRE(test_value.get()=="false");
}

TEST_CASE("test_bits")
{
	YLeaf test_value{YType::bits, "bits-field"};
	test_value["bit1"] = true;
	test_value["bit2"] = true;
	test_value["bit3"] = true;
	test_value["bit4"] = true;
	REQUIRE(test_value.get()=="bit1 bit2 bit3 bit4");

	test_value["bit3"] = false;
	REQUIRE(test_value.get()=="bit1 bit2 bit4");
}

TEST_CASE("test_bits_assign")
{
	YLeaf test_value{YType::bits, "bits-field"};
	Bits test{};
	test["bit1"] = true;
	test["bit2"] = true;
	test["bit3"] = true;
	test["bit4"] = true;
	test_value = test;
	REQUIRE(test_value.get()=="bit1 bit2 bit3 bit4");

	test["bit3"] = false;
	test_value = test;
	REQUIRE(test_value.get()=="bit1 bit2 bit4");

	std::vector<YLeaf> vs;
	vs.push_back(test_value);
	REQUIRE(vs[0].get()=="bit1 bit2 bit4");
	REQUIRE(test_value.get()=="bit1 bit2 bit4");
}

TEST_CASE("test_deci64")
{
	YLeaf test_value{YType::decimal64, "value"};
	test_value = Decimal64("3.2");
	REQUIRE(test_value.get()=="3.2");

	test_value = Decimal64("1.2");
	REQUIRE(test_value.get()=="1.2");
}
