#pragma once
#include <string>
#include <span>
#include <memory>
#include <vector>

namespace TryAlgebraCore::To
{
	struct GenericTerm;
	struct VariableMeta
	{
		std::span<std::unique_ptr<GenericTerm>> captured;
		bool isCaptured = false;
		int captureSizeNondet = 0;
	};

	struct GenericTerm
	{
		std::wstring label;
		std::vector<std::unique_ptr<GenericTerm>> children;
		GenericTerm* parent = nullptr;
		GenericTerm* subj = nullptr;
		bool isVariable = false;
		bool isPattern = false;
		std::shared_ptr<VariableMeta> variableMeta;
		int num = 0;
		bool isPureVar()
		{
			return isVariable
				&& variableMeta.get() != nullptr
				&& !variableMeta->isCaptured;
		}
	};
	class ToProperTerm
	{
	public:
		void run(const std::wstring& str);
	};
}