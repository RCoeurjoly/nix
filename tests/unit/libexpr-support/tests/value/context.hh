#pragma once
///@file

#include <rapidcheck/gen/Arbitrary.h>

#include "value/context.hh"

namespace rc {
using namespace nix;

template<>
struct Arbitrary<StringContextElem::Opaque> {
    static Gen<StringContextElem::Opaque> arbitrary();
};

template<>
struct Arbitrary<StringContextElem::Built> {
    static Gen<StringContextElem::Built> arbitrary();
};

template<>
struct Arbitrary<StringContextElem::DrvDeep> {
    static Gen<StringContextElem::DrvDeep> arbitrary();
};

template<>
struct Arbitrary<StringContextElem> {
    static Gen<StringContextElem> arbitrary();
};

}
