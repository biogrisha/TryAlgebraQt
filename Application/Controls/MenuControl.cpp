#include "MenuControl.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "FunctionLibraries/FileHelpers.h"
#include "MathEditor/CursorComponentGenerator.h"
#include "MathEditor/MathElementGenerator.h"
#include "Application.h"
#include "Modules/MainWindowModule.h"
#include "AppGlobal.h"
#include "DocumentControl.h"

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::openDocument(const QUrl& url)
{	
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	auto compatibilityData = FTACompatibilityData::MakeTypedShared();
	compatibilityData->CursorComponentGenerator = std::make_shared<CursorComponentGeneratorQt>();
	compatibilityData->MeGenerator = std::make_shared<MathElementGeneratorQt>();
	AppGlobal::mainModule->OpenDocument(url.toLocalFile().toStdWString(), docInfo, compatibilityData);
	
	auto documentControl = new DocumentControl(AppGlobal::application);
	documentControl->setDocInfo(docInfo);
	AppGlobal::application->setCurrentDocument(documentControl);
}

