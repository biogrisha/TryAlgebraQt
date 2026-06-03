#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore
{
    void TextBuffer::insert(const std::wstring& str, int pos)
    {
        m_buffer.insert(m_buffer.begin() + pos, str.begin(), str.end());
    }

    void TextBuffer::del(int from, int to)
    {
        m_buffer.erase(m_buffer.begin() + from, m_buffer.begin() + to);
    }

    const std::wstring& TextBuffer::getBuff() const 
    {
        return m_buffer;
    }

    bool TextBuffer::isEmpty()
    {
        return m_buffer.empty();
    }

    size_t TextBuffer::getSize()
    {
        return m_buffer.size();
    }

    std::optional<uint64_t> TextBuffer::getLineNumber(uint64_t char_num)
    {
        if (char_num > m_buffer.size())
        {
            return std::nullopt;
        }
        if (char_num == m_buffer.size())
        {
            return getLinesCount() - 1;
        }
        uint64_t line_num = 0;
        uint64_t ch_i = 0;
        for (auto ch : m_buffer)
        {
            if (char_num == ch_i)
            {
                return line_num;
            }
            if (ch == L'\n')
            {
                ++line_num;
            }
            ++ch_i;
        }
        return std::nullopt;
    }

    size_t TextBuffer::getLinesCount()
    {
        uint64_t line_num = 1;
        for (auto ch : m_buffer)
        {
            if (ch == L'\n')
            {
                ++line_num;
            }
        }
        return line_num;
    }

    TextBufferIterator::TextBufferIterator(const TextBuffer& text_buffer, int line_num)
        : m_buffer(text_buffer.getBuff())
        , m_ch_id(0)
    {
        for (const wchar_t& ch : m_buffer)
        {
            if (line_num == 0)
            {
                return;
            }
            if (ch == '\n')
            {
                --line_num;
            }
            ++m_ch_id;
        }
    }

    bool TextBufferIterator::isEnd()
    {
        return m_ch_id == m_buffer.size();
    }

    const wchar_t& TextBufferIterator::next()
    {
        const wchar_t& ch = m_buffer[m_ch_id];
        ++m_ch_id;
        return ch;
    }
    std::optional<wchar_t> TextBufferIterator::lookAhead(size_t num)
    {
        if (m_ch_id + num < m_buffer.size())
        {
            return m_buffer[m_ch_id + num];
        }
        return {};
    }

    void TextBufferIterator::back()
    {
        --m_ch_id;
    }

    const wchar_t& TextBufferIterator::current()
    {
        const wchar_t& ch = m_buffer[m_ch_id];
        return ch;
    }
}
