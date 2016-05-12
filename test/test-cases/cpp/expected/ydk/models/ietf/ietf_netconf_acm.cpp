#include "ydk/make_unique.h"

#include "ietf_netconf_acm.h"

namespace ydk {
namespace ietf_netconf_acm {
Nacm::Groups::Group::Group() {

}

Nacm::Groups::Groups() {

}

Nacm::RuleList::Rule::Rule() {

}

Nacm::RuleList::RuleList() {

}

Nacm::Nacm() {
    groups = std::make_unique<Nacm::Groups>();

}


}
}

