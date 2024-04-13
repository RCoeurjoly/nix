#include <nlohmann/json.hpp>
#include <gtest/gtest.h>
#include <rapidcheck/gtest.h>

#include "tests/path.hh"
#include "tests/libexpr.hh"
#include "tests/value/context.hh"

namespace nix {

// Test a few cases of invalid string context elements.

TEST(StringContextElemTest, empty_invalid) {
    EXPECT_THROW(
        StringContextElem::parse(""),
        BadStringContextElem);
}

TEST(StringContextElemTest, single_bang_invalid) {
    EXPECT_THROW(
        StringContextElem::parse("!"),
        BadStringContextElem);
}

TEST(StringContextElemTest, double_bang_invalid) {
    EXPECT_THROW(
        StringContextElem::parse("!!/"),
        BadStorePath);
}

TEST(StringContextElemTest, eq_slash_invalid) {
    EXPECT_THROW(
        StringContextElem::parse("=/"),
        BadStorePath);
}

TEST(StringContextElemTest, slash_invalid) {
    EXPECT_THROW(
        StringContextElem::parse("/"),
        BadStorePath);
}

/**
 * Round trip (string <-> data structure) test for
 * `StringContextElem::Opaque`.
 */
TEST(StringContextElemTest, opaque) {
    std::string_view opaque = "g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x";
    auto elem = StringContextElem::parse(opaque);
    auto * p = std::get_if<StringContextElem::Opaque>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->path, StorePath { opaque });
    ASSERT_EQ(elem.to_string(), opaque);
}

/**
 * Round trip (string <-> data structure) test for
 * `StringContextElem::DrvDeep`.
 */
TEST(StringContextElemTest, drvDeep) {
    std::string_view drvDeep = "=g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv";
    auto elem = StringContextElem::parse(drvDeep);
    auto * p = std::get_if<StringContextElem::DrvDeep>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->drvPath, StorePath { drvDeep.substr(1) });
    ASSERT_EQ(elem.to_string(), drvDeep);
}

/**
 * Round trip (string <-> data structure) test for a simpler
 * `StringContextElem::Built`.
 */
TEST(StringContextElemTest, built_opaque) {
    std::string_view built = "!foo!g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv";
    auto elem = StringContextElem::parse(built);
    auto * p = std::get_if<StringContextElem::Built>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->output, "foo");
    ASSERT_EQ(*p->drvPath, ((SingleDerivedPath) SingleDerivedPath::Opaque {
        .path = StorePath { built.substr(5) },
    }));
    ASSERT_EQ(elem.to_string(), built);
}

/**
 * Round trip (string <-> data structure) test for a more complex,
 * inductive `StringContextElem::Built`.
 */
TEST(StringContextElemTest, built_built) {
    /**
     * We set these in tests rather than the regular globals so we don't have
     * to worry about race conditions if the tests run concurrently.
     */
    ExperimentalFeatureSettings mockXpSettings;
    mockXpSettings.set("experimental-features", "dynamic-derivations ca-derivations");

    std::string_view built = "!foo!bar!g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv";
    auto elem = StringContextElem::parse(built, mockXpSettings);
    auto * p = std::get_if<StringContextElem::Built>(&elem.raw);
    ASSERT_TRUE(p);
    ASSERT_EQ(p->output, "foo");
    auto * drvPath = std::get_if<SingleDerivedPath::Built>(&*p->drvPath);
    ASSERT_TRUE(drvPath);
    ASSERT_EQ(drvPath->output, "bar");
    ASSERT_EQ(*drvPath->drvPath, ((SingleDerivedPath) SingleDerivedPath::Opaque {
        .path = StorePath { built.substr(9) },
    }));
    ASSERT_EQ(elem.to_string(), built);
}

/**
 * Without the right experimental features enabled, we cannot parse a
 * complex inductive string context element.
 */
TEST(StringContextElemTest, built_built_xp) {
    ASSERT_THROW(
        StringContextElem::parse("!foo!bar!g1w7hy3qg1w7hy3qg1w7hy3qg1w7hy3q-x.drv"),        MissingExperimentalFeature);
}

#ifndef COVERAGE

RC_GTEST_PROP(
    StringContextElemTest,
    prop_round_rip,
    (const StringContextElem & o))
{
    RC_ASSERT(o == StringContextElem::parse(o.to_string()));
}

#endif

}
