#ifndef _IETF_NETCONF_WITH_DEFAULTS_
#define _IETF_NETCONF_WITH_DEFAULTS_

#include <memory>
#include <vector>
#include <string>
#include "types.hpp"
#include "errors.hpp"

namespace ydk {
namespace ietf_netconf_with_defaults {


class WithDefaultsModeEnum : public Enum
{
    public:
        static const Enum::Value report_all;
        static const Enum::Value report_all_tagged;
        static const Enum::Value trim;
        static const Enum::Value explicit_;

};


}
}

#endif /* _IETF_NETCONF_WITH_DEFAULTS_ */

