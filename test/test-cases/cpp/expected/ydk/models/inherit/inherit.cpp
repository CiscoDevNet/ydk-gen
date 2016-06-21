#include "ydk/make_unique.h"

#include "inherit.h"

namespace ydk {
namespace inherit {

Runner::One::One() {

}

Runner::Runner() {
    one = std::make_unique<Runner::One>();

}


}
}

