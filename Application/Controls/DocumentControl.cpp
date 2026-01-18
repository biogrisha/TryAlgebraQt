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
	auto filesControl = AppGlobal::application->getFilesControl();
	filesControl->setMeDocStatePtr(&m_meDocState);
	//connect to event when document selected
	QObject::connect(filesControl, &FilesControl::onCurrentDocumentChanged, this, &DocumentControl::onCurrentDocumentChanged, Qt::DirectConnection);
}

float DocumentControl::scrollHandleSize()
{
	return m_scrollHandleSize;
}

float DocumentControl::scrollHandlePos()
{
	return m_scrollHandlePos;
}

void DocumentControl::setScrollHandleSize(float newSize)
{
	if (newSize == m_scrollHandleSize)
	{
		return;
	}
	m_scrollHandleSize = newSize;
	emit scrollHandleSizeChanged(newSize);
}

void DocumentControl::setScrollHandlePos(float newPos)
{
	m_scrollHandlePos = newPos;
}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	//cache math document and wait until it's ready
	m_mathDocument = mathDocument;
	QObject::connect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady, Qt::ConnectionType::DirectConnection);
}

void DocumentControl::keyInput(int key, const QString& text, int modifiers)
{	
	if (m_docInfo.expired())
	{
		return;
	}
	auto doc = m_docInfo.lock()->MathDocument;
	bool bShift = modifiers == Qt::Modifier::SHIFT;
	bool bCtrl = modifiers == Qt::Modifier::CTRL;
	switch (key) {
	case Qt::Key_Left:
		doc->StepX(-1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Right:
		doc->StepX(1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Up:
		doc->StepY(-1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Down:
		doc->StepY(1, bShift);
		updateElements(true, false, true);
		break;
	case Qt::Key_Backspace:
		doc->DeleteBackward();
		updateElements(true, true, true);
		break;
	case Qt::Key_Delete:
		doc->DeleteForward();
		updateElements(true, true, true);
		break;
	case Qt::Key_Z:
		if (bCtrl)
		{
			doc->Undo();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_Y:
		if (bCtrl)
		{
			doc->Redo();
			updateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	default:
		if (!text.isEmpty() && text != "\b")
		{
			doc->AddMathElements(text.toStdWString());
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
	auto& meList = FTAMeHelpers::GetMathElementsList();
	for (auto& me : meList)
	{
		m_meInfoModel->addMathElementInfo(MathElementInfo(QString::fromStdWString(me.first), ""));
	}
	return m_meInfoModel;
}

void DocumentControl::addMeByName(const QString& meName)
{
	if (m_docInfo.expired())
	{
		return;
	}
	//searching for me by its full name
	auto& meList = FTAMeHelpers::GetMathElementsList();
	auto foundMe = meList.find(meName.toStdWString());
	if (foundMe != meList.end())
	{
		//found me
		//adding it into math document
		auto doc = m_docInfo.lock()->MathDocument;
		doc->AddMathElements(foundMe->second);
		//rendering content
		updateElements(true, true, true);
	}
}

void DocumentControl::onCurrentDocumentChanged(qint32 ind)
{
	if (ind == -1)
	{
		//new index is invalid
		//clearing visual content
		clearDocument();
		m_docInfo.reset();
		return;
	}
	// new doc id is valid
	//search for new doc info
	m_docInfo = AppGlobal::mainModule->GetAllDocuments()[ind];
	auto MeDoc = m_docInfo.lock()->MathDocument->GetMeDocument();
	m_onLinesCountUpdated = MeDoc->OnLinesCountUpdated.AddFunc(this, &DocumentControl::onLinesCountUpdated);
	m_onLinesOnPageUpdated = MeDoc->OnLinesOnPageUpdated.AddFunc(this, &DocumentControl::onLinesOnPageUpdated);
	onLinesCountUpdated(MeDoc.Get());
	if (m_isMathDocumentReady)
	{
		//math document allowed to render
		//render content
		updateElements(true, true, true);
	}
}

void DocumentControl::onResized(const QSize& newSize)
{
	m_docInfo.lock()->MathDocument->SetHeight(newSize.height());
	//rendering current document content
	updateElements(true, true, true);
}

float DocumentControl::getScrollHandleSize()
{
	return 0.7f;
}

void DocumentControl::scrollY(bool Up)
{
	m_docInfo.lock()->MathDocument->ScrollY(Up ? -1 : 1);
	onLinesCountUpdated(m_docInfo.lock()->MathDocument->GetMeDocument().Get());
	updateElements(true, true, true);
}

void DocumentControl::updateElements(bool bRect, bool bText, bool bCaret)
{
	m_meDocState.Clear(bText, bRect);
	auto doc = m_docInfo.lock()->MathDocument;
	if (bRect || bText)
	{
		doc->Draw();
	}
	if(bCaret)
	{
		doc->UpdateCaret();
	}
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

void DocumentControl::onLinesCountUpdated(MathElementV2::FTAMeDocument* doc)
{
	float newScrollSize = qMin(float(doc->GetLinesOnPage()) / float(doc->GetLinesCount()), 1.f);
	setScrollHandleSize(newScrollSize);
}

void DocumentControl::onLinesOnPageUpdated(MathElementV2::FTAMeDocument* doc)
{
	float newScrollSize = qMin(float(doc->GetLinesOnPage()) / float(doc->GetLinesCount()), 1.f);
	setScrollHandleSize(newScrollSize);
}


