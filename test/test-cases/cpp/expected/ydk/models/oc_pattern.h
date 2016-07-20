#ifndef _OC_PATTERN_
#define _OC_PATTERN_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {

class OcA : public Entity {
    public:
        OcA();

    class B : public Entity {
        public:
            B();

        public:
            std::string b;

    };

    public:
        std::string a;
        std::unique_ptr<OcA::B> b;

};


}

#endif /* _OC_PATTERN_ */

