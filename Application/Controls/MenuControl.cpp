#include "Controls/MenuControl.h"
#include <MathEditor/include/MathDocument.h>
#include <Application.h>
#include <ApplicationModel.h>
#include <AppGlobal.h>
#include <Controls/FilesControl.h>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <string>

MenuControl::MenuControl(QObject* parent)
	: QObject(parent)
{
}

void MenuControl::saveDocument()
{
	DocumentsModel* docModel = AppGlobal::appMod->docModel();
	auto currDocPath = docModel->curDocPath();
	if (!currDocPath)
	{
		return;
	}
	auto currDocInfo = docModel->docInfo(currDocPath.value());
	QFile file(currDocPath.value());
	if (file.open(QIODevice::WriteOnly | QIODevice::Text))
	{
		QTextStream out(&file);
		out << currDocInfo->meDoc()->getText();
	}
}

void MenuControl::openDocument(const QUrl& url)
{
	auto docModel = AppGlobal::appMod->docModel();
	QString filePath = url.toLocalFile();
	if (docModel->isDocumentOpened(filePath))
	{
		docModel->setCurrentDocument(filePath);
		return;
	}
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qCritical() << "Failed to open file:" << url.toLocalFile();
		return;
	}

	QTextStream stream(&file);

	stream.setEncoding(QStringConverter::Utf8);

	QString text = stream.readAll();
	DocumentInfo docInfo(url.toLocalFile(), std::make_unique<TryAlgebraCore::MathDocument>());
	docInfo.meDoc()->setText(text.toStdWString());
	docModel->addDocInfo(std::move(docInfo));
	docModel->setCurrentDocument(url.toLocalFile());
}

void MenuControl::newDocument(const QUrl& url)
{
	auto docModel = AppGlobal::appMod->docModel();
	QString filePath = url.toLocalFile();
	if (docModel->isDocumentOpened(filePath))
	{
		docModel->setCurrentDocument(filePath);
		return;
	}
	QFile file(filePath);

	if (!file.open(QIODevice::WriteOnly))
	{
		qCritical() << "Failed to open file:" << filePath;
		return;
	}
	file.write("");
	file.close();

	DocumentInfo docInfo(url.toLocalFile(), std::make_unique<TryAlgebraCore::MathDocument>());
	docModel->addDocInfo(std::move(docInfo));
	docModel->setCurrentDocument(url.toLocalFile());
}

