import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    width: 200
    height: 100
	property alias mathCanvas: mathCanvas
	Component.onCompleted: {
        docControl.bindMathDocumentItem(mathCanvas)
    }

	DocumentControl{
		id: docControl
	}


	MathDocumentCanvas {
		id:mathCanvas
		focus: true
		anchors.left: parent.left
		anchors.leftMargin: 2
        anchors.top: parent.top
        anchors.bottom: parent.bottom
		anchors.right: parent.right

		MeListSearchBar{
			id: meListSearchBar
			docControl: docControl
			mathCanvas: mathCanvas
			x:100
			y:100
			z:1000
		}

		Keys.onPressed: (event) => {
			docControl.keyInput(event.key, event.text, event.modifiers)
		}

		MouseArea { 
			acceptedButtons: Qt.LeftButton | Qt.RightButton
			anchors.fill: parent
			onClicked: { 
				mathCanvas.focus = true 
			} 
			onPressed: (event) => {
				meListSearchBar.close()
				docControl.mouseBtnDown(event.x, event.y, event.button)
			}
			onReleased: (event) => {
				docControl.mouseBtnUp(event.x, event.y, event.button)
			}
			onPositionChanged: (event) => {
				docControl.mousePosUpdated(event.x, event.y, event.button)
			}
			onWheel: (event) => {
				meListSearchBar.close()
				docControl.scrollY(event.angleDelta.y > 0)
			}
		}

		Connections {
		target: docControl
		
			function onMeListRequested(x,y) {
				meListSearchBar.open()
				meListSearchBar.x = x
				meListSearchBar.y = y
			}
		}
		
	}
        
	ScrollBar {
		id: vbar

		orientation: Qt.Vertical
		policy: ScrollBar.AlwaysOn

		anchors.right: parent.right
		anchors.top: parent.top
		anchors.bottom: parent.bottom

		property bool scrollingLock1: false
		property bool scrollingLock2: false
		onPositionChanged: {
			if(!scrollingLock1)
			{
				scrollingLock2 = true;
				docControl.moveScrollHandle(position / (1 - size))
				scrollingLock2 = false;
			}
		}

		Connections {
			target: docControl

			function onScrollDataChanged(currentLine, linesCount, linesCountOnScreen) {
				if(!vbar.scrollingLock2)
				{
					vbar.scrollingLock1 = true;
					if(linesCount > 1)
					{
						vbar.position = currentLine / (linesCount + 19)
						vbar.size = 20 / (linesCount + 19)
					}
					else
					{
						vbar.size = 1
						vbar.position = 0
					}
					vbar.scrollingLock1 = false;
				}
			}
		}
	}
}
