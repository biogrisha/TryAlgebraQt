import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    id: root

    property alias model: tree.model

    color: "#252525"

    function setRoot(path) {
        tree.rootIndex = tree.model.index(path)
    }

    TreeView {
        id: tree

        anchors.fill: parent
        anchors.margins: 6

        clip: true

        delegate: Rectangle {
            id: rowItem

            implicitWidth: 300
            implicitHeight: 28

            required property int row
            required property int column
            required property int depth
            required property bool expanded
            required property bool hasChildren

            color: mouseArea.containsMouse
                   ? "#3A3A3A"
                   : row % 2 === 0 ? "#292929" : "#252525"

            radius: 3

            Row {
                anchors.verticalCenter: parent.verticalCenter

                x: 8 + depth * 18
                spacing: 6

                Text {
                    width: 12

                    text: rowItem.hasChildren
                          ? (rowItem.expanded ? "▾" : "▸")
                          : ""

                    color: "#AAAAAA"
                    font.pixelSize: 12
                }

                Text {
                    text: tree.model.data(
                        tree.index(rowItem.row, rowItem.column)
                    )

                    color: "#E8E8E8"
                    font.pixelSize: 13

                    anchors.verticalCenter: parent.verticalCenter
                }
            }

            MouseArea {
                id: mouseArea

                anchors.fill: parent
                hoverEnabled: true

                onClicked: {
                    if (rowItem.hasChildren)
                        tree.toggleExpanded(rowItem.row)
                }
            }
        }
    }
}