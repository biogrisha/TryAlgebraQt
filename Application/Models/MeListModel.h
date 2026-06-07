#pragma once

#include <QAbstractListModel>
#include <QStringList>
#include <QImage>

class MathElementInfo
{
public:
    MathElementInfo(const QString& fullName, const QPoint& viewPos, const QSize& viewSize);
	QString meFullName() const;
    QPoint viewPos() const;
    QSize viewSize() const;
private:
	QString m_meFullName;
    QPoint m_viewPos;
    QSize m_viewSize;

};

/*
* Model used to display math elements in math elents palette
*/
class MeListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum MathElementInfoRoles {
        MeName = Qt::UserRole + 1,
        Image = Qt::UserRole + 2,
        ViewPos = Qt::UserRole + 3,
        ViewSize = Qt::UserRole + 4,
    };

    MeListModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addMathElementInfo(const MathElementInfo& Info);
    void setImage(QImage&& image);
    QImage* image() { return &m_image; }
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<MathElementInfo> m_meInfos;
    QImage m_image;
};

