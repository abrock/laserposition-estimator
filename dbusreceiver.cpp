#include "cameramanager.h"
#include "misc.h"

#include "dbusreceiver.h"

#include <tclap/CmdLine.h>

#include "captureinterface.hpp"

#include <ParallelTime/paralleltime.h>

DBusReceiver::DBusReceiver() {
  std::cout << "Setting up DBusReceiver" << std::endl;
  QDBusConnection::sessionBus().registerObject("/", this);
  QDBusConnection::sessionBus().connect(QString(), QString(),
                                        CaptureTCLAPInterface::staticInterfaceName(),
                                        CaptureTCLAPInterface::staticRequestName(),
                                        this, SLOT(dbusRequestSlot(QString)));
  std::cout << "Finished setting up DBusReceiver" << std::endl;
}

DBusReceiver &DBusReceiver::getInstance() {
  static DBusReceiver instance;
  std::cout << "Produced an instance of DBusReceiver" << std::endl;
  return instance;
}

void DBusReceiver::setManager(CameraManager &_manager)
{
  getInstance().manager = &_manager;
}

namespace {
void requestThread(QString const a) {
  std::stringstream return_msg;
  return_msg << "Message received: " << std::endl
             << a.toStdString() << std::endl;

  std::cout << "Received message: " << std::endl
            << a.toStdString() << std::endl;

  DBusReceiver &dbus = DBusReceiver::getInstance();

  try {

    TCLAP::CmdLine cmd("Cmdline for DBus command parsing", ' ', "0.1");
    cmd.setExceptionHandling(false);

    TCLAP::ValueArg<double> pos_arg(
          "p", "position", "Position of the machine", true, 0, "", cmd);

    TCLAP::UnlabeledMultiArg<std::string> comment_arg("comment", "comment", false, "", cmd);

    std::vector<std::string> vec = Misc::splitString(a.toStdString(), ' ');
    cmd.parse(vec);

    std::string comment;
    if (comment_arg.isSet() && !comment_arg.getValue().empty()) {
      for (std::string const &str : comment_arg.getValue()) {
        comment += str + " ";
      }
    }

    dbus.manager->setTestVal(pos_arg.getValue());
    dbus.manager->storeLog();

  }
  catch (TCLAP::ArgException const &e) {
    return_msg << " got an ArgException: " << std::endl << e.what();
  }
  catch (std::exception const& e) {
    return_msg << " got an exception: " << std::endl << e.what();
  }
  catch (...) {
    return_msg << " got an unknown exception: " << std::endl;
  }

  ParallelTime t;

  double time_out = 20;
  while (t.realTime() < time_out) {
    usleep(200'000);
    if (dbus.manager->get_n_samples_until_completion() <= 0) {
      break;
    }
  }

  if (dbus.manager->get_n_samples_until_completion() > 0) {
    return_msg << "Warning: After " << time_out << "s the sampling hasn't finished." << std::endl;
  }

  dbus.sendDbus(QString::fromStdString(return_msg.str()));
}
} // anonymout namespace

void DBusReceiver::dbusRequestSlot(QString const a) {
  std::thread t(requestThread, a);
  t.detach();
}

void DBusReceiver::sendDbus(const QString a) {
  std::cout << "Sending response: " << a.toStdString() << std::endl;
  QDBusMessage msg = QDBusMessage::createSignal("/", CaptureTCLAPInterface::staticInterfaceName(), CaptureTCLAPInterface::staticResponseName());
  QString b;
  msg << a << b;
  QDBusConnection::sessionBus().send(msg);
}
