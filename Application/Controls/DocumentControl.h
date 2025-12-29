#pragma once
#include "FreeTypeWrap.h"
#include "FunctionLibraries/FileHelpers.h"
#include <QObject>
#include "../MathDocument.h"
class FTAMathDocumentInfo;

class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DocumentControl(QObject *parent);
	DocumentControl() = default;
	~DocumentControl();
	void setDocInfo(const std::weak_ptr<FTAMathDocumentInfo>& docInfo);

public slots:
	void bindMathDocumentItem(MathDocument* mathDocument);

private:
	MathDocument* m_mathDocument = nullptr;
	std::weak_ptr<FTAMathDocumentInfo> m_docInfo;
};

