#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
#include <qqml.h>
#include "FunctionLibraries/FileHelpers.h"
#include "Modules/CommonTypes/MulticastDelegate.h"
#include "Models/DocumentTabModel.h"

class TabsControl : public QObject
{
	Q_OBJECT
public:
	TabsControl(QObject* parent);
public slots:
	void selectTab(const QString& fileName);
	void closeTab(const QString& fileName);
private:
	void onDocumentAdded(const std::weak_ptr<FTAMathDocumentInfo>& docInfo);
	FTAMulticastDelegate<const std::weak_ptr<FTAMathDocumentInfo>&>::HndlPtr m_onAddedHndl;
	DocumentTabInfoModel* m_tabInfoModel = nullptr;
};