#pragma once
#include <span>
#include <memory>
#include <vector>
#include <string>
#include <unordered_set>
#include <functional>

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

	struct DioEqVar
	{
		VariableMeta* var = nullptr;
		int coef = 0;
	};

	struct DioEquation
	{
		std::vector<DioEqVar> prevVars;
		std::vector<DioEqVar> initVars;
		int rhs = 0;

		std::unique_ptr<DioEquation> next;
	};

	enum class DioStatus
	{
		exceeded,
		succeeded,
		solutionFound
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

	struct Block
	{
		int minPatCapture = 0;
		std::span<std::unique_ptr<TermIntermediate>> pat;
		std::unordered_set<TermIntermediate*> varsRec;
		std::vector<TermIntermediate*> vars;
		std::span<std::unique_ptr<TermIntermediate>> subj;
	};

	struct Bundle
	{
		static Block* findBlock(const std::vector<Bundle*>& siblings, const TermIntermediate* parent);
		std::vector<Block*> blocks;
		DioEquation eq;
		std::vector<Bundle*> children;
	};

	struct Level
	{
		std::vector<Block> blocks;
		std::vector<Bundle> bundles;
	};

	bool compare(TermIntermediate* lhs, TermIntermediate* rhs);

	void unifyVariables(std::vector<std::unique_ptr<TermIntermediate>>& terms, std::vector<TermIntermediate*>& variables);

	bool markPatternNodes(std::vector<std::unique_ptr<TermIntermediate>>& t);

	void collectBlocks(std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<Block>& blocks);

	void collectVariables(Block& bl);

	void collectVariablesRec(const std::span<std::unique_ptr<TermIntermediate>>& block, std::unordered_set<TermIntermediate*>& vars);

	void collectBundles(std::vector<Level>& levels);

	void initDiophantineEq(Bundle& bundle);

	bool bundlesRelated(Bundle& b1, Bundle& b2);

	void setupBundlesTree(std::vector<Level>& levels);

	std::vector<Level> generateLevels(std::vector<std::unique_ptr<TermIntermediate>>& pat);

	bool compare1(std::vector<Bundle*>& childBundles, std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<std::unique_ptr<TermIntermediate>>& subj);

	void setupDiophantineRhs(Bundle& bundle);

	bool initVariables(Bundle& bundle);

	DioStatus solve(int pos, DioEquation& eq, int remainder, std::function<bool()> callback);

	bool comparissonTest(std::vector<Level>& levels, std::vector<std::unique_ptr<TermIntermediate>>& pat, std::vector<std::unique_ptr<TermIntermediate>>& subj, int levelI = 0);

	bool compare2(std::vector<Bundle*>& childBundles);

	void markVariables(std::vector<std::unique_ptr<TermIntermediate>>& pat);

	std::vector<std::unique_ptr<TermIntermediate>> parseToTermIntermediate(const std::wstring& str);

	bool tryMatch(std::vector<Bundle*>& childBundles
		, std::vector<std::unique_ptr<TermIntermediate>>& pat
		, std::vector<std::unique_ptr<TermIntermediate>>& subj);

	void copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from
		, std::vector<std::unique_ptr<TermIntermediate>>& to, TermIntermediate* parent = nullptr);
	std::vector<std::unique_ptr<TermIntermediate>> copyTermIntermediate(const std::span<std::unique_ptr<TermIntermediate>>& from);
	void expandVars(std::vector<std::unique_ptr<TermIntermediate>>& term);
}