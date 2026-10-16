#include "MatrixScaling.h"
#include <Me/include/MeGlobals.h>

namespace TryAlgebraCore {

	int MatrixScaling::rows() const
	{
		return m_rows;
	}

	int MatrixScaling::columns() const
	{
		return m_columns;
	}

	std::wstring MatrixScaling::scale(int rows, int columns) const
	{
		if (rows <= 0 || columns <= 0)
		{
			return {};
		}

		std::wstring result = L"\\" + MeNames::matrix + L"\\"
			+ std::to_wstring(rows) + L"," + std::to_wstring(columns) + L"\\{";
		for (int row = 0; row < rows; ++row)
		{
			for (int column = 0; column < columns; ++column)
			{
				const auto cell = cells.find(std::to_string(row) + "," + std::to_string(column));
				if (cell != cells.end())
				{
					result += SimpleMe::toStr(cell->second->children);
				}
				if (row != rows - 1 || column != columns - 1)
				{
					result += L"\\,";
				}
			}
		}
		result += L"\\}";
		return result;
	}

	std::optional<MatrixScaling> MatrixScaling::tryCreate(const std::wstring& str)
	{
		auto meList = SimpleMe::fromStr(str);
		if (meList.size() != 1 || meList.back()->label != MeNames::matrix)
		{
			return std::nullopt;
		}
		auto matSize = MeNames::matrixSize(meList.back()->meta);
		auto& cells = meList.back()->children;
		MatrixScaling res;
		int i = 0;
		for (int row = 0; row < matSize.rows; ++row)
		{
			for (int cl = 0; cl < matSize.columns; ++cl)
			{
				res.cells.emplace(std::to_string(row) + "," + std::to_string(cl), std::move(cells[i]));
				++i;
			}
		}
		res.m_rows = matSize.rows;
		res.m_columns = matSize.columns;
		return res;
	}
}
