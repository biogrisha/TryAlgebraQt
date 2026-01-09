#pragma once
#include <QObject>
#include <QString>
#include <QUrl>
class FMathDocumentState;
class FilesControl : public QObject
{
	Q_OBJECT
public:
	FilesControl(QObject* parent);
	void openDocument(const QUrl& url);
	void setMeDocStatePtr(FMathDocumentState* meDocState);
	void selectDocument(qint32 ind);
signals:
	void onDocumentOpened(qint32 currentDocInd);
	void onDocumentClosed(qint32 currentDocInd);
	void onCurrentDocumentChanged(qint32 currentDocInd);
private:
	int m_currentDocInd = 0;
	FMathDocumentState* m_meDocState = nullptr;
};