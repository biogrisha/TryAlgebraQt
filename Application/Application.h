#pragma once

#include <QObject>
#include <QString>
#include <FreeTypeWrap.h>

class MenuControl;
class DocumentControl;
class TabsControl;
class FilesControl;
class FFreeTypeWrap;
class ApplicationModel;
class Application  : public QObject
{
	Q_OBJECT
	
public:
	~Application();
	Application(QObject *parent);

	//Returns freetype module
	FFreeTypeWrap* getFreeTypeWrap();

	//Returns control to manipulate files
	FilesControl* getFilesControl();

public slots:
	//Returns tabs control
	TabsControl* getTabs();

	//Returns menu control
	MenuControl* getMenu();

	//Returns document control
	DocumentControl* getDocumentControl();
private:
	void PrepareMeAtlas();

	MenuControl* m_menu_control = nullptr;
	DocumentControl* m_document_control = nullptr;
	TabsControl* m_tabs_control = nullptr;
	FilesControl* m_files_control = nullptr;

	ApplicationModel* m_app_model = nullptr;
	//free type module
	FFreeTypeWrap m_ft_wrap;
};
