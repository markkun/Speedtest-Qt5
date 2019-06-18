#pragma once
#include <QObject>

// Required derivation from QObject
class DownloaderState : public QObject {
  Q_OBJECT

public:
  explicit DownloaderState() = default; // Required for QML

  enum State { IDLE, DOWNLOADING, FINISHED };
  Q_ENUMS(State)
};
