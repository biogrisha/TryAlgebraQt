import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    width: 200
    height: 100
    property DocumentControl m_docControl: null
    MathDocument {
        id:mathDoc
        anchors.fill: parent
        focus: true
        Keys.onPressed: (event) => {
            if (event.key === Qt.Key_Backslash) 
            {
                mathElementsList.model = m_docControl.getMeInfoModel();
            }  
            else if(event.key === Qt.Key_Z)
            {
                mathElementsList.model = null
            }
            else
            {
                m_docControl.keyInput(event.key, event.text, event.modifiers)
            }
        }
    }

    function setDocumentControl(docControl) 
    { 
        m_docControl = docControl
        m_docControl.bindMathDocumentItem(mathDoc)
    }

    Column {
        //TextEdit {
        //    width: 180
        //    text: "<b>Hello</b> <i>World!</i>"
        //    font.family: "Helvetica"
        //    font.pointSize: 20
        //    color: "blue"
        //    focus: true
        //}
        ListView {
            id: mathElementsList
            width: 180; height: 200
    
            delegate:  Button {
                id:button
                flat: true
                required property string meName
                text: meName
                background: Rectangle {
                    implicitWidth: 200
                    implicitHeight: 40
                    color: button.down ? "#c2c2c2" : "#e3e3e3"
                    border.color: "#c2c2c2"
                    border.width: 1
                    radius: 1
                }
                onClicked: console.log(button.meName)
            }
        }
    }

}
