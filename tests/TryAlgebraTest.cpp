#include <gtest/gtest.h>
#include <gmock/gmock.h>
//#include "Navigation.h"
//#include "MathDocumentTest.h"
#include "SelectionTest.h"
//#include "TextBufferTest.h"
//#include "ParseTest.h"
//#include "TextPosToMePathTest.h"
#include "TestFramework.h"
#include "StepTest.h"
int main(int argc, char* argv[])
{
	TestFramework::runAllTests();
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}