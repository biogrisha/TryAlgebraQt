// Fill out your copyright notice in the Description page of Project Settings.


#include "FunctionLibraries/CommonHelpers.h"

#include <sstream>
#include <string>


void CommonHelpers::RemoveSubstr(std::wstring& Str, const std::wstring& SubStr)
{ 
	std::wstring::size_type n = SubStr.length();
	for (std::wstring::size_type i = Str.find(SubStr);
	     i != std::wstring::npos;
	     i = Str.find(SubStr))
		Str.erase(i, n);
}

std::vector<std::wstring> CommonHelpers::SplitStr(const std::wstring& Str, const wchar_t& Delimiter)
{
	std::vector<std::wstring> Result;
	std::wstring temp;
	std::wstringstream wss(Str);
	while(std::getline(wss, temp, Delimiter))
	{
		Result.push_back(temp);
	}
	return Result;
}

void CommonHelpers::ToLower(std::wstring& Str)
{
	for (wchar_t& Ch : Str)
	{
		Ch = static_cast<wchar_t>(std::tolower(Ch));
	}
}
