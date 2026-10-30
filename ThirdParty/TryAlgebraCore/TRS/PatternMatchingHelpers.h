#pragma once
#include <span>
#include <memory>
#include <vector>
#include <string_view>
#include <string>

namespace TryAlgebraCore::Trs
{
	struct TermIntermediate;
	struct VariableMeta
	{
		std::span<std::unique_ptr<TermIntermediate>> captured;
		bool isCaptured = false;
		int captureSizeNondet = 0;
		enum class Type
		{
			ZeroMulti,
			OneMulti,
			Uni,
		} type = Type::OneMulti;
	};

	struct TermIntermediate
	{
		std::wstring label;
		std::vector<std::unique_ptr<TermIntermediate>> children;
		TermIntermediate* parent = nullptr;
		TermIntermediate* subj = nullptr;
		bool isVariable = false;
		bool isPattern = false;
		std::shared_ptr<VariableMeta> variableMeta;
		bool isPureVar()
		{
			return isVariable
				&& variableMeta.get() != nullptr
				&& !variableMeta->isCaptured;
		}
	};

	class PatternMatcher
	{
	public:
		using Id = uint16_t;
		PatternMatcher();
		~PatternMatcher();
		Id addPat(std::vector<std::unique_ptr<TermIntermediate>>&& pat);
		Id addInst(std::vector<std::unique_ptr<TermIntermediate>>&& inst);
		bool match(Id pat, const std::span<std::unique_ptr<TermIntermediate>>& subj);
		std::vector<std::unique_ptr<TermIntermediate>> instantiate(Id instId) const;
		const std::vector<std::unique_ptr<TermIntermediate>>& pat(Id patId) const;
		void clear();
	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	void markVariables(std::vector<std::unique_ptr<TermIntermediate>>& pat);

	void termIntermediateToStr(const std::vector<std::unique_ptr<TermIntermediate>>& terms, std::wstring& res);
	std::vector<std::unique_ptr<TermIntermediate>> parseToTermIntermediate(const std::wstring_view& str);

	void copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from
		, std::vector<std::unique_ptr<TermIntermediate>>& to, TermIntermediate* parent = nullptr);

	std::vector<std::unique_ptr<TermIntermediate>> copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from);

	std::unique_ptr<TermIntermediate> copyTermIntermediate(const std::unique_ptr<TermIntermediate>& from);

	//rewriting
	void rewriteLeftRightOneTime(
		std::vector<std::unique_ptr<TermIntermediate>>& subj,
		PatternMatcher& matcher,
		PatternMatcher::Id pat,
		PatternMatcher::Id inst);
}