import QtQuick 2.12
import QtQuick.Window 2.12
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import QtCharts 2.3

// Import our C++ components
import org.vagr9k.speedtest.downloader 1.0
import org.vagr9k.speedtest.downloader.state 1.0

Window {
    id: mainWindow
    visible: true
    minimumWidth: 800
    minimumHeight: 700
    title: qsTr("Speedtest")

    // Initialize the downloader component
    Downloader {
        id: mainDownloader
        // On each "progress" event, append another point to the graph
        onProgress: {
            progressGraph.append(progress, Math.round(speed / 1024))
            if (progressGraphY.max < speed / 1024) {
                progressGraphY.max = progressGraphY.max * 2
            }
        }
    }

    // Container for the UI
    ColumnLayout {
        id: uiContainer
        width: parent.width
        height: parent.height

        // Benchmark view
        ColumnLayout {
            id: benchmarkView
            Layout.fillHeight: true
            Layout.fillWidth: true
            // Hide when the downloader is idle
            visible: (mainDownloader.state === DownloaderState.IDLE ? false : true)
            ChartView {
                id: chart
                antialiasing: true
                Layout.fillHeight: true
                Layout.fillWidth: true

                LineSeries {
                    id: progressGraph
                    name: "Speed (KB/s)"
                    axisX: ValueAxis {
                        id: progressGraphX
                        min: 0
                        max: 100
                    }
                    axisY: ValueAxis {
                        id: progressGraphY
                        min: 0
                        max: 10000
                    }
                }
            }
            // Display download stats
            RowLayout {
                id: benchmarkInfo
                Layout.alignment: Qt.AlignCenter

                spacing: 30
                Text {
                    text: "Started: " + new Date(mainDownloader.startTime).toTimeString()
                }
                Text {
                    text: "Time elapsed: " + Math.round(
                              mainDownloader.elapsedTime) + "ms"
                }
                Text {
                    text: "Size: " + Math.round(
                              mainDownloader.downloadSize / 1024 / 1024) + "MB"
                }
                Text {
                    text: "Average speed: " + Math.round(
                              mainDownloader.avgSpeed / 1024) + "KB/s"
                }
            }
        }

        // Instructions view
        ColumnLayout {
            id: readmeContainer
            Layout.alignment: Qt.AlignCenter
            // Hide when benchmark is in progress or finished
            visible: (mainDownloader.state === DownloaderState.IDLE ? true : false)
            Text {
                font.bold: true
                text: "Input a remote resource URL to start the speedtest"
            }
            Text {
                text: "You can use on of the following examples:"
            }

            Text {
                property string url: 'https://speed.hetzner.de/100MB.bin'
                text: "<a href='" + url + "'>" + url + "</a>"
                onLinkActivated: {
                    urlInput.text = url
                }
            }
            Text {
                property string url: 'http://www.ovh.net/files/100Mb.dat'
                text: "<a href='" + url + "'>" + url + "</a>"
                onLinkActivated: {
                    urlInput.text = url
                }
            }
        }

        // App controls
        RowLayout {
            id: userInputContainer
            Layout.alignment: Qt.AlignBottom
            Layout.fillWidth: true

            TextField {
                id: urlInput
                Layout.fillWidth: true
                placeholderText: qsTr("Remote data URL")
                // Only allow editing when benchmark isn't in progress
                readOnly: (mainDownloader.state === DownloaderState.DOWNLOADING)
            }
            Button {
                id: startButton
                text: getButtonText()
                // Disable the button in case of an empty URL input
                enabled: (urlInput.text.length > 0)

                onClicked: {
                    // Clear the graph
                    progressGraph.clear()

                    // Start/Stop benchmark depending on state
                    if (mainDownloader.state === DownloaderState.DOWNLOADING) {
                        mainDownloader.stopBenchmark()
                    } else {

                        mainDownloader.startBenchmark(urlInput.text)
                    }
                }

                // Pick and return the correct button text depending on downloader state
                function getButtonText() {
                    if (mainDownloader.state === DownloaderState.DOWNLOADING)
                        return qsTr("Cancel speedtest")

                    return qsTr("Start speedtest")
                }
            }
        }
    }
}
