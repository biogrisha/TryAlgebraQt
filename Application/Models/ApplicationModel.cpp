#include "ApplicationModel.h"

ApplicationModel::ApplicationModel(QObject* parent)
	: QObject(parent)
{
}

void ApplicationModel::addMathDoc(const std::wstring& file_path, std::unique_ptr<TryAlgebraCore2::MathDocument>&& math_doc)
{
	auto new_doc = m_documents.emplace(file_path, std::move(math_doc));
	if(new_doc.second)
	{
		m_curr_doc = file_path;
		emit onNewDoc();
		emit onCurrentDocChanged();
	}
}

bool ApplicationModel::isDocOpened(const std::wstring& file_path)
{
	return m_documents.contains(file_path);
}

TryAlgebraCore2::MathDocument* ApplicationModel::getCurrentDoc()
{
	auto found_doc = m_documents.find(m_curr_doc);
	return found_doc != m_documents.end() ? found_doc->second.get() : nullptr;
}

