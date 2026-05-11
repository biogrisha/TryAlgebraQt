#pragma once
#include <QObject>
#include <vector>
#include <memory>
#include <map>
#include <MathEditor/include/MathDocument.h>
class ApplicationModel : public QObject
{
	Q_OBJECT

public: 
	ApplicationModel(QObject* parent = nullptr);
	void addMathDoc(const std::wstring& file_path, std::unique_ptr<TryAlgebraCore2::MathDocument>&& math_doc);
	bool isDocOpened(const std::wstring& file_path);
signals:
	void onNewDoc();
	void onCurrentDocChanged();
private:
	std::map<std::wstring, std::unique_ptr<TryAlgebraCore2::MathDocument>> m_documents;
	std::wstring m_curr_doc;
};