#ifndef _MAIN_
#define _MAIN_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {

class MainA : public Entity {
    public:
        MainA();

    class MainAug1_C : public Entity {
        public:
            MainAug1_C();

        public:
            std::string two;

    };

    class MainAug2_C : public Entity {
        public:
            MainAug2_C();

        public:
            int three;

    };

    class MainAug2_D : public Entity {
        public:
            MainAug2_D();

        public:
            int poo;

    };

    class MainAug3_C : public Entity {
        public:
            MainAug3_C();

        public:
            int meh;

    };

    class MainAug3_D : public Entity {
        public:
            MainAug3_D();

        public:
            std::string buh;

    };

    public:
        std::unique_ptr<MainA::MainAug1_C> main_aug1_c;
        std::unique_ptr<MainA::MainAug2_C> main_aug2_c;
        std::unique_ptr<MainA::MainAug2_D> main_aug2_d;
        std::unique_ptr<MainA::MainAug3_C> main_aug3_c;
        std::unique_ptr<MainA::MainAug3_D> main_aug3_d;
        int one;

};


}

#endif /* _MAIN_ */

