#pragma once
#include "Modules/CommonTypes/Vector2D.h"
#include "Modules/MathElementsV2/Me/MathElementBaseV2.h"
class FTACursorVisualBase
{
public:
	virtual ~FTACursorVisualBase() = default;
	virtual void Show();
	virtual void Hide();
	bool IsShown() const;
private:
	bool bVisible = true;
};
