cmake_minimum_required (VERSION 3.10)

add_executable(TryAlgebraTest 
    TryAlgebraTest.cpp 
    Navigation.h
    MathDocumentTest.h
    TestHelpers.h
    SelectionTest.h
)

target_link_libraries(TryAlgebraTest 
    PRIVATE 
    GTest::gtest
    GTest::gtest_main
    TryAlgebraCore
)
