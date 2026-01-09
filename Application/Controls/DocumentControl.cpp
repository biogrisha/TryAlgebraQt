#include "DocumentControl.h"
#include <QUrl>

#include "FilesControl.h"
#include "Application.h"
#include "AppGlobal.h"

#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"

#include "Modules/MathElementsV2/CompatibilityData.h"
#include "Modules/MainWindowModule.h"
#include "Modules/MathDocument/MathDocument.h"
#include "FunctionLibraries/FileHelpers.h"
#include "FunctionLibraries/MathElementsHelpers.h"

DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{
	auto filesControl = AppGlobal::application->getFilesControl();
	filesControl->setMeDocStatePtr(&m_meDocState);
	QObject::connect(filesControl, &FilesControl::onCurrentDocumentChanged, this, &DocumentControl::onCurrentDocumentChanged, Qt::DirectConnection);
}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	m_mathDocument = mathDocument;
	QObject::connect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady, Qt::ConnectionType::DirectConnection);
}

void DocumentControl::keyInput(int key, const QString& text, int modifiers)
{	
	auto doc = m_docInfo.lock()->MathDocument;
	bool bShift = modifiers == Qt::Modifier::SHIFT;
	bool bCtrl = modifiers == Qt::Modifier::CTRL;
	switch (key) {
	case Qt::Key_Left:
		doc->StepX(-1, bShift);
		UpdateElements(true, false, true);
		break;
	case Qt::Key_Right:
		doc->StepX(1, bShift);
		UpdateElements(true, false, true);
		break;
	case Qt::Key_Up:
		doc->StepY(-1, bShift);
		UpdateElements(true, false, true);
		break;
	case Qt::Key_Down:
		doc->StepY(1, bShift);
		UpdateElements(true, false, true);
		break;
	case Qt::Key_Backspace:
		doc->DeleteBackward();
		UpdateElements(true, true, true);
		break;
	case Qt::Key_Delete:
		doc->DeleteForward();
		UpdateElements(true, true, true);
		break;
	case Qt::Key_Z:
		if (bCtrl)
		{
			doc->Undo();
			UpdateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	case Qt::Key_Y:
		if (bCtrl)
		{
			doc->Redo();
			UpdateElements(true, true, true);
			break;
		}
		[[fallthrough]];
	default:
		if (!text.isEmpty() && text != "\b")
		{
			doc->AddMathElements(text.toStdWString());
			UpdateElements(true, true, true);
		}
		break;
	}
}

void DocumentControl::mathDocumentReady()
{
	m_mathDocument->setMeDocState(&m_meDocState);
	QObject::disconnect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady);
	UpdateElements(true, true, true);
	isMathDocumentReady = true;
}

MathElementInfoModel* DocumentControl::getMeInfoModel()
{
	if (m_meInfoModel)
	{
		return m_meInfoModel;
	}
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
	auto& meList = FTAMeHelpers::GetMathElementsList();
	auto foundMe = meList.find(meName.toStdWString());
	if (foundMe != meList.end())
	{
		auto doc = m_docInfo.lock()->MathDocument;
		doc->AddMathElements(foundMe->second);
		UpdateElements(true, true, true);
	}
}

void DocumentControl::onCurrentDocumentChanged(qint32 ind)
{
	m_docInfo = AppGlobal::mainModule->GetAllDocuments()[ind];
	if (isMathDocumentReady)
	{
		UpdateElements(true, true, true);
	}
}

void DocumentControl::UpdateElements(bool bRect, bool bText, bool bCaret)
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


