#pragma once
#include <Me/include/MeParser.h>
#include <Me/include/MeBase.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace TryAlgebraCore;
inline std::unique_ptr<MeContainer> parse(const std::wstring& str)
{
    TextBuffer tb;
    tb.insert(str, 0);
    MeParser pr(tb,0);
    std::unique_ptr<MeContainer> cont = MyRTTI::MakeTypedUnique<MeContainer>();
    pr.parseLine(cont.get());
    return cont;
}

inline std::wstring str1 = L"abc\\ft\\A\\{abc\\,efg\\}";

void absToChildPosTest(const AbsPath& path, int child_pos, bool valid, MeBase* cont)
{
    std::optional<int> child_id = MeHelpers::absToChildPos(cont, path.back().from);
    EXPECT_EQ(child_id.has_value(), valid);
    if(child_id.has_value())
        EXPECT_EQ(child_id.value(), child_pos);
}

TEST(MeHelper, absToChildPosTest0)
{
    absToChildPosTest({ {0,0} }, 0, true, parse(str1).get());
}

TEST(MeHelper, absToChildPosTest1)
{
    absToChildPosTest({ {1,1} }, 1, true, parse(str1).get());
}

//index after string
TEST(MeHelper, absToChildPosTest20)
{
    absToChildPosTest({ {20,20} }, 4, false, parse(str1).get());
}

//child in from_to
TEST(MeHelper, absToChildPosTestFrom_to0)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    absToChildPosTest({ {10,10} }, 0, true, from_to.get());
}

//child in from_to
TEST(MeHelper, absToChildPosTestFrom_to1)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    absToChildPosTest({ {15,15} }, 1, true, from_to.get());
}

//cont3->from_to0->cont
TEST(MeHelper, absToChildPosTestFrom_to0Cont1)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    auto& cont0 = from_to->getChildren()[0];
    absToChildPosTest({ {12,12} }, 2, true, cont0.get());
}

//cont3->from_to0->cont
TEST(MeHelper, absToChildPosTestFrom_to1Cont0)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    auto& cont1 = from_to->getChildren()[1];
    absToChildPosTest({ {15,15} }, 0, true, cont1.get());
}

//navigation

TEST(MeNavigation, step)
{
    /*auto cont = parse(str1);
    std::optional<int> child_id = MeHelpers::absToChildPos(cont.get(), path.back().from);
    EXPECT_EQ(child_id.has_value(), valid);
    if (child_id.has_value())
        EXPECT_EQ(child_id.value(), child_pos);*/
}
