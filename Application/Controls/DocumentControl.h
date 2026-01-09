#pragma once
#include <qqml.h>
#include <QObject>
#include <QKeyEvent>
#include <QElapsedTimer>
#include "MathDocumentState.h"
#include "FreeTypeWrap.h"
#include "FunctionLibraries/FileHelpers.h"
#include "../MathDocument.h"
#include "../Models/MathElementInfoModel.h"

class FTAMathDocumentInfo;

class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DocumentControl(QObject *parent);
	DocumentControl() = default;

public slots:
	void bindMathDocumentItem(MathDocument* mathDocument);
	void keyInput(int key, const QString& text, int modifiers);
	void mathDocumentReady();
	MathElementInfoModel* getMeInfoModel();
	void addMeByName(const QString& meName);
	void onCurrentDocumentChanged(qint32 ind);
private:
	void UpdateElements(bool bRect, bool bText, bool bCaret);
	MathDocument* m_mathDocument = nullptr;
	std::weak_ptr<FTAMathDocumentInfo> m_docInfo;
	MathElementInfoModel* m_meInfoModel = nullptr;
	FMathDocumentState m_meDocState;
	bool isMathDocumentReady = false;
	QElapsedTimer m_timer;
};

