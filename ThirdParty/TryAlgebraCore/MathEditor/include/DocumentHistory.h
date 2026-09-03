#pragma once
#include <string>
#include <variant>
#include <vector>
#include <MathEditor/include/Structs.h>
#include <optional>

namespace TryAlgebraCore
{
	struct InvertDelete
	{
		MePath pos;
		std::wstring string;
	};

	struct InvertInsert
	{
		MePath pos;
		int size = 0;
	};

	struct InvertReplace
	{
		MePath pos;
		int insertSize = 0;
		std::wstring deletedStr;
	};

	using InvAction = std::variant<InvertDelete, InvertInsert, InvertReplace>;
	class DocumentHistory
	{
	public:
		void recordDeletion(const MePath& pos, std::wstring text);
		void recordInsertion(const MePath& pos, int size);
		void recordReplace(const MePath& pos, int insertSize, std::wstring deleteString);
		void setUndoMode(bool val);
		std::optional<InvAction> pop(bool undo);
		void clearRedo();
	private:
		bool m_undoMode = true;
		std::vector<InvAction> m_undoHistory;
		std::vector<InvAction> m_redoHistory;
	};

}