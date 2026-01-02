#pragma once
#include "FreeTypeWrap.h"
#include "FunctionLibraries/FileHelpers.h"
#include <QObject>
#include <QKeyEvent>
#include <qqml.h>
#include <../MathDocument.h>

class FTAMathDocumentInfo;

class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DocumentControl(QObject *parent);
	DocumentControl() = default;
	~DocumentControl();
	void setDocument(const QUrl& filePath);

public slots:
	void bindMathDocumentItem(MathDocument* mathDocument);
	void keyInput(int key, const QString& text, int modifiers);
	void mathDocumentReady();

private:
	MathDocument* m_mathDocument = nullptr;
	std::weak_ptr<FTAMathDocumentInfo> m_docInfo;
	std::vector<FGlyphData> m_glyphs;
	FCaretData m_caretData;
};

