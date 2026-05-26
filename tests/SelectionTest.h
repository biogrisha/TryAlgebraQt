#pragma once

#include "TestHelpers.h"
#include <MathEditor/include/MathDocument.h>
#include <FreeTypeWrap.h>
#include <MathDocumentState.h>
using namespace TryAlgebraCore;

inline int xDpi, yDpi = 96;

struct SelectionFixture
{
	static inline std::wstring str1 = L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg\\}efg";
	static inline std::wstring str2 = L"abc\\ft\\A\\{abc\\,e\\ft\\A\\{\\,\\}fg\\}e\nfg";
	SelectionFixture(const std::wstring& str)
	{
		ft.Init(xDpi, yDpi);
		vt.ft = &ft;
		vt.mdoc_state = &md;

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
glm::vec2 getPosCase(PosCase pos_case, MeBase* me)
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

MeHelpers::GetByPathRes meToPosToMe(const SelectionFixture& sf, MeBase* me, PosCase pos_case)
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

void getPathAtPosTest_leafTl(const std::vector<size_t>& path)
{
	SelectionFixture sf(SelectionFixture::str1);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), path);
	auto res = meToPosToMe(sf, me, PosCase::tl);
	EXPECT_EQ(res.status, MeHelpers::GetByPathStatus::me);
	EXPECT_EQ(res.me, me);
}

TEST(MathDocumentTest, getPathAtPosTest_leafTl_0)
{	
	getPathAtPosTest_leafTl({ 0 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafTl_1)
{
	getPathAtPosTest_leafTl({ 1 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafTl_2)
{
	getPathAtPosTest_leafTl({ 6 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafTl_3)
{
	getPathAtPosTest_leafTl({ 3,0,0 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafTl_4)
{
	getPathAtPosTest_leafTl({ 3,1,2 });
}

void getPathAtPosTest_leafBr(const std::vector<size_t>& path)
{
	SelectionFixture sf(SelectionFixture::str1);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), path);
	auto res = meToPosToMe(sf, me, PosCase::br);
	bool check = res.status == MeHelpers::GetByPathStatus::me && res.me == me;
	EXPECT_FALSE(check);
}

TEST(MathDocumentTest, getPathAtPosTest_leafBr_0)
{
	getPathAtPosTest_leafBr({ 0 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafBr_1)
{
	getPathAtPosTest_leafBr({ 3 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafBr_2)
{
	getPathAtPosTest_leafBr({ 6 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafBr_3)
{
	getPathAtPosTest_leafBr({ 3,1,2 });
}

void getPathAtPosTest_leafBrInside(const std::vector<size_t>& path, const std::vector<size_t>& path2)
{
	SelectionFixture sf(SelectionFixture::str2);
	auto me1 = MeHelpers::getByTreePath(sf.cont.get(), path);

	auto res = meToPosToMe(sf, me1, PosCase::br_inside);
	auto me2 = MeHelpers::getByTreePath(sf.cont.get(), path2);
	EXPECT_EQ(res.status, MeHelpers::GetByPathStatus::me);
	EXPECT_EQ(res.me, me2);
}

TEST(MathDocumentTest, getPathAtPosTest_leafBrInside_0)
{
	getPathAtPosTest_leafBrInside({ 0 }, { 1 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafBrInside_1)
{
	getPathAtPosTest_leafBrInside({ 1 }, { 2 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafBrInside_2)
{
	getPathAtPosTest_leafBrInside({ 4 }, { 5 });
}

void getPathAtPosTest_leafBrInside_lastInCont(const std::vector<size_t>& path)
{
	SelectionFixture sf(SelectionFixture::str2);
	auto me = MeHelpers::getByTreePath(sf.cont.get(), path);

	auto res = meToPosToMe(sf, me, PosCase::br_inside);
	EXPECT_EQ(res.status, MeHelpers::GetByPathStatus::last);
	EXPECT_EQ(res.me, me);
}

TEST(MathDocumentTest, getPathAtPosTest_leafBrInside_lastInCont_0)
{
	getPathAtPosTest_leafBrInside_lastInCont({ 7 });
}

TEST(MathDocumentTest, getPathAtPosTest_leafBrInside_lastInCont_1)
{
	getPathAtPosTest_leafBrInside_lastInCont({ 3,0,2 });
}