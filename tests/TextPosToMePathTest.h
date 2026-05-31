#pragma once
#include "TestHelpers.h"
#include "TestGlobals.h"
namespace TextPosToMePathTest
{
	struct TTextPosToMePath
	{
		void operator()()
		{
			TestGlobals::ContCalculated fx(str);
			auto& children = fx.cont->getChildren();
			children.erase(children.begin(), children.begin() + trim_cont_left);
			auto path = MeHelpers::textPosToMePath(fx.cont.get(), tst_pos);
			EXPECT_EQ(exp_valid, path.has_value());
			if (path.has_value())
			{
				EXPECT_EQ(exp_path, path.value());
			}
		}
		std::wstring str;
		uint64_t trim_cont_left = 0;
		uint64_t tst_pos = 0;
		MePath exp_path;
		bool exp_valid = true;
	};

	TEST(TextPosToMePathTest, t1)
	{		
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = 0,
			.exp_path {
				LeafPos{0}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t2)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = std::wcslen(L"ab\\ft\\A\\{abc\\,efgh\\}"),
			.exp_valid = false,
		};
		tst();
	}
	TEST(TextPosToMePathTest, t3)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = std::wcslen(L"a"),
			.exp_path = {
				LeafPos{std::wcslen(L"a")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t4)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = std::wcslen(L"ab\\ft\\A\\{"),
			.exp_path = {
				MePos{std::wcslen(L"ab"), std::wcslen(L"ab\\ft\\A\\{abc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t5)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = std::wcslen(L"ab\\ft\\A\\{abc\\,"),
			.exp_path = {
				MePos{std::wcslen(L"ab"), std::wcslen(L"ab\\ft\\A\\{abc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{abc\\,")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{abc\\,")}
			}
		};
		tst();
	}
	TEST(TextPosToMePathTest, t6)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = std::wcslen(L"ab\\ft\\A\\{abc\\,efgh"),
			.exp_path = {
				MePos{std::wcslen(L"ab"), std::wcslen(L"ab\\ft\\A\\{abc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{abc\\,")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{abc\\,efgh")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t7)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{abc\\,efgh\\}",
			.tst_pos = std::wcslen(L"ab\\ft\\A\\{abc\\,"),
			.exp_path = {
				MePos{std::wcslen(L"ab"), std::wcslen(L"ab\\ft\\A\\{abc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{abc\\,")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{abc\\,")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t8)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",
			.tst_pos = std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,"),
			.exp_path = {
				MePos{std::wcslen(L"ab"), std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t9)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos = std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}b"),
			.exp_path = {
				MePos{std::wcslen(L"ab"), std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}b")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t10)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos =	std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{"),
			.exp_path = {
				MePos{	std::wcslen(L"ab"), 
						std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{")},
				MePos{	std::wcslen(L"ab\\ft\\A\\{a"), 
						std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t11)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos = std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,1"),
			.exp_path = {
				MePos{	std::wcslen(L"ab"),
						std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{")},
				MePos{	std::wcslen(L"ab\\ft\\A\\{a"),
						std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,1")}
			},
		};
		tst();
	}
	TEST(TextPosToMePathTest, t12)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos = std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123"),
			.exp_path = {
				MePos{	std::wcslen(L"ab"),
						std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{")},
				MePos{	std::wcslen(L"ab\\ft\\A\\{a"),
						std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}")},
				ContPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,")},
				LeafPos{std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123")}
			},
		};
		tst();
	}

	TEST(TextPosToMePathTest, t13)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos = std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\"),
			.exp_valid = false
		};
		tst();
	}

	TEST(TextPosToMePathTest, t14)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos = std::wcslen(L"ab\\f"),
			.exp_valid = false
		};
		tst();
	}

	TEST(TextPosToMePathTest, t15)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",


			.tst_pos = std::wcslen(L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}") + 1,
			.exp_valid = false
		};
		tst();
	}

	TEST(TextPosToMePathTest, t16)
	{
		TTextPosToMePath tst{
			.str = L"ab\\ft\\A\\{a\\ft\\B\\{\\,123\\}bc\\,efgh\\}",
			.trim_cont_left = 2,

			.tst_pos = 0,
			.exp_valid = false
		};
		tst();
	}
}