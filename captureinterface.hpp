#ifndef CAPTUREINTERFACE_HPP
#define CAPTUREINTERFACE_HPP

/*
#include <QtCore/QObject>
#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QVariant>
// */
#include <QtDBus/QtDBus>

class CaptureTCLAPInterface//: public QDBusAbstractInterface
{
//    Q_OBJECT
public:
    static inline const char *staticInterfaceName()
    { return "org.acquisition.captureTCLAP"; }
    static inline const char *staticRequestName()
    { return "request"; }
    static inline const char *staticResponseName()
    { return "response"; }

//signals: // SIGNALS
//    void request(QString const a);
//    void response(QString const a);
};

#endif // CAPTUREINTERFACE_HPP
