#ifndef DBUSRECEIVER_H
#define DBUSRECEIVER_H

#include <QObject>
#include <QtDBus/QtDBus>

class CameraManager;

class DBusReceiver: public QObject {
    Q_OBJECT
private:
    DBusReceiver();
public:
    DBusReceiver(const DBusReceiver&) = delete;
    void operator = (DBusReceiver const& ) = delete;

    CameraManager *manager = nullptr;

    static DBusReceiver& getInstance();

    static void setManager(CameraManager &_manager);

    void sendDbus(QString const a);

signals:
    void somesig(int somearg);

public slots:
    void dbusRequestSlot(const QString a);
};

#endif // DBUSRECEIVER_H
