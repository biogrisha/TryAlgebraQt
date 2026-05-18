#pragma once
#include <vector>
#include <string>
#include <optional>
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
        const wchar_t& next();
        void back();
        const wchar_t& current();
        int getChId() { return m_ch_id; }
    private:
        const std::wstring& m_buffer;
        int m_ch_id;
    };
}