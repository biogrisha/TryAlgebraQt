#pragma once
#include <ostream>
#include <variant>

namespace TryAlgebraCore{
	struct MePos
	{
		uint64_t from = 0;
		uint64_t to = 0;
        bool operator==(const MePos&) const = default;
	};

    struct ContPos
    {
        uint64_t pos = 0;
        bool operator==(const ContPos&) const = default;
    };

    struct LeafPos
    {
        uint64_t pos = 0;
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

    inline std::ostream& operator<<(std::ostream& os, const MePos& p)
    {
        return os << "MePos{from=" << p.from << ", to=" << p.to << "}";
    }

    inline std::ostream& operator<<(std::ostream& os, const ContPos& p)
    {
        return os << "ContPos{from=" << p.pos << "}";
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