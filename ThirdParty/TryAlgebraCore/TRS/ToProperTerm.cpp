#include "ToProperTerm.h"
#include <TRS/MeParserGeneric.h>

namespace TryAlgebraCore::To
{
	void ToProperTerm::run(const std::wstring& string)
	{
		Trs::MeParserGeneric parser(string);
		auto* termsPtr = &m_terms;
		GenericTerm* parent = nullptr;
		GenericTerm* lastTerm = nullptr;
		parser.createMe = [&termsPtr, &parent, &lastTerm](const std::wstring_view& str)
			{
				auto t = std::make_unique<GenericTerm>();
				t->label = str;
				t->parent = parent;
				lastTerm = t.get();
				termsPtr->push_back(std::move(t));
			};
		parser.addMeta = [&lastTerm](const std::wstring_view& str)
			{
				lastTerm->label += str;
			};
		parser.addGlyph = [&termsPtr, &parent, &lastTerm](wchar_t g)
			{
				auto t = std::make_unique<GenericTerm>();
				t->label = std::wstring(1, g);
				t->parent = parent;
				lastTerm = t.get();
				termsPtr->push_back(std::move(t));
			};
		parser.startChildren = [&termsPtr, &parent, &lastTerm]()
			{
				auto t = std::make_unique<GenericTerm>();
				t->label = L"cont";
				t->parent = lastTerm;
				termsPtr = &t->children;
				parent = t.get();
				lastTerm->children.push_back(std::move(t));
				lastTerm = nullptr;
			};
		parser.endChildren = [&termsPtr, &parent, &lastTerm, &terms = m_terms]()
			{
				lastTerm = nullptr;
				parent = parent->parent->parent;

				termsPtr = parent ? &parent->children : &terms;
			};
		parser.nextChild = [&termsPtr, &parent, &lastTerm]()
			{
				auto t = std::make_unique<GenericTerm>();
				t->label = L"cont";
				t->parent = parent->parent;
				termsPtr = &t->children;
				parent = t.get();
				parent->parent->children.push_back(std::move(t));
				lastTerm = nullptr;
			};
		parser.parse();


	}
	const std::vector<std::unique_ptr<GenericTerm>>& ToProperTerm::get() const
	{
		return m_terms;
	}
}
