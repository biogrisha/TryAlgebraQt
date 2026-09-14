import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    id: root

    FileSystemViewControl {
        id: control
    }

    color: "#252525"

    Text {
        id: rootPathLabel

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.margins: 8

        text: UserApplication.applicationModel().projectFolder()

        color: "#E8E8E8"
        font.pixelSize: 13
        font.bold: true

        elide: Text.ElideMiddle
    }

    TreeView {
        id: tree

        anchors.top: rootPathLabel.bottom
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        anchors.margins: 6

        model: UserApplication.applicationModel().fileSystemModel()
        rootIndex: UserApplication.applicationModel().fileSystemRootIndex()
        selectionModel: ItemSelectionModel {}

        clip: true

        columnWidthProvider: function(column) {
            if (column === 0)
                return tree.width

            return 0
        }

        delegate: Rectangle {
            id: rowItem

            implicitWidth: 300
            implicitHeight: 28

            required property int row
            required property int column
            required property int depth
            required property bool expanded
            required property bool hasChildren

            function getRowColor(hovered, selected) {
                if (hovered && selected)
                    return "#555555"

                if (hovered)
                    return "#3A3A3A"

                if (selected)
                    return "#444444"

                return "#292929"
            }

            color: getRowColor(
                mouseArea.containsMouse,
                tree.selectionModel.currentIndex ===
                    tree.index(rowItem.row, rowItem.column)
            )

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
                    tree.selectionModel.setCurrentIndex(tree.index(rowItem.row, rowItem.column),
                        ItemSelectionModel.NoUpdate)
                    control.selectFile(tree.index(rowItem.row, rowItem.column));
                }
            }
        }
    }
}