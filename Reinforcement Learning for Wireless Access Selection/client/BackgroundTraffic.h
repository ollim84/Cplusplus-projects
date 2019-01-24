//
// Copyright 2004 Andras Varga
//
// This library is free software, you can redistribute it and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

#ifndef __INET_BACKGROUNDTRAFFIC_H
#define __INET_BACKGROUNDTRAFFIC_H

#include <omnetpp.h>
#include "TCPGenericCliAppBase.h"
#include "NotificationBoard.h"


/**
 * An example request-reply based client application.
 */
class INET_API BackgroundTraffic : public TCPGenericCliAppBase, public INotifiable
{
  protected:
    cMessage *timeoutMsg;
    bool earlySend;  // if true, don't wait with sendRequest() until established()
    int numRequestsToSend; // requests to send in this session
    NotificationBoard* nb; // Notification board

    /** Utility: sends a request to the server */
    virtual void sendRequest();

  public:
    BackgroundTraffic();
    virtual ~BackgroundTraffic();

  protected:
    /** Redefined to schedule a connect(). */
    virtual void initialize();

    /** Redefined. */
    virtual void handleTimer(cMessage *msg);

    /** Redefined. */
    virtual void socketEstablished(int connId, void *yourPtr);

    /** Redefined. */
    virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);

    /** Redefined to start another session after a delay. */
    virtual void socketClosed(int connId, void *yourPtr);

    /** Redefined to reconnect after a delay. */
    virtual void socketFailure(int connId, void *yourPtr, int code);

    virtual void receiveChangeNotification(int category, const cPolymorphic *details);
};

#endif


