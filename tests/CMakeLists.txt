cmake_minimum_required (VERSION 3.10)

add_executable(TryAlgebraTest 
    TryAlgebraTest.cpp 
    Navigation.h
    MathDocumentTest.h
    TestHelpers.h
    SelectionTest.h
    TextBufferTest.h
    ParseTest.h
    StepTest.h
    TestGlobals.h
    TextPosToMePathTest.h
    TestFramework.h
    IsLastLineTest.h
    ConvertMeToTermsTest.h
    VariatorTest.h
    Prints.h
)

target_link_libraries(TryAlgebraTest 
    PRIVATE 
    GTest::gtest
    GTest::gtest_main
    TryAlgebraCore
)
