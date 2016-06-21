#include "ydk/make_unique.h"

#include "oc_pattern.h"

namespace ydk {
namespace oc {

A::B::B() {

}

A::A() {
    b = std::make_unique<A::B>();

}


}
}

