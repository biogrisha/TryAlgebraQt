// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef VULKANTEXTUREIMPORT_H
#define VULKANTEXTUREIMPORT_H

#include <QtQuick/QQuickItem>

class CustomTextureNodePrivate;

//! [1]
class MathDocument : public QQuickItem
{
    Q_OBJECT
        Q_PROPERTY(qreal t READ t WRITE setT NOTIFY tChanged)
        QML_ELEMENT

public:
    MathDocument();

    qreal t() const { return m_t; }
    void setT(qreal t);

signals:
    void tChanged();

protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private slots:
    void invalidateSceneGraph();

private:
    void releaseResources() override;

    CustomTextureNodePrivate* m_node = nullptr;
    qreal m_t = 0;
};
//! [1]

#endif // VULKANTEXTUREIMPORT_H
