#include "MathElementInfoModel.h"
#include <QDebug>

MathElementInfo::MathElementInfo(const QString& fullName, const QString& sign)
{
	m_meFullName = fullName;
	m_meSign = sign;
}

QString MathElementInfo::meFullName() const
{
	return m_meFullName;
}

QString MathElementInfo::meSign() const
{
	return m_meSign;
}

MathElementInfoModel::MathElementInfoModel(QObject* parent)
	:QAbstractListModel(parent)
{
}

int MathElementInfoModel::rowCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return m_meInfos.count();
}

QVariant MathElementInfoModel::data(const QModelIndex& index, int role) const
{
	if (index.row() < 0 || index.row() >= m_meInfos.count())
		return QVariant();

	const MathElementInfo& meInfo = m_meInfos[index.row()];
	if (role == MathElementInfoRoles::MeName)
		return meInfo.meFullName();
	return QVariant();
}

void MathElementInfoModel::addMathElementInfo(const MathElementInfo& Info)
{
	beginInsertRows(QModelIndex(), rowCount(), rowCount());
	m_meInfos << Info;
	endInsertRows();
}

QHash<int, QByteArray> MathElementInfoModel::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[MathElementInfoRoles::MeName] = "meName";
	return roles;
}
