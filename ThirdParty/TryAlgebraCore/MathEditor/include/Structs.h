#pragma once
#include <ostream>
#include <variant>

namespace TryAlgebraCore{
	struct MePos
	{
		size_t from = 0;
		size_t to = 0;
        bool operator==(const MePos&) const = default;
	};

    struct ContPos
    {
        size_t from = 0;
        bool operator==(const ContPos&) const = default;
    };

    struct LeafPos
    {
        size_t pos = 0;
        bool operator==(const LeafPos&) const = default;
    };


    class MePath : public std::vector<std::variant<MePos, ContPos, LeafPos>>
    {
    public:
        using Base = std::vector<std::variant<MePos, ContPos, LeafPos>>;
        using Base::Base;
        using Base::operator=;
        
        bool operator==(const MePath& other) const
        {
            return static_cast<const Base&>(*this) == static_cast<const Base&>(other);
        }

        bool operator!=(const MePath& other) const
        {
            return !(*this == other);
        }
    };

    inline size_t getPosOrFrom(const std::variant<MePos, ContPos, LeafPos>& v)
    {
        return std::visit([](const auto& x) -> size_t
            {
                using T = std::decay_t<decltype(x)>;

                if constexpr (std::is_same_v<T, LeafPos>)
                {
                    return x.pos;
                }
                else
                {
                    return x.from;
                }
            }, v);
    }


    inline std::ostream& operator<<(std::ostream& os, const MePos& p)
    {
        return os << "MePos{from=" << p.from << ", to=" << p.to << "}";
    }

    inline std::ostream& operator<<(std::ostream& os, const ContPos& p)
    {
        return os << "ContPos{from=" << p.from << "}";
    }

    inline std::ostream& operator<<(std::ostream& os, const LeafPos& p)
    {
        return os << "LeafPos{pos=" << p.pos << "}";
    }

    inline std::ostream& operator<<(std::ostream& os, const MePath& path)
    {
        os << "[";

        bool first = true;

        for (const auto& item : path)
        {
            if (!first)
                os << ", ";

            std::visit([&](const auto& v)
                {
                    os << v;
                }, item);

            first = false;
        }

        os << "]";
        return os;
    }
}