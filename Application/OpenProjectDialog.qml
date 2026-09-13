import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Dialogs

Rectangle {
    id: root

    width: 680
    height: 470

    color: "#947fa0"
    radius: 6

    Button {
        id: closeButton

        width: 40
        height: 32

        anchors {
            top: parent.top
            right: parent.right
        }

        text: "×"

        onClicked: root.Window.window.close()
    }

    Column {
        anchors.fill: parent
        anchors.margins: 30
        spacing: 14

        Text {
            text: "Try Algebra"
            color: "#e7f2ff"
            font.pixelSize: 25
        }

        RowLayout {
            width: parent.width

            Text {
                text: "Select project"
                color: "white"
                font.pixelSize: 21

                Layout.fillWidth: true
            }

            Button {
                text: "Open folder"

                onClicked: {
                    folderDialog.open()
                }
            }
        }

        Rectangle {
            width: parent.width
            height: 275

            color: "#a49bb2"

            ListView {
                id: projectList

                anchors {
                    fill: parent
                    margins: 12
                }

                model: UserApplication.applicationModel().recentFolders()
                spacing: 2
                clip: true

                delegate: Rectangle {
                    required property string modelData

                    width: projectList.width
                    height: 22

                    color: {
                        if (mouseArea.containsMouse)
                            return "#eeeeee"

                        return "#dddddd"
                    }

                    border.color: "#cccccc"

                    Text {
                        anchors {
                            left: parent.left
                            leftMargin: 7
                            verticalCenter: parent.verticalCenter
                        }

                        text: modelData
                        color: "#222222"
                        font.pixelSize: 16
                    }

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            UserApplication.projectSelected(modelData)
                            root.Window.window.close()
                        }
                    }
                }
            }
        }
    }

    FolderDialog {
        id: folderDialog

        title: "Select project folder"

        onAccepted: {
            UserApplication.projectSelectedByUrl(selectedFolder)
            root.Window.window.close()
        }
    }
}