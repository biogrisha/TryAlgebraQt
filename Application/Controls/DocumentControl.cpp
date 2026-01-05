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
	cursorGen->m_caretDataPtr = &m_caretData;
	compatibilityData->CursorComponentGenerator = cursorGen;
	auto meGen = std::make_shared<MathElementGeneratorQt>();
	meGen->m_glyphsPtr = &m_glyphs;
	meGen->initMeGenerators();
	compatibilityData->MeGenerator = meGen;
	AppGlobal::mainModule->OpenDocument(filePath.toLocalFile().toStdWString(), docInfo, compatibilityData);
	m_docInfo = docInfo;
}

void DocumentControl::keyInput(int key, const QString& text, int modifiers)
{	
	auto doc = m_docInfo.lock()->MathDocument;
	if(!text.isEmpty() && text != "\b")
	{
		doc->AddMathElements(text.toStdWString());
		doc->Draw();
		m_mathDocument->moveGlyphData(std::move(m_glyphs));
		m_mathDocument->updateCaret(m_caretData);
	}
	else 
	{
		switch (key) { 
		case Qt::Key_Left:
			doc->StepX(-1);
			doc->UpdateCaret();
			break;
		case Qt::Key_Right: 
			doc->StepX(1);
			doc->UpdateCaret();
			break;
		case Qt::Key_Up: 
			doc->StepY(-1);
			doc->UpdateCaret();
			break; 
		case Qt::Key_Down:
			doc->StepY(1);
			doc->UpdateCaret();
			break;
		case Qt::Key_Backspace:
			doc->DeleteBackward();
			doc->Draw();
			m_mathDocument->moveGlyphData(std::move(m_glyphs));
			break;
		default: 
			qDebug() << "Other key:" << key << "text:" << text; 
			break;
		}
		m_mathDocument->updateCaret(m_caretData);
	}

	m_mathDocument->update();
}

void DocumentControl::mathDocumentReady()
{
	QObject::disconnect(m_mathDocument, &MathDocument::onNodeCreated, this, &DocumentControl::mathDocumentReady);
	m_docInfo.lock()->MathDocument->Draw();
	m_mathDocument->moveGlyphData(std::move(m_glyphs));
	m_mathDocument->updateCaret(m_caretData);
	m_glyphs.clear();
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
		doc->Draw();
		m_mathDocument->moveGlyphData(std::move(m_glyphs));
		m_mathDocument->updateCaret(m_caretData);
		m_mathDocument->update();
	}
}

