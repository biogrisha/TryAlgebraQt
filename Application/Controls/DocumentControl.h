#pragma once
#include "FreeTypeWrap.h"
#include "FunctionLibraries/FileHelpers.h"
#include <QObject>
#include <QKeyEvent>
#include <qqml.h>
#include <../MathDocument.h>
#include <QElapsedTimer>
#include "../Models/MathElementInfoModel.h"
#include "MathDocumentState.h"
class FTAMathDocumentInfo;

class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DocumentControl(QObject *parent);
	DocumentControl() = default;
	void setDocument(const QUrl& filePath);

public slots:
	void bindMathDocumentItem(MathDocument* mathDocument);
	void keyInput(int key, const QString& text, int modifiers);
	void mathDocumentReady();
	MathElementInfoModel* getMeInfoModel();
	void addMeByName(const QString& meName);
private:
	MathDocument* m_mathDocument = nullptr;
	std::weak_ptr<FTAMathDocumentInfo> m_docInfo;
	MathElementInfoModel* m_meInfoModel = nullptr;
	FMathDocumentState m_meDocState;

	QElapsedTimer m_timer;
};

