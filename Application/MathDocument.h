// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef VULKANTEXTUREIMPORT_H
#define VULKANTEXTUREIMPORT_H
#include "MathDocumentRenderingStructs.h"
#include <QtQuick/QQuickItem>

class CustomTextureNodePrivate;

class MathDocument : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    MathDocument();
    void moveGlyphData(std::vector<FGlyphData>&& glyphs);
protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private slots:
    void invalidateSceneGraph();

private:
    void releaseResources() override;

    CustomTextureNodePrivate* m_node = nullptr;
    std::vector<FGlyphData> m_glyphs;
};

#endif // VULKANTEXTUREIMPORT_H
