#pragma once
#include <qqml.h>
#include <QObject>
#include <vector>
#include <memory>
#include <map>
#include <MathEditor/include/MathDocument.h>
#include <Models/MeListModel.h>
class ApplicationModel : public QObject
{
	Q_OBJECT
	QML_ELEMENT
public: 
	ApplicationModel(QObject* parent = nullptr);
	void addMathDoc(const std::wstring& file_path, std::unique_ptr<TryAlgebraCore::MathDocument>&& math_doc);
	bool isDocOpened(const std::wstring& file_path);
	TryAlgebraCore::MathDocument* getCurrentDoc();
signals:
	void onNewDoc();
	void onCurrentDocChanged();
public slots:
	MeListModel* meListModel();
private:
	std::map<std::wstring, std::unique_ptr<TryAlgebraCore::MathDocument>> m_documents;
	std::wstring m_curr_doc;
	MeListModel* m_meListModel;
};