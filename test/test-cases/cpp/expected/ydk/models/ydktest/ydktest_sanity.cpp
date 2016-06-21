#include "ydk/make_unique.h"

#include "ydktest_sanity.h"

namespace ydk {
namespace ydktest {

BaseIdentity_Identity::BaseIdentity_Identity() {


}

Runner::InbtwList::Ldata::Subc::SubcSubl1::SubcSubl1() {

}

Runner::InbtwList::Ldata::Subc::Subc() {

}

Runner::InbtwList::Ldata::Ldata() {
    subc = std::make_unique<Runner::InbtwList::Ldata::Subc>();

}

Runner::InbtwList::InbtwList() {

}

Runner::LeafRef::One::Two::Two() {

}

Runner::LeafRef::One::One() {
    two = std::make_unique<Runner::LeafRef::One::Two>();

}

Runner::LeafRef::LeafRef() {
    one = std::make_unique<Runner::LeafRef::One>();

}

Runner::NotSupported1::NotSupported12::NotSupported12() {

}

Runner::NotSupported1::NotSupported1() {
    not_supported_1_2 = std::make_unique<Runner::NotSupported1::NotSupported12>();

}

Runner::NotSupported2::NotSupported2() {

}

Runner::One::OneAug::OneAug() {

}

Runner::One::One() {
    one_aug = std::make_unique<Runner::One::OneAug>();

}

Runner::OneList::Ldata::Ldata() {

}

Runner::OneList::OneAugList::Ldata::Ldata() {

}

Runner::OneList::OneAugList::OneAugList() {

}

Runner::OneList::OneList() {
    one_aug_list = std::make_unique<Runner::OneList::OneAugList>();

}

Runner::Runner2::Runner2() {

}

Runner::Three::Sub1::Sub2::Sub2() {

}

Runner::Three::Sub1::Sub1() {
    sub2 = std::make_unique<Runner::Three::Sub1::Sub2>();

}

Runner::Three::Three() {
    sub1 = std::make_unique<Runner::Three::Sub1>();

}

Runner::ThreeList::Ldata::Subl1::SubSubl1::SubSubl1() {

}

Runner::ThreeList::Ldata::Subl1::Subl1() {

}

Runner::ThreeList::Ldata::Ldata() {

}

Runner::ThreeList::ThreeList() {

}

Runner::Two::Sub1::Sub1() {

}

Runner::Two::Two() {
    sub1 = std::make_unique<Runner::Two::Sub1>();

}

Runner::TwoList::Ldata::Subl1::Subl1() {

}

Runner::TwoList::Ldata::Ldata() {

}

Runner::TwoList::TwoList() {

}

Runner::Ytypes::BuiltInT::BuiltInT() {

}

Runner::Ytypes::DerivedT::DerivedT() {

}

Runner::Ytypes::Ytypes() {
    built_in_t = std::make_unique<Runner::Ytypes::BuiltInT>();
    derived_t = std::make_unique<Runner::Ytypes::DerivedT>();

}

Runner::Runner() {
    inbtw_list = std::make_unique<Runner::InbtwList>();
    leaf_ref = std::make_unique<Runner::LeafRef>();
    not_supported_1 = std::make_unique<Runner::NotSupported1>();
    one = std::make_unique<Runner::One>();
    one_list = std::make_unique<Runner::OneList>();
    runner_2 = std::make_unique<Runner::Runner2>();
    three = std::make_unique<Runner::Three>();
    three_list = std::make_unique<Runner::ThreeList>();
    two = std::make_unique<Runner::Two>();
    two_list = std::make_unique<Runner::TwoList>();
    ytypes = std::make_unique<Runner::Ytypes>();

}

SubTest::OneAug::OneAug() {

}

SubTest::SubTest() {
    one_aug = std::make_unique<SubTest::OneAug>();

}

ChildIdentity_Identity::ChildIdentity_Identity() {

}

ChildChildIdentity_Identity::ChildChildIdentity_Identity() {

}


}
}

