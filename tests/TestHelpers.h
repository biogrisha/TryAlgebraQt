#pragma once
#include <Me/include/MeParser.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace TryAlgebraCore;
inline std::unique_ptr<MeContainer> parse(const std::wstring& str)
{
    TextBuffer tb;
    tb.insert(str, 0);
    MeParser pr(tb, 0);
    std::unique_ptr<MeContainer> cont = MyRTTI::MakeTypedUnique<MeContainer>();
    pr.parseLine(cont.get());
    return cont;
}
