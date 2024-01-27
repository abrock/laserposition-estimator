import QtQuick 2.12
import QtQuick.Window 2.15
import QtQuick.Controls 1.4
import QtQuick.Controls.Styles 1.4
import QtQuick.Layouts 1.12
import Qt.labs.settings 1.0

Window {
    width: 640
    height: 480
    visible: true
    title: qsTr("Laserpoisition-Estimator")

    TabView {
        width: parent.width
        height: 20

        Tab {
            title: "Acquisition"
            anchors.topMargin: 9
            anchors.leftMargin: 9

            RowLayout {
                Settings {
                    category: "laserposition_estimator"
                    id: set
                    property int exposure: 10000
                    property int ref_a: 0
                    property int ref_b: 100
                    property int test_val : 50
                }
                ColumnLayout {
                    anchors.topMargin: 9
                    anchors.leftMargin: 9
                    RowLayout {
                        Text {
                            text: qsTr("Exposure time [us]")
                        }
                        SpinBox {
                            value: set.exposure
                            stepSize: 1
                            maximumValue: 300000
                            id: exposure
                            onValueChanged: {
                                set.exposure = value
                                cameraManager.setExposure(value)
                            }
                            Component.onCompleted: cameraManager.setExposure(value)
                        }
                    }
                    RowLayout {
                        Text {
                            text: "Position: "
                        }
                        Text {
                            text: "unknown"
                            id: position_text
                            Connections {
                                target: cameraManager
                                function onPositionDetected(val) {
                                    position_text.text = val;
                                }
                            }

                        }
                    }
                    RowLayout {
                        Text {
                            text: "Reference A: "
                        }
                        SpinBox {
                            value: set.ref_a
                            stepSize: 1
                            maximumValue: 300000
                            id: ref_a
                            onValueChanged: {
                                set.ref_a = value
                                cameraManager.setRefA(value)
                            }
                            Component.onCompleted: cameraManager.setRefA(value)
                        }
                        Button {
                            text: "Assign current position"
                            onClicked: cameraManager.assignRefA();
                        }
                        Text {
                            text: "unknown"
                            id: ref_a_text
                            Connections {
                                target: cameraManager
                                function onRefPosSetA(val) {
                                    ref_a_text.text = val;
                                }
                            }
                        }
                    }
                    RowLayout {
                        Text {
                            text: "Reference B: "
                        }
                        SpinBox {
                            value: set.ref_b
                            stepSize: 1
                            maximumValue: 300000
                            id: ref_b
                            onValueChanged: {
                                set.ref_b = value
                                cameraManager.setRefB(value)
                            }
                            Component.onCompleted: cameraManager.setRefB(value)
                        }
                        Button {
                            text: "Assign current position"
                            onClicked: cameraManager.assignRefB();
                        }
                        Text {
                            text: "unknown"
                            id: ref_b_text
                            Connections {
                                target: cameraManager
                                function onRefPosSetB(val) {
                                    ref_b_text.text = val;
                                }
                            }
                        }
                    }
                    RowLayout {
                        Text {
                            text: "Test value: "
                        }
                        SpinBox {
                            value: set.test_val
                            stepSize: 1
                            maximumValue: 300000
                            id: test_val
                            onValueChanged: {
                                set.test_val = value
                                cameraManager.setTestVal(value)
                            }
                            Component.onCompleted: cameraManager.setTestVal(value)
                        }
                    }
                    GridLayout {
                        columns: 2
                        Text {
                            text: "Expected: "
                            width: 200
                        }
                        Text {
                            text: "unknown"
                            id: test_exptected
                            Connections {
                                target: cameraManager
                                function onTestValEvaluated(expected, error) {
                                    test_exptected.text = expected;
                                }
                            }
                        }

                        Text {
                            text: "Actual: "
                            width: 200
                        }
                        Text {
                            text: "unknown"
                            id: test_actual
                            Connections {
                                target: cameraManager
                                function onPositionDetected(val) {
                                    test_actual.text = val;
                                }
                            }
                        }

                        Text {
                            text: "Error: "
                            width: 200
                        }
                        Text {
                            text: "unknown"
                            id: test_error
                            Connections {
                                target: cameraManager
                                function onTestValEvaluated(expected, error) {
                                    test_error.text = error;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

}
