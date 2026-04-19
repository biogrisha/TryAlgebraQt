#include <MathEditor/include/TextBuffer.h>

namespace TryAlgebraCore2
{
    TextBuffer::TextBuffer()
    {
        auto& piece = pieces.emplace_back();
        piece.push_back(Character(L' '));
    }

    std::vector<Character>* TextBuffer::findPiece(const std::vector<int>& path)
    {
        std::vector<Character>* piece = &pieces[0];
        // find piece by path
        for (int i = 0; i < path.size() - 1; ++i)
        {
            auto& ch = piece->at(path[i]);
            piece = &pieces[ch.i];
        }
        return piece;
    }

    const std::vector<Character>* TextBuffer::findPiece(const std::vector<int>& path) const
    {
        const std::vector<Character>* piece = &pieces[0];
        // find piece by path
        for (int i = 0; i < path.size() - 1; ++i)
        {
            auto& ch = piece->at(path[i]);
            piece = &pieces[ch.i];
        }
        return piece;
    }

    void TextBuffer::insert(const std::wstring& str, std::vector<int>& path)
    {
        std::vector<Character>* piece = findPiece(path);

        if (path.back() == piece->size())
        {
            // last index in path points at the end of piece
            for (auto ch : str)
            {
                // appending character to piece
                piece->push_back(Character(ch));
            }
            // update path to point to the next index
            path.back() = piece->size();
        }
        else if (path.back() - 1 > 0 && piece->at(path.back() - 1).type == Type::Piece)
        {
            // previous element is piece
            path.back()--;
            int id = piece->at(path.back()).i;
            piece = &pieces[id];

            for (auto ch : str)
            {
                // appending character to piece
                piece->push_back(Character(ch));
            }
            // update path to point to the last index
            path.back() = piece->size();
        }
        else
        {
            path.back()--;
            Character ch_copy = piece->at(path.back());
            Character& ch_insert = piece->at(path.back());
            ch_insert.type = Type::Piece;
            ch_insert.i = pieces.size();
            auto& piece = pieces.emplace_back();
            piece.resize(str.size() + 1);

            // put element where we insert at the start so that consequent insertions happen at the end of array
            // so that when user typing characters end up at the end of piece
            piece[0] = ch_copy;
            int i = 1;
            for (auto ch : str)
            {
                piece[i] = Character(ch);
                ++i;
            }
            path.push_back(piece.size());
        }
    }

    std::vector<int> TextBuffer::prev(std::vector<int> pos)
    {
        while (pos.size() > 0)
        {
            if (pos.back() > 0)
            {
                // if there is a space to go back
                // decr id
                pos.back()--;
                // find the piece at which path points
                auto piece = findPiece(pos);
                Character& ch = piece->at(pos.back());
                if (ch.type != Type::Piece)
                {
                    // if character at new position is not Piece -> return it
                    return pos;
                }
                // if char at new pos is piece -> add last index of piece into the path
                piece = &pieces[ch.i];
                pos.push_back(piece->size() - 1);
                return pos;
            }
            // if there is no characters before this id -> go a level down
            pos.pop_back();
        }
        return pos;
    }

    std::vector<int> TextBuffer::next(std::vector<int> pos)
    {
        next(findPiece(pos), pos);
        return pos;
    }

    void TextBuffer::next(const std::vector<Character>* curr_piece, std::vector<int>& pos) const
    {
        const std::vector<Character>* piece = curr_piece;
        pos.back()++;
        if (pos.back() == piece->size())
        {
            // next == piece.size
            pos.pop_back();
            if (!pos.empty())
            {
                // go down->step forward
                pos.back()++;
            }
            return;
        }

        // next != piece.size
        // get next ch
        Character ch = piece->at(pos.back());
        while (ch.type == Type::Piece)
        {
            // if ch is piece -> go up
            ch = pieces[ch.i].front();
            pos.push_back(0);
        }
    }

    Character* TextBuffer::getCharacter(const std::vector<int>& pos)
    {
        if (pos.empty())
        {
            return nullptr;
        }
        return &findPiece(pos)->at(pos.back());
    }

    void TextBuffer::erase(const std::vector<int>& start, const std::vector<int>& end)
    {
        auto start_ch = getCharacter(start);
        auto end_ch = getCharacter(end);
        auto prev_path = prev(start);
        auto next_path = next(end);
        auto prev_start = getCharacter(prev_path);
        auto next_end = getCharacter(next_path);

        bool is_prev_start = prev_start && prev_start->type == Type::Deletion;
        bool is_next_end = next_end && next_end->type == Type::Deletion;

        if (is_prev_start && is_next_end)
        {
            deletions[prev_start->i].pair_deletion = next_path;
            deletions[next_end->i].pair_deletion = prev_path;
            return;
        }

        if (is_prev_start)
        {
            auto& end_del = deletions.emplace_back();
            end_del.pair_deletion = prev_path;
            deletions[prev_start->i].pair_deletion = end;
            return;
        }

        if (is_next_end)
        {
            auto& start_del = deletions.emplace_back();
            start_del.pair_deletion = next_path;
            deletions[next_end->i].pair_deletion = start;
            return;
        }

        auto& end_del = deletions.emplace_back();
        end_del.pair_deletion = prev_path;
        auto& start_del = deletions.emplace_back();
        start_del.pair_deletion = next_path;
    }

    void TextBuffer::displayBuffer()
    {
        displayBuffer(pieces.front());
    }

    void TextBuffer::displayBuffer(const std::vector<Character>& buf, int depth)
    {
        //std::wstring indent(depth * 2, L' ');

        //for (size_t idx = 0; idx < buf.size(); ++idx)
        //{
        //    const auto& ch = buf[idx];

        //    if (ch.type == Type::Character)
        //    {
        //        std::wcout << indent << L"[" << idx << L"] '" << ch.ch << L"'\n";
        //    }
        //    else if (ch.type == Type::Piece) // assuming you renamed Piece → Insertion
        //    {
        //        std::wcout << indent << L"[" << idx << L"] -> insertion(" << ch.i << L")\n";

        //        if (ch.i >= 0 && ch.i < pieces.size())
        //        {
        //            displayBuffer(pieces[ch.i], depth + 1);
        //        }
        //        else
        //        {
        //            std::wcout << indent << L"  (invalid reference!)\n";
        //        }
        //    }
        //}
    }

    TextBufferIterator::TextBufferIterator(const TextBuffer& text_buffer)
        :m_text_buffer(text_buffer)
    {

    }

    void TextBufferIterator::setPos(const std::vector<int>& pos)
    {
        m_pos = pos;
        m_current_piece = m_text_buffer.findPiece(m_pos);
    }

    bool TextBufferIterator::isEnd()
    {
        return m_pos.empty();
    }

    const Character& TextBufferIterator::next()
    {
        const Character& ch = m_current_piece->at(m_pos.back());
        m_text_buffer.next(m_current_piece, m_pos);
        return ch;
    }
}
