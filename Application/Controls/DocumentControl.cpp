#include "DocumentControl.h"
#include "FunctionLibraries/FileHelpers.h"
DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{}

DocumentControl::~DocumentControl()
{}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	m_mathDocument = mathDocument;
	m_mathDocument->setText("helloworld");
}

void DocumentControl::setDocInfo(FTAMathDocumentInfo* docInfo)
{
	//m_docInfo = docInfo;
}

