#include <QDateTime>
#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QObject>
#include <QVector>
#include <numeric>

#include "DownloaderState.hpp"

// A C++ QML component allowing benchmarking remote data downloads
class Downloader : public QObject {
  Q_OBJECT
  // NOLINTNEXTLINE(readability-identifier-naming)
  Q_PROPERTY(DownloaderState::State state READ state WRITE setState NOTIFY
                 stateChanged)
  // NOLINTNEXTLINE(readability-identifier-naming)
  Q_PROPERTY(qint64 startTime READ startTime WRITE setStartTime NOTIFY
                 startTimeChanged)
  // NOLINTNEXTLINE(readability-identifier-naming)
  Q_PROPERTY(qint64 downloadSize READ downloadSize WRITE setDownloadSize NOTIFY
                 downloadSizeChanged)
  // NOLINTNEXTLINE(readability-identifier-naming)
  Q_PROPERTY(qint64 elapsedTime READ elapsedTime WRITE setElapsedTime NOTIFY
                 elapsedTimeChanged)
  // NOLINTNEXTLINE(readability-identifier-naming)
  Q_PROPERTY(
      double avgSpeed READ avgSpeed WRITE setAvgSpeed NOTIFY avgSpeedChanged)
public:
  Downloader();

  // QML accessible API
  Q_INVOKABLE void startBenchmark(QString uri);
  Q_INVOKABLE void stopBenchmark();

  // Exported properties
  DownloaderState::State state();
  void setState(DownloaderState::State state);

  qint64 startTime();
  void setStartTime(qint64 timestamp);

  qint64 downloadSize();
  void setDownloadSize(qint64 size);

  qint64 elapsedTime();
  void setElapsedTime(qint64 time);

  double avgSpeed();
  void setAvgSpeed(double speed);

  // Exported events
  void onFinish();
  void onProgress(qint64 received, qint64 total);

signals:
  void stateChanged();
  void startTimeChanged();
  void downloadSizeChanged();
  void elapsedTimeChanged();
  void avgSpeedChanged();
  void progress(double progress, double speed);
  void finish();

private:
  // Object init and reset
  void init();
  // Clean up for the received data
  void cleanupNetworkReply();
  // Network access handler
  QNetworkAccessManager *m_manager;
  // Reply data
  QNetworkReply *m_reply;
  // State
  DownloaderState::State m_state;
  // Stats
  qint64 m_lastReceivedByteCount;
  qint64 m_lastProgressTimeStamp;
  qint64 m_startTime;
  qint64 m_downloadSize;
  qint64 m_elapsedTime;
  double m_avgSpeed;
  QVector<double> m_speedHistory;
};
