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

void MenuControl::openDocument(const QUrl& url)
{	
    QFile file(url.toLocalFile());

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qCritical() << "Failed to open file:" << url.toLocalFile();
        return;
    }

    QTextStream stream(&file);

    // optional: force UTF-8
    stream.setEncoding(QStringConverter::Utf8);

    QString text = stream.readAll();

    auto new_doc = std::make_unique<TryAlgebraCore::MathDocument>();
    new_doc->setText(text.toStdWString());
    AppGlobal::app_mod->addMathDoc(url.toLocalFile().toStdWString(), std::move(new_doc));
}

