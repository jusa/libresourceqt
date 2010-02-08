#ifndef DBUSCONNECTIONEVENTLOOP_H
#define DBUSCONNECTIONEVENTLOOP_H

#include <QObject>
#include <QList>
#include <QMultiHash>
#include <QHash>

#include <dbus/dbus.h>

//#define _DISPLAY_DEBUG_

#ifdef _DISPLAY_DEBUG_
	#include <stdlib.h>

	#define MYDEBUG()		MyDebug deb(__FUNCTION__)
	#define MYDEBUGC(...)	qDebug(__VA_ARGS__)

	class MyDebug
	{
	public:
		MyDebug(const char* func)
		{
			funcName = strdup(func);
			qDebug("--> Entering: %s() ...", funcName);
		}
		~MyDebug()
		{
			qDebug("<-- Leaving: %s() ...", funcName);
			free(funcName);
		}

	private:
		char* funcName;
	};
#else
	#define MYDEBUG()
	#define MYDEBUGC(...)
#endif

class QSocketNotifier;
class QTimerEvent;

/**
* This class is handling dbus notifications with QT events. QEventLoop must
*  be handled in order to handle dbus events.
* Usage: DBUSConnectionEventLoop myLoop; myLoop.addConnection(bus);
*/
class DBUSConnectionEventLoop : public QObject
{
	Q_OBJECT
private:
	Q_DISABLE_COPY(DBUSConnectionEventLoop)

public:
    DBUSConnectionEventLoop();
    virtual ~DBUSConnectionEventLoop();

public:
    /**
     * Add new dbus connection into handler.
     * \return true if everything went well.
     */
	bool addConnection(DBusConnection* conn);

    /**
     * Helper class for dbus watcher
     */
    class Watcher
    {
    public:
        Watcher() : watch(0), read(0), write(0) {}

        DBusWatch* 			watch;
        QSocketNotifier*	read;
        QSocketNotifier*	write;
    };

    typedef QMultiHash<int, Watcher> 	Watchers;
    typedef QHash<int, DBusTimeout*> 	Timeouts;
    typedef QList<DBusConnection*>		Connections;

    /**
     * DBusWatcher objects
     */
    Watchers 	watchers;

    /**
     * DBusTimeout objects
     */
    Timeouts 	timeouts;

    /**
     * DBusConnection objects
     */
    Connections	connections;

public slots:
    void readSocket(int fd);
    void writeSocket(int fd);
    void dispatch();

protected:
    void timerEvent(QTimerEvent *e);

    static dbus_bool_t addWatch(DBusWatch *watch, void *data);
    static void removeWatch(DBusWatch *watch, void *data);
    static void toggleWatch(DBusWatch *watch, void *data);
    static dbus_bool_t addTimeout(DBusTimeout *timeout, void *data);
    static void removeTimeout(DBusTimeout *timeout, void *data);
    static void toggleTimeout(DBusTimeout *timeout, void *data);
    static void wakeupMain(void *data);
};

#endif // DBUSCONNECTIONEVENTLOOP_H
