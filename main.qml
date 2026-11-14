// Copyright (C) 2019 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import TryAlgebra
import com.Application
import QtCore
import QtQuick
import QtQuick.Controls.Basic
import QtQuick.Dialogs

Item {

	Component.onCompleted: 
    { 
        menuControl = UserApplication.getMenu()
    }

    id: window
    visible: true
    property MenuControl menuControl: null
    
    TabsControl
    {
        id: tabsControl
    }

     //============== Menu Bar =================
    MenuBar {
        id: menuBar

        Menu {
            title: qsTr("File")
            Action {
                text: qsTr("New")
                onTriggered: {
                    fileDialog.reason = "newFile"
                    fileDialog.fileMode = FileDialog.SaveFile
                    fileDialog.options &= ~FileDialog.ReadOnly
                    fileDialog.open()
                }
            }
            Action {
                text: qsTr("Open")
                onTriggered: {
                    fileDialog.reason = "openFile"
                    fileDialog.fileMode = FileDialog.OpenFile
                    fileDialog.options |= FileDialog.ReadOnly
                    fileDialog.open()
                }
            }
            Action {
                text: qsTr("Save")
                onTriggered: {
                    menuControl.saveDocument()
                }
            }
            Action {
                text: qsTr("Bindings")
                onTriggered: {
                    menuControl.openBindings()
                }
            }
        }

        Menu {
            title: qsTr("View")

            Action {
                id: showTabsAction
                text: qsTr("Show Tabs")
                checkable: true
                checked: true
            }

            Action {
                id: showProjectTreeAction
                text: qsTr("Show Project Tree")
                checkable: true
                checked: true
            }
        }
    }

    Button {
        id: compileButton
        text: qsTr("Compile")

        width: 90
        height: menuBar.height - 4

        anchors.right: parent.right
        anchors.rightMargin: 4
        anchors.verticalCenter: menuBar.verticalCenter

        background: Rectangle {
            radius: 3
            color: compileButton.down
                   ? "#d6a900"
                   : "#f0c419"
        }

        contentItem: Text {
            text: compileButton.text
            font: compileButton.font
            color: "#202020"
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
        }

        onClicked: {
            menuControl.compile()
        }
    }

    //============== File dialog menu =================
    FileDialog {
        id:fileDialog
        property string reason: "openFile"
        nameFilters: ["Text files (*.mdoc)"]
        onAccepted:
        {
            switch (reason) { 
                case "openFile":
                    menuControl.openDocument(fileDialog.selectedFile)
                    break 
                case "newFile":
                    menuControl.newDocument(fileDialog.selectedFile)
                break 
            }
        }
    }

    Rectangle {
        id: tabsArea
        visible: showTabsAction.checked
        width: visible ? 150 : 0
		anchors.top: menuBar.bottom 
        anchors.bottom: parent.bottom
		anchors.left: parent.left
        color: "#a3a3a3"
        ListView {
		    id: tabsList
            model: UserApplication.applicationModel().docModel()
            anchors.fill: parent
		    clip: true
		    delegate:  Button {
			    id:button
                required property int index
			    required property string fileName
                required property string filePath
                required property bool currentDoc

			    width: 150
			    text: fileName
			    background: Rectangle {
				    anchors.left: button.left
				    anchors.right: button.right
				    color: button.down ? "#c2c2c2" : (currentDoc ? "#cdd9cc" : "#e3e3e3")
				    border.color: "#c2c2c2"
				    border.width: 1
				    radius: 1
			    }
			    onClicked:
			    {
				    tabsControl.selectTab(button.index)
                    documentArea.mathCanvas.forceActiveFocus()
			    }
                Button {
                    id: closeTabButton
                    text: "x"
                    width: 20
                    height: 20
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    onClicked:
                    {
                        tabsControl.closeTab(button.index)
                        documentArea.mathCanvas.forceActiveFocus()
                    }
                }
		    }
	    }
    }

    MathDocumentWrap {
        id: documentArea

        anchors.top: menuBar.bottom
        anchors.bottom: parent.bottom
        anchors.left: tabsArea.right
        anchors.right: fileSystemView.left
        anchors.rightMargin: fileSystemView.visible ? 10 : 0
    }

    FileSystemView {
        id: fileSystemView

        visible: showProjectTreeAction.checked
        width: visible ? 250 : 0

        anchors.top: menuBar.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
    }
}
