// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

//! [1]
import QtQuick
Item {
    id: root

    width: 500
    height: 500

    Rectangle {
        
        anchors.left: parent.left 
        anchors.right: parent.right 
        anchors.bottom: parent.bottom 
        anchors.topMargin: parent.height / 2 
        anchors.top: parent.top
        MathDocument {
            anchors.fill: parent
        }
    }
}
//! [2]
