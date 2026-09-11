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
        documentLoader.source = ""
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
                    if(!documentLoader.source.href)
                    {
                        documentLoader.source = "/qt/qml/com/Application/MathDocumentWrap.qml"
                    }
                }
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
                    if(!documentLoader.source.href)
                    {
                        documentLoader.source = "/qt/qml/com/Application/MathDocumentWrap.qml"
                    }
                    break 
                case "newFile":
                    menuControl.newDocument(fileDialog.selectedFile)
                    if(!documentLoader.source.href)
                    {
                        documentLoader.source = "/qt/qml/com/Application/MathDocumentWrap.qml"
                    }
                break 
            }
        }
    }

    Rectangle {
        id: tabsArea
        width: 150 
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
                    documentLoader.item.mathCanvas.forceActiveFocus()
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
                        documentLoader.item.mathCanvas.forceActiveFocus()
                    }
                }
		    }
	    }
    }

    Rectangle {
        anchors.top: menuBar.bottom
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