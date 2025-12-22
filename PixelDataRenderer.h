#pragma once

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>

class VulkanRendererPrivate;
class MathDocumentItem  : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    MathDocumentItem();

public slots:
    void onUpdateText(const QString& text);
    void sync();
    void onSceneGraphInvalidated();

private slots:
    void handleWindowChanged(QQuickWindow* win);

private:
    void releaseResources() override;

    VulkanRendererPrivate* m_renderer = nullptr;
    QString m_text;

};

