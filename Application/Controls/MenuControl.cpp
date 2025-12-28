#include "MenuControl.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "FunctionLibraries/FileHelpers.h"
#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"
#include "Application.h"
#include "Modules/MainWindowModule.h"
#include "AppGlobal.h"

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::openDocument(const QUrl& url)
{	
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	auto documentControl = new DocumentControl(AppGlobal::application);
	AppGlobal::mainModule->OpenDocument(url.toLocalFile().toStdWString(), docInfo, AppGlobal::application->m_compatibilityData);
	documentControl->setDocInfo(docInfo.lock().get());
	AppGlobal::application->setCurrentDocument(documentControl);
}

