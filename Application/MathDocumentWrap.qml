import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    width: 200
    height: 100
    property DocumentControl m_docControl: null

    function setDocumentControl(docControl) 
        { 
            m_docControl = docControl
            m_docControl.bindMathDocumentItem(mathDoc)
			m_docControl.scrollHandlePosChanged.connect(scrollHandlePosChanged)
            filteredMeList.model = m_docControl.getMeInfoModel();
        }



	Flickable {
		id: flick

		width: 150; height: 30;
		contentWidth: meSearchBar.contentWidth
		contentHeight: meSearchBar.contentHeight

		function ensureVisible(r)
		{
			if (contentX >= r.x)
				contentX = r.x;
			else if (contentX+width <= r.x+r.width)
				contentX = r.x+r.width-width;
			if (contentY >= r.y)
				contentY = r.y;
			else if (contentY+height <= r.y+r.height)
				contentY = r.y+r.height-height;
		}

		TextEdit {
			id: meSearchBar
			width: flick.width
			focus: true
			wrapMode: TextEdit.Wrap
			onCursorRectangleChanged: flick.ensureVisible(cursorRectangle)
			onTextChanged:
			{
				filteredMeList.invalidate()
			}
		}
	}

	SortFilterProxyModel {
		id: filteredMeList
		filters: [
			FunctionFilter {
				component RoleData: QtObject { property string meName }
				function filter(data: RoleData) : bool {
					if(meSearchBar.text === "")
					{
						return true
					}
					return data.meName.toLowerCase().includes(meSearchBar.text.toLowerCase())
				}
			}
		]
	}

	ListView {
		focus: false
		id: mathElementsList
		width: 150 
		height: 300
		anchors.top: flick.bottom 
		anchors.left: parent.left
		model: filteredMeList
		clip: true
		delegate:  Button {
			id:button
			required property string meName
			width: 150
			text: meName
			background: Rectangle {
				anchors.left: button.left
				anchors.right: button.right
				color: button.down ? "#c2c2c2" : "#e3e3e3"
				border.color: "#c2c2c2"
				border.width: 1
				radius: 1
			}
			onClicked:
			{
				m_docControl.addMeByName(button.meName)
				mathDoc.focus = true
			}
		}
	}

	MathDocument {
		id:mathDoc
		focus: true
		anchors.left: mathElementsList.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
		anchors.right: parent.right
	
		Keys.onPressed: (event) => {
			m_docControl.keyInput(event.key, event.text, event.modifiers)
		}
		MouseArea { 
			anchors.fill: parent
			onClicked: { 
				mathDoc.focus = true 
			} 
			onPressed: (event) => {
				m_docControl.mouseBtnDown(event.x, event.y)
			}
			onReleased: (event) => {
				m_docControl.mouseBtnUp(event.x, event.y)
			}
			onPositionChanged: (event) => {
				m_docControl.mousePosUpdated(event.x, event.y)
			}
			onWheel: (event) => {
				m_docControl.scrollY(event.angleDelta.y > 0)
			}
		}
	}
        
	ScrollBar { 
        id: vbar 
        orientation: Qt.Vertical 
        anchors.right: parent.right 
        anchors.top: parent.top 
        anchors.bottom: parent.bottom // Full range 
        size: m_docControl ? m_docControl.scrollHandleSize : 0.1
		onPositionChanged: {
			m_docControl.moveScrollHandle(position)
		}
    }

	function scrollHandlePosChanged(newPos: real)
	{
		vbar.position = newPos
	}
}
