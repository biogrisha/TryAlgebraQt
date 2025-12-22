// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

//! [1]
import QtQuick
import TryAlgebra

Item {
    id: root

    width: 320
    height: 480

    MathDocumentItem {
      id: renderer
      focus: true
      Keys.onPressed: (event)=>
      {
        renderer.onUpdateText(event.text);
      }
    }

    Rectangle {
        color: Qt.rgba(1, 1, 1, 0.7)
        radius: 10
        border.width: 1
        border.color: "white"
        anchors.fill: label
        anchors.margins: -10
    }

    Text {
        id: label
        color: "black"
        wrapMode: Text.WordWrap
        text: qsTr("Vulkan in Qt hell yeah")
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.bottom: parent.bottom
        anchors.topMargin: root.SafeArea.margins.top + 20
        anchors.leftMargin: root.SafeArea.margins.left + 20
        anchors.rightMargin: root.SafeArea.margins.right + 20
        anchors.bottomMargin: root.SafeArea.margins.bottom + 20
    }
}
//! [2]
