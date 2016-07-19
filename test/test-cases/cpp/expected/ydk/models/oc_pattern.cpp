#include "ydk/make_unique.h"

#include "oc_pattern.h"

namespace ydk {

OcA::B::B() {

}

OcA::OcA() {
    b = std::make_unique<OcA::B>();

}


}

