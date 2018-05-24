import QtQuick 2.7
import QtQuick.Window 2.2
import QtQuick.Controls 1.4
import QtQuick.Controls 2.2
import an.framelessWindow 1.0

FramelessWindow
{
    id: root
    visible: true
    taskbarHint: true
    width: 640
    height: 480
    x: (Screen.desktopAvailableWidth - actualWidth) / 2
    y: (Screen.desktopAvailableHeight - actualHeight) / 2
    actualHeight: height
    actualWidth: width
    title: qsTr("MPS Chat 服务器")

    property var ipArray: new Array;

    function displayServerInfo(ip, port)
    {
        serverIP.text = "服务器 IP : " + ip;
        serverPort.text = "服务器 端口 : " + port;
    }

    function addMessage(msg)
    {
        messageText.text += "\n" + msg;
    }

    function addNewClient(ip)
    {
        ipArray.push(ip);
        myModel.append({ "ip": ip,
                         "username": "00000001",
                         "nickname": "MPS",
                         "state": "在线"});
        addMessage(ip + "已经连接...");
    }

    function removeClient(client)
    {
        var index = ipArray.indexOf(client);
        ipArray.splice(index, 1);
        myModel.remove(index);
    }

    /*Image
    {
        id: background
        source: "qrc:/9.jpg"
    }*/
    Rectangle
    {
        anchors.fill: parent
        radius: consoleWindow.radius
        color: "#FFCC99"
    }

    ResizeMouseArea
    {
        id: resizeMouseArea
        target: root
    }

    Row
    {
        width: 102
        height: 40
        anchors.right: parent.right
        anchors.rightMargin: 6
        anchors.top: parent.top
        anchors.topMargin: 6

        CusButton
        {
            id: menuButton
            width: 34
            height: 24

            onClicked:
            {
            }
            Component.onCompleted:
            {
                buttonNormalImage = "qrc:/ButtonImage/menu_normal.png";
                buttonPressedImage = "qrc:/ButtonImage/menu_down.png";
                buttonHoverImage = "qrc:/ButtonImage/menu_hover.png";
            }
        }

        CusButton
        {
            id: minButton
            width: 34
            height: 24

            onClicked:
            {
                root.showMinimized();
            }
            Component.onCompleted:
            {
                buttonNormalImage = "qrc:/ButtonImage/min_normal.png";
                buttonPressedImage = "qrc:/ButtonImage/min_down.png";
                buttonHoverImage = "qrc:/ButtonImage/min_hover.png";
            }
        }

        CusButton
        {
            id: closeButton
            width: 34
            height: 24

            onClicked:
            {
                root.close();
            }
            Component.onCompleted:
            {
                buttonNormalImage = "qrc:/ButtonImage/close_normal.png";
                buttonPressedImage = "qrc:/ButtonImage/close_down.png";
                buttonHoverImage = "qrc:/ButtonImage/close_hover.png";
                buttonDisableImage = "qrc:/ButtonImage/close_disable.png";
            }
        }
    }

    ListModel
    {
        id: myModel
    }

    Row
    {
        id: serverRect
        anchors.horizontalCenter: parent.horizontalCenter
        width: 400
        height: 45

        Text
        {
            id: serverIP
            height: 45
            width: 200
            color: "red"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "微软雅黑"
            font.pointSize: 11
        }

        Text
        {
            id: serverPort
            height: 45
            width: 200
            color: "red"
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignHCenter
            font.family: "微软雅黑"
            font.pointSize: 11
        }
    }

    TableView
    {
        id: tableView
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: serverRect.bottom
        anchors.bottom: consoleWindow.top
        anchors.bottomMargin: 20
        model: myModel
        backgroundVisible: false
        headerDelegate: Component
        {
            id: headerDelegate

            Rectangle
            {
                id: headerRect;
                height: 30
                width: 100
                border.color: "blue"
                color: styleData.selected ? "gray" : "transparent"
                radius: 3

                Text
                {
                    text: styleData.value
                    anchors.centerIn: parent
                    font.family: "微软雅黑"
                    font.pointSize: 10
                    color: "red"
                }
            }
        }
        rowDelegate: Component
        {
            id: rowDelegate
            Rectangle
            {
                color: "transparent"
                height: 42
            }
        }
        itemDelegate: Component
        {
            id: itemDelegate

            Rectangle
            {
                id: tableCell
                anchors.fill: parent
                anchors.topMargin: 4
                anchors.leftMargin: 1
                border.color: "blue"
                radius: 3
                color: styleData.selected ? "#44EEEEEE" : "#66B5E61D"

                Text
                {
                    id: textID
                    text: styleData.value
                    font.family: "微软雅黑"
                    anchors.fill: parent
                    color: "black"
                    elide: Text.ElideRight
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignHCenter
                }
            }
        }


        TableViewColumn
        {
            role: "username"
            title: "帐号"
            width: 100
        }

        TableViewColumn
        {
            role: "ip"
            title: "连接IP"
            width: 150
        }

        TableViewColumn
        {
            role: "nickname"
            title: "昵称"
            width: 100
        }

        TableViewColumn
        {
            role: "state"
            title: "状态"
            width: 100
        }
    }

    Rectangle
    {
        id: consoleWindow
        opacity: 0.8
        radius: 10
        clip: true
        height: 150
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        Flickable
        {
            id: flick
            //interactive: false
            anchors.fill: parent
            anchors.margins: 15
            contentHeight: messageText.contentHeight
            contentWidth: messageText.contentWidth

            Text
            {
                id: messageText
                width: flick.width
                height: flick.height
                font.family: "微软雅黑";
                text: "服务器运行中..."
                color: "#400040"
                onTextChanged: flick.contentY = Math.max(0, contentHeight - height);
            }

            ScrollBar.vertical: ScrollBar
            {
                width: 12
                policy: ScrollBar.AlwaysOn
            }
        }
    }
}
