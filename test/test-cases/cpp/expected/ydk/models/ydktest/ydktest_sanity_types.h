#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {
namespace ydktest_sanity_types {
class YdktestType_Identity : public BaseIdentity_Identity, Entity {
    public:
        YdktestType_Identity();

    public:

};

class AnotherOne_Identity : public YdktestType_Identity, Entity {
    public:
        AnotherOne_Identity();

    public:

};

class Other_Identity : public YdktestType_Identity, Entity {
    public:
        Other_Identity();

    public:

};


}
}

