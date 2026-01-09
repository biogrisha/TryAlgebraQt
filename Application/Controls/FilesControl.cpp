#include "FilesControl.h"

#include "FunctionLibraries/FileHelpers.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "../MathEditor/CursorComponentGenerator.h"
#include "../MathEditor/MathElementGenerator.h"
#include "Modules/MainWindowModule.h"
#include "AppGlobal.h"

FilesControl::FilesControl(QObject* parent)
	: QObject(parent)
{
}

void FilesControl::openDocument(const QUrl& url)
{
	int docInd = AppGlobal::mainModule->FindDocumentInd(url.toLocalFile().toStdWString());
	if (docInd != -1)
	{
		m_currentDocInd = docInd;
		emit onCurrentDocumentChanged(docInd);
		return;
	}
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	auto compatibilityData = FTACompatibilityData::MakeTypedShared();
	auto cursorGen = std::make_shared<CursorComponentGeneratorQt>();
	cursorGen->m_meDocState = m_meDocState;
	compatibilityData->CursorComponentGenerator = cursorGen;
	auto meGen = std::make_shared<MathElementGeneratorQt>();
	meGen->m_meDocState = m_meDocState;
	meGen->initMeGenerators();
	compatibilityData->MeGenerator = meGen;
	AppGlobal::mainModule->OpenDocument(url.toLocalFile().toStdWString(), compatibilityData);
	docInd = AppGlobal::mainModule->GetAllDocuments().size() - 1;
	emit onDocumentOpened(docInd);
	emit onCurrentDocumentChanged(docInd);
}

void FilesControl::setMeDocStatePtr(FMathDocumentState* meDocState)
{
	m_meDocState = meDocState;
}

void FilesControl::selectDocument(qint32 ind)
{
	emit onCurrentDocumentChanged(ind);
}
