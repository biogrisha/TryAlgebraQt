#pragma once

#include <QtQuick/QQuickItem>
#include <QtQuick/QQuickWindow>

class VulkanRendererPrivate;
class PixelDataRenderer  : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
    QML_ELEMENT

public:
    PixelDataRenderer();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

public slots:
    void sync();
    void cleanup();

private slots:
    void handleWindowChanged(QQuickWindow* win);

private:
    void releaseResources() override;

    qreal m_t = 0;
    VulkanRendererPrivate* m_renderer = nullptr;
};

