#ifndef _OC_PATTERN_
#define _OC_PATTERN_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {
namespace oc {

class A : public Entity {
    public:
        A();

    class B : public Entity {
        public:
            B();

        public:
            std::string b;

    };

    public:
        std::string a;
        std::unique_ptr<A::B> b;

};


}
}

#endif /* _OC_PATTERN_ */

