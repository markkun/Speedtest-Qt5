#include <QApplication>
#include <QCoreApplication>
#include <QQmlApplicationEngine>

#include "Downloader/Downloader.hpp"
#include "Downloader/DownloaderState.hpp"

int main(int argc, char *argv[]) {
  QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
  QApplication app(argc, argv);

  // Register C++ QML types
  qmlRegisterUncreatableType<DownloaderState>(
      "org.vagr9k.speedtest.downloader.state", 1, 0, "DownloaderState",
      "Not creatable enum type");
  qmlRegisterType<Downloader>("org.vagr9k.speedtest.downloader", 1, 0,
                              "Downloader");

  QQmlApplicationEngine engine;
  const QUrl url(QStringLiteral("qrc:/main.qml"));
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
          QCoreApplication::exit(-1);
      },
      Qt::QueuedConnection);
  engine.load(url);

  // Run the event loop
  return app.exec();
}
