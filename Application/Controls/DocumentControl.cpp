#include "DocumentControl.h"
#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "Modules/MainWindowModule.h"
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/MathDocument/MathDocument.h"
#include <QUrl>
#include "AppGlobal.h"

DocumentControl::DocumentControl(QObject *parent)
	: QObject(parent)
{}

DocumentControl::~DocumentControl()
{}

void DocumentControl::bindMathDocumentItem(MathDocument* mathDocument)
{
	m_mathDocument = mathDocument;
	m_mathDocument->moveGlyphData(std::move(m_glyphs));
	m_glyphs.clear();
}

void DocumentControl::setDocument(const QUrl& filePath)
{
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	auto compatibilityData = FTACompatibilityData::MakeTypedShared();
	compatibilityData->CursorComponentGenerator = std::make_shared<CursorComponentGeneratorQt>();
	auto meGen = std::make_shared<MathElementGeneratorQt>();
	meGen->m_glyphsPtr = &m_glyphs;
	compatibilityData->MeGenerator = meGen;
	AppGlobal::mainModule->OpenDocument(filePath.toLocalFile().toStdWString(), docInfo, compatibilityData);
	m_docInfo = docInfo;
}

void DocumentControl::keyInput(int key, const QString& text, int modifiers)
{	
	auto doc = m_docInfo.lock()->MathDocument;
	doc->AddMathElements(text.toStdWString());
	doc->Draw();
	m_mathDocument->moveGlyphData(std::move(m_glyphs));
}

