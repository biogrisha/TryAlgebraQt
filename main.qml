// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import TryAlgebra
import com.Application
import QtCore
import QtQuick
import QtQuick.Controls
import QtQuick.Dialogs

ApplicationWindow {
    Component.onCompleted: 
    { 
        menuControl = UserApplication.GetMenu()
    }

    id: window
    visible: true
    visibility: "Maximized"
    width: 640
    height: 480
    property MenuControl menuControl: null
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            Action {
                text: qsTr("Open")
                onTriggered: {
                    fileDialog.reason = "openFile"
                    fileDialog.open()
                    documentLoader.source = "/qt/qml/com/Application/MathDocumentWrap.qml"
                }
            }
        }
    }

    FileDialog {
        id:fileDialog
        property string reason: "openFile"

        onAccepted:
        {
            switch (reason) { 
                case "openFile":
                menuControl.openDocument(fileDialog.selectedFile)
                break 
                case "saveFile":
                console.log("Save file") 
                break 
            }
        }
    }

    
    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: parent.width/30
        anchors.rightMargin: parent.width/30
        Loader {
            id: documentLoader
            anchors.fill: parent
        }
    }

}
