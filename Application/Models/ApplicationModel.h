#pragma once
#include <qqml.h>
#include <QObject>
#include <vector>
#include <memory>
#include <map>
#include <MathEditor/include/MathDocument.h>
#include <Models/MeListModel.h>
#include <Models/DocumentsModel.h>

class ApplicationModel : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public: 
	ApplicationModel(QObject* parent = nullptr);
signals:
	void onNewDoc();
public slots:
	MeListModel* meListModel();
	DocumentsModel* docModel();
private:
	MeListModel* m_meListModel;
	DocumentsModel* m_docModel;
};