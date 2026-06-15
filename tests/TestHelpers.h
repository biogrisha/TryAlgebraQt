#pragma once
#include <Me/include/MeParser.h>
#include <Me/include/MeContainer.h>
#include <Helpers/include/MeHelpers.h>
#include <TRS/Trs.h>

using namespace TryAlgebraCore;
namespace TestHelpers
{
    inline std::unique_ptr<MeContainer> parse(const std::wstring& str)
    {
        TextBuffer tb;
        tb.insert(str, 0);
        MeParser pr(tb, 0);
        std::unique_ptr<MeContainer> cont = MyRTTI::MakeTypedUnique<MeContainer>();
        while (pr.parseLine(cont.get()));
        return cont;
    }
    inline MeHelpers::GetByPathRes getByTextPos(MeBase* cont, const uint32_t pos)
    {
        auto path = MeHelpers::textPosToMePath(cont, pos);
        if (path.has_value())
        {
            return MeHelpers::getByPath(cont, path.value());
        }
        return {};
    }
    inline bool compareTerms(const std::vector<Trs::Term*>& lhs, const std::vector<Trs::Term*>& rhs)
    {
        if (lhs.size() != rhs.size())
        {
            return false;
        }
        for (int i = 0; i < lhs.size(); ++i)
        {
            if (lhs[i]->label != rhs[i]->label)
            {
                return false;
            }
            if (!compareTerms(lhs[i]->children, rhs[i]->children))
            {
                return false;
            }
        }
        return true;
    }
    inline Trs::Term* makeTerm(const std::wstring& label, std::vector<Trs::Term*>& children, Trs::Term* parent = nullptr)
    {
        auto term = new Trs::Term;
        term->label = label;
        if (parent)
        {
            term->parents.insert(parent);
        }
        children.push_back(term);
        return term;
    }
}
