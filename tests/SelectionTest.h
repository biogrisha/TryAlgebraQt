#pragma once

#include "TestHelpers.h"
#include <MathEditor/include/MathDocument.h>
#include <FreeTypeWrap.h>
#include <MathDocumentState.h>
using namespace TryAlgebraCore;

inline int xDpi, yDpi = 96;
namespace SelectionTest
{
	inline std::wstring str1 = L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg\\}efg";
	inline std::wstring str2 = L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg\\}e\nfg";
}
struct SelectionFixture
{
	
	SelectionFixture(const std::wstring& str)
	{
		ft.Init(xDpi, yDpi);
		vt.ft = &ft;
		vt.mdocState = &md;

		cont = parse(str);
		cont->calculate(&vt);
		cont->calculatePos();
	}
	FFreeTypeWrap ft;
	FMathDocumentState md;
	VisualToolkit vt;
	std::unique_ptr<MeContainer> cont;
	
};
enum class PosCase
{
	tl,
	br,
	br_inside
};
inline glm::vec2 getPosCase(PosCase pos_case, MeBase* me)
{
	switch (pos_case)
	{
	case PosCase::tl:
	{
		return me->getPos();
	}
	case PosCase::br:
	{
		return me->getPos() + me->getSize();
	}
	case PosCase::br_inside:
	{
		return me->getPos() + me->getSize() - glm::vec2{ 1,1 };
	}
	default:
		break;
	}
	
}

inline MeHelpers::GetByPathRes meToPosToMe(const SelectionFixture& sf, MeBase* me, PosCase pos_case)
{
	auto pos = getPosCase(pos_case, me);
	MePath path;
	if(MeHelpers::getPathAtPos(sf.cont.get(), pos, path))
	{
		return MeHelpers::getByPath(sf.cont.get(), path);
	}
	else
	{
		return {};
	}
}

inline MePath treePathToMePath(const std::vector<size_t>& tree_path, SelectionFixture& sf)
{
	MePath path;
	auto me = MeHelpers::getByTreePath(sf.cont.get(), tree_path);
	MeHelpers::getPathAtPos(sf.cont.get(), me->getPos(), path);
	return path;
}

inline void getPathAtPosTest_leafTl(const std::vector<size_t>& path)
{
	SelectionFixture sf(SelectionTest::str1);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), path);
	auto res = meToPosToMe(sf, me, PosCase::tl);
	EXPECT_EQ(res.status, MeHelpers::GetByPathStatus::me);
	EXPECT_EQ(res.me, me);
}

TEST(SelectionTest, getPathAtPosTest_leafTl_0)
{	
	getPathAtPosTest_leafTl({ 0 });
}

TEST(SelectionTest, getPathAtPosTest_leafTl_1)
{
	getPathAtPosTest_leafTl({ 1 });
}

TEST(SelectionTest, getPathAtPosTest_leafTl_2)
{
	getPathAtPosTest_leafTl({ 6 });
}

TEST(SelectionTest, getPathAtPosTest_leafTl_3)
{
	getPathAtPosTest_leafTl({ 3,0,0 });
}

TEST(SelectionTest, getPathAtPosTest_leafTl_4)
{
	getPathAtPosTest_leafTl({ 3,1,2 });
}

inline void getPathAtPosTest_leafBr(const std::vector<size_t>& path)
{
	SelectionFixture sf(SelectionTest::str1);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), path);
	auto res = meToPosToMe(sf, me, PosCase::br);
	bool check = res.status == MeHelpers::GetByPathStatus::me && res.me == me;
	EXPECT_FALSE(check);
}

TEST(SelectionTest, getPathAtPosTest_leafBr_0)
{
	getPathAtPosTest_leafBr({ 0 });
}

TEST(SelectionTest, getPathAtPosTest_leafBr_1)
{
	getPathAtPosTest_leafBr({ 3 });
}

TEST(SelectionTest, getPathAtPosTest_leafBr_2)
{
	getPathAtPosTest_leafBr({ 6 });
}

TEST(SelectionTest, getPathAtPosTest_leafBr_3)
{
	getPathAtPosTest_leafBr({ 3,1,2 });
}

inline void getPathAtPosTest_leafBrInside(const std::vector<size_t>& path, const std::vector<size_t>& path2)
{
	SelectionFixture sf(SelectionTest::str2);
	auto me1 = MeHelpers::getByTreePath(sf.cont.get(), path);

	auto res = meToPosToMe(sf, me1, PosCase::br_inside);
	auto me2 = MeHelpers::getByTreePath(sf.cont.get(), path2);
	EXPECT_EQ(res.status, MeHelpers::GetByPathStatus::me);
	EXPECT_EQ(res.me, me2);
}

TEST(SelectionTest, getPathAtPosTest_leafBrInside_0)
{
	getPathAtPosTest_leafBrInside({ 0 }, { 1 });
}

TEST(SelectionTest, getPathAtPosTest_leafBrInside_1)
{
	getPathAtPosTest_leafBrInside({ 1 }, { 2 });
}

TEST(SelectionTest, getPathAtPosTest_leafBrInside_2)
{
	getPathAtPosTest_leafBrInside({ 4 }, { 5 });
}

inline void getPathAtPosTest_leafBrInside_lastInCont(const std::vector<size_t>& path)
{
	SelectionFixture sf(SelectionTest::str2);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), path);

	auto res = meToPosToMe(sf, me, PosCase::br_inside);
	EXPECT_EQ(res.status, MeHelpers::GetByPathStatus::last);
	EXPECT_EQ(res.me, me);
}

TEST(SelectionTest, getPathAtPosTest_leafBrInside_lastInCont_0)
{
	getPathAtPosTest_leafBrInside_lastInCont({ 7 });
}

TEST(SelectionTest, getPathAtPosTest_leafBrInside_lastInCont_1)
{
	getPathAtPosTest_leafBrInside_lastInCont({ 3,0,2 });
}

namespace SelectionTest
{
	inline std::wstring str4 = L"dd\\ft\\A\\{\\,\\}";
	inline TreePath str4_tr_2me_0cont = { 2,0 };
	inline MePath str4_2me_0cont_0 = { 
		MePos{std::wcslen(L"dd"), std::wcslen(L"dd\\ft\\A\\{\\,\\}")},
		ContPos{std::wcslen(L"dd\\ft\\A\\{")},
		LeafPos{std::wcslen(L"dd\\ft\\A\\{")},
	};
}

inline void getPathAtPosTest_emptyCont(const std::wstring& str, const TreePath& cont_path, MePath& res_path)
{
	SelectionFixture sf(str);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), cont_path);
	MePath path;
	MeHelpers::getPathAtPos(sf.cont.get(), me->getPos(), path);
	EXPECT_EQ(res_path, path);
}

TEST(SelectionTest, getPathAtPosTest_emptyCont)
{
	getPathAtPosTest_emptyCont(SelectionTest::str4, SelectionTest::str4_tr_2me_0cont, SelectionTest::str4_2me_0cont_0);
}

namespace SelectionTest
{
	inline std::wstring str3 = L"abc\\ft\\A\\{abc\\,efg\\}dvsdv\\ft\\A\\{abc\\,efg\\}";
	std::vector<size_t> str3_3me_0cont_1me = { 3,0,1 };
	std::vector<size_t> str3_9me_0cont_1me = { 9,0,1 };
	std::vector<size_t> str3_1me = { 1 };
	std::vector<size_t> str3_2me = { 2 };
}

void orderTest(const std::vector<size_t>& tree_path1, const std::vector<size_t>& tree_path2, bool swap)
{
	SelectionFixture sf(SelectionTest::str3);
	auto path1 = treePathToMePath(tree_path1, sf);
	auto path2 = treePathToMePath(tree_path2, sf);
	auto path1_copy = path1;
	auto path2_copy = path2;
	MeHelpers::orderPaths(path1, path2);
	EXPECT_EQ(path1, swap ? path2_copy : path1_copy);
	EXPECT_EQ(path2, swap ? path1_copy : path2_copy);
}

TEST(SelectionTest, OrderTest0)
{
	orderTest(SelectionTest::str3_3me_0cont_1me, SelectionTest::str3_9me_0cont_1me, false);
}

