#include <Controls/FileSystemViewControl.h>
#include <Models/ApplicationModel.h>
#include <AppGlobal.h>
#include <Actions/Actions.h>

void FileSystemViewControl::selectFile(const QModelIndex& index)
{
	QString filePath = AppGlobal::appMod->fileSystemModel()->filePath(index);
	Actions::openDocument(QUrl::fromLocalFile(filePath));
}

