#pragma once

#include <QAbstractListModel>
#include <QStringList>

class MathElementInfo
{
public:
    MathElementInfo(const QString& fullName, const QString& sign);
	QString meFullName() const;
	QString meSign() const;
private:
	QString m_meFullName;
	QString m_meSign;
};

class MathElementInfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum MathElementInfoRoles {
        MeName = Qt::UserRole + 1,
    };

    MathElementInfoModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addMathElementInfo(const MathElementInfo& Info);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<MathElementInfo> m_meInfos;
};

