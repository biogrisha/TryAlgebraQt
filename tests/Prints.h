#pragma once
#include <iostream>
#include <vector>
std::ostream& operator<<(std::ostream& os, const std::vector<int>& vec)
{
	os << '[';

	for (std::size_t i = 0; i < vec.size(); ++i)
	{
		if (i)
			os << ", ";

		os << vec[i];
	}

	return os << ']';
}
