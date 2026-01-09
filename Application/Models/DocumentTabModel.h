#pragma once
#include <QAbstractListModel>
#include <QStringList>

class DocumentTabInfo
{
public:
    DocumentTabInfo(const QString& inFileName, const QString& inFilePath);
    QString fileName() const;
    QString filePath() const;
private:
    QString m_fileName;
    QString m_filePath;
};

class DocumentTabInfoModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DocumentTabInfoRoles {
        FileName = Qt::UserRole + 1,
        FilePath
    };

    DocumentTabInfoModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    void addDocumentTabInfo(const DocumentTabInfo& Info);

    void removeDocumentTabInfo(const QString& fileName);

    void removeDocumentTabInfo(qint32 ind);
protected:
    QHash<int, QByteArray> roleNames() const;
private:
    QList<DocumentTabInfo> m_documents;
};