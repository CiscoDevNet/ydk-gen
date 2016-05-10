#include "ydk/make_unique.h"

#include "main.h"

namespace ydk {
namespace main {

A::MainAug1_C::MainAug1_C() {

}

A::MainAug2_C::MainAug2_C() {

}

A::MainAug2_D::MainAug2_D() {

}

A::MainAug3_C::MainAug3_C() {

}

A::MainAug3_D::MainAug3_D() {

}

A::A() {
    main_aug1_c = std::make_unique<A::MainAug1_C>();
    main_aug2_c = std::make_unique<A::MainAug2_C>();
    main_aug2_d = std::make_unique<A::MainAug2_D>();
    main_aug3_c = std::make_unique<A::MainAug3_C>();
    main_aug3_d = std::make_unique<A::MainAug3_D>();

}


}
}

