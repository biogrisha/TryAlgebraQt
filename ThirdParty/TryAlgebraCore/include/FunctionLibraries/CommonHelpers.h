// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <memory>
#include <string>
#include <vector>

namespace CommonHelpers
{
	inline int InvalidIndex = -1;

	template <typename VecType, typename T>
	int Find(const std::vector<VecType>& Vec, const T& Element)
	{
		for (int i = 0; i < Vec.size(); i++)
		{
			if (Vec[i] == Element)
			{
				return i;
			}
		}
		return InvalidIndex;
	}

	template <typename VecType>
	void RemoveSwap(std::vector<VecType>& vec, size_t index)
	{
		if (index < vec.size())
		{
			vec[index] = vec.back(); // Swap with last element
			vec.pop_back(); // Remove last element
		}
	}

	template <typename VecType>
	bool IsValidId(const std::vector<VecType>& vec, int index)
	{
		if (index >= 0 && index < vec.size())
		{
			return true;
		}
		return false;
	}

	template <typename VecType, typename Pred>
	VecType* FindByPredicate(std::vector<VecType>& vec, const Pred& pred)
	{
		for (auto& El : vec)
		{
			if (pred(El))
			{
				return &El;
			}
		}
		return nullptr;
	}

	void RemoveSubstr(std::wstring& Str, const std::wstring& SubStr);

	std::vector<std::wstring> SplitStr(const std::wstring& Str, const wchar_t& Delimiter);

	void ToLower(std::wstring& Str);

	template <typename T, typename... Args>
	void MakeUnique(std::unique_ptr<T>& UniquePtr, Args&&... args)
	{
		UniquePtr = std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	void MakeShared(std::shared_ptr<T>& SharedPtr, Args&&... args)
	{
		SharedPtr = std::make_shared<T>(std::forward<Args>(args)...);
	}

	template <typename T, typename... Args>
	bool IsIn(T value, Args... args) {
		return ((value == args) || ...);
	}
}
