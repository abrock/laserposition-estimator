#include <string>
#include <iostream>
#include <unistd.h>

#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
#include <QtDBus/QtDBus>
#include <QApplication>

#include <ParallelTime/paralleltime.h>

#include "captureinterface.hpp"

class DBusSender : QObject {
  Q_OBJECT
public:
  ParallelTime t;
  DBusSender() {
    QDBusConnection::sessionBus().registerObject("/", this);
    QDBusConnection::sessionBus().connect(QString(), QString(),
                                          CaptureTCLAPInterface::staticInterfaceName(),
                                          CaptureTCLAPInterface::staticResponseName(),
                                          this, SLOT(responseSlot(QString, QString)));
    std::cout << "Finished setting up DbusSender" << std::endl;
  }

  void send(QString const a, QString const b) {
    QDBusMessage msg = QDBusMessage::createSignal("/", CaptureTCLAPInterface::staticInterfaceName(), CaptureTCLAPInterface::staticRequestName());
    msg << a << b;
    QDBusConnection::sessionBus().send(msg);
    t.start();
    std::cout << "Message was sent:" << std::endl
              << a.toStdString() << std::endl
              << b.toStdString() << std::endl;
  }

public slots:
  void responseSlot(QString a, QString b) {
    std::cout << "Got answer at time " << t.realTime() << ": " << a.toStdString() << ", " << b.toStdString() << std::endl;
    exit(EXIT_SUCCESS);
  }

signals:

  void capture(QString a, QString b);



};

int main(int argc, const char ** argv) {
  int fake_argc = 0;
  QApplication app(fake_argc, nullptr);
  std::string a;
  for (size_t ii = 0; ii < size_t(argc); ++ii) {
    a += std::string(argv[ii]) + " ";
  }

  if (!QDBusConnection::sessionBus().isConnected()) {
    qCritical() << "Cannot connect to the D-Bus session bus.\n";
    return EXIT_FAILURE;
  }

  DBusSender send;
  send.send(a.c_str(), "");
  std::cout << "Sending capture command with message " << a << std::endl;

  return app.exec();
}
#include "laserposition-dbus-sender.moc"
