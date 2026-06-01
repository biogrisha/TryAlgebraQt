#pragma once
#include <Me/include/MeParser.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace TryAlgebraCore;
namespace TestHelpers
{
    inline std::unique_ptr<MeContainer> parse(const std::wstring& str)
    {
        TextBuffer tb;
        tb.insert(str, 0);
        MeParser pr(tb, 0);
        std::unique_ptr<MeContainer> cont = MyRTTI::MakeTypedUnique<MeContainer>();
        while (pr.parseLine(cont.get()));
        return cont;
    }
    inline MeHelpers::GetByPathRes getByTextPos(MeBase* cont, const uint32_t pos)
    {
        auto path = MeHelpers::textPosToMePath(cont, pos);
        if (path.has_value())
        {
            return MeHelpers::getByPath(cont, path.value());
        }
        return {};
    }
}
