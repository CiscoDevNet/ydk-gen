#include "ydk/make_unique.h"

#include "ietf_netconf.h"

namespace ydk {
namespace ietf {

CancelCommitRpc::Input::Input() {

}

CancelCommitRpc::CancelCommitRpc() {
    input = std::make_unique<CancelCommitRpc::Input>();

}

CloseSessionRpc::CloseSessionRpc() {

}

CommitRpc::Input::Input() {

}

CommitRpc::CommitRpc() {
    input = std::make_unique<CommitRpc::Input>();

}

CopyConfigRpc::Input::Source::Source() {

}

CopyConfigRpc::Input::Target::Target() {

}

CopyConfigRpc::Input::Input() {
    source = std::make_unique<CopyConfigRpc::Input::Source>();
    target = std::make_unique<CopyConfigRpc::Input::Target>();

}

CopyConfigRpc::CopyConfigRpc() {
    input = std::make_unique<CopyConfigRpc::Input>();

}

DeleteConfigRpc::Input::Target::Target() {

}

DeleteConfigRpc::Input::Input() {
    target = std::make_unique<DeleteConfigRpc::Input::Target>();

}

DeleteConfigRpc::DeleteConfigRpc() {
    input = std::make_unique<DeleteConfigRpc::Input>();

}

DiscardChangesRpc::DiscardChangesRpc() {

}

EditConfigRpc::Input::Target::Target() {

}

EditConfigRpc::Input::Input() {
    target = std::make_unique<EditConfigRpc::Input::Target>();

}

EditConfigRpc::EditConfigRpc() {
    input = std::make_unique<EditConfigRpc::Input>();

}

GetConfigRpc::Input::Source::Source() {

}

GetConfigRpc::Input::Input() {
    source = std::make_unique<GetConfigRpc::Input::Source>();

}

GetConfigRpc::Output::Output() {

}

GetConfigRpc::GetConfigRpc() {
    input = std::make_unique<GetConfigRpc::Input>();
    output = std::make_unique<GetConfigRpc::Output>();

}

GetRpc::Input::Input() {

}

GetRpc::Output::Output() {

}

GetRpc::GetRpc() {
    input = std::make_unique<GetRpc::Input>();
    output = std::make_unique<GetRpc::Output>();

}

KillSessionRpc::Input::Input() {

}

KillSessionRpc::KillSessionRpc() {
    input = std::make_unique<KillSessionRpc::Input>();

}

LockRpc::Input::Target::Target() {

}

LockRpc::Input::Input() {
    target = std::make_unique<LockRpc::Input::Target>();

}

LockRpc::LockRpc() {
    input = std::make_unique<LockRpc::Input>();

}

UnlockRpc::Input::Target::Target() {

}

UnlockRpc::Input::Input() {
    target = std::make_unique<UnlockRpc::Input::Target>();

}

UnlockRpc::UnlockRpc() {
    input = std::make_unique<UnlockRpc::Input>();

}

ValidateRpc::Input::Source::Source() {

}

ValidateRpc::Input::Input() {
    source = std::make_unique<ValidateRpc::Input::Source>();

}

ValidateRpc::ValidateRpc() {
    input = std::make_unique<ValidateRpc::Input>();

}


}
}

