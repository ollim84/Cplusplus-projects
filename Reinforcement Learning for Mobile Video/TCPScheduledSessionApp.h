//
// Copyright 2004 Andras Varga
// Copyright 2011 Zarrar Yousaf (NEC)
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

#ifndef __INET_TCPSCHEDULEDSESSIONAPP_H
#define __INET_TCPSCHEDULEDSESSIONAPP_H

#include <vector>
#include <omnetpp.h>
#include "TCPSocket.h"
#include "INotifiable.h"
#include"NotificationBoard.h"
#include "ContextInformation_m.h"
#include "AppContext_m.h"


/**
 * Single-connection TCP application.
 */
class INET_API TCPScheduledSessionApp : public cSimpleModule, public INotifiable
{
//  protected:
//    struct Command
//    {
//        simtime_t tSend;
//        int numBytes;
//    };
//    typedef std::vector<Command> CommandVector;
//    CommandVector commands;

    TCPSocket socket;

    long totalAppBytes;// specify the total Bytes for the app. One exhausted (i.e., all Bytes sent), then the app will commence CLOSE with the peer.
//    unsigned int initBytesToSend; // specify how many bytes should be sent initially before the scheduling function kicks in. This could correspond to the initial byte burst to fill the playbuffer of the users.
    unsigned int bytesToSend; //> specify how many Bytes (B) the app is allowed to send to the TCP layer. This amount is dictated by the FS4V module
	unsigned int remainingBytesToSend; // specify how many out of the totalAppBytes remain to be sent. During initialization, remainingBytesToSend = totalAppBytes.

	//===================== HACK ==============================================
	unsigned int chunkSize; // the size of a single data unit of the File which is transmitted as a chunk
	double chunkTxRate; //the rate at which chunks will be transmitted
//=============================================================================
	double sendDataTriggerTime; // specifies the time of generating a trigger to send out data to the TCP layer

    const char *address;
    int port;
    const char *connectAddress;
    int connectPort;

    bool active;
    simtime_t tOpen;
    simtime_t tSend;
    simtime_t tClose;
//    long sendBytes;
    bool fs4vpControl;
    int numberOfVideos; // OM
    double idleTime; //OM
    int receivedVideos; //OM


    // statistics
    int packetsRcvd;
    long bytesRcvd;
    long bytesSent;
    int indicationsRcvd;
    long packetsSent; //ZY 25.10.2011
    double codecRate; //ZY 19.06.2012

  public:
    TCPScheduledSessionApp() : cSimpleModule() {}
    virtual ~TCPScheduledSessionApp();

    const char* getConnectAddress();

  protected:
    NotificationBoard *nb;
    AppContext *aci; //application context information

    virtual int numInitStages() const {return 4;} //this specifies into how many stages does one want the initialise function to be called.
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void openConnection();
 //   virtual void sendData(cMessage *triggerTimer, simtime_t interval, int bytesToSend);// Called when the app module works standalone
    virtual void sendData(int bytesToSend, int chunkSize, double ict, simtime_t txTimeSlot); //overloaded function. Called when the trigger arrives from the FS4VP module
    virtual void createInitialTrigger();
    virtual void registerWithScheduler(cModule *fs4vp);

    virtual void sendData(cMessage *msg); //HACK function

    virtual void count(cMessage *msg);
    virtual void finish();


  private:
     virtual void receiveChangeNotification(int category, const cPolymorphic *details);
};

#endif


