#pragma once
#include <Me/include/MeParser.h>
#include <Me/include/MeBase.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeFromTo.h>
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

void absToChildPosTest(size_t pos, size_t child_pos, bool valid, MeBase* cont)
{
    std::optional<size_t> child_id = MeHelpers::absToChildPos(cont, pos);
    EXPECT_EQ(child_id.has_value(), valid);
    if(child_id.has_value())
        EXPECT_EQ(child_id.value(), child_pos);
}

TEST(MeHelper, absToChildPosTest0)
{
    absToChildPosTest(0, 0, true, parse(str1).get());
}

TEST(MeHelper, absToChildPosTest1)
{
    absToChildPosTest(1, 1, true, parse(str1).get());
}

//index after string
TEST(MeHelper, absToChildPosTest20)
{
    absToChildPosTest(20, 4, false, parse(str1).get());
}

//child in from_to
TEST(MeHelper, absToChildPosTestFrom_to0)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    absToChildPosTest(10, 0, true, from_to.get());
}

//child in from_to
TEST(MeHelper, absToChildPosTestFrom_to1)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    absToChildPosTest(15, 1, true, from_to.get());
}

//cont3->from_to0->cont
TEST(MeHelper, absToChildPosTestFrom_to0Cont1)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    auto& cont0 = from_to->getChildren()[0];
    absToChildPosTest(12, 2, true, cont0.get());
}

//cont3->from_to0->cont
TEST(MeHelper, absToChildPosTestFrom_to1Cont0)
{
    auto cont = parse(str1);
    auto& from_to = cont->getChildren()[3];
    auto& cont1 = from_to->getChildren()[1];
    absToChildPosTest(15, 0, true, cont1.get());
}

//navigation
//path always points at element in 
void fromToStepTest(MePath path_start, MePath path_res, StepDir dir, StepFrom step_from)
{
    auto cont = parse(str1);
    auto from_to = MyRTTI::Cast<MeFromTo>(cont->getChildren()[3].get());
    from_to->step(dir, step_from, path_start);
    EXPECT_EQ(path_start, path_res);
}

TEST(MeNavigation, fromToStepTest1)
{
    MePath path_start = {
        MePos{3,20},
        ContPos{10},
        LeafPos{10}
    };

    MePath path_res = {
        LeafPos{3},
    };
    fromToStepTest(path_start, path_res, StepDir::left, StepFrom::inside);
}

TEST(MeNavigation, fromToStepTest2)
{
    MePath path_start = {
        MePos{3,20},
        ContPos{10},
        LeafPos{13}
    };

    MePath path_res = {
        MePos{3,20},
        ContPos{15},
        LeafPos{15},
    };
    fromToStepTest(path_start, path_res, StepDir::right, StepFrom::inside);
}

TEST(MeNavigation, fromToStepTest3)
{
    MePath path_start = {
        MePos{3,20},
        ContPos{15},
        LeafPos{18}
    };

    MePath path_res = {
        LeafPos{20},
    };
    fromToStepTest(path_start, path_res, StepDir::right, StepFrom::inside);
}

TEST(MeNavigation, fromToStepTest4)
{
    MePath path_start = {
        MePos{3,20},
        ContPos{15},
        LeafPos{15}
    };

    MePath path_res = {
        MePos{3,20},
        ContPos{10},
        LeafPos{13}
    };
    fromToStepTest(path_start, path_res, StepDir::left, StepFrom::inside);
}

TEST(MeNavigation, fromToStepTest5)
{
    MePath path_start = {
        LeafPos{20}
    };

    MePath path_res = {
        MePos{3,20},
        ContPos{15},
        LeafPos{18}
    };
    fromToStepTest(path_start, path_res, StepDir::left, StepFrom::outside);
}

TEST(MeNavigation, fromToStepTest6)
{
    MePath path_start = {
        LeafPos{3}
    };

    MePath path_res = {
        MePos{3,20},
        ContPos{10},
        LeafPos{10}
    };
    fromToStepTest(path_start, path_res, StepDir::right, StepFrom::outside);
}

inline std::wstring str2 = L"abc\\ft\\A\\{abc\\,efg\\}efg";
inline std::wstring str3 = L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}fg\\}efg";
inline std::wstring str4 = L"\\ft\\A\\{\\,efg\\}efg";

void getByPathTest(const MePath& path, MeBase* from, MeBase* res_me, MeHelpers::GetByPathStatus status)
{
    auto res = MeHelpers::getByPath(from, path);
    EXPECT_EQ(res.status, status);
    EXPECT_EQ(res.me, res_me);
}
TEST(MeNavigation, getByPathTest1)
{
    MePath path = {
        LeafPos{3}
    };
    auto cont = parse(str2);
    auto ch = cont->getChildren()[3].get();
    getByPathTest(path, cont.get(), ch, MeHelpers::GetByPathStatus::me);
}

TEST(MeNavigation, getByPathTest2)
{
    MePath path = {
        LeafPos{23}
    };
    auto cont = parse(str2);
    auto ch = cont->getChildren().back().get();
    getByPathTest(path, cont.get(), ch, MeHelpers::GetByPathStatus::last);
}
