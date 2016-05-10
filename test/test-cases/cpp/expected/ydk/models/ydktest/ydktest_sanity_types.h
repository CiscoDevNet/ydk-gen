#ifndef _YDKTEST_SANITY_TYPES_
#define _YDKTEST_SANITY_TYPES_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

#include "ydk/models/ydktest/ydktest_sanity.h"

namespace ydk {
namespace ydktest {

class YdktestType_Identity : public BaseIdentity_Identity {
    public:
        YdktestType_Identity();

    public:

};

class AnotherOne_Identity : public YdktestType_Identity {
    public:
        AnotherOne_Identity();

    public:

};

class Other_Identity : public YdktestType_Identity {
    public:
        Other_Identity();

    public:

};


}
}

#endif /* _YDKTEST_SANITY_TYPES_ */

