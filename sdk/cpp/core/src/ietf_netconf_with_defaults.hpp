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
        static const Enum::YLeaf report_all;
        static const Enum::YLeaf report_all_tagged;
        static const Enum::YLeaf trim;
        static const Enum::YLeaf explicit_;

};


}
}

#endif /* _IETF_NETCONF_WITH_DEFAULTS_ */

