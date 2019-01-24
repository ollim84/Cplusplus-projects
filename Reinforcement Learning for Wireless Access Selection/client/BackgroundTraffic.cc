//
// Copyright (C) 2004 Andras Varga
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#include "BackgroundTraffic.h"


#define MSGKIND_CONNECT  0
#define MSGKIND_SEND     1


Define_Module(BackgroundTraffic);

BackgroundTraffic::BackgroundTraffic()
{
    timeoutMsg = NULL;
}

BackgroundTraffic::~BackgroundTraffic()
{
    cancelAndDelete(timeoutMsg);
}

void BackgroundTraffic::initialize()
{
    TCPGenericCliAppBase::initialize();

    timeoutMsg = new cMessage("timer");

    numRequestsToSend = 0;
    earlySend = false;  // TBD make it parameter

    nb = NotificationBoardAccess().get();
    if (nb==NULL)
    {
        error("Notification board not found. Client needs a notification board to know when the DHCP has assigned an IP address.");
    }

    // for a wireless interface subscribe the DHCP event to start the TCP application
    nb->subscribe(this, NF_INTERFACE_IPv4CONFIG_CHANGED);

    WATCH(numRequestsToSend);
    WATCH(earlySend);

    timeoutMsg->setKind(MSGKIND_CONNECT);
   // scheduleAt((simtime_t)par("startTime"), timeoutMsg);
}

void BackgroundTraffic::receiveChangeNotification(int category, const cPolymorphic *details)
{
    Enter_Method_Silent();
    printNotificationBanner(category, details);

    //3 notifications are generated from the DHPC client, 2 with details, one without details
    //this is also called when DHCP lease time expires --> set lease larger

    // host associated. Link is up. DHCP has assigned an IP address.
    if (category == NF_INTERFACE_IPv4CONFIG_CHANGED && simTime() > 0)
    {
        if (!details)
        {
        	scheduleAt(simTime() + (simtime_t)par("startTime"), timeoutMsg);
        }
    }
}

void BackgroundTraffic::sendRequest()
{
     EV << "sending request, " << numRequestsToSend-1 << " more to go\n";

     long requestLength = par("requestLength");
     long replyLength = par("replyLength");
     if (requestLength<1) requestLength=1;
     if (replyLength<1) replyLength=1;

     sendPacket(requestLength, replyLength);
}

void BackgroundTraffic::handleTimer(cMessage *msg)
{
    switch (msg->getKind())
    {
        case MSGKIND_CONNECT:
            EV << "starting session\n";
            connect(); // active OPEN

            // significance of earlySend: if true, data will be sent already
            // in the ACK of SYN, otherwise only in a separate packet (but still
            // immediately)
            if (earlySend)
                sendRequest();
            break;

        case MSGKIND_SEND:
           sendRequest();
           numRequestsToSend--;
           // no scheduleAt(): next request will be sent when reply to this one
           // arrives (see socketDataArrived())
           break;
    }
}

void BackgroundTraffic::socketEstablished(int connId, void *ptr)
{
    TCPGenericCliAppBase::socketEstablished(connId, ptr);

    // determine number of requests in this session
    numRequestsToSend = (long) par("numRequestsPerSession");
    if (numRequestsToSend<1) numRequestsToSend=1;

    // perform first request if not already done (next one will be sent when reply arrives)
    if (!earlySend)
        sendRequest();
    numRequestsToSend--;
}

void BackgroundTraffic::socketDataArrived(int connId, void *ptr, cPacket *msg, bool urgent)
{
    TCPGenericCliAppBase::socketDataArrived(connId, ptr, msg, urgent);

    if (numRequestsToSend>0)
    {
        EV << "reply arrived\n";
        timeoutMsg->setKind(MSGKIND_SEND);
        scheduleAt(simTime()+(simtime_t)par("thinkTime"), timeoutMsg);
    }
    else
    {
        EV << "reply to last request arrived, closing session\n";
        close();
    }
}

void BackgroundTraffic::socketClosed(int connId, void *ptr)
{
    TCPGenericCliAppBase::socketClosed(connId, ptr);

    // start another session after a delay
    timeoutMsg->setKind(MSGKIND_CONNECT);
    scheduleAt(simTime()+(simtime_t)par("idleInterval"), timeoutMsg);
}

void BackgroundTraffic::socketFailure(int connId, void *ptr, int code)
{
    TCPGenericCliAppBase::socketFailure(connId, ptr, code);

    // reconnect after a delay
    timeoutMsg->setKind(MSGKIND_CONNECT);
    scheduleAt(simTime()+(simtime_t)par("reconnectInterval"), timeoutMsg);
}

