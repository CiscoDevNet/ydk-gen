#include "ydk/make_unique.h"

#include "ydktest_filterread.h"

namespace ydk {

A::B::C::C() {

}

A::B::D::E::E() {

}

A::B::D::D() {
    e = std::make_unique<A::B::D::E>();

}

A::B::F::F() {

}

A::B::B() {
    c = std::make_unique<A::B::C>();
    d = std::make_unique<A::B::D>();
    f = std::make_unique<A::B::F>();

}

A::Lst::Lst() {

}

A::A() {
    b = std::make_unique<A::B>();

}


}

