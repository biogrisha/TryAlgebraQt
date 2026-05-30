#pragma once
#include "TestHelpers.h"
#include <Me/include/MeParser.h>
#include <Me/include/MeBase.h>
#include <Me/include/MeContainer.h>
#include <Me/include/MeFromTo.h>
#include <Helpers/include/MeHelpers.h>
#include <gtest/gtest.h>
#include <gmock/gmock.h>

using namespace TryAlgebraCore;
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

inline size_t str3_fts = std::wcslen(L"abc");
inline size_t str3_fte = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}fg\\}");
inline size_t str3_ft_cont1 = std::wcslen(L"abc\\ft\\A\\{");
inline size_t str3_ft_cont2 = std::wcslen(L"abc\\ft\\A\\{abc\\,");
inline size_t str3_ft_cont2_fts = std::wcslen(L"abc\\ft\\A\\{abc\\,e");
inline size_t str3_ft_cont2_fte = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}");
inline size_t str3_ft_cont2_ft_cont1 = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{");
inline size_t str3_ft_cont2_ft_cont2 = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,");

inline std::wstring str4 = L"\\ft\\A\\{\\,efg\\}efg";
inline size_t str4_0s = 0;
inline size_t str4_0e = std::wcslen(L"\\ft\\A\\{\\,efg\\}");
inline size_t str4_00 = std::wcslen(L"\\ft\\A\\{");
inline size_t str4_000 = std::wcslen(L"\\ft\\A\\{");

void getByPathTest(const MePath& path, MeBase* from, MeBase* res_me, const std::optional<size_t>& res_pos, MeHelpers::GetByPathStatus status)
{
    auto res = MeHelpers::getByPath(from, path);
    EXPECT_EQ(res.status, status);
    EXPECT_EQ(res.me, res_me);
    EXPECT_EQ(res.pos, res_pos);
}
void getByPathTest(const std::wstring& str, const MePath& path, const std::vector<uint64_t>& me_res_tree_path, MeHelpers::GetByPathStatus status)
{
    auto cont = parse(str);
    auto me_res = MeHelpers::getByTreePath(cont.get(), me_res_tree_path);
    auto res = MeHelpers::getByPath(cont.get(), path);
    EXPECT_EQ(res.status, status);
    EXPECT_EQ(res.me, me_res);
}
TEST(MeNavigation, getByPathTest1)
{
    MePath path = {
        LeafPos{3}
    };
    auto cont = parse(str2);
    auto ch = cont->getChildren()[3].get();
    getByPathTest(path, cont.get(), ch, 3, MeHelpers::GetByPathStatus::me);
}

TEST(MeNavigation, getByPathTest2)
{
    MePath path = {
        LeafPos{23}
    };
    auto cont = parse(str2);
    auto ch = cont->getChildren().back().get();
    getByPathTest(path, cont.get(), ch, {}, MeHelpers::GetByPathStatus::last);
}

TEST(MeNavigation, getByPathTest3)
{
    MePath path = {
        MePos{3, 20},
        ContPos{15},
        LeafPos{17}
    };
    auto cont = parse(str2);
    auto& ch = (*cont)[3][1][2];
    getByPathTest(path, cont.get(), &ch, 2, MeHelpers::GetByPathStatus::me);
}

TEST(MeNavigation, getByPathTest4)
{
    MePath path = {
        MePos{3, 20},
        ContPos{15},
        LeafPos{18}
    };
    auto cont = parse(str2);
    auto& ch = (*cont)[3][1][2];
    getByPathTest(path, cont.get(), &ch, {}, MeHelpers::GetByPathStatus::last);
}

TEST(MeNavigation, getByPathTest5)
{
    MePath path = {
        MePos{str3_fts, str3_fte},
        ContPos{str3_ft_cont2},
        MePos{str3_ft_cont2_fts, str3_ft_cont2_fte},
        ContPos{str3_ft_cont2_ft_cont1},
        LeafPos{str3_ft_cont2_ft_cont1}
    };
    auto cont = parse(str3);
    auto& ch = (*cont)[3][1][1][0][0];
    getByPathTest(path, cont.get(), &ch, 0, MeHelpers::GetByPathStatus::me);
}

TEST(MeNavigation, getByPathTest6)
{
    MePath path = {
        MePos{str4_0s, str4_0e},
        ContPos{str4_00},
        LeafPos{str4_000}
    };
    auto cont = parse(str4);
    auto& ch = (*cont)[0][0];
    getByPathTest(path, cont.get(), &ch, {}, MeHelpers::GetByPathStatus::cont);
}

namespace MeNavigationTest
{
    inline std::wstring str1 = L"dd\\ft\\A\\{\\,\\}dd";
    inline uint32_t str1_2mes = std::wcslen(L"dd");
    inline uint32_t str1_2mee = std::wcslen(L"dd\\ft\\A\\{\\,\\}");
    inline uint32_t str1_2me_0cont = std::wcslen(L"dd\\ft\\A\\{");
    inline uint32_t str1_2me_0cont_0 = std::wcslen(L"dd\\ft\\A\\{");
}

TEST(MeNavigationTest, getByPathTest7)
{
    MePath path = {
        MePos{MeNavigationTest::str1_2mes, MeNavigationTest::str1_2mee},
        ContPos{MeNavigationTest::str1_2me_0cont},
        LeafPos{MeNavigationTest::str1_2me_0cont_0}
    };
    getByPathTest(MeNavigationTest::str1, path, { 2,0 }, MeHelpers::GetByPathStatus::cont);
}

void stepTest(MePath path_start, MePath path_res, StepDir dir, StepFrom step_from, const std::wstring& str)
{
    auto cont = parse(str);
    cont->step(dir, step_from, path_start);
    EXPECT_EQ(path_start, path_res);
}

inline std::wstring str5 = L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}fg\\}efg";
inline size_t str5_last = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}fg\\}efg");
inline size_t str5_3fts = std::wcslen(L"abc");
inline size_t str5_3fte = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}fg\\}");
inline size_t str5_3ft_0cont = std::wcslen(L"abc\\ft\\A\\{");
inline size_t str5_3ft_0cont_0me = std::wcslen(L"abc\\ft\\A\\{");
inline size_t str5_3ft_1cont = std::wcslen(L"abc\\ft\\A\\{abc\\,");
inline size_t str5_3ft_1cont_last = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}fg");
inline size_t str5_3ft_1cont_1fts = std::wcslen(L"abc\\ft\\A\\{abc\\,e");
inline size_t str5_3ft_1cont_1fte = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b\\}");
inline size_t str5_3ft_1cont_1ft_0cont = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{");
inline size_t str5_3ft_1cont_1ft_0cont_0me = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{");
inline size_t str5_3ft_1cont_1ft_1cont = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,");
inline size_t str5_3ft_1cont_1ft_1cont_last = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{a\\,b");

TEST(MeNavigation, cont_step1)
{
    MePath path = {
        LeafPos{0}
    };
    MePath path_res = {
        LeafPos{0}
    };
    stepTest(path, path_res, StepDir::left, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step2)
{
    MePath path = {
        LeafPos{str5_last}
    };
    MePath path_res = {
        LeafPos{str5_last}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step3)
{
    MePath path = {
        LeafPos{0}
    };
    MePath path_res = {
        LeafPos{1}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step4)
{
    MePath path = {
        LeafPos{str5_last}
    };
    MePath path_res = {
        LeafPos{str5_last - 1}
    };
    stepTest(path, path_res, StepDir::left, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step5)
{
    MePath path = {
        LeafPos{str5_last - 1}
    };
    MePath path_res = {
        LeafPos{str5_last - 2}
    };
    stepTest(path, path_res, StepDir::left, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step6)
{
    MePath path = {
        LeafPos{str5_3fts}
    };
    MePath path_res = {
        MePos{str5_3fts, str5_3fte},
        ContPos{str5_3ft_0cont},
        LeafPos{str5_3ft_0cont_0me}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step7)
{
    MePath path = {
        MePos{str5_3fts, str5_3fte},
        ContPos{str5_3ft_1cont},
        LeafPos{str5_3ft_1cont_1fts}
    };
    MePath path_res = {
        MePos{str5_3fts, str5_3fte},
        ContPos{str5_3ft_1cont},
        MePos{str5_3ft_1cont_1fts, str5_3ft_1cont_1fte},
        ContPos{str5_3ft_1cont_1ft_0cont},
        LeafPos{str5_3ft_1cont_1ft_0cont_0me}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, str5);
}

TEST(MeNavigation, cont_step8)
{
    MePath path = {
        MePos{str5_3fts, str5_3fte},
        ContPos{str5_3ft_1cont},
        LeafPos{str5_3ft_1cont_1fte}
    };
    MePath path_res = {
        MePos{str5_3fts, str5_3fte},
        ContPos{str5_3ft_1cont},
        MePos{str5_3ft_1cont_1fts, str5_3ft_1cont_1fte},
        ContPos{str5_3ft_1cont_1ft_1cont},
        LeafPos{str5_3ft_1cont_1ft_1cont_last}
    };
    stepTest(path, path_res, StepDir::left, StepFrom::none, str5);
}

inline std::wstring str6 =                                L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg\\}efg";
inline size_t str6_3fts =                     std::wcslen(L"abc");
inline size_t str6_3fte =                     std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg\\}");
inline size_t str6_3ft_0cont =                std::wcslen(L"abc\\ft\\A\\{");
inline size_t str6_3ft_0cont_0me =            std::wcslen(L"abc\\ft\\A\\{");
inline size_t str6_3ft_1cont =                std::wcslen(L"abc\\ft\\A\\{abc\\,");
inline size_t str6_3ft_1cont_last =           std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg");
inline size_t str6_3ft_1cont_1fts =           std::wcslen(L"abc\\ft\\A\\{abc\\,e");
inline size_t str6_3ft_1cont_1fte =           std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}");
inline size_t str6_3ft_1cont_1ft_0cont =      std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{");
inline size_t str6_3ft_1cont_1ft_0cont_0me =  std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{");
inline size_t str6_3ft_1cont_1ft_1cont =      std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,");
inline size_t str6_3ft_1cont_1ft_1cont_last = std::wcslen(L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,");

TEST(MeNavigation, cont_step9)
{
    MePath path = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        LeafPos{str6_3ft_1cont_1fte}
    };
    MePath path_res = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        MePos{str6_3ft_1cont_1fts, str6_3ft_1cont_1fte},
        ContPos{str6_3ft_1cont_1ft_1cont},
        LeafPos{str6_3ft_1cont_1ft_1cont_last}
    };
    stepTest(path, path_res, StepDir::left, StepFrom::none, str6);
}

TEST(MeNavigation, cont_step10)
{
    MePath path = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        LeafPos{str6_3ft_1cont_1fts}
    };
    MePath path_res = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        MePos{str6_3ft_1cont_1fts, str6_3ft_1cont_1fte},
        ContPos{str6_3ft_1cont_1ft_0cont},
        LeafPos{str6_3ft_1cont_1ft_0cont_0me}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, str6);
}
TEST(MeNavigation, cont_step11)
{
    MePath path = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        MePos{str6_3ft_1cont_1fts, str6_3ft_1cont_1fte},
        ContPos{str6_3ft_1cont_1ft_0cont},
        LeafPos{str6_3ft_1cont_1ft_0cont_0me}
    };
    MePath path_res = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        MePos{str6_3ft_1cont_1fts, str6_3ft_1cont_1fte},
        ContPos{str6_3ft_1cont_1ft_1cont},
        LeafPos{str6_3ft_1cont_1ft_1cont_last}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, str6);
}

TEST(MeNavigation, cont_step12)
{
    MePath path = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        MePos{str6_3ft_1cont_1fts, str6_3ft_1cont_1fte},
        ContPos{str6_3ft_1cont_1ft_1cont},
        LeafPos{str6_3ft_1cont_1ft_1cont_last}
    };
    MePath path_res = {
        MePos{str6_3fts, str6_3fte},
        ContPos{str6_3ft_1cont},
        MePos{str6_3ft_1cont_1fts, str6_3ft_1cont_1fte},
        ContPos{str6_3ft_1cont_1ft_0cont},
        LeafPos{str6_3ft_1cont_1ft_0cont_0me}
    };
    stepTest(path, path_res, StepDir::left, StepFrom::none, str6);
}

namespace MeNavigationTest{
    inline std::wstring str2 = L"\\ft\\A\\{st\\ft\\A\\{st\\,dsf\\}\\,ds1\\}";
    inline uint64_t str2_0mes = 0;
    inline uint64_t str2_0mee =          std::wcslen(L"\\ft\\A\\{st\\ft\\A\\{st\\,dsf\\}\\,ds1\\}");
    inline uint64_t str2_0me_1cont =     std::wcslen(L"\\ft\\A\\{st\\ft\\A\\{st\\,dsf\\}\\,");
    inline uint64_t str2_0me_1cont_3 =   std::wcslen(L"\\ft\\A\\{st\\ft\\A\\{st\\,dsf\\}\\,ds1");
    inline uint64_t str2_1 =             std::wcslen(L"\\ft\\A\\{st\\ft\\A\\{st\\,dsf\\}\\,ds1\\}");
}

TEST(MeNavigationTest, getByPathTest8)
{
    MePath path = {
        MePos{MeNavigationTest::str2_0mes, MeNavigationTest::str2_0mee},
        ContPos{MeNavigationTest::str2_0me_1cont},
        LeafPos{MeNavigationTest::str2_0me_1cont_3}
    };
    auto cont = parse(MeNavigationTest::str2);
    getByPathTest(path, cont.get(), &(*cont)[0][1][2], std::nullopt, MeHelpers::GetByPathStatus::last);
}

TEST(MeNavigationTest, step1)
{
    MePath path = {
        MePos{MeNavigationTest::str2_0mes, MeNavigationTest::str2_0mee},
        ContPos{MeNavigationTest::str2_0me_1cont},
        LeafPos{MeNavigationTest::str2_0me_1cont_3}
    };
    MePath path_res = {
        LeafPos{MeNavigationTest::str2_1}
    };
    stepTest(path, path_res, StepDir::right, StepFrom::none, MeNavigationTest::str2);
}
