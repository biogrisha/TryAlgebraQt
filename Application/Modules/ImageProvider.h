#pragma once
#include <qquickimageprovider.h>
#include <QImage>
#include <QPainter>

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider()
        : QQuickImageProvider(QQuickImageProvider::Image)
    {
    }

    void setImage(QImage* img)
    {
        m_image = img;
    }

    QImage requestImage(const QString& id,
        QSize* size,
        const QSize& requestedSize) override
    {
        Q_UNUSED(id)

        if (size)
            *size = m_image->size();

        return *m_image;
    }

private:
    QImage* m_image = nullptr;
};