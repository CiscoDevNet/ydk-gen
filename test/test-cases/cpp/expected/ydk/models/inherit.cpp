#include "ydk/make_unique.h"

#include "inherit.h"

namespace ydk {

InheritRunner::One::One() {

}

InheritRunner::InheritRunner() {
    one = std::make_unique<InheritRunner::One>();

}


}

