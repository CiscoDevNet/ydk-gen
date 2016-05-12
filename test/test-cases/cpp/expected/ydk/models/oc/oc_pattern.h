#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {
namespace oc_pattern {
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

