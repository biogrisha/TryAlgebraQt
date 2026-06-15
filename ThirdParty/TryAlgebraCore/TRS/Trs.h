#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <cctype>
#include <map>
#include <unordered_set>
#include <chrono>

namespace TryAlgebraCore::Trs
{

    struct Term
    {
        std::wstring term_str;
        std::wstring label;
        std::vector<Term*> children;
        std::unordered_set<Term*> parents;
        std::vector<Term*> e_reps;
        Term* e_rep = nullptr;
        bool pat = false;
        std::vector<int> comp_order;
        bool pending_cong = false;
        bool del = false;
        bool variable = false;
    };

    struct TermsStorage
    {
        std::map<std::wstring, std::unique_ptr<Term>> terms_map;
        std::vector<std::unique_ptr<Term>> bin;
    };

    struct Identity
    {
        std::wstring lhs;
        std::wstring rhs;
        Term* t_lhs = nullptr;
        Term* t_rhs = nullptr;
    };

    struct Arg
    {
        Term* term = nullptr;
        int node_id = 0;
    };

    class Parser
    {
    public:
        Parser(const std::wstring& str)
            : m_str(str)
        {
        }

        void parse();

        void consumeTermName();

        Term* m_current_term = nullptr;
        Term* m_parent_term = nullptr;
        const std::wstring& m_str;
        int m_pos = 0;
    };

    class Matcher
    {
    public:
        struct BStackEl
        {
            int parent_i = -1;
            int child_i = 0;
            int eq_i = -1;
            Term* lhs = nullptr;
            Term* rhs = nullptr;
            Term* rhs_main = nullptr;

            void nextRhs();
            bool updateEq();
            bool getChild(Term*& out_lhs, Term*& out_rhs);
        };
    public:

        bool match(Term* lhs, Term* rhs);

        // false means we finished comparisson
        bool next();

        bool back();

        bool in();

        std::vector<BStackEl> bstack;
        std::map<Term*, Arg> args;
        bool first_call = true;
    };

    class Trs
    {
    public:
        void func(std::vector<Identity>& identities, const std::wstring& lhs, const std::wstring& rhs);
    private:
        TermsStorage ts;
    };

    void deleteRecursive(Term* term);

    Term* find(Term* t);

    void compact(Term* term, int term_id, std::map<std::wstring, std::unique_ptr<Term>>& terms_map, bool before_merge = true);

    bool cong(Term* t1, Term* t2);

    void unionTerms(Term* t1, Term* t2, TermsStorage& storage, bool congruent = false);

    void updateCongruence(Term* t, TermsStorage& storage);

    void merge(Term* t1, Term* t2, TermsStorage& storage, bool rec = false);

    void markPatternNodes(Term* t);

    void setupOrder(Term* t);

    void rewrite(Term* pat, std::map<Term*, Arg>& args, std::wstring& res);

    Term* instantiate(Term* pat, std::map<Term*, Arg>& args, std::map<std::wstring, std::unique_ptr<Term>>& terms_map);

    void updateCongruence(Term* t, TermsStorage& storage);

}