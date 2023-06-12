/*
 * This file Copyright (C) 2007-2014 Mnemosyne LLC
 *
 * It may be used under the GNU GPL versions 2 or 3
 * or any future license endorsed by Mnemosyne LLC.
 *
 */

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include <event2/util.h>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <miniupnpc/upnperrors.h>

#include "transmission.h"
#include "log.h"
#include "net.h"
#include "port-forwarding.h"
#include "session.h"
#include "tr-assert.h"
#include "upnp.h"
#include "utils.h"

static char const* getKey(void)
{
    return _("Port Forwarding (UPnP)");
}

typedef enum
{
    TR_UPNP_IDLE,
    TR_UPNP_ERR,
    TR_UPNP_DISCOVER,
    TR_UPNP_MAP,
    TR_UPNP_UNMAP
}
tr_upnp_state;

struct tr_upnp
{
    bool hasDiscovered;
    struct UPNPUrls urls;
    struct IGDdatas data;
    int port;
    char lanaddr[16];
    struct {
        const char *proto;
        char proto_no[8];
        char id[8];
    } pinhole[2]; // TCP and UDP
    bool isMapped;
    tr_upnp_state state;
};

/**
***
**/

tr_upnp* tr_upnpInit(void)
{
    tr_upnp* ret = tr_new0(tr_upnp, 1);

    ret->state = TR_UPNP_DISCOVER;
    ret->port = -1;
    ret->pinhole[0].proto = "TCP";
    ret->pinhole[1].proto = "UDP";
    tr_snprintf(ret->pinhole[0].proto_no, sizeof(ret->pinhole[0].proto_no), "%d", IPPROTO_TCP);
    tr_snprintf(ret->pinhole[1].proto_no, sizeof(ret->pinhole[1].proto_no), "%d", IPPROTO_UDP);

    return ret;
}

void tr_upnpClose(tr_upnp* handle)
{
    TR_ASSERT(!handle->isMapped);
    TR_ASSERT(handle->state == TR_UPNP_IDLE || handle->state == TR_UPNP_ERR || handle->state == TR_UPNP_DISCOVER);

    if (handle->hasDiscovered)
    {
        FreeUPNPUrls(&handle->urls);
    }

    tr_free(handle);
}

/**
***  Wrappers for miniupnpc functions
**/

static struct UPNPDev* tr_upnpDiscover(int msec)
{
    struct UPNPDev* ret;
    bool have_err;

#if (MINIUPNPC_API_VERSION >= 8) /* adds ipv6 and error args */
    int err = UPNPDISCOVER_SUCCESS;

#if (MINIUPNPC_API_VERSION >= 14) /* adds ttl */
    ret = upnpDiscover(msec, NULL, NULL, 0, 0, 2, &err);
#else
    ret = upnpDiscover(msec, NULL, NULL, 0, 0, &err);
#endif

    have_err = err != UPNPDISCOVER_SUCCESS;
#else
    ret = upnpDiscover(msec, NULL, NULL, 0);
    have_err = ret == NULL;
#endif

    if (have_err)
    {
        tr_logAddNamedDbg(getKey(), "upnpDiscover failed (errno %d - %s)", errno, tr_strerror(errno));
    }

    return ret;
}

static int tr_upnpGetSpecificPortMappingEntry(tr_upnp* handle, char const* proto)
{
    int err;
    char intClient[16];
    char intPort[16];
    char portStr[16];

    *intClient = '\0';
    *intPort = '\0';

    tr_snprintf(portStr, sizeof(portStr), "%d", (int)handle->port);

    tr_logAddNamedInfo(getKey(), "Checking for an existing port mapping: %s %s", proto, portStr);

#if (MINIUPNPC_API_VERSION >= 10) /* adds remoteHost arg */
    err = UPNP_GetSpecificPortMappingEntry(handle->urls.controlURL, handle->data.first.servicetype, portStr, proto,
        NULL /*remoteHost*/, intClient, intPort, NULL /*desc*/, NULL /*enabled*/, NULL /*duration*/);
#elif (MINIUPNPC_API_VERSION >= 8) /* adds desc, enabled and leaseDuration args */
    err = UPNP_GetSpecificPortMappingEntry(handle->urls.controlURL, handle->data.first.servicetype, portStr, proto, intClient,
        intPort, NULL /*desc*/, NULL /*enabled*/, NULL /*duration*/);
#else
    err = UPNP_GetSpecificPortMappingEntry(handle->urls.controlURL, handle->data.first.servicetype, portStr, proto, intClient,
        intPort);
#endif

    return err;
}

static int tr_upnpAddPortMapping(tr_upnp const* handle, char const* proto, tr_port port, char const* desc)
{
    int err;
    char portStr[16];

    tr_snprintf(portStr, sizeof(portStr), "%d", (int)port);

    errno = 0;
#if (MINIUPNPC_API_VERSION >= 8)
    err = UPNP_AddPortMapping(handle->urls.controlURL, handle->data.first.servicetype, portStr, portStr, handle->lanaddr, desc,
        proto, NULL, NULL);
#else
    err = UPNP_AddPortMapping(handle->urls.controlURL, handle->data.first.servicetype, portStr, portStr, handle->lanaddr, desc,
        proto, NULL);
#endif

    if (err != 0)
    {
        tr_logAddNamedError(getKey(), "%s port forwarding failed with error %d (errno %d - %s)", proto, err, errno,
            tr_strerror(errno));
    }

    return err;
}

static void tr_upnpDeletePortMapping(tr_upnp const* handle, char const* proto, tr_port port)
{
    char portStr[16];

    tr_snprintf(portStr, sizeof(portStr), "%d", (int)port);

    UPNP_DeletePortMapping(handle->urls.controlURL, handle->data.first.servicetype, portStr, proto, NULL);
}

static bool tr_upnpCanPinhole(tr_upnp const* handle)
{
    unsigned char const* ipv6 = tr_globalIPv6();
    if (ipv6 == NULL)
    {
        return false;
    }

    int firewall_enabled = 0, inbound_pinhole_allowed = 0;
    UPNP_GetFirewallStatus(handle->urls.controlURL_6FC, handle->data.IPv6FC.servicetype, &firewall_enabled, &inbound_pinhole_allowed);
    if (firewall_enabled == 0 || inbound_pinhole_allowed == 0)
    {
        return false;
    }

    return true;
}

static void tr_upnpDeletePinholes(tr_upnp* handle)
{
    int res;

    for (int i = 0; i < 2; i++)
    {
        if (handle->pinhole[i].id[0] != '\0')
        {
            res = UPNP_DeletePinhole(handle->urls.controlURL_6FC, handle->data.IPv6FC.servicetype, handle->pinhole[i].id);
            if (res != UPNPCOMMAND_SUCCESS)
            {
                tr_logAddNamedError(getKey(), "[%s] %s: %d (%s)", handle->pinhole[i].proto, _("IPv6 pinhole deletion failed with error"), res, strupnperror(res));

                // Try to update the lease time to 1s.

                res = UPNP_UpdatePinhole(handle->urls.controlURL_6FC, handle->data.IPv6FC.servicetype, handle->pinhole[i].id, "1");
                if (res != UPNPCOMMAND_SUCCESS)
                {
                    tr_logAddNamedError(getKey(), "[%s] %s: %d (%s)", handle->pinhole[i].proto, _("IPv6 pinhole updating failed with error"), res, strupnperror(res));
                }
                else
                {
                    tr_logAddNamedInfo(getKey(), "[%s] %s: (%s: %s, 1s)", handle->pinhole[i].proto, _("IPv6 pinhole updated"), _("unique ID"), handle->pinhole[i].id);
                }
            }
            else
            {
                tr_logAddNamedInfo(getKey(), "[%s] %s (%s: %s)", handle->pinhole[i].proto, _("IPv6 pinhole deleted"), _("unique ID"), handle->pinhole[i].id);
            }

            memset(handle->pinhole[i].id, 0, sizeof(handle->pinhole[i].id));
        }
    }
}

#define TR_PINHOLE_LEASE_TIME "3600"

static void tr_upnpAddOrUpdatePinholes(tr_upnp* handle, tr_port port)
{
    int res;
    unsigned char const* ipv6 = tr_globalIPv6();
    char ipv6_str[INET6_ADDRSTRLEN];
    evutil_inet_ntop(AF_INET6, ipv6, ipv6_str, INET6_ADDRSTRLEN);

    char port_str[16];
    tr_snprintf(port_str, sizeof(port_str), "%d", (int)port);

    if (handle->pinhole[0].id[0] == '\0' || handle->pinhole[1].id[0] == '\0')
    {
        // First time being called this session.

        for (int i = 0; i < 2; i++)
        {
            res = UPNP_AddPinhole(handle->urls.controlURL_6FC, handle->data.IPv6FC.servicetype, "::", port_str, ipv6_str, port_str, handle->pinhole[i].proto_no, TR_PINHOLE_LEASE_TIME, handle->pinhole[i].id);
            if (res != UPNPCOMMAND_SUCCESS)
            {
                tr_logAddNamedError(getKey(), "[%s] %s: %d (%s) ([::]:%s -> [%s]:%s, %ss)", handle->pinhole[i].proto, _("IPv6 pinhole punching failed with error"), res, strupnperror(res), port_str, ipv6_str, port_str, TR_PINHOLE_LEASE_TIME);
            }
            else
            {
                tr_logAddNamedInfo(getKey(), "[%s] %s: [::]:%s -> [%s]:%s (%s: %s, %ss)", handle->pinhole[i].proto, _("IPv6 pinhole added"), port_str, ipv6_str, port_str, _("unique ID"), handle->pinhole[i].id, TR_PINHOLE_LEASE_TIME);
            }
        }

        // I have a Fritzbox 7590 that, when trying to add a pinhole that
        // already exists, returns the highest unique ID in its list, even if
        // it's from a different pinhole.
        //
        // Since we always add pinholes in pairs, we can check if this is
        // happening by seeing if the two IDs we got back are equal. Then we
        // assume that no new pinholes have been added by some other program.

        if (strncmp(handle->pinhole[0].id, handle->pinhole[1].id, sizeof(handle->pinhole[0].id)) == 0)
        {
            errno = 0;
            unsigned long udp_id = strtoul(handle->pinhole[1].id, NULL, 10);
            if (errno == 0 && udp_id > 0)
            {
                // The UPnP protocol specifies that pinhole IDs are 16 bit unsigned integers.
                tr_snprintf(handle->pinhole[0].id, sizeof(handle->pinhole[0].id), "%u", (unsigned int)(udp_id - 1));
                tr_logAddNamedInfo(getKey(), "%s: %s", _("correcting TCP pinhole ID to"), handle->pinhole[0].id);
            }
        }
    }
    else
    {
        // Update existing pinholes.

        for (int i = 0; i < 2; i++)
        {
            res = UPNP_UpdatePinhole(handle->urls.controlURL_6FC, handle->data.IPv6FC.servicetype, handle->pinhole[i].id, TR_PINHOLE_LEASE_TIME);
            if (res != UPNPCOMMAND_SUCCESS)
            {
                tr_logAddNamedError(getKey(), "[%s] %s: %d (%s)", handle->pinhole[i].proto, _("IPv6 pinhole updating failed with error"), res, strupnperror(res));
		// Delete them so they get created again.
		tr_upnpDeletePinholes(handle);
            }
            else
            {
                tr_logAddNamedInfo(getKey(), "[%s] %s: [::]:%s -> [%s]:%s (%s: %s, %ss)", handle->pinhole[i].proto, _("IPv6 pinhole updated"), port_str, ipv6_str, port_str, _("unique ID"), handle->pinhole[i].id, TR_PINHOLE_LEASE_TIME);
            }
        }
    }
}

/**
***
**/

enum
{
    UPNP_IGD_NONE = 0,
    UPNP_IGD_VALID_CONNECTED = 1,
    UPNP_IGD_VALID_NOT_CONNECTED = 2,
    UPNP_IGD_INVALID = 3
};