TEST(SelectionTest, OrderTest1)
{
	orderTest(SelectionTest::str3_9me_0cont_1me, SelectionTest::str3_3me_0cont_1me, true);
}

TEST(SelectionTest, OrderTest2)
{
	orderTest(SelectionTest::str3_1me, SelectionTest::str3_2me, false);
}

TEST(SelectionTest, OrderTest3)
{
	orderTest(SelectionTest::str3_2me, SelectionTest::str3_1me, true);
}

TEST(SelectionTest, OrderTest4)
{
	orderTest(SelectionTest::str3_3me_0cont_1me, SelectionTest::str3_1me, true);
}

TEST(SelectionTest, OrderTest5)
{
	orderTest(SelectionTest::str3_1me, SelectionTest::str3_3me_0cont_1me, false);
}


TEST(SelectionTest, OrderTest6)
{
	orderTest(SelectionTest::str3_1me, SelectionTest::str3_3me_0cont_1me, false);
}

void trimToCommonContainerTest(MePath& path1, MePath& path2, const MePath& path1_res, const MePath& path2_res)
{
	MeHelpers::orderPaths(path1, path2);
	MeHelpers::trimToCommonContainer(path1, path2);
	EXPECT_EQ(path1, path1_res);
	EXPECT_EQ(path2, path2_res);
	EXPECT_NE(path1, path2);
}

TEST(SelectionTest, trimToCommonContainerTest1)
{
	MePath p1 = {
		MePos{0,10},
		ContPos{5},
		LeafPos{6}
	};

	MePath p2 = {
		MePos{0,10},
		ContPos{15},
		LeafPos{16}
	};
	MePath p1_res = {
		LeafPos{0}
	};

	MePath p2_res = {
		LeafPos{10}
	};
	trimToCommonContainerTest(p1, p2, p1_res, p2_res);
}

TEST(SelectionTest, trimToCommonContainerTest2)
{
	MePath p1 = {
		LeafPos{6}
	};

	MePath p2 = {
		LeafPos{16}
	};
	MePath p1_res = {
		LeafPos{6}
	};

	MePath p2_res = {
		LeafPos{16}
	};
	trimToCommonContainerTest(p1, p2, p1_res, p2_res);
}

TEST(SelectionTest, trimToCommonContainerTest3)
{
	MePath p1 = {
		MePos{10, 30},
		ContPos{14},
		MePos{15, 21},
		ContPos{17},
		LeafPos{20}
	};

	MePath p2 = {
		LeafPos{16}
	};
	MePath p1_res = {
		LeafPos{10}
	};

	MePath p2_res = {
		LeafPos{16}
	};
	trimToCommonContainerTest(p1, p2, p1_res, p2_res);
}

TEST(SelectionTest, trimToCommonContainerTest4)
{
	MePath p2 = {
		MePos{10, 30},
		ContPos{14},
		MePos{15, 21},
		ContPos{17},
		LeafPos{18}
	};

	MePath p1 = {
		MePos{10, 30},
		ContPos{14},
		MePos{15, 21},
		ContPos{17},
		LeafPos{20}
	};

	MePath p1_res = {
		MePos{10, 30},
		ContPos{14},
		MePos{15, 21},
		ContPos{17},
		LeafPos{18}
	};

	MePath p2_res = {
		MePos{10, 30},
		ContPos{14},
		MePos{15, 21},
		ContPos{17},
		LeafPos{20}
	};
	trimToCommonContainerTest(p1, p2, p1_res, p2_res);
}

TEST(SelectionTest, trimToCommonContainerTest5)
{
	MePath p2 = {
		MePos{10, 30},
		ContPos{14},
		MePos{15, 21},
		ContPos{17},
		LeafPos{18}
	};

	MePath p1 = {
		MePos{10, 30},
		ContPos{14},
		LeafPos{22}
	};

	MePath p1_res = {
		MePos{10, 30},
		ContPos{14},
		LeafPos{15}
	};

	MePath p2_res = {
		MePos{10, 30},
		ContPos{14},
		LeafPos{22}
	};
	trimToCommonContainerTest(p1, p2, p1_res, p2_res);
}