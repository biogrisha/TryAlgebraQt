#pragma once
#include <span>
#include "Trs.h"
#include <Me/include/MeBase.h>
namespace TryAlgebraCore::Trs
{

	class PatternMatcher
	{
        struct BStackEl
        {
            int parent_i = -1;
            int child_i = 0;
        };

        struct State
        {
            std::vector<Term*>& pat;
            std::vector<Term*>& terms;
            std::vector<int> sizePerVar;
            int groundStartNum = 0;
            int groundEndNum = 0;
            bool compBoundaries();

            bool compIntermediate();
        };

    public:
        bool match(const std::vector<Term*>& pattern, const std::span<Term*>& terms);

        bool next();

        bool back();

        bool in();

        std::vector<BStackEl> bstack;
        std::map<Term*, Arg> args;
        bool first_call = true;
	};

    bool compare(Term* lhs, Term* rhs);
	bool match(const std::span<Term*>& terms, const std::vector<Term*>& pattern, int from);
	void recognizeVariables(std::vector<Term*>& pattern);
	void convertMeToTerms(const std::span<std::unique_ptr<MeBase>>& meList, std::vector<Term*>& result, Term* parent);
    void unifyVariables(std::vector<Term*>& terms, std::vector<Term*>& variables);
    void clearReps(std::vector<Term*>& terms);
}