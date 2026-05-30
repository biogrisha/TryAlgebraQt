#pragma once
#include "TestHelpers.h"
#include <MathEditor/include/TextBuffer.h>

TEST(TextBufferTest, getLineNumTest1)
{
	TextBuffer tb;
	tb.insert(L"123\n456\n678", 0);
	EXPECT_EQ(tb.getLineNumber(0), 0);
	EXPECT_EQ(tb.getLineNumber(3), 0);
	EXPECT_EQ(tb.getLineNumber(4), 1);
	EXPECT_EQ(tb.getLineNumber(6), 1);
	EXPECT_EQ(tb.getLineNumber(7), 1);
	EXPECT_EQ(tb.getLineNumber(8), 2);
	EXPECT_EQ(tb.getLineNumber(8), 2);
}

TEST(TextBufferTest, getLineNumTest2)
{
	TextBuffer tb;
	tb.insert(L"012\n4\n6\n\n\n", 0);
	EXPECT_EQ(tb.getLineNumber(0), 0);
	EXPECT_EQ(tb.getLineNumber(3), 0);
	EXPECT_EQ(tb.getLineNumber(4), 1);
	EXPECT_EQ(tb.getLineNumber(6), 2);
	EXPECT_EQ(tb.getLineNumber(7), 2);
	EXPECT_EQ(tb.getLineNumber(8), 3);
	EXPECT_EQ(tb.getLineNumber(9), 4);
	EXPECT_EQ(tb.getLineNumber(10), 5);
}