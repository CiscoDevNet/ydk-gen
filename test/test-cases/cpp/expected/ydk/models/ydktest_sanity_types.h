#ifndef _YDKTEST_SANITY_TYPES_
#define _YDKTEST_SANITY_TYPES_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

#include "ydk/models/ydktest_sanity.h"

namespace ydk {

class YdktestTypeIdentity : public BaseIdentityIdentity {
    public:
        YdktestTypeIdentity();

    public:

};

class AnotherOneIdentity : public YdktestTypeIdentity {
    public:
        AnotherOneIdentity();

    public:

};

class OtherIdentity : public YdktestTypeIdentity {
    public:
        OtherIdentity();

    public:

};


}

#endif /* _YDKTEST_SANITY_TYPES_ */

