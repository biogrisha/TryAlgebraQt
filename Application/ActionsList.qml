import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
    id: root

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
                    sourceComponent: parserAction

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
                                        root.visible = false;
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
                                        root.visible = false;
                                    }
                                }
                            }
                        }
                    }

                    Component {
                        id: formulaAction

                        Rectangle {
                            id: actionItem

                            width: actionLoader.width
                            implicitHeight: 34
                            radius: 4
                            color: actionHover.hovered ? "#eeeeee" : "transparent"

                            HoverHandler {
                                id: actionHover
                            }

                            Button {
                                id: formulaButton

                                y: (parent.height - height) / 2
                                width: 48
                                height: 24
                                text: qsTr("Parse")
                                hoverEnabled: true

                                contentItem: Text {
                                    text: actionLoader.actionData.name()
                                    font.pixelSize: 11
                                    color: "#ffffff"
                                    horizontalAlignment: Text.AlignHCenter
                                    verticalAlignment: Text.AlignVCenter
                                }

                                background: Rectangle {
                                    radius: 3
                                    color: formulaButton.down
                                        ? "#4e4e4e"
                                        : formulaButton.hovered ? "#666666" : "#585858"
                                }
                                onClicked: {

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
