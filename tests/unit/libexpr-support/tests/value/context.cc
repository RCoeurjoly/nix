#include <rapidcheck.h>

#include "tests/path.hh"
#include "tests/value/context.hh"

namespace rc {
using namespace nix;

Gen<StringContextElem::DrvDeep> Arbitrary<StringContextElem::DrvDeep>::arbitrary()
{
    return gen::just(StringContextElem::DrvDeep {
        .drvPath = *gen::arbitrary<StorePath>(),
    });
}

Gen<StringContextElem> Arbitrary<StringContextElem>::arbitrary()
{
    switch (*gen::inRange<uint8_t>(0, std::variant_size_v<StringContextElem::Raw>)) {
    case 0:
        return gen::just<StringContextElem>(*gen::arbitrary<StringContextElem::Opaque>());
    case 1:
        return gen::just<StringContextElem>(*gen::arbitrary<StringContextElem::DrvDeep>());
    case 2:
        return gen::just<StringContextElem>(*gen::arbitrary<StringContextElem::Built>());
    default:
        assert(false);
    }
}

}
