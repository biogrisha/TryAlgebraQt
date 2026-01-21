#include "MeVisualBase.h"

MeVisualBase::MeVisualBase(FMathDocumentState* meDocState)
{
	m_meDocState = meDocState;
}

void MeVisualBase::Show(MathElementV2::FTAMathElementBase* me)
{
	auto pos = me->GetAbsolutePosition();
	auto size = me->GetAbsoluteSize();
	if (m_meDocState->IsRectsUpdated())
	{
		//Highlighting was updated
		if (me->bSelected)
		{
			//Me is selected->add highlight rect
			m_meDocState->AddRect(FRectInst({ pos.x, pos.y }, { size.x, size.y }, { 0,0,0.8,1 }));
		}
	}
	FTAVisual::Show(me);
}
