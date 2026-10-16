#include <memory>
#include <vector>
#include <string>

namespace TryAlgebraCore
{
	struct SimpleMe
	{
		bool ch = false;
		std::wstring label;
		std::wstring meta;
		SimpleMe* parent = nullptr;
		std::vector<std::unique_ptr<SimpleMe>> children;
		static std::vector<std::unique_ptr<SimpleMe>> fromStr(const std::wstring& str);
		static std::wstring toStr(const std::vector<std::unique_ptr<SimpleMe>>& meList);
	};

}