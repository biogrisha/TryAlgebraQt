import QtQuick 2.3
import com.Application

Rectangle {
    width: 200
    height: 100
    //property DocumentControl m_docControl: null
    MathDocument {
        id:mathDoc
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            console.log("Key pressed:")
        }
    }

    function setDocumentControl(docControl) 
    { 
        //m_docControl = docControl
        //m_docControl.bindMathDocumentItem(mathDoc)
    }
}
