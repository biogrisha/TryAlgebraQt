#include "SimpleTree.h"
#include <TRS/MeParserGeneric.h>
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore
{
	namespace {

		void toStrImpl(const std::vector<std::unique_ptr<SimpleMe>>& meList, std::wstring& str)
		{
			for (auto& me : meList)
			{
				if (me->ch)
				{
					str += me->label;
					continue;
				}
				str += L"\\" + me->label;
				if (!me->meta.empty())
				{
					str += L"\\" + me->meta;
				}
				if (!me->children.empty())
				{
					str += L"\\{";
					for (auto& cont : me->children)
					{
						toStrImpl(cont->children, str);
						if (cont != me->children.back())
						{
							str += L"\\,";
						}
					}
					str += L"\\}";
				}
				else
				{
					str += L"\\\\";
				}
			}
		}
	}

	std::vector<std::unique_ptr<SimpleMe>> SimpleMe::fromStr(const std::wstring& str)
	{
		Trs::MeParserGeneric parser(str);
		std::vector<std::unique_ptr<SimpleMe>> res;
		auto* termsPtr = &res;
		SimpleMe* parent = nullptr;
		SimpleMe* lastTerm = nullptr;
		parser.createMe = [&termsPtr, &parent, &lastTerm](const std::wstring_view& str)
			{
				auto t = std::make_unique<SimpleMe>();
				t->label = str;
				t->parent = parent;
				lastTerm = t.get();
				termsPtr->push_back(std::move(t));
			};
		parser.addMeta = [&lastTerm](const std::wstring_view& str)
			{
				lastTerm->meta = str;
			};
		parser.addGlyph = [&termsPtr, &parent, &lastTerm](wchar_t g)
			{
				auto t = std::make_unique<SimpleMe>();
				t->label = std::wstring(1, g);
				t->parent = parent;
				t->ch = true;
				lastTerm = t.get();
				termsPtr->push_back(std::move(t));
			};
		parser.startChildren = [&termsPtr, &parent, &lastTerm]()
			{
				auto t = std::make_unique<SimpleMe>();
				t->label = MeNames::cont;
				t->parent = lastTerm;
				termsPtr = &t->children;
				parent = t.get();
				lastTerm->children.push_back(std::move(t));
				lastTerm = nullptr;
			};
		parser.endChildren = [&termsPtr, &parent, &lastTerm, &terms = res]()
			{
				lastTerm = nullptr;
				parent = parent->parent->parent;

				termsPtr = parent ? &parent->children : &terms;
			};
		parser.nextChild = [&termsPtr, &parent, &lastTerm]()
			{
				auto t = std::make_unique<SimpleMe>();
				t->label = MeNames::cont;
				t->parent = parent->parent;
				termsPtr = &t->children;
				parent = t.get();
				parent->parent->children.push_back(std::move(t));
				lastTerm = nullptr;
			};
		parser.parse();
		return res;
	}

	std::wstring SimpleMe::toStr(const std::vector<std::unique_ptr<SimpleMe>>& meList)
	{
		std::wstring res;
		toStrImpl(meList, res);
		return res;
	}

}