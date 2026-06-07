#include "MeListModel.h"
#include <QDebug>

MathElementInfo::MathElementInfo(const QString& fullName, const QPoint& viewPos, const QSize& viewSize)
	: m_meFullName(fullName)
	, m_viewPos(viewPos)
	, m_viewSize(viewSize)
{

}

QString MathElementInfo::meFullName() const
{
	return m_meFullName;
}

QPoint MathElementInfo::viewPos() const
{
	return m_viewPos;
}

QSize MathElementInfo::viewSize() const
{
	return m_viewSize;
}

MeListModel::MeListModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

int MeListModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_meInfos.count();
}

QVariant MeListModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= m_meInfos.count())
		return QVariant();

	const MathElementInfo& meInfo = m_meInfos[index.row()];
	if (role == MathElementInfoRoles::MeName)
		return meInfo.meFullName();
	if (role == MathElementInfoRoles::Image)
		return m_image;
	if (role == MathElementInfoRoles::ViewPos)
		return meInfo.viewPos();
	if (role == MathElementInfoRoles::ViewSize)
		return meInfo.viewSize();
	return QVariant();
}

void MeListModel::addMathElementInfo(const MathElementInfo& Info)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_meInfos << Info;
	endInsertRows();
}

void MeListModel::setImage(QImage&& image)
{
	m_image = std::move(image);
}

QHash<int, QByteArray> MeListModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[MathElementInfoRoles::MeName] = "meName";
	roles[MathElementInfoRoles::Image] = "image";
	roles[MathElementInfoRoles::ViewPos] = "viewPos";
	roles[MathElementInfoRoles::ViewSize] = "viewSize";
	return roles;
}
