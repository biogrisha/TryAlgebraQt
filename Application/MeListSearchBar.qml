import QtQuick
import QtQuick.Controls.Basic
import com.Application

Rectangle {
	id: root
	required property var docControl
	required property var mathCanvas

	visible: false
	width: 200
	height: searchBar.height + mathElementsList.height

	color: "#d0d0d0"
	radius: 4

	function open() {
		meSearchBar.text = ""
		root.visible = true
		meSearchBar.forceActiveFocus()

	}

	function close() {
		root.visible = false
		mathCanvas.forceActiveFocus()
	}

	SortFilterProxyModel {
		id: filteredMeList
		model: UserApplication.applicationModel().meListModel();
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

	Column {
		anchors.fill: parent

		Rectangle {
			id: searchBar

			width: parent.width
			height: 50
			color: "#d0d0d0"

			TextField {
				id: meSearchBar

				anchors.fill: parent
				anchors.margins: 4

				background: null
				placeholderText: "Search..."

				onTextChanged: {
					filteredMeList.invalidate()
					mathElementsList.currentIndex = mathElementsList.count > 0 ? 0 : -1
				}

				Keys.onPressed: (event) => {
					if (event.key === Qt.Key_Down) {
						if (mathElementsList.currentIndex < mathElementsList.count - 1)
							mathElementsList.currentIndex++

						event.accepted = true
					}
					else if (event.key === Qt.Key_Up) {
						if (mathElementsList.currentIndex > 0)
							mathElementsList.currentIndex--

						event.accepted = true
					}
					else if (event.key === Qt.Key_Return || event.key === Qt.Key_Enter) {
						if (mathElementsList.count > 0) {
							var item = mathElementsList.currentItem

							if (item)
								item.select()
						}

						event.accepted = true
					}
					if(event.key === Qt.Key_Escape)
					{
						close()
						event.accepted = true
					}
				}

			}
		}

		ListView {
			id: mathElementsList

			width: parent.width
			height: 300

			focus: false
			model: filteredMeList
			clip: true

			currentIndex: count > 0 ? 0 : -1
			//used to prevent underlying items handle wheel events
			WheelHandler {
				id: wheelHandler

				target: null
				blocking: true

				onWheel: (event) => {
					var delta = event.pixelDelta.y !== 0
						? event.pixelDelta.y
						: event.angleDelta.y
					
					mathElementsList.contentY -= delta
					
					mathElementsList.returnToBounds()
				}
			}

			ScrollBar.vertical: ScrollBar {
				policy: ScrollBar.AsNeeded
			}

			delegate: Button {
				id: button

				required property string meName
				required property point viewPos
				required property size viewSize

				width: mathElementsList.width
				height: viewSize.height + 10
				
				background: Rectangle {
					color: button.ListView.isCurrentItem
						? "#b0b0b0"
						: "transparent"

					radius: 3
				}

				function select() {
					docControl.addMeByName(meName)
					close()
				}

				contentItem: Row {
					spacing: 8

					Item {
						width: button.viewSize.width
						height: button.viewSize.height
						clip: true

						Image {
							source: "image://MeAtlas/atlas"

							width: 500
							height: 282

							x: -button.viewPos.x
							y: -button.viewPos.y
						}
					}

					Text {
						text: button.meName
						anchors.verticalCenter: parent.verticalCenter
					}
				}

				onClicked: {
					docControl.addMeByName(button.meName)
					close()
				}
			}
		}
	}
}