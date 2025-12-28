#pragma once

#include <QObject>
#include "MathDocument.h"
class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DocumentControl(QObject *parent);
	DocumentControl();
	~DocumentControl();

public slots:
	void bindMathDocumentItem(MathDocument* mathDocument);

private:
	MathDocument* m_mathDocument = nullptr;
};

