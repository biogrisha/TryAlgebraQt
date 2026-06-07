#include "ApplicationModel.h"

ApplicationModel::ApplicationModel(QObject* parent)
	: QObject(parent)
	, m_meListModel(new MeListModel(this))
	, m_docModel(new DocumentsModel(this))
{
}

DocumentsModel* ApplicationModel::docModel()
{
	return m_docModel;
}

MeListModel* ApplicationModel::meListModel()
{
	return m_meListModel;
}

