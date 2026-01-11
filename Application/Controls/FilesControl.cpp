#include "Controls/FilesControl.h"

#include <FunctionLibraries/FileHelpers.h>
#include <Modules/MathElementsV2/CompatibilityData.h>
#include <Modules/MainWindowModule.h>

#include <MathEditor/CursorComponentGenerator.h>
#include <MathEditor/MathElementGenerator.h>
#include <AppGlobal.h>

FilesControl::FilesControl(QObject* parent)
	: QObject(parent)
{
}

void FilesControl::openDocument(const QUrl& url)
{
	//searching doc id by file path among opened docs
	int docInd = AppGlobal::mainModule->FindDocumentInd(url.toLocalFile().toStdWString());
	if (docInd != -1)
	{
		//found id
		//selecting doc
		selectDocument(docInd);
		return;
	}
	//no doc found 
	//creating compatibility data
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	auto compatibilityData = FTACompatibilityData::MakeTypedShared();
	auto cursorGen = std::make_shared<CursorComponentGeneratorQt>();
	cursorGen->m_meDocState = m_meDocState;
	compatibilityData->CursorComponentGenerator = cursorGen;
	auto meGen = std::make_shared<MathElementGeneratorQt>();
	meGen->m_meDocState = m_meDocState;
	meGen->initMeGenerators();
	compatibilityData->MeGenerator = meGen;

	//Opening doc by file path
	AppGlobal::mainModule->OpenDocument(url.toLocalFile().toStdWString(), compatibilityData);
	//selecting document
	docInd = AppGlobal::mainModule->GetAllDocuments().size() - 1;
	emit onDocumentOpened(docInd);
	selectDocument(docInd);
}

void FilesControl::setMeDocStatePtr(FMathDocumentState* meDocState)
{
	m_meDocState = meDocState;
}

void FilesControl::selectDocument(qint32 ind)
{
	m_currentDocInd = ind;
	emit onCurrentDocumentChanged(ind);
}

void FilesControl::closeDocument(qint32 ind)
{
	qint32 docNum = AppGlobal::mainModule->GetAllDocuments().size();
	//closing document
	AppGlobal::mainModule->CloseDocument(ind);
	emit onDocumentClosed(ind);
	//updating current doc id
	if (m_currentDocInd > ind)
	{
		m_currentDocInd--;
		emit onCurrentDocumentChanged(m_currentDocInd);
	}
	else if(m_currentDocInd == ind)
	{
		if (docNum == 1)
		{
			m_currentDocInd = -1;
		}
		else
		{
			m_currentDocInd = qBound(0, m_currentDocInd, docNum - 2);
		}
		emit onCurrentDocumentChanged(m_currentDocInd);
	}
}
