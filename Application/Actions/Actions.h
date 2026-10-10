#pragma once
#include <QUrl>
class Actions
{
public:
	static void openDocument(const QUrl& url);
	static void compile();
	static void saveAll();
private:
	static QStringList getMdocFiles(const QString& folder);
};