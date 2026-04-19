#pragma once
#include <qqml.h>
#include <QObject>
#include <QKeyEvent>
#include <QElapsedTimer>

#include <MathDocumentState.h>
#include <FreeTypeWrap.h>
#include <FunctionLibraries/FileHelpers.h>

#include <MathDocument.h>
#include <Models/MathElementInfoModel.h>

#include <MathEditor/include/MathDocument.h>

class FTAMathDocumentInfo;

/*
* Control used to interact with selected math document
*/
class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public:
	DocumentControl(QObject *parent);
	DocumentControl() = default;

public slots:

	//Sets MathDocument Item being used for layout rendering
	void bindMathDocumentItem(MathDocument* mathDocument);

	//Handles key input
	void keyInput(int key, const QString& text, int modifiers);

	//Handles signal when math document is ready to render
	void mathDocumentReady();

	//Returns me info model
	//Note that the model is owned by DocumentControl
	MathElementInfoModel* getMeInfoModel();

	//Adds math element from its string form
	void addMeByName(const QString& meName);

	//Called when new math document is selected
	void onCurrentDocumentChanged(qint32 ind);

	//Handles item size update
	void onResized(const QSize& newSize);
	
	float getScrollHandleSize();

	void scrollY(bool Up);

	void moveScrollHandle(float newPos);

	void mouseBtnDown(float x, float y);

	void mouseBtnUp(float x, float y);

	void mousePosUpdated(float x, float y);
private:
	//Updates the rendering data of the selected elements
	void updateElements(bool bRect, bool bText, bool bCaret);

	//Forces rendering to clear items texture
	void clearDocument();

	//Math document used to render content
	MathDocument* m_mathDocument = nullptr;
	//Model for math elements selector
	MathElementInfoModel* m_meInfoModel = nullptr;
	//Render state of math document
	FMathDocumentState m_meDocState;
	//Indicates that MathDocument Item is ready to render
	bool m_isMathDocumentReady = false;
	bool m_bLmbDown = false;

	std::unique_ptr<TryAlgebraCore2::MathDocument> m_math_doc;
};

