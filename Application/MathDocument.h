#pragma once

#include "MathDocumentState.h"
#include <QtQuick/QQuickItem>

class CustomTextureNodePrivate;

class MathDocument : public QQuickItem
{
    Q_OBJECT
    QML_ELEMENT

public:
    MathDocument();
    
    //Set new state to update rendering
    void setMeDocState(FMathDocumentState* meDocState);

    //Is underlying node created
    bool isNodeCreated();

protected:
    QSGNode* updatePaintNode(QSGNode*, UpdatePaintNodeData*) override;
    void geometryChange(const QRectF& newGeometry, const QRectF& oldGeometry) override;

private slots:
    void invalidateSceneGraph();
signals:
    void onNodeCreated();
private:
    void releaseResources() override;
    CustomTextureNodePrivate* m_node = nullptr;
};

