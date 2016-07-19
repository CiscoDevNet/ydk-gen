#include "ydk/make_unique.h"

#include "main.h"

namespace ydk {

MainA::MainAug1_C::MainAug1_C() {

}

MainA::MainAug2_C::MainAug2_C() {

}

MainA::MainAug2_D::MainAug2_D() {

}

MainA::MainAug3_C::MainAug3_C() {

}

MainA::MainAug3_D::MainAug3_D() {

}

MainA::MainA() {
    main_aug1_c = std::make_unique<MainA::MainAug1_C>();
    main_aug2_c = std::make_unique<MainA::MainAug2_C>();
    main_aug2_d = std::make_unique<MainA::MainAug2_D>();
    main_aug3_c = std::make_unique<MainA::MainAug3_C>();
    main_aug3_d = std::make_unique<MainA::MainAug3_D>();

}


}

