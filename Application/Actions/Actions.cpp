#include "Actions.h"
#include <ApplicationModel.h>
#include <AppGlobal.h>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <string>
#include <QStandardPaths>
#include <QDir>
#include <QDirIterator>

void Actions::openDocument(const QUrl& url)
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

void Actions::compile()
{
	//setup bindings
	{
		const QString configDir =
			QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

		const QString filePath = QDir(configDir).filePath("keybindings.mdoc");
		QFile file(filePath);

		if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			qCritical() << "Failed to open file:" << filePath;
			return;
		}

		QTextStream stream(&file);

		stream.setEncoding(QStringConverter::Utf8);


		QString keyBindingConfig = stream.readAll();
		AppGlobal::appMod->keyBinding()->setConfiguration(keyBindingConfig.toStdWString());
	}

	//setup formulas
	{
		auto filePaths = getMdocFiles(AppGlobal::appMod->projectFolder());
		for (const auto& filePath : filePaths)
		{

			QFile file(filePath);

			if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
			{
				qCritical() << "Failed to open file:" << filePath;
				return;
			}

			QTextStream stream(&file);
			stream.setEncoding(QStringConverter::Utf8);
			QString fileContent = stream.readAll();
			AppGlobal::appMod->formulasSystem()->addFile(fileContent.toStdWString());

		}
	}

}

QStringList Actions::getMdocFiles(const QString& folder)
{
	QStringList result;

	QDirIterator it(
		folder,
		{ "*.mdoc" },
		QDir::Files,
		QDirIterator::Subdirectories
	);

	while (it.hasNext())
		result.push_back(it.next());

	return result;
}
