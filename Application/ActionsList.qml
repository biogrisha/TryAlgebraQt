import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    id: root

    property var selectedFormula: null

    visible: false
    width: 240
    height: 300

    color: "#fafafa"
    radius: 6
    border.width: 1
    border.color: "#c8c8c8"
    clip: true

    MeActionsControl {
        id: meActionsControl
    }

    MeActionsModel {
        id: meActionsModel
    }

    function open() {
        root.visible = true
        sectionsList.model = null
        root.selectedFormula = null
        meActionsModel.update()
        sectionsList.model = meActionsModel.sections()
    }

    ListView {
        id: sectionsList

        anchors.fill: parent
        anchors.margins: 6

        spacing: 4
        clip: true
        boundsBehavior: Flickable.StopAtBounds

        WheelHandler {
            target: null
            blocking: true

            onWheel: (event) => {
                var delta = event.pixelDelta.y !== 0
                    ? event.pixelDelta.y
                    : event.angleDelta.y

                sectionsList.contentY -= delta
                sectionsList.returnToBounds()
            }
        }

        ScrollBar.vertical: ScrollBar {
            width: 5
            policy: ScrollBar.AsNeeded
        }

        delegate: Column {
            id: sectionDelegate

            required property var modelData
            property var section: modelData

            width: sectionsList.width - 6
            spacing: 2

            Text {
                width: parent.width
                height: 24
                leftPadding: 6
                text: sectionDelegate.section.name()
                font.pixelSize: 12
                font.bold: true
                verticalAlignment: Text.AlignVCenter
                color: "#444444"
                elide: Text.ElideRight
            }

            Repeater {
                model: sectionDelegate.section.actions()

                delegate: Loader {
                    id: actionLoader

                    required property var modelData
                    property var actionData: modelData

                    width: sectionDelegate.width
                    height: item ? item.implicitHeight : 0
                    sourceComponent: actionData.type() === "parser"
                        ? parserAction
                        : actionData.type() === "formula" ? formulaAction : null

                    Component {
                        id: parserAction

                        Rectangle {
                            id: actionItem

                            width: actionLoader.width
                            implicitHeight: 34
                            radius: 4
                            color: actionHover.hovered ? "#eeeeee" : "transparent"

                            HoverHandler {
                                id: actionHover
                            }

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 7
                                anchors.rightMargin: 4
                                spacing: 4

                                Text {
                                    width: parent.width
                                        - parseButton.width
                                        - inverseButton.width
                                        - parent.spacing * 2
                                    height: parent.height
                                    text: actionLoader.actionData.name()
                                    font.pixelSize: 12
                                    verticalAlignment: Text.AlignVCenter
                                    color: "#282828"
                                    elide: Text.ElideRight
                                }

                                Button {
                                    id: parseButton

                                    y: (parent.height - height) / 2
                                    width: 48
                                    height: 24
                                    text: qsTr("Parse")
                                    hoverEnabled: true

                                    contentItem: Text {
                                        text: parseButton.text
                                        font.pixelSize: 11
                                        color: "#ffffff"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    background: Rectangle {
                                        radius: 3
                                        color: parseButton.down
                                            ? "#4e4e4e"
                                            : parseButton.hovered ? "#666666" : "#585858"
                                    }
                                    onClicked: {
                                        meActionsControl.parse(actionLoader.actionData.name())
                                    }
                                }

                                Button {
                                    id: inverseButton

                                    y: (parent.height - height) / 2
                                    width: 54
                                    height: 24
                                    text: qsTr("Inverse")
                                    hoverEnabled: true

                                    contentItem: Text {
                                        text: inverseButton.text
                                        font.pixelSize: 11
                                        color: "#444444"
                                        horizontalAlignment: Text.AlignHCenter
                                        verticalAlignment: Text.AlignVCenter
                                    }

                                    background: Rectangle {
                                        radius: 3
                                        color: inverseButton.down
                                            ? "#dddddd"
                                            : inverseButton.hovered ? "#f3f3f3" : "#ffffff"
                                        border.width: 1
                                        border.color: "#b8b8b8"
                                    }

                                    onClicked: {
                                        meActionsControl.parseInverse(actionLoader.actionData.name())
                                    }
                                }
                            }
                        }
                    }

                    Component {
                        id: formulaAction

                        Rectangle {
                            id: formulaItem

                            property int currentPart: 1
                            readonly property int partCount: actionLoader.actionData.partsNum()

                            width: actionLoader.width
                            implicitHeight: 34
                            radius: 4
                            color: root.selectedFormula === actionLoader.actionData
                                ? "#cfe3ff"
                                : formulaHover.hovered ? "#eeeeee" : "transparent"

                            HoverHandler {
                                id: formulaHover
                            }

                            Row {
                                anchors.fill: parent
                                anchors.leftMargin: 7
                                anchors.rightMargin: 4
                                spacing: 4

                                Button {
                                    id: formulaNameButton

                                    width: parent.width
                                        - previousPartButton.width
                                        - partCounter.width
                                        - nextPartButton.width
                                        - parent.spacing * 3
                                    height: parent.height
                                    text: actionLoader.actionData.name()

                                    contentItem: Text {
                                        text: formulaNameButton.text
                                        font.pixelSize: 12
                                        color: "#282828"
                                        horizontalAlignment: Text.AlignLeft
                                        verticalAlignment: Text.AlignVCenter
                                        elide: Text.ElideRight
                                    }

                                    background: Item {}

                                    onClicked: {
                                        root.selectedFormula = actionLoader.actionData
                                        meActionsControl.applyFormula(actionLoader.actionData, currentPart)
                                    }
                                }

                                Button {
                                    id: previousPartButton

                                    y: (parent.height - height) / 2
                                    width: 24
                                    height: 24
                                    text: "\u25c0"

                                    onClicked: {
                                        root.selectedFormula = actionLoader.actionData
                                        formulaItem.currentPart = formulaItem.currentPart > 1
                                            ? formulaItem.currentPart - 1
                                            : formulaItem.partCount
                                        meActionsControl.applyFormula(actionLoader.actionData, currentPart)
                                    }
                                }

                                Text {
                                    id: partCounter

                                    width: 28
                                    height: parent.height
                                    text: formulaItem.currentPart + "/" + formulaItem.partCount
                                    font.pixelSize: 11
                                    color: "#444444"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                Button {
                                    id: nextPartButton

                                    y: (parent.height - height) / 2
                                    width: 24
                                    height: 24
                                    text: "\u25b6"

                                    onClicked: {
                                        root.selectedFormula = actionLoader.actionData
                                        formulaItem.currentPart = formulaItem.currentPart
                                            < formulaItem.partCount
                                            ? formulaItem.currentPart + 1
                                            : 1
                                        meActionsControl.applyFormula(actionLoader.actionData, currentPart)
                                    }
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: parent.width
                height: 1
                color: "#e4e4e4"
            }
        }
    }
}
