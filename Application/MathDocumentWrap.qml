import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    width: 200
    height: 100
	focus: true
    property DocumentControl m_docControl: null

    function setDocumentControl(docControl) 
        { 
            m_docControl = docControl
            m_docControl.bindMathDocumentItem(mathDoc)
            mathElementsList.model = m_docControl.getMeInfoModel();
        }



	ListView {
		id: mathElementsList
		width: 150 
		anchors.top: parent.top 
		anchors.bottom: parent.bottom

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
			onClicked: m_docControl.addMeByName(button.meName)
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
	}
        

}
