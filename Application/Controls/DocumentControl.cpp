#include "DocumentControl.h"
#include <QUrl>

#include <Modules/MathElementsV2/CompatibilityData.h>
#include <Modules/MathElementsV2/Me/MeDocument.h>
#include <Modules/MainWindowModule.h>
#include <Modules/MathDocument/MathDocument.h>
#include <FunctionLibraries/FileHelpers.h>
#include <FunctionLibraries/MathElementsHelpers.h>

#include <Application.h>
#include <AppGlobal.h>
#include <FilesControl.h>
#include <MathEditor/CursorComponentGenerator.h>
#include <MathEditor/MathElementGenerator.h>


DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{
	m_math_doc = std::make_unique<TryAlgebraCore2::MathDocument>();
	auto filesControl = AppGlobal::application->getFilesControl();
	filesControl->setMeDocStatePtr(&m_meDocState);
	//connect to event when document selected
	connect(filesControl, &FilesControl::onCurrentDocumentChanged, this, &DocumentControl::onCurrentDocumentChanged, Qt::DirectConnection);
}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	//cache math document and wait until it's ready
	m_mathDocument = mathDocument;
	QObject::connect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady, Qt::ConnectionType::DirectConnection);
}

void DocumentControl::keyInput(int key, const QString& text, int modifiers)
{	
	
	bool bShift = modifiers == Qt::Modifier::SHIFT;
	bool bCtrl = modifiers == Qt::Modifier::CTRL;
	switch (key) {
	case Qt::Key_Left:
		//doc->StepX(-1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Right:
		//doc->StepX(1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Up:
		//doc->StepY(-1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Down:
		//doc->StepY(1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Backspace:
		//doc->DeleteBackward();
		updateElements(true, true, true);
		break;
	case Qt::Key_Delete:
		//doc->DeleteForward();
		updateElements(true, true, true);
		break;
	case Qt::Key_Z:
		if (bCtrl)
		{
			//doc->Undo();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_Y:
		if (bCtrl)
		{
			//doc->Redo();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_C:
		if (bCtrl)
		{
			//doc->CopySelected();
			break;
		}
		[[fallthrough]];
	case Qt::Key_X:
		if (bCtrl)
		{
			//doc->CutSelected();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_V:
		if (bCtrl)
		{
			//doc->Paste();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	default:
		if (!text.isEmpty() && text != "\b")
		{
			m_math_doc->type(text.toStdWString());
			m_meDocState.addGlyphs(m_math_doc->getRenderingData(AppGlobal::application->getFreeTypeWrap()));
			updateElements(true, true, true);
		}
		break;
	}
}

void DocumentControl::mathDocumentReady()
{
	//caching doc state ptr in MathDocument
	m_mathDocument->setMeDocState(&m_meDocState);
	//disconnecting from "renderer ready"
	QObject::disconnect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady);
	QObject::connect(m_mathDocument, &MathDocument::onResized, this, &DocumentControl::onResized);
	m_isMathDocumentReady = true;
}

MathElementInfoModel* DocumentControl::getMeInfoModel()
{
	if (m_meInfoModel)
	{
		//model is created->return it
		return m_meInfoModel;
	}
	//model not created yet-> create it
	m_meInfoModel = new MathElementInfoModel(this);
	/*auto& meList = FTAMeHelpers::GetMathElementsList();
	for (auto& me : meList)
	{
		m_meInfoModel->addMathElementInfo(MathElementInfo(QString::fromStdWString(me.first), ""));
	}*/
	return m_meInfoModel;
}

void DocumentControl::addMeByName(const QString& meName)
{
	
}

void DocumentControl::onCurrentDocumentChanged(qint32 ind)
{
	
}

void DocumentControl::onResized(const QSize& newSize)
{
	//m_docInfo.lock()->MathDocument->SetHeight(newSize.height());
	//rendering current document content
	//updateElements(true, true, true);
}

float DocumentControl::getScrollHandleSize()
{
	return 0.7f;
}

void DocumentControl::scrollY(bool Up)
{
	
}

void DocumentControl::moveScrollHandle(float newPos)
{
	
}

void DocumentControl::mouseBtnDown(float x, float y)
{
	//m_bLmbDown = true;
	//m_docInfo.lock()->MathDocument->UpdateSelecting({ x, y });
	//updateElements(true, false, true);
}

void DocumentControl::mouseBtnUp(float x, float y)
{
	//m_bLmbDown = false;
	//m_docInfo.lock()->MathDocument->StopSelecting();
}

void DocumentControl::mousePosUpdated(float x, float y)
{
	
}

void DocumentControl::updateElements(bool bRect, bool bText, bool bCaret)
{
	m_mathDocument->update();
}

void DocumentControl::clearDocument()
{
	//clearing render data
	m_meDocState.Clear(true, true);
	//move caret outside visible area
	m_meDocState.SetCaret({ {-100, -100}, {1,1} });
	m_mathDocument->update();
}



