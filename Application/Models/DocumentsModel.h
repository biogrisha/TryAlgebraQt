#pragma once
#include <QAbstractListModel>
#include <QStringList>
#include <MathEditor/include/MathDocument.h>

class DocumentInfo
{
public:
    DocumentInfo(const QString& filePath, std::unique_ptr<TryAlgebraCore::MathDocument>&& meDoc);
    QString filePath() const;
    TryAlgebraCore::MathDocument* meDoc();
private:
    QString m_filePath;
    std::unique_ptr<TryAlgebraCore::MathDocument> m_meDoc;
};

/*
* Model used to display documents as tabs
*/
class DocumentsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum DocumentTabInfoRoles {
        FilePath = Qt::UserRole + 1,
        FileName,
    };

    DocumentsModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    DocumentInfo* docInfo(const QModelIndex& index);

    DocumentInfo* docInfo(const QString& filePath);

    void addDocInfo(DocumentInfo&& Info);

    void removeDocInfo(const QString& filePath);

    void removeDocInfo(qint32 ind);

    void setCurrentDocument(const QString& filePath);

    std::optional<QString> curDocPath();
signals:
    void onCurrentDocChanged(const QString& docPath);
    void onDocumentAdded(DocumentInfo* docInfo);
    void onBeforeDocRemoved(DocumentInfo* docInfo);
protected:
    QHash<int, QByteArray> roleNames() const;

private:
    std::vector<DocumentInfo> m_documents;
    std::optional<QString> m_curDocPath;
};  