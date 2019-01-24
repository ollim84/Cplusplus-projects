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

#ifndef __INET_SUPERCOMCLIENTAPP_H
#define __INET_SUPERCOMCLIENTAPP_H

#include <omnetpp.h>
#include "TCPGenericCliAppBase.h"
#include "TCPSocket.h"
#include "StatisticsCollector.h"


/**
 * A request-reply based client application for Supercomputing.
 */
class INET_API SuperComClientApp : public TCPGenericCliAppBase
{
  protected:
    cMessage *connectMsg;
    bool earlySend;  // if true, don't wait with sendRequest() until established()
    int numRequestsToSend; // requests to send in this session
    int appType;
    long inputData;
    int cores;
    int nodes;
    int memory;
    double runTime;
    int load;
    double networkLoad;
    double loadedTime;
    bool requestSent;
    int priority;
    int jobRequests;
    int sentRequests;
    cQueue jobQueue;
    bool federated;
    IPvXAddress rmsAddr_var;
    bool contactRMS;

    simtime_t subTime;
    simtime_t compTime;

	// Statistics
	StatisticsCollector *stats;
	cModule *modp;

    /** Utility functions */
    virtual void sendRequest();
    virtual void sendInputData();
    virtual void writeData();
    virtual void initializeJobs();
    virtual void createJobQueue();

  public:
    SuperComClientApp();
    virtual ~SuperComClientApp();
    int getMemoryFromFile(int jobNumber);
    int getLoadFromFile(int jobNumber);
    double getLoadedTimeFromFile(int jobNumber);
    long getInputDataFromFile(int jobNumber);
    void setRMSAddress(IPvXAddress rmsAddr);
    IPvXAddress getRMSAddress();


  protected:
    /** Redefined methods from TCPGeneriCliAppBase */
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void handleTimer(cMessage *msg);
    virtual void socketEstablished(int connId, void *yourPtr);
    virtual void socketDataArrived(int connId, void *yourPtr, cPacket *msg, bool urgent);
    virtual void socketClosed(int connId, void *yourPtr);
    virtual void socketPeerClosed(int connId, void *yourPtr);
    virtual void socketFailure(int connId, void *yourPtr, int code);

};

#endif


