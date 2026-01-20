#pragma once
#include <qqml.h>
#include <QObject>
#include <QKeyEvent>
#include <QElapsedTimer>

#include <MathDocumentState.h>
#include <FreeTypeWrap.h>
#include <FunctionLibraries/FileHelpers.h>
#include <Modules/CommonTypes/MulticastDelegate.h>
#include <Modules/MathElementsV2/Me/MeDocument.h>

#include <MathDocument.h>
#include <Models/MathElementInfoModel.h>

class FTAMathDocumentInfo;

/*
* Control used to interact with selected math document
*/
class DocumentControl  : public QObject
{
	Q_OBJECT
	QML_ELEMENT
	Q_PROPERTY(float scrollHandleSize READ scrollHandleSize WRITE setScrollHandleSize NOTIFY scrollHandleSizeChanged)
public:
	DocumentControl(QObject *parent);
	DocumentControl() = default;

	float scrollHandleSize();
	float scrollHandlePos();
	void setScrollHandleSize(float newSize);
	void setScrollHandlePos(float newPos);
signals:
	void scrollHandleSizeChanged(float newSize);
	void scrollHandlePosChanged(float newPos);

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
private:
	//Updates the rendering data of the selected elements
	void updateElements(bool bRect, bool bText, bool bCaret);

	//Forces rendering to clear items texture
	void clearDocument();

	void onLinesCountUpdated(MathElementV2::FTAMeDocument* doc);
	
	void onCurrentLineUpdated(MathElementV2::FTAMeDocument* doc);

	//Math document used to render content
	MathDocument* m_mathDocument = nullptr;
	//Selected document info
	std::weak_ptr<FTAMathDocumentInfo> m_docInfo;
	//Model for math elements selector
	MathElementInfoModel* m_meInfoModel = nullptr;
	//Render state of math document
	FMathDocumentState m_meDocState;
	//Indicates that MathDocument Item is ready to render
	bool m_isMathDocumentReady = false;

	float m_scrollHandleSize = 0.5;
	float m_scrollHandlePos = 0.f;
	bool m_updateScrollFromHandle = true;
	bool m_updateScrollFromDoc = true;

	FTAMulticastDelegate<MathElementV2::FTAMeDocument*>::HndlPtr m_onLinesCountUpdated;
	FTAMulticastDelegate<MathElementV2::FTAMeDocument*>::HndlPtr m_onCurrentLineUpdated;
};

