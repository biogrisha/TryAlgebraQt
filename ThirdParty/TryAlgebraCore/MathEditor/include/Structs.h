#pragma once

namespace TryAlgebraCore{
	struct AbsPathEl
	{
		size_t from = 0;
		size_t to = 0;
	};

	class AbsPath : public std::vector<AbsPathEl>
	{
	};
}