#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore2
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

    int TextBuffer::getSize()
    {
        return m_buffer.size();
    }

    TextBufferIterator::TextBufferIterator(const TextBuffer& text_buffer, int line_num)
        : m_buffer(text_buffer.getBuff())
        , m_from_ind(0)
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
            ++m_from_ind;
        }
    }

    bool TextBufferIterator::isEnd()
    {
        return m_from_ind == m_buffer.size();
    }

    bool TextBufferIterator::isNewLine()
    {
        return m_buffer[m_from_ind] == '\r';
    }

    const wchar_t& TextBufferIterator::next()
    {
        const wchar_t& ch = m_buffer[m_from_ind] ;
        ++m_from_ind;
        return ch;
    }
}
