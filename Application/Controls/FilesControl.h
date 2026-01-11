#pragma once
#include <QObject>
#include <QString>
#include <QUrl>

class FMathDocumentState;

/*
* Control used by other controls to manipulate documents
*/
class FilesControl : public QObject
{
	Q_OBJECT
public:
	FilesControl(QObject* parent);

	//Opens document by url
	void openDocument(const QUrl& url);

	//Sets doc state used by compatibility data
	void setMeDocStatePtr(FMathDocumentState* meDocState);

	//Selects new current document
	void selectDocument(qint32 ind);

	//Closes document
	void closeDocument(qint32 ind);
signals:
	//When document created or opened
	void onDocumentOpened(qint32 currentDocInd);

	//When document closed
	void onDocumentClosed(qint32 currentDocInd);

	//When new document selected
	void onCurrentDocumentChanged(qint32 currentDocInd);
private:
	//Index of currently selected document
	//Documents are ordered in the main module
	int m_currentDocInd = 0;
	//Doc state used in compatibility data
	FMathDocumentState* m_meDocState = nullptr;
};