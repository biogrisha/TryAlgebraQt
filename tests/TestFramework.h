#pragma once
#include <string>
#include <vector>
#include <functional>

#define MYTEST(TestName)                         \
inline struct TestName                               \
{                                                    \
    TestName()                                       \
    {                                                \
        ::TestFramework::tests.push_back(&TestName::runTests); \
    }                                                \
                                                     \
    static void runTests();                          \
                                                     \
} TestName##_instance;                               \
                                                     \
void TestName::runTests()                            \


namespace TestFramework
{
	constexpr const char* RESET = "\033[0m";
	constexpr const char* RED = "\033[31m";
	constexpr const char* GREEN = "\033[32m";
	constexpr const char* YELLOW = "\033[33m";
	constexpr const char* BLUE = "\033[34m";

	std::vector<std::function<void()>> tests;
	std::vector<struct TestData> failed;

	template<typename T>
	struct Cases
	{
		Cases(const std::vector<T>& cases, const std::function<void(const TestData& tst, const T& data)>& func)
		{
			size_t failed_num = 0;
			for (auto& cs : cases)
			{
				func(cs.tst, cs);
				size_t new_failed_num = ::TestFramework::failed.size();
				if (new_failed_num != failed_num)
				{
					std::cout << RED << cs.tst.name << "  " << cs.tst.case_test << "\n" << RESET;
				}
				else
				{
					std::cout << GREEN << cs.tst.name << "  " << cs.tst.case_test << "\n" << RESET;
				}
			}
		}
	};

	struct TestData
	{
		std::string name;
		std::string case_test;

		template <class L, class R>
		bool eq(const L& l, const R& r) const
		{
			bool res = l == r;

			if (!res) {
				::TestFramework::failed.push_back(*this);
			}

			return res;
		}
	};

	inline void runAllTests()
	{
		for (auto& fn : ::TestFramework::tests)
		{
			fn();
		}

		std::cout << RED << "=========== FAILED:\n" << RESET;
		for (auto& cs : ::TestFramework::failed)
		{
			std::cout << RED << cs.name << "  " << cs.case_test << "\n" << RESET;
		}
	}

}