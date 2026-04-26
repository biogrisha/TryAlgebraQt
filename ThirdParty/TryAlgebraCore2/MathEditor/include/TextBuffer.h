#pragma once
#include <vector>
#include <string>

namespace TryAlgebraCore2
{

    class TextBuffer
    {
    public:
        void insert(const std::wstring& str, int pos);
        void del(int from, int to);
        const std::wstring& getBuff() const;
        bool isEmpty();
        int getSize();
    private:
        std::wstring m_buffer;
    };

    class TextBufferIterator
    {
    public:
        TextBufferIterator(const TextBuffer& text_buffer, int line_num);
        bool isEnd();
        bool isNewLine();
        const wchar_t& next();
    private:
        const std::wstring& m_buffer;
        int m_from_ind;
    };
}