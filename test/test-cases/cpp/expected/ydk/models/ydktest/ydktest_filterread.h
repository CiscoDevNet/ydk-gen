#ifndef _YDKTEST_FILTERREAD_
#define _YDKTEST_FILTERREAD_

#include <memory>
#include <vector>
#include <string>
#include "ydk/entity.h"
#include "ydk/types.h"

namespace ydk {
namespace ydktest {

class A : public Entity {
    public:
        A();

    class B : public Entity {
        public:
            B();

        class C : public Entity {
            public:
                C();

            public:

        };

        class D : public Entity {
            public:
                D();

            class E : public Entity {
                public:
                    E();

                public:
                    std::string e1;
                    std::string e2;

            };

            public:
                std::string d1;
                std::string d2;
                std::string d3;
                std::unique_ptr<A::B::D::E> e;

        };

        public:
            std::string b1;
            std::string b2;
            std::string b3;
            std::unique_ptr<A::B::C> c;
            std::unique_ptr<A::B::D> d;

    };

    class Lst : public Entity {
        public:
            Lst();

        public:
            int number;
            std::string value;

    };

    public:
        std::string a1;
        std::string a2;
        std::string a3;
        std::unique_ptr<A::B> b;
        std::vector< std::unique_ptr<A::Lst> > lst;

};


}
}

#endif /* _YDKTEST_FILTERREAD_ */