int tr_upnpPulse(tr_upnp* handle, int port, bool isEnabled, bool doPortCheck)
{
    int ret;

    if (isEnabled && handle->state == TR_UPNP_DISCOVER)
    {
        struct UPNPDev* devlist;

        devlist = tr_upnpDiscover(2000);

        errno = 0;

        if (UPNP_GetValidIGD(devlist, &handle->urls, &handle->data, handle->lanaddr,
            sizeof(handle->lanaddr)) == UPNP_IGD_VALID_CONNECTED)
        {
            tr_logAddNamedInfo(getKey(), _("Found Internet Gateway Device \"%s\""), handle->urls.controlURL);
            tr_logAddNamedInfo(getKey(), _("Local Address is \"%s\""), handle->lanaddr);
            handle->state = TR_UPNP_IDLE;
            handle->hasDiscovered = true;
        }
        else
        {
            handle->state = TR_UPNP_ERR;
            tr_logAddNamedDbg(getKey(), "UPNP_GetValidIGD failed (errno %d - %s)", errno, tr_strerror(errno));
            tr_logAddNamedDbg(getKey(), "If your router supports UPnP, please make sure UPnP is enabled!");
        }

        freeUPNPDevlist(devlist);
    }

    if (handle->state == TR_UPNP_IDLE || handle->state == TR_UPNP_ERR)
    {
        if (handle->isMapped && (!isEnabled || handle->port != port))
        {
            handle->state = TR_UPNP_UNMAP;
        }
    }

    if (isEnabled && handle->isMapped && doPortCheck)
    {
        if (tr_upnpGetSpecificPortMappingEntry(handle, "TCP") != UPNPCOMMAND_SUCCESS ||
            tr_upnpGetSpecificPortMappingEntry(handle, "UDP") != UPNPCOMMAND_SUCCESS)
        {
            tr_logAddNamedInfo(getKey(), _("Port %d isn't forwarded"), handle->port);
            handle->isMapped = false;
        }

        // There is no reliable way to check for an existing inbound pinhole,
        // because the "CheckPinholeWorking" UPnP action is optional and not all
        // routers implement it, so we unconditionally update the lease time
        // for the pinholes we created.
        if (tr_upnpCanPinhole(handle))
        {
            tr_upnpAddOrUpdatePinholes(handle, port);
        }
    }

    if (handle->state == TR_UPNP_UNMAP)
    {
        tr_upnpDeletePortMapping(handle, "TCP", handle->port);
        tr_upnpDeletePortMapping(handle, "UDP", handle->port);

        tr_logAddNamedInfo(getKey(), _("Stopping port forwarding through \"%s\", service \"%s\""), handle->urls.controlURL,
            handle->data.first.servicetype);

        tr_upnpDeletePinholes(handle);

        handle->isMapped = false;
        handle->state = TR_UPNP_IDLE;
        handle->port = -1;
    }

    if (handle->state == TR_UPNP_IDLE || handle->state == TR_UPNP_ERR)
    {
        if (isEnabled && !handle->isMapped)
        {
            handle->state = TR_UPNP_MAP;
        }
    }

    if (handle->state == TR_UPNP_MAP)
    {
        int err_tcp = -1;
        int err_udp = -1;
        errno = 0;

        if (handle->urls.controlURL == NULL)
        {
            handle->isMapped = false;
        }
        else
        {
            char desc[64];
            tr_snprintf(desc, sizeof(desc), "%s at %d", TR_NAME, port);

            err_tcp = tr_upnpAddPortMapping(handle, "TCP", port, desc);
            err_udp = tr_upnpAddPortMapping(handle, "UDP", port, desc);

            if (tr_upnpCanPinhole(handle))
            {
                tr_upnpAddOrUpdatePinholes(handle, port);
            }

            handle->isMapped = err_tcp == 0 || err_udp == 0;
        }

        tr_logAddNamedInfo(getKey(), _("Port forwarding through \"%s\", service \"%s\". (local address: %s:%d)"),
            handle->urls.controlURL, handle->data.first.servicetype, handle->lanaddr, port);

        if (handle->isMapped)
        {
            tr_logAddNamedInfo(getKey(), "%s", _("Port forwarding successful!"));
            handle->port = port;
            handle->state = TR_UPNP_IDLE;
        }
        else
        {
            tr_logAddNamedInfo(getKey(), "If your router supports UPnP, please make sure UPnP is enabled!");
            handle->port = -1;
            handle->state = TR_UPNP_ERR;
        }
    }

    switch (handle->state)
    {
    case TR_UPNP_DISCOVER:
        ret = TR_PORT_UNMAPPED;
        break;

    case TR_UPNP_MAP:
        ret = TR_PORT_MAPPING;
        break;

    case TR_UPNP_UNMAP:
        ret = TR_PORT_UNMAPPING;
        break;

    case TR_UPNP_IDLE:
        ret = handle->isMapped ? TR_PORT_MAPPED : TR_PORT_UNMAPPED;
        break;

    default:
        ret = TR_PORT_ERROR;
        break;
    }

    return ret;
}
