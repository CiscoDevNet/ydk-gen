#ifndef _IETF_NETCONF_
#define _IETF_NETCONF_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

#include "ydk/models/ietf_netconf_with_defaults.h"

namespace ydk {

class CancelCommitRpc : public Entity {
    public:
        CancelCommitRpc();

    class Input : public Entity {
        public:
            Input();

        public:
            std::string persist_id;

    };

    public:
        std::unique_ptr<CancelCommitRpc::Input> input;

};

class CloseSessionRpc : public Entity {
    public:
        CloseSessionRpc();

    public:

};

class CommitRpc : public Entity {
    public:
        CommitRpc();

    class Input : public Entity {
        public:
            Input();

        public:
            std::string confirm_timeout;
            Empty confirmed;
            std::string persist;
            std::string persist_id;

    };

    public:
        std::unique_ptr<CommitRpc::Input> input;

};

class CopyConfigRpc : public Entity {
    public:
        CopyConfigRpc();

    class Input : public Entity {
        public:
            Input();

        class Source : public Entity {
            public:
                Source();

            public:
                Empty candidate;
                Empty running;
                Empty startup;
                std::string url;

        };

        class Target : public Entity {
            public:
                Target();

            public:
                Empty candidate;
                Empty running;
                Empty startup;
                std::string url;

        };

        public:
            std::unique_ptr<CopyConfigRpc::Input::Source> source;
            std::unique_ptr<CopyConfigRpc::Input::Target> target;
            std::string with_defaults;

    };

    public:
        std::unique_ptr<CopyConfigRpc::Input> input;

};

class DeleteConfigRpc : public Entity {
    public:
        DeleteConfigRpc();

    class Input : public Entity {
        public:
            Input();

        class Target : public Entity {
            public:
                Target();

            public:
                Empty startup;
                std::string url;

        };

        public:
            std::unique_ptr<DeleteConfigRpc::Input::Target> target;

    };

    public:
        std::unique_ptr<DeleteConfigRpc::Input> input;

};

class DiscardChangesRpc : public Entity {
    public:
        DiscardChangesRpc();

    public:

};

class EditConfigRpc : public Entity {
    public:
        EditConfigRpc();

    class Input : public Entity {
        public:
            Input();

        class Target : public Entity {
            public:
                Target();

            public:
                Empty candidate;
                Empty running;

        };

        public:
            std::string default_operation;
            std::string error_option;
            std::unique_ptr<EditConfigRpc::Input::Target> target;
            std::string test_option;
            std::string url;

    };

    public:
        std::unique_ptr<EditConfigRpc::Input> input;

};

class GetConfigRpc : public Entity {
    public:
        GetConfigRpc();

    class Input : public Entity {
        public:
            Input();

        class Source : public Entity {
            public:
                Source();

            public:
                Empty candidate;
                Empty running;
                Empty startup;

        };

        public:
            std::unique_ptr<GetConfigRpc::Input::Source> source;
            std::string with_defaults;

    };

    class Output : public Entity {
        public:
            Output();

        public:

    };

    public:
        std::unique_ptr<GetConfigRpc::Input> input;
        std::unique_ptr<GetConfigRpc::Output> output;

};

class GetRpc : public Entity {
    public:
        GetRpc();

    class Input : public Entity {
        public:
            Input();

        public:
            std::string with_defaults;

    };

    class Output : public Entity {
        public:
            Output();

        public:

    };

    public:
        std::unique_ptr<GetRpc::Input> input;
        std::unique_ptr<GetRpc::Output> output;

};

class KillSessionRpc : public Entity {
    public:
        KillSessionRpc();

    class Input : public Entity {
        public:
            Input();

        public:
            std::string session_id;

    };

    public:
        std::unique_ptr<KillSessionRpc::Input> input;

};

class LockRpc : public Entity {
    public:
        LockRpc();

    class Input : public Entity {
        public:
            Input();

        class Target : public Entity {
            public:
                Target();

            public:
                Empty candidate;
                Empty running;
                Empty startup;

        };

        public:
            std::unique_ptr<LockRpc::Input::Target> target;

    };

    public:
        std::unique_ptr<LockRpc::Input> input;

};

class UnlockRpc : public Entity {
    public:
        UnlockRpc();

    class Input : public Entity {
        public:
            Input();

        class Target : public Entity {
            public:
                Target();

            public:
                Empty candidate;
                Empty running;
                Empty startup;

        };

        public:
            std::unique_ptr<UnlockRpc::Input::Target> target;

    };

    public:
        std::unique_ptr<UnlockRpc::Input> input;

};

class ValidateRpc : public Entity {
    public:
        ValidateRpc();

    class Input : public Entity {
        public:
            Input();

        class Source : public Entity {
            public:
                Source();

            public:
                Empty candidate;
                Empty running;
                Empty startup;
                std::string url;

        };

        public:
            std::unique_ptr<ValidateRpc::Input::Source> source;

    };

    public:
        std::unique_ptr<ValidateRpc::Input> input;

};


}

#endif /* _IETF_NETCONF_ */

