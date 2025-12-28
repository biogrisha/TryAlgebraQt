#include "DocumentControl.h"

DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{}

DocumentControl::DocumentControl()
{
}

DocumentControl::~DocumentControl()
{}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	m_mathDocument = mathDocument;
}

