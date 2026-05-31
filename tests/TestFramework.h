#pragma once
#include <string>
#include <vector>
#include <functional>
#include <concepts>

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

#define MY_EQ(lhs, rhs)\
if (!tst.eq(lhs, rhs)) \
	return;			   \


namespace TestFramework
{
	template<typename T>
	concept Streamable =
		requires(std::ostream & os, const T & value)
	{
		{ os << value } -> std::same_as<std::ostream&>;
	};

	constexpr const char* RESET = "\033[0m";
	constexpr const char* RED = "\033[31m";
	constexpr const char* GREEN = "\033[32m";
	constexpr const char* YELLOW = "\033[33m";
	constexpr const char* BLUE = "\033[34m";

	struct ColorGuard {
		ColorGuard(const char* color) { std::cout << color; }
		~ColorGuard() { std::cout << RESET << std::flush; }
	};

	std::vector<std::function<void()>> tests;
	std::vector<struct TestData> failed;

	template<typename T, bool OnlyLastTest = false>
	struct Cases
	{
		Cases(const std::vector<T>& cases, const std::function<void(const TestData& tst, const T& data)>& func)
		{
			size_t failed_num = ::TestFramework::failed.size();
			if constexpr (OnlyLastTest)
			{
				auto cs = cases.back();
				func(cs.tst, cs);
				size_t new_failed_num = ::TestFramework::failed.size();
				if (new_failed_num != failed_num)
				{
					failed_num = new_failed_num;
					ColorGuard color(RED);
					std::cout << cs.tst.name << "  " << cs.tst.case_test << "\n";
				}
				else
				{
					ColorGuard color(GREEN);
					std::cout << cs.tst.name << "  " << cs.tst.case_test << "\n";
				}
			}
			else
			{
				for (auto& cs : cases)
				{
					func(cs.tst, cs);
					size_t new_failed_num = ::TestFramework::failed.size();
					if (new_failed_num != failed_num)
					{
						failed_num = new_failed_num;
						ColorGuard color(RED);
						std::cout << cs.tst.name << "  " << cs.tst.case_test << "\n";
					}
					else
					{
						ColorGuard color(GREEN);
						std::cout << cs.tst.name << "  " << cs.tst.case_test << "\n";
					}
				}
			}
		}
	};

	struct TestData
	{
		std::string name;
		std::string case_test;

		template <typename L, typename R>
		bool eq(const L& l, const R& r) const
		{
			bool res = l == r;

			if (!res) {
				::TestFramework::failed.push_back(*this);
			}

			if(!res)
			{
				ColorGuard color(RED);
				if constexpr (Streamable<L> && Streamable<R>) {
					std::cout << "Comparisson failed \n" << l << " == " << r << "\n";
				}
				else {
					std::cout << "Comparisson failed "
						<< "<non-streamable> == <non-streamable>"
						<< "\n";
				}
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

		if (::TestFramework::failed.empty())
		{
			return;
		}

		{
			ColorGuard color(RED);
			std::cout << RED << "=========== FAILED:\n";
			for (auto& cs : ::TestFramework::failed)
			{
				std::cout << cs.name << "  " << cs.case_test << "\n";
			}
		}
	}

}