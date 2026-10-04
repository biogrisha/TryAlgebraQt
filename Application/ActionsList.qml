import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
	
    MeActionsControl
    {
        id: meActionsControl
    }

    function open() {
		root.visible = true
        actionsList.model = meActionsControl.transformers()
	}
	id: root

    visible: false
	width: 200
	height: 300

	color: "#d0d0d0"
	radius: 4
	ListView {
        id: actionsList

        anchors.fill: parent

        model: meActionsControl.transformers()
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        WheelHandler {
            target: null
            blocking: true

            onWheel: (event) => {
                var delta = event.pixelDelta.y !== 0
                    ? event.pixelDelta.y
                    : event.angleDelta.y

                actionsList.contentY -= delta
                actionsList.returnToBounds()
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AsNeeded
        }

        delegate: Column {
            required property var modelData

            width: ListView.view.width

            Text {
                text: modelData
                font.bold: true
            }

           
        }
    }
}