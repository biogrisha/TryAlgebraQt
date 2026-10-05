import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    id: root

    visible: false
    width: 200
    height: 300

    color: "#d0d0d0"
    radius: 4

    MeActionsControl {
        id: meActionsControl
    }

    MeActionsModel {
        id: meActionsModel
    }

    function open() {
        root.visible = true

        meActionsModel.update()
    }

    TreeView {
        id: actionsTree

        anchors.fill: parent
        model: meActionsModel

        clip: true
        boundsBehavior: Flickable.StopAtBounds

        WheelHandler {
            target: null
            blocking: true

            onWheel: (event) => {
                var delta = event.pixelDelta.y !== 0
                    ? event.pixelDelta.y
                    : event.angleDelta.y

                actionsTree.contentY -= delta
                actionsTree.returnToBounds()
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        delegate: TreeViewDelegate {
            id: treeDelegate

            width: TreeView.view.width

            contentItem: Text {
                text: treeDelegate.display

                leftPadding: treeDelegate.depth * 16
                rightPadding: 4

                verticalAlignment: Text.AlignVCenter
                font.bold: treeDelegate.depth === 0

                color: "#202020"
            }
        }
    }
}