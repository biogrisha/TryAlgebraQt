#include "MenuControl.h"
#include "AppGlobal.h"
#include "Modules/MathElementsV2/CompatibilityData.h"
#include "FunctionLibraries/FileHelpers.h"

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::openDocument(const QUrl& url)
{	
	std::shared_ptr<class FTACompatibilityData> compatibilityData;
	std::weak_ptr<FTAMathDocumentInfo> docInfo;
	AppGlobal::mainModule->OpenDocument(url.toLocalFile().toStdWString(), docInfo, compatibilityData);
}

