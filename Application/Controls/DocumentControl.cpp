#include "DocumentControl.h"
#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "Modules/MainWindowModule.h"
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/MathDocument/MathDocument.h"
#include <QUrl>
#include "AppGlobal.h"
#include "FunctionLibraries/MathElementsHelpers.h"

DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	m_mathDocument = mathDocument;
	QObject::connect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady, Qt::ConnectionType::DirectConnection);
}

void DocumentControl::setDocument(const QUrl& filePath)
{
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	auto compatibilityData = FTACompatibilityData::MakeTypedShared();
	auto cursorGen = std::make_shared<CursorComponentGeneratorQt>();
	cursorGen->m_meDocState = &m_meDocState;
	compatibilityData->CursorComponentGenerator = cursorGen;
	auto meGen = std::make_shared<MathElementGeneratorQt>();
	meGen->m_meDocState = &m_meDocState;
	meGen->initMeGenerators();
	compatibilityData->MeGenerator = meGen;
	AppGlobal::mainModule->OpenDocument(filePath.toLocalFile().toStdWString(), docInfo, compatibilityData);
	m_docInfo = docInfo;
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


