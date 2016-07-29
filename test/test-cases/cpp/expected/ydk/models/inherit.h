#ifndef _INHERIT_
#define _INHERIT_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {

class InheritRunner : public Entity {
    public:
        InheritRunner();

    class One : public Entity {
        public:
            One();

        public:
            std::string name;
            int number;

    };

    public:
        int jumper;
        std::unique_ptr<InheritRunner::One> one;

};


}

#endif /* _INHERIT_ */

