// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import TryAlgebra
import com.Application
import QtCore
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs

ApplicationWindow {
    Component.onCompleted: 
    { 
        menuControl = UserApplication.getMenu()
        tabsControl = UserApplication.getTabs()
        tabsList.model = tabsControl.getTabsModel()
        documentLoader.source = ""
    }

    id: window
    visible: true
    visibility: "Maximized"
    width: 640
    height: 480
    property MenuControl menuControl: null
    property TabsControl tabsControl: null
    menuBar: MenuBar {
        Menu {
            title: qsTr("File")
            Action {
                text: qsTr("Open")
                onTriggered: {
                    fileDialog.reason = "openFile"
                    fileDialog.open()
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
                    if(!documentLoader.source.href)
                    {
                        documentLoader.source = "/qt/qml/com/Application/MathDocumentWrap.qml"
                        documentLoader.item.setDocumentControl(UserApplication.getDocumentControl())
                    }
                    break 
                case "saveFile":
                    console.log("Save file")
                break 
            }
        }
    }

    Rectangle {
        id: tabsArea
        width: 150 
		anchors.top: parent.top 
        anchors.bottom: parent.bottom
		anchors.left: parent.left
        color: "#a3a3a3"
        ListView {
		    id: tabsList
            anchors.fill: parent
		    clip: true
		    delegate:  Button {
			    id:button
                required property int index
			    required property string fileName
                required property string filePath
			    width: 150
			    text: fileName
			    background: Rectangle {
				    anchors.left: button.left
				    anchors.right: button.right
				    color: button.down ? "#c2c2c2" : (tabsControl.currentTabId === button.index ? "#cdd9cc" : "#e3e3e3")
				    border.color: "#c2c2c2"
				    border.width: 1
				    radius: 1
			    }
			    onClicked:
			    {
				    tabsControl.selectTab(button.index)
			    }
                Button {
                    id: closeTabButton
                    text: "x"
                    width: 20
                    height: 20
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                }
		    }
	    }
    }



    Rectangle {
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.left: tabsArea.right
        anchors.right: parent.right
        anchors.rightMargin: 20

        Loader {
            id: documentLoader
            anchors.fill: parent
            focus: true
        }
    }

}
