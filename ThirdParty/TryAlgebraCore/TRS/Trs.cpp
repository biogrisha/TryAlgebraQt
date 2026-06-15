#include "Trs.h"

namespace TryAlgebraCore::Trs
{

void Parser::parse()
{
    while (m_pos != m_str.size())
    {
        int term_start = m_pos;
        consumeTermName();
        int label_end = m_pos;
        auto t = new Term();
        t->e_reps.push_back(t);
        t->e_rep = t;
        m_current_term = t;
        if (m_parent_term)
        {
            m_current_term->parents.insert(m_parent_term);
            m_parent_term->children.push_back(m_current_term);
        }
        if (m_pos > m_str.size())
        {
            return;
        }
        if (m_str[m_pos] == '(')
        {
            ++m_pos;
            m_parent_term = m_current_term;
            parse();
            m_current_term = m_parent_term;
            m_parent_term = !m_current_term->parents.empty() ? *m_current_term->parents.begin() : nullptr;
        }
        m_current_term->label = m_str.substr(term_start, label_end - term_start);
        m_current_term->term_str = m_str.substr(term_start, m_pos - term_start);
        if (m_pos >= m_str.size())
        {
            return;
        }
        if (m_str[m_pos] == ')')
        {
            ++m_pos;
            return;
        }
        ++m_pos;
    }
}
void Parser::consumeTermName()
{
    int i = m_pos;
    for (; i < m_str.size(); ++i)
    {
        if (m_str[i] == '(' || m_str[i] == ')' || m_str[i] == ',')
        {
            break;
        }
    }
    m_pos = i;
}

void deleteRecursive(Term* term)
{
    for (auto ch : term->children)
    {
        deleteRecursive(ch);
    }
    delete term;
}

Term* find(Term* t)
{
    while (t->e_rep != t)
    {
        t = t->e_rep;
    }
    return t;
}
void compact(Term* term, int term_id, std::map<std::wstring, std::unique_ptr<Term>>& terms_map, bool before_merge)
{
    auto found_term = terms_map.find(term->term_str);
    if (found_term == terms_map.end())
    {
        //in this case this means that this term and its parents are not in terms_map
        //therefor we just put it into map without changing its parents
        term->pending_cong = !before_merge;
        terms_map.emplace(term->term_str, std::unique_ptr<Term>(term));
    }
    else
    {
        //term already in map, this means, that it and its children are in terms_map
        //its parent still added into terms_map only once
        //but two identical terms could have same parent e.g. f(a,a)
        // so we need to add parent uniquely here
        // update this element in parent with found term
        //->delete this term recursively
        if (!term->parents.empty())
        {
            find(found_term->second.get())->parents.insert(*term->parents.begin());
            (*term->parents.begin())->children[term_id] = found_term->second.get();
        }

        deleteRecursive(term);
        return;
    }
    int i = 0;
    for (auto ch : term->children)
    {
        compact(ch, i, terms_map, before_merge);
        ++i;
    }
}
bool cong(Term* t1, Term* t2)
{
    if (t1->label != t2->label)
    {
        return false;
    }
    for (int i = 0; i < t1->children.size(); ++i)
    {
        if (find(t1->children[i]) != find(t2->children[i]))
        {
            return false;
        }
    }
    return true;
}
void unionTerms(Term* t1, Term* t2, TermsStorage& storage, bool congruent)
{
    Term* main_t = find(t1);
    Term* sub_t = find(t2);
    if (t1 == t2)
    {
        return;
    }

    if (main_t != sub_t)
    {
        for (auto par : sub_t->parents)
        {
            main_t->parents.insert(par);
        }
        main_t->e_reps.insert(main_t->e_reps.end(), sub_t->e_reps.begin(), sub_t->e_reps.end());
        for (auto rep : main_t->e_reps)
        {
            rep->e_rep = main_t;
        }
        sub_t->e_reps.clear();
        sub_t->parents.clear();
    }

    if (congruent)
    {
        for (auto par : main_t->parents)
        {
            for (int i = 0; i < par->children.size(); ++i)
            {
                if (par->children[i] == t2)
                {
                    par->children[i] = main_t;
                }
            }
        }
        for (auto ch : t2->children)
        {
            ch = find(ch);
            ch->parents.erase(t2);
            ch->parents.insert(t1);
        }

        //opt: can be moved before e_reps.insert
        for (size_t i = 0; i < main_t->e_reps.size(); ++i)
        {
            if (main_t->e_reps[i] == t2)
            {
                std::swap(main_t->e_reps[i], main_t->e_reps.back());
                main_t->e_reps.pop_back();
                break;
            }
        }
        auto it = storage.terms_map.find(t2->term_str);

        if (it != storage.terms_map.end())
        {
            auto value = std::move(it->second);
            value->del = true;
            storage.terms_map.erase(it);
            storage.bin.push_back(std::move(value));
        }

    }
}

void merge(Term* t1, Term* t2, TermsStorage& storage, bool rec)
{
    auto t1_top = find(t1);
    auto t2_top = find(t2);
    if (t1 == t2)
    {
        return;
    }
    auto pars1 = t1_top->parents;
    auto pars2 = t2_top->parents;
    unionTerms(t1, t2, storage, rec);
    for (auto par1 : pars1)
    {
        if (find(par1) == t1_top || par1->del)
        {
            continue;
        }
        for (auto par2 : pars2)
        {
            if (find(par2) == t1_top || par2->del)
            {
                continue;
            }

            if (cong(par1, par2))
            {
                merge(par1, par2, storage, true);
            }
        }
    }
}
void markPatternNodes(Term* t)
{
    bool pat_temp = false;
    for (auto ch : t->children)
    {
        markPatternNodes(ch);
        pat_temp |= ch->pat;
    }
    if (pat_temp)
    {
        t->pat = true;
        return;
    }
    if (t->variable)
    {
        t->pat = true;
    }
}
void setupOrder(Term* t)
{
    for (int i = 0; i < t->children.size(); ++i)
    {
        if (!t->children[i]->pat)
        {
            t->comp_order.push_back(i);
        }
    }
    for (int i = 0; i < t->children.size(); ++i)
    {
        if (t->children[i]->pat)
        {
            t->comp_order.push_back(i);
        }
    }
    for (auto ch : t->children)
    {
        setupOrder(ch);
    }
}
void rewrite(Term* pat, std::map<Term*, Arg>& args, std::wstring& res)
{
    if (pat->label[0] == '`')
    {
        auto arg = args.find(pat);
        if (arg == args.end())
        {
            res += pat->label;
        }
        else
        {
            res += arg->second.term->term_str;
        }
        return;
    }
    res += pat->label;
    if (!pat->children.empty())
    {
        res += '(';
        for (auto& ch : pat->children)
        {
            rewrite(ch, args, res);
            res += ',';
        }
        res.back() = ')';
    }
}
Term* instantiate(Term* pat, std::map<Term*, Arg>& args, std::map<std::wstring, std::unique_ptr<Term>>& terms_map)
{
    std::wstring str;
    rewrite(pat, args, str);
    auto found = terms_map.find(str);
    if (found != terms_map.end())
    {
        return found->second.get();
    }
    Parser pr(str);
    pr.parse();
    compact(pr.m_current_term, 0, terms_map);

    return terms_map.find(str)->second.get();
}
void updateCongruence(Term* t, TermsStorage& storage)
{
    if (!t->pending_cong)
    {
        return;
    }
    t->pending_cong = false;
    if (t->children.empty())
    {
        return;
    }
    for (auto ch : t->children)
    {
        updateCongruence(ch, storage);
    }
    auto pars = find(t->children.back())->parents;
    for (auto par : pars)
    {
        if (find(t) != find(par) && cong(t, par))
        {
            //if found congruent parent, this means that this parent congruent to other parents and there is no need to continue
            unionTerms(par, t, storage, true);
            break;
        }
    }
    
}

void Matcher::BStackEl::nextRhs()
{

    ++eq_i;
    if (!lhs->pat)
    {
        // don't iterate over e-class for ground terms
        return;
    }
    if (lhs->label[0] == '`')
    {
        return;
    }
    for (; eq_i < rhs_main->e_reps.size(); ++eq_i)
    {
        if (rhs_main->e_reps[eq_i]->label == lhs->label)
        {
            return;
        }
    }
}
bool Matcher::BStackEl::updateEq()
{
    nextRhs();
    if ((eq_i > 0 && (!lhs->pat || lhs->label[0] == '`')) || eq_i >= rhs_main->e_reps.size())
    {
        return false;
    }
    rhs = rhs_main->e_reps[eq_i];
    child_i = 0;
    return true;
}
bool Matcher::BStackEl::getChild(Term*& out_lhs, Term*& out_rhs)
{
    if (rhs->children.empty())
    {
        return false;
    }
    if (child_i >= lhs->children.size())
    {
        return false;
    }
    out_lhs = lhs->children[lhs->comp_order[child_i]];
    out_rhs = rhs->children[lhs->comp_order[child_i]];
    return true;
}
bool Matcher::match(Term* lhs, Term* rhs)
{
    if (first_call)
    {
        first_call = false;
        BStackEl el;
        el.lhs = lhs;
        el.rhs_main = find(rhs);
        if (!el.updateEq())
        {
            return false;
        }
        bstack.push_back(el);
    }
    else if (!back())
    {
        return false;
    }
    while (true)
    {
        BStackEl& top = bstack.back();
        if (!top.lhs->pat)
        {
            if (find(top.lhs) == find(top.rhs))
            {
                if (!next())
                {
                    return true;
                }
                BStackEl& new_top = bstack.back();
                if (!new_top.updateEq())
                {
                    if (!back())
                    {
                        return false;
                    }
                }
            }
            else
            {
                if (!back())
                {
                    return false;
                }
                continue;
            }
        }
        else if (top.lhs->label[0] == '`')
        {
            auto found_arg = args.find(top.lhs);
            if (found_arg == args.end())
            {
                auto new_arg = args.emplace(top.lhs, Arg());
                new_arg.first->second.node_id = bstack.size();
                new_arg.first->second.term = top.rhs;
                if (!next())
                {
                    return true;
                }
                BStackEl& new_top = bstack.back();
                if (!new_top.updateEq())
                {
                    if (!back())
                    {
                        return false;
                    }
                }
            }
            else if (find(found_arg->second.term) != find(top.rhs))
            {
                if (!back())
                {
                    return false;
                }
            }
            else
            {
                if (!next())
                {
                    return true;
                }
                BStackEl& new_top = bstack.back();
                if (!new_top.updateEq())
                {
                    if (!back())
                    {
                        return false;
                    }
                }
            }
        }
        else
        {
            if (!in())
            {
                return true;
            }
            BStackEl& new_top = bstack.back();
            if (!new_top.updateEq())
            {
                if (!back())
                {
                    return false;
                }
            }

        }

    }

}

// false means we finished comparisson
bool Matcher::next()
{
    auto& top = bstack.back();
    int parent_i = top.parent_i;
    while (parent_i >= 0)
    {
        auto& par_el = bstack[parent_i];
        Term* lhs = nullptr;
        Term* rhs = nullptr;
        if (par_el.getChild(lhs, rhs))
        {
            ++par_el.child_i;
            BStackEl new_el;
            new_el.lhs = lhs;
            new_el.rhs_main = find(rhs);
            new_el.parent_i = parent_i;
            bstack.push_back(new_el);
            return true;
        }
        else
        {
            parent_i = par_el.parent_i;
        }
    }
    return false;
}
bool Matcher::back()
{
    for (int i = bstack.size() - 1; i >= 0; --i)
    {
        auto& top = bstack.back();
        if (top.updateEq())
        {
            // clear args
            for (auto it = args.begin(); it != args.end();)
            {
                if (it->second.node_id > i)
                {
                    it = args.erase(it); // returns next valid iterator
                }
                else
                {
                    ++it;
                }
            }
            return true;
        }
        if (top.parent_i >= 0)
        {
            auto& par = bstack[top.parent_i];
            --par.child_i;
        }
        bstack.pop_back();
    }
    return false;
}
bool Matcher::in()
{
    auto& top = bstack.back();
    Term* lhs = nullptr;
    Term* rhs = nullptr;

    if (!top.getChild(lhs, rhs))
    {
        return next();
    }
    ++top.child_i;
    BStackEl new_el;
    new_el.lhs = lhs;
    new_el.rhs_main = find(rhs);
    new_el.parent_i = bstack.size() - 1;
    bstack.push_back(new_el);
    return true;
}
void Trs::func(std::vector<Identity>& identities, const std::wstring& lhs, const std::wstring& rhs)
{
    ts.bin.clear();
    ts.terms_map.clear();

    Term* t_lhs = nullptr;
    Term* t_rhs = nullptr;

    {
        Parser pr(lhs);
        pr.parse();
        compact(pr.m_current_term, 0, ts.terms_map);
        t_lhs = ts.terms_map.find(lhs)->second.get();
        markPatternNodes(t_lhs);
        setupOrder(t_lhs);
    }

    {
        Parser pr(rhs);
        pr.parse();
        compact(pr.m_current_term, 0, ts.terms_map);
        t_rhs = ts.terms_map.find(rhs)->second.get();
    }

    for (auto& id : identities)
    {
        {
            Parser pr(id.lhs);
            pr.parse();
            compact(pr.m_current_term, 0, ts.terms_map);
            id.t_lhs = ts.terms_map.find(id.lhs)->second.get();
            markPatternNodes(id.t_lhs);
            setupOrder(id.t_lhs);
        }

        {
            Parser pr(id.rhs);
            pr.parse();
            compact(pr.m_current_term, 0, ts.terms_map);
            id.t_rhs = ts.terms_map.find(id.rhs)->second.get();
            markPatternNodes(id.t_rhs);
            setupOrder(id.t_rhs);
        }
    }


    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 30; ++i)
    {
        std::vector<Identity> new_ids;
        for (auto& id : identities)
        {
            for (auto& t : ts.terms_map)
            {
                if (t.second->pat)
                {
                    continue;
                }
                if (t.second->e_rep != t.second.get())
                {
                    continue;
                }
                Matcher mc;
                while (mc.match(id.t_lhs, t.second.get()))
                {
                    std::wstring str;
                    rewrite(id.t_rhs, mc.args, str);
                    auto& new_id = new_ids.emplace_back();
                    new_id.t_lhs = t.second.get();
                    new_id.rhs = std::move(str);
                }
            }
        }
        for (auto& new_id : new_ids)
        {
            if (new_id.t_lhs->term_str == new_id.rhs)
            {
                continue;
            }
            auto found = ts.terms_map.find(new_id.rhs);
            if (found != ts.terms_map.end())
            {
                new_id.t_rhs = found->second.get();
                //if rhs already exists
                //if it congruent to lhs, then they are equal already
                //since system is closed under congruence
            }
            else
            {
                Parser pr(new_id.rhs);
                pr.parse();
                compact(pr.m_current_term, 0, ts.terms_map, false);
                new_id.t_rhs = ts.terms_map.find(new_id.rhs)->second.get();
                //if new term
                //update congruence
                //if still different with lhs -> merge
                updateCongruence(new_id.t_rhs, ts);
            }
            if (find(new_id.t_lhs) == find(new_id.t_rhs))
            {
                continue;
            }
            merge(new_id.t_lhs, new_id.t_rhs, ts);
            ts.bin.clear();
        }
        std::cout << ts.terms_map.size() << '\n';
        Matcher mc;
        if (mc.match(t_lhs, t_rhs))
        {
            return;
        }
    }
}
}