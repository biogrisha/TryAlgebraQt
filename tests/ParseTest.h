#pragma once
#include "TestHelpers.h"
#include <MathEditor/include/TextBuffer.h>
namespace ParseTest {
    inline std::wstring str = L"\\ft\\A\\{st\\ft\\A\\{st\\,dsf\\}\\,ds1\\}";
}

void grandParentTest(const std::wstring& str, std::vector<size_t> pos_ch)
{
    auto cont = parse(str);
    auto ch = MeHelpers::getByTreePath(cont.get(), pos_ch);

    pos_ch.resize(std::max(static_cast<int>(pos_ch.size()) - 2, 0));
    MeBase* par = nullptr;
    if (!pos_ch.empty())
    {
        par = MeHelpers::getByTreePath(cont.get(), pos_ch);
    }
    EXPECT_EQ(ch->getParent()->getParent(), par);
}
TEST(ParseTest, grandParentTest1)
{
    grandParentTest(ParseTest::str, { 0, 1, 2 });
}
TEST(ParseTest, grandParentTest2)
{
    grandParentTest(ParseTest::str, { 0 });
}
TEST(ParseTest, grandParentTest3)
{
    grandParentTest(ParseTest::str, { 0, 0, 2 });
}