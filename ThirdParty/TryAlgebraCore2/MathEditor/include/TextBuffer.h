#pragma once
#include <vector>
#include <string>

namespace TryAlgebraCore2
{
    enum class Type : char
    {
        Piece,
        Deletion,
        Character
    };

    struct Deletion
    {
        std::vector<int> pair_deletion;
    };

    struct Character
    {
        Type type;
        union
        {
            int16_t i;
            wchar_t ch;
        };

        Character(int16_t value) : type(Type::Piece), i(value) {}
        Character(wchar_t value) : type(Type::Character), ch(value) {}
        Character() = default;
    };

    // assume that path is always correct and does not point at the deleted area
    // assume that insert can't happen at {0}
    // assume that deletion start and end also correct and don't point at deleted elements
    // all these assumptions will be controlled by text editor
    struct TextBuffer
    {
        TextBuffer();

        std::vector<Character>* findPiece(const std::vector<int>& path);

        const std::vector<Character>* findPiece(const std::vector<int>& path) const;

        void insert(const std::wstring& str, std::vector<int>& path);

        std::vector<int> prev(std::vector<int> pos);

        std::vector<int> next(std::vector<int> pos);

        void next(const std::vector<Character>* curr_piece, std::vector<int>& pos) const;

        Character* getCharacter(const std::vector<int>& pos);

        void erase(const std::vector<int>& start, const std::vector<int>& end);

        void displayBuffer();

        void displayBuffer(const std::vector<Character>& buf, int depth = 0);

        std::vector<std::vector<Character>> pieces;
        std::vector<Deletion> deletions;
    };

    class TextBufferIterator
    {
    public:
        TextBufferIterator(const TextBuffer& text_buffer);
        void setPos(const std::vector<int>& pos);
        bool isEnd();
        const Character& next();
    private:
        const TextBuffer& m_text_buffer;
        std::vector<int> m_pos;
        const std::vector<Character>* m_current_piece = nullptr;
    };
}