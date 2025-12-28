#pragma once

#include <memory>
#include <QObject>
#include <QString>
#include "Controls/MenuControl.h"
#include "MathDocument.h"
class FTAMainModule;
class FFreeTypeWrap;

class Application  : public QObject
{
	Q_OBJECT
	
public:
	~Application();
	Application(QObject *parent);
public slots:
	MenuControl* GetMenu();
	void setMathDocument(QQuickItem* doc);
	FFreeTypeWrap* getFreeTypeWrap();
private:
	MenuControl* m_menuControl = nullptr;
	FTAMainModule* m_mainModule = nullptr;
	FFreeTypeWrap* m_freeTypeWrap = nullptr;
public:
	std::shared_ptr<class FTACompatibilityData> m_compatibilityData;
};
