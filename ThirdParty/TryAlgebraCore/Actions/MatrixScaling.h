#include <string>
#include <Actions/SimpleTree.h>
#include <unordered_map>
#include <optional>

namespace TryAlgebraCore
{
	class MatrixScaling
	{
	public:
		int rows() const;
		int columns() const;
		std::wstring scale(int rows, int columns) const;
		static std::optional<MatrixScaling> tryCreate(const std::wstring& str);
	private:
		std::unordered_map<std::string, std::unique_ptr<SimpleMe>> cells;
		int m_rows = 0;
		int m_columns = 0;
	};
}
