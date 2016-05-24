#ifndef _YDKTEST_SANITY_
#define _YDKTEST_SANITY_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {
namespace ydktest {

class BaseIdentity_Identity : public Entity {
    public:
        BaseIdentity_Identity();



};

class Runner : public Entity {
    public:
        Runner();

    class InbtwList : public Entity {
        public:
            InbtwList();

        class Ldata : public Entity {
            public:
                Ldata();

            class Subc : public Entity {
                public:
                    Subc();

                class SubcSubl1 : public Entity {
                    public:
                        SubcSubl1();

                    public:
                        int number;
                        std::string name;

                };

                public:
                    std::string name;
                    int number;
                    std::vector< std::unique_ptr<Runner::InbtwList::Ldata::Subc::SubcSubl1> > subc_subl1;

            };

            public:
                int number;
                std::string name;
                std::unique_ptr<Runner::InbtwList::Ldata::Subc> subc;

        };

        public:
            std::vector< std::unique_ptr<Runner::InbtwList::Ldata> > ldata;

    };

    class LeafRef : public Entity {
        public:
            LeafRef();

        class One : public Entity {
            public:
                One();

            class Two : public Entity {
                public:
                    Two();

                public:
                    std::string self_ref_one_name;

            };

            public:
                std::string name_of_one;
                std::unique_ptr<Runner::LeafRef::One::Two> two;

        };

        public:
            std::unique_ptr<Runner::LeafRef::One> one;
            std::string ref_inbtw;
            std::string ref_one_name;
            int ref_three_sub1_sub2_number;
            int ref_two_sub1_number;

    };

    class NotSupported1 : public Entity {
        public:
            NotSupported1();

        class NotSupported12 : public Entity {
            public:
                NotSupported12();

            public:
                std::string some_leaf;

        };

        public:
            std::unique_ptr<Runner::NotSupported1::NotSupported12> not_supported_1_2;
            std::string not_supported_leaf;

    };

    class NotSupported2 : public Entity {
        public:
            NotSupported2();

        public:
            int number;

    };

    class One : public Entity {
        public:
            One();

        class OneAug : public Entity {
            public:
                OneAug();

            public:
                std::string name;
                int number;

        };

        public:
            std::string name;
            int number;
            std::unique_ptr<Runner::One::OneAug> one_aug;

    };

    class OneList : public Entity {
        public:
            OneList();

        class Ldata : public Entity {
            public:
                Ldata();

            public:
                int number;
                std::string name;

        };

        class OneAugList : public Entity {
            public:
                OneAugList();

            class Ldata : public Entity {
                public:
                    Ldata();

                public:
                    int number;
                    std::string name;

            };

            public:
                bool enabled;
                std::vector< std::unique_ptr<Runner::OneList::OneAugList::Ldata> > ldata;

        };

        public:
            std::vector< std::unique_ptr<Runner::OneList::Ldata> > ldata;
            std::unique_ptr<Runner::OneList::OneAugList> one_aug_list;

    };

    class Runner2 : public Entity {
        public:
            Runner2();

        public:
            std::string some_leaf;

    };

    class Three : public Entity {
        public:
            Three();

        class Sub1 : public Entity {
            public:
                Sub1();

            class Sub2 : public Entity {
                public:
                    Sub2();

                public:
                    int number;

            };

            public:
                int number;
                std::unique_ptr<Runner::Three::Sub1::Sub2> sub2;

        };

        public:
            std::string name;
            int number;
            std::unique_ptr<Runner::Three::Sub1> sub1;

    };

    class ThreeList : public Entity {
        public:
            ThreeList();

        class Ldata : public Entity {
            public:
                Ldata();

            class Subl1 : public Entity {
                public:
                    Subl1();

                class SubSubl1 : public Entity {
                    public:
                        SubSubl1();

                    public:
                        int number;
                        std::string name;

                };

                public:
                    int number;
                    std::string name;
                    std::vector< std::unique_ptr<Runner::ThreeList::Ldata::Subl1::SubSubl1> > sub_subl1;

            };

            public:
                int number;
                std::string name;
                std::vector< std::unique_ptr<Runner::ThreeList::Ldata::Subl1> > subl1;

        };

        public:
            std::vector< std::unique_ptr<Runner::ThreeList::Ldata> > ldata;

    };

    class Two : public Entity {
        public:
            Two();

        class Sub1 : public Entity {
            public:
                Sub1();

            public:
                int number;

        };

        public:
            std::string name;
            int number;
            std::unique_ptr<Runner::Two::Sub1> sub1;

    };

    class TwoList : public Entity {
        public:
            TwoList();

        class Ldata : public Entity {
            public:
                Ldata();

            class Subl1 : public Entity {
                public:
                    Subl1();

                public:
                    int number;
                    std::string name;

            };

            public:
                int number;
                std::string name;
                std::vector< std::unique_ptr<Runner::TwoList::Ldata::Subl1> > subl1;

        };

        public:
            std::vector< std::unique_ptr<Runner::TwoList::Ldata> > ldata;

    };

    class Ytypes : public Entity {
        public:
            Ytypes();

        class BuiltInT : public Entity {
            public:
                BuiltInT();

            public:
                std::string bincoded;
                bool bool_value;
                std::string deci64;
                std::string embeded_enum;
                Empty emptee;
                std::string enum_int_value;
                std::string enum_value;
                std::string identity_ref_value;
                int leaf_ref;
                std::vector<std::string> llstring;
                std::vector<std::string> llunion;
                std::string name;
                int number16;
                int number32;
                int number64;
                int number8;
                int u_number16;
                int u_number32;
                int u_number64;
                int u_number8;
                std::string younion;
                std::vector<std::string> younion_list;
                std::string younion_recursive;

        };

        class DerivedT : public Entity {
            public:
                DerivedT();

            public:

        };

        public:
            std::unique_ptr<Runner::Ytypes::BuiltInT> built_in_t;
            std::unique_ptr<Runner::Ytypes::DerivedT> derived_t;

    };

    public:
        std::unique_ptr<Runner::InbtwList> inbtw_list;
        std::unique_ptr<Runner::LeafRef> leaf_ref;
        std::unique_ptr<Runner::NotSupported1> not_supported_1;
        std::vector< std::unique_ptr<Runner::NotSupported2> > not_supported_2;
        std::unique_ptr<Runner::One> one;
        std::unique_ptr<Runner::OneList> one_list;
        std::unique_ptr<Runner::Runner2> runner_2;
        std::unique_ptr<Runner::Three> three;
        std::unique_ptr<Runner::ThreeList> three_list;
        std::unique_ptr<Runner::Two> two;
        std::unique_ptr<Runner::TwoList> two_list;
        std::unique_ptr<Runner::Ytypes> ytypes;

};

class SubTest : public Entity {
    public:
        SubTest();

    class OneAug : public Entity {
        public:
            OneAug();

        public:
            std::string name;
            int number;

    };

    public:
        std::unique_ptr<SubTest::OneAug> one_aug;

};

class ChildIdentity_Identity : public BaseIdentity_Identity {
    public:
        ChildIdentity_Identity();

    public:

};

class ChildChildIdentity_Identity : public ChildIdentity_Identity {
    public:
        ChildChildIdentity_Identity();

    public:

};


}
}

#endif /* _YDKTEST_SANITY_ */

