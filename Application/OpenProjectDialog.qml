import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root

    width: 680
    height: 470

    color: "#947fa0"
    radius: 6

    ListModel {
        id: projectModel

        ListElement { path: "../../folder1" }
        ListElement { path: "../../folder2" }
        ListElement { path: "../../folder3" }
    }

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
                    console.log("Open folder clicked")
                    UserApplication.projectSelected("")
                    root.Window.window.close()
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

                model: projectModel
                spacing: 2
                clip: true

                delegate: Rectangle {
                    required property string path
                    required property int index

                    width: projectList.width
                    height: 22

                    color: {
                        if (projectList.currentIndex === index)
                            return "#c7c2cc"

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

                        text: path
                        color: "#222222"
                        font.pixelSize: 16
                    }

                    MouseArea {
                        id: mouseArea

                        anchors.fill: parent
                        hoverEnabled: true

                        onClicked: {
                            projectList.currentIndex = index
                            console.log("Selected:", path)
                        }

                        onDoubleClicked: {
                            console.log("Open project:", path)
                        }
                    }
                }
            }
        }
    }
}