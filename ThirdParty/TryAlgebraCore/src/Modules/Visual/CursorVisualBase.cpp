#include "Modules/Visual/CursorVisualBase.h"

void FTACursorVisualBase::Show()
{
	bVisible = true;
}

void FTACursorVisualBase::Hide()
{
	bVisible = false;
}

bool FTACursorVisualBase::IsShown() const
{
	return bVisible;
}
