#include "Downloader.hpp"

Downloader::Downloader() : m_reply(Q_NULLPTR) {
  m_manager = new QNetworkAccessManager(this);
  init();
}

void Downloader::init() {
  setState(DownloaderState::State::IDLE);
  setStartTime(0);
  setDownloadSize(0);
  setElapsedTime(0);
  setAvgSpeed(0);
  m_speedHistory.clear();

  m_lastReceivedByteCount = 0;
  m_lastProgressTimeStamp = 0;
}

void Downloader::startBenchmark(QString uri) {
  init();

  // Create a request object from URI
  QNetworkRequest request;
  request.setUrl(QUrl(uri));

  // Make a GET request
  m_reply = m_manager->get(request);

  // Connect the signals to memeber functions
  QObject::connect(m_reply, &QNetworkReply::downloadProgress, this,
                   &Downloader::onProgress);
  QObject::connect(m_reply, &QNetworkReply::finished, this,
                   &Downloader::onFinish);

  qint64 current_timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();
  // Store an initial progress tiemstamp to base the speed calculations on
  m_lastProgressTimeStamp = current_timestamp;
  // Store start time
  setStartTime(current_timestamp);

  // Update the state
  setState(DownloaderState::State::DOWNLOADING);
}

void Downloader::cleanupNetworkReply() {
  QObject::disconnect(m_reply, &QNetworkReply::downloadProgress, this,
                      &Downloader::onProgress);
  QObject::disconnect(m_reply, &QNetworkReply::finished, this,
                      &Downloader::onFinish);
  // Abort connections
  m_reply->abort();
  // Discard the recieved data by closing the IO device
  m_reply->close();
  // Request object deletion later (outside of signal handlers)
  m_reply->deleteLater();
  m_reply = Q_NULLPTR;
}

void Downloader::stopBenchmark() {
  cleanupNetworkReply();
  setState(DownloaderState::State::IDLE);
}

void Downloader::onFinish() {
  cleanupNetworkReply();

  setState(DownloaderState::State::FINISHED);
  emit finish();
}
void Downloader::onProgress(qint64 received, qint64 total) {
  qint64 timestamp = QDateTime::currentDateTime().toMSecsSinceEpoch();

  // Calculate speed
  double speed = (received - m_lastReceivedByteCount) /
                 (timestamp - m_lastProgressTimeStamp) * 1000;
  m_speedHistory.push_back(speed);

  // Calculate progress
  double current_progress =
      static_cast<double>(received) / static_cast<double>(total) * 100;

  m_lastReceivedByteCount = received;
  m_lastProgressTimeStamp = timestamp;

  // Update stats
  setDownloadSize(total);
  setElapsedTime(timestamp - startTime());
  setAvgSpeed(
      std::accumulate(m_speedHistory.begin(), m_speedHistory.end(), 0.0) /
      m_speedHistory.size());

  // Emit a signal for QML
  emit progress(current_progress, speed);
}

// Property setters and signals
DownloaderState::State Downloader::state() { return m_state; }

void Downloader::setState(DownloaderState::State state) {
  if (state == m_state)
    return;

  m_state = state;
  emit stateChanged();
}

qint64 Downloader::downloadSize() { return m_downloadSize; }

void Downloader::setDownloadSize(qint64 size) {
  if (size == m_downloadSize)
    return;

  m_downloadSize = size;

  emit downloadSizeChanged();
}

qint64 Downloader::startTime() { return m_startTime; }

void Downloader::setStartTime(qint64 timestamp) {
  if (timestamp == m_startTime)
    return;

  m_startTime = timestamp;

  emit startTimeChanged();
}

qint64 Downloader::elapsedTime() { return m_elapsedTime; }

void Downloader::setElapsedTime(qint64 time) {
  if (time == m_elapsedTime)
    return;

  m_elapsedTime = time;

  emit elapsedTimeChanged();
}

double Downloader::avgSpeed() { return m_avgSpeed; }

void Downloader::setAvgSpeed(double speed) {
  if (speed == m_avgSpeed)
    return;

  m_avgSpeed = speed;

  emit avgSpeedChanged();
}
