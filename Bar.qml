import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.0
import QtQuick.Extras 1.4
//import QtQuick.Controls.Styles 1.4
//import QtQuick.Extras 1.4
//import QtQuick.Extras.Private 1.0
//import QtGraphicalEffects 1.0
import QtCharts 2.3

Window {
    id: winapp
    visible: true
    width: 1000
    height: 630
    color: "#1b2153"
    title: qsTr("FirstProject")

    Gauge {
        id: gauge1
        x: 20
        y: 53
        width: 44
        height: 547
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 30
        value: 500
        minimumValue: -5000
        maximumValue: 5000
        tickmarkStepSize: 500
        formatValue: function(value) {
            return value.toFixed(0);
        }
        property double minBarValue: 0
        style: GaugeStyle{
            valueBar: Item {
                 implicitWidth: 16
                 Rectangle {
                    width: parent.width
                    height: {
                        var maxSpan = Math.abs(gauge1.maximumValue) + Math.abs(gauge1.minimumValue)
                        var fill = Math.abs(gauge1.value)
                        var fillPercent = fill/maxSpan

                        var fillHeight = gauge1.height * fillPercent
                        return fillHeight - fillHeight/50
                    }
                    Component.onCompleted: {
                        if (gauge1.value < 0) {
                            y = -height
                        }
                    }
                 }
             }
        }
    }
}
