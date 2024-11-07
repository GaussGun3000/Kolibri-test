import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.3
import QtQuick.Layouts 1.3
import QtQml 2.15

ApplicationWindow {
    visible: true
    width: mainColumn.implicitWidth + 20
    height: mainColumn.implicitHeight + 20
    title: "File Processor"

    property string fileMask: ""
    property bool deleteAfterProcessing: false
    property string outputFolder: ""
    property bool overwriteFiles: false
    property int intervalSeconds: 0
    property string operationType: "OR"
    property string operationValue: ""

    ColumnLayout {
        id: mainColumn
        anchors.centerIn: parent
        spacing: 20

        TextField {
            id: fileMaskInput
            placeholderText: "File Mask (*.txt, ...)"
            Layout.fillWidth: true
            onTextChanged: fileMask = text
        }

        CheckBox {
            id: deleteCheckbox
            text: "Delete input files"
            Layout.fillWidth: true
            onCheckedChanged: deleteAfterProcessing = checked
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 10

            TextField {
                id: outputFolderInput
                placeholderText: "Output Folder Path"
                text: outputFolder
                readOnly: true
                focus: false
                Layout.fillWidth: true
            }

            Button {
                text: "Browse..."
                onClicked: fileDialog.open()
                Layout.preferredWidth: implicitWidth
            }

            FileDialog {
                id: fileDialog
                title: "Select Output Folder"
                folder: shortcuts.home
                selectFolder: true
                onAccepted: {
                    outputFolder = fileDialog.fileUrl.toString()
                    if (outputFolder.startsWith("file:///")) {
                               outputFolder = outputFolder.substring(8)
                    }
                    outputFolderInput.text = outputFolder
                }
            }
        }

        ComboBox {
            id: overwriteComboBox
            model: ["Create copies", "Overwrite"]
            Layout.fillWidth: true
            onCurrentIndexChanged: overwriteFiles = currentIndex === 1
        }

        Row {
            Layout.fillWidth: true
            spacing: 10

            ColumnLayout {
                spacing: 5
                Layout.fillWidth: true
                Label { text: "Process Interval (seconds):" }
                TextField {
                    id: intervalInput
                    placeholderText: "0 for one-time process"
                    inputMethodHints: Qt.ImhDigitsOnly
                    Layout.fillWidth: true
                    onTextChanged: intervalSeconds = parseInt(text) || 0
                }
            }
        }

        ComboBox {
            id: operationTypeComboBox
            model: ["OR", "AND", "XOR"]
            Layout.fillWidth: true
            onCurrentIndexChanged: operationType = currentText
        }

        TextField {
            id: operationValueInput
            placeholderText: "8-byte value"
            validator: RegExpValidator { regExp: /[ -~]{0,8}/ } // ASCII range for printable characters
            Layout.fillWidth: true
            onTextChanged: {
                if (text.length > 8) {
                    text = text.slice(0, 8);
                }
                operationValue = text;
            }
        }

        Button {
            text: "Start Processing"
            Layout.fillWidth: true
            enabled: fileMask.length > 0 && outputFolder.length > 0 && operationValue.length === 8
            onClicked: {
                processor.startProcessing(fileMask, deleteAfterProcessing, outputFolder, overwriteFiles, intervalSeconds, operationType, operationValue)
            }
        }
        Label {
               id: countdownLabel
               text: "Timer: " + processor.secondsLeft
           }

           Connections {
               target: processor
               function onCountdownUpdate(secondsLeft) {
                   countdownLabel.text = "Timer: " + secondsLeft;
               }
           }
    }
}
