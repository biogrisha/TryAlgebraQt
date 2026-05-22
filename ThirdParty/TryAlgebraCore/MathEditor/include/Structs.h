#pragma once
#include <variant>

namespace TryAlgebraCore{
	struct MePos
	{
		size_t from = 0;
		size_t to = 0;
	};

    struct ContPos
    {
        size_t from = 0;
    };

    struct LeafPos
    {
        size_t pos = 0;
    };


    class MePath : public std::vector<std::variant<MePos, ContPos, LeafPos>>
    {
    public:
        using Base = std::vector<std::variant<MePos, ContPos, LeafPos>>;
        using Base::Base;
        using Base::operator=;
    };
}