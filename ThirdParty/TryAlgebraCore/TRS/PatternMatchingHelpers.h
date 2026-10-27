#pragma once
#include <span>
#include <memory>
#include <vector>
#include <string_view>
#include <unordered_set>
#include <functional>
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

	class PatContext
	{
	public:
		PatContext();
		~PatContext();
		void addPat(const std::vector<std::unique_ptr<TermIntermediate>>& pat);
	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};

	/*
		sub
		pat
		subj

		PatContext ctx;
		ctx.addPat(pat1);
		ctx.addPat(pat2);
		ctx.addInst(inst);

		match(ctx, pat1, subj1);
		match(ctx, pat2, subj2);
		auto newInst = instantiate(ctx, inst);
	*/

	//=========================move this out======================
	void markVariables(std::vector<std::unique_ptr<TermIntermediate>>& pat);

	void termIntermediateToStr(const std::vector<std::unique_ptr<TermIntermediate>>& terms, std::wstring& res);
	std::vector<std::unique_ptr<TermIntermediate>> parseToTermIntermediate(const std::wstring_view& str);

	void copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from
		, std::vector<std::unique_ptr<TermIntermediate>>& to, TermIntermediate* parent = nullptr);

	std::vector<std::unique_ptr<TermIntermediate>> copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from);

	std::unique_ptr<TermIntermediate> copyTermIntermediate(const std::unique_ptr<TermIntermediate>& from);

	void expandVars(std::vector<std::unique_ptr<TermIntermediate>>& term);
	//=============================================================

}