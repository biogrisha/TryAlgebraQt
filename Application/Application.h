#pragma once

#include <QObject>
#include <QString>
#include <FreeTypeWrap.h>
#include <QQuickView>

class MenuControl;
class DocumentControl;
class TabsControl;
class FilesControl;
class FFreeTypeWrap;
class ApplicationModel;
class QQmlEngine;
class Application : public QObject
{
	Q_OBJECT

public:
	~Application();
	Application(QObject* parent = nullptr);

	//Returns freetype module
	FFreeTypeWrap* getFreeTypeWrap();

	//Returns control to manipulate files
	FilesControl* getFilesControl();

	void generateMeAtlas();
	void setQmlEngine(QQmlEngine* engine);
public slots:
	//Returns menu control
	MenuControl* getMenu();

	ApplicationModel* applicationModel();
	void projectSelectedByUrl(const QUrl& url);
	void projectSelected(QString folderPath);
private:

	MenuControl* m_menu_control = nullptr;
	FilesControl* m_files_control = nullptr;

	ApplicationModel* m_app_model = nullptr;
	//free type module
	FFreeTypeWrap m_ft_wrap;
	QQmlEngine* m_qmlEngine = nullptr;
	std::unique_ptr<QVulkanInstance> m_vulkanInst;
	std::unique_ptr<QQuickView> m_mainWindow;
};
