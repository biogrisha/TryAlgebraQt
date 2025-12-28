import QtQuick 2.3
import com.Application

Rectangle {
    width: 200
    height: 100
    MathDocument {
        id:mathDoc
        anchors.fill: parent
        focus: true
        Keys.onPressed: {
            console.log("Key pressed:")
            UserApplication.setMathDocument(mathDoc)
        }
    }
}
