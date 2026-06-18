#pragma once
#include <span>
#include "Trs.h"
#include <Me/include/MeBase.h>

namespace TryAlgebraCore::Trs
{
    using TermRawSh = std::shared_ptr<struct TermRaw>;
    struct TermRaw
    {
        bool variable = false;
        std::wstring label;
        std::vector<TermRawSh> children;
        std::vector<TermRawSh> captured;
        std::vector<int> compOrder;
    };

    struct Variator
    {
        Variator(int size, int sum);
        int step();
        std::vector<int> m_offsets;
        std::vector<int> m_sizes;
        int m_sum = 0;
    };

	class PatternMatcher
	{
        struct BStackEl
        {
            int parent_i = -1;
            int child_i = 0;
        };

        struct State
        {
            std::vector<TermRawSh>& pat;
            std::vector<TermRawSh>& terms;

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
        bool first_call = true;
	};

    
    bool compareWithVariable(TermRaw* var, const std::span<TermRawSh>& terms);
    bool compare(TermRaw* lhs, TermRaw* rhs);
	bool match(const std::span<Term*>& terms, const std::vector<Term*>& pattern, int from);
	void recognizeVariables(std::vector<TermRawSh>& pattern);
	void convertMeToTerms(const std::span<std::unique_ptr<MeBase>>& meList, std::vector<TermRawSh>& result, TermRaw* parent);
    void unifyVariables(std::vector<TermRawSh>& terms, std::vector<TermRaw*>& variables);
    
}