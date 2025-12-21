// Fill out your copyright notice in the Description page of Project Settings.

#pragma once
#include <map>
#include <string>
#include <vector>

class FTACommandBindings
{
public:
	void UpdateBindings();
	void AddToCommand(const std::wstring& Key);
	
	/**
	 * 
	 * @param SelectedMeData 
	 * @param OutSubPath Relative cursor path, pointing to Cursor placement 
	 * @return Resulting Me data
	 */
	std::wstring ApplyCommand(const std::wstring& SelectedMeData, std::vector<int>& OutSubPath);
private:
	std::map<std::wstring, std::wstring> Bindings;
	std::wstring CurrentCommand;
};
