#pragma once
///@file

#include "nixexpr.hh"
#include "eval.hh"

#include <string>
#include <map>
#include <nlohmann/json_fwd.hpp>

namespace nix {

nlohmann::json printValueAsJSON(EvalState & state, bool strict,
    Value & v, const PosIdx pos, StringContext & context, bool copyToStore = true);

void printValueAsJSON(EvalState & state, bool strict,
    Value & v, const PosIdx pos, std::ostream & str, StringContext & context, bool copyToStore = true);

}
