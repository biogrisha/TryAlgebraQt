#include <MathEditor/include/DocumentHistory.h>

namespace TryAlgebraCore
{
	void DocumentHistory::recordDeletion(const MePath& pos, const MePath& invPos, std::wstring text)
	{
		std::vector<InvAction>* buffer = (m_undoMode ? &m_undoHistory : &m_redoHistory);
		buffer->push_back(InvertDelete{ pos, invPos, std::move(text) });
	}
	void DocumentHistory::recordInsertion(const MePath& pos, const MePath& invPos, int size)
	{
		std::vector<InvAction>* buffer = (m_undoMode ? &m_undoHistory : &m_redoHistory);
		buffer->push_back(InvertInsert{ pos, invPos, size });
	}
	void DocumentHistory::recordReplace(const MePath& pos, const MePath& invPos, int insertSize, std::wstring deleteString)
	{
		std::vector<InvAction>* buffer = (m_undoMode ? &m_undoHistory : &m_redoHistory);
		buffer->push_back(InvertReplace{ pos, invPos, insertSize, deleteString });
	}

	void DocumentHistory::setUndoMode(bool val)
	{
		m_undoMode = val;
	}
	std::optional<InvAction> DocumentHistory::pop(bool undo)
	{
		std::vector<InvAction>* buffer = undo ? &m_undoHistory : &m_redoHistory;
		if (buffer->empty())
		{
			return std::nullopt;
		}
		InvAction res = std::move(buffer->back());
		buffer->pop_back();
		return res;
	}

	void DocumentHistory::clearRedo()
	{
		m_redoHistory.clear();
	}
}