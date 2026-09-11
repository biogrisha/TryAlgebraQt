#include "Actions.h"
#include <ApplicationModel.h>
#include <AppGlobal.h>
#include <QFile>
#include <QTextStream>
#include <QString>
#include <string>
#include <QStandardPaths>
#include <QDir>
void Actions::compile()
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
