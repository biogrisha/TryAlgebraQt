#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "Navigation.h"
#include "MathDocumentTest.h"
#include "SelectionTest.h"
#include "TextBufferTest.h"
#include "ParseTest.h"

int main(int argc, char* argv[])
{
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}