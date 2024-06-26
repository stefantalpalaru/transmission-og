/*
 * This file Copyright (C) 2015 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#pragma once

#ifdef ENABLE_COM_INTEROP
#include "ComInteropHelper.h"
#endif
#ifdef ENABLE_DBUS_INTEROP
#include "DBusInteropHelper.h"
#endif

class QAxObject;
class QString;
class QVariant;

class InteropHelper {
public:
    bool isConnected() const;

    bool addMetainfo(QString const &metainfo);

    static void initialize();
    static void registerObject(QObject *parent);

private:
#ifdef ENABLE_DBUS_INTEROP
    DBusInteropHelper myDbusClient;
#endif
#ifdef ENABLE_COM_INTEROP
    ComInteropHelper myComClient;
#endif
};
