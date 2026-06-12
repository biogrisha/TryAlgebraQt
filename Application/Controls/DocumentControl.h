#pragma once
#include <qqml.h>
#include <QObject>
#include <QKeyEvent>
#include <QElapsedTimer>

#include <MathDocumentState.h>
#include <FreeTypeWrap.h>

#include <MathDocumentCanvas.h>
#include <Models/MeListModel.h>
#include <Models/DocumentsModel.h>

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
	void bindMathDocumentItem(MathDocumentCanvas* mathDocument);

	//Handles key input
	void keyInput(int key, QString text, int modifiers);

	//Handles signal when math document is ready to render
	void canvasReady();

	//Adds math element from its string form
	void addMeByName(const QString& meName);

	//Called when new math document is selected
	void onCurrentDocChanged(const QString& docPath);

	void onBeforeDocRemoved(DocumentInfo* docInfo);

	//Handles item size update
	void onResized(const QSize& new_size);
	
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
	MathDocumentCanvas* m_docCanvas = nullptr;
	//Model for math elements selector
	MeListModel* m_meInfoModel = nullptr;
	//Render state of math document
	FMathDocumentState m_visual_state;
	//Indicates that MathDocument Item is ready to render
	bool m_isCanvasReady = false;
	bool m_bLmbDown = false;

	TryAlgebraCore::MathDocument* m_currDoc;
};

