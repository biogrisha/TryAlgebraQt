// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

class FTAVisual
{
public:
	virtual ~FTAVisual() = default;
	virtual void Show();
	virtual void Hide();
	bool IsShown() const {return bShown;};
private:
	bool bShown = false;
};
