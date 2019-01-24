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

#ifndef __INET_TCPSINKAPPQ_H
#define __INET_TCPSINKAPPQ_H

#include <omnetpp.h>
#include "INETDefs.h"
#include "CliAppContext_m.h"
#include "StatisticsCollector.h" // OM




/**
 * Extends the TCPSinkApp wwith a queue. Accepts any number of incoming TCP connections, and queues the packets
 * and periodically deques them. Models the playout buffer of a browser.
 */
class INET_API TCPSinkAppQ : public cSimpleModule
{
public:
	void sendStats();
  protected:
	cQueue playBuffer;	 	//ZY 24.10.2011
	long playBufferSize; 	//ZY 25.10.2011
	double playRate; 		//ZY 25.10.2011
	double pauseRate;		//ZY 07.03.2012
    long bytesRcvd;
    int segmentsRcvd;
    int initAdvBufferSize;
    int minAdvBufferSize;
    int port; //port Id
    bool pauseEvent;
    int userClass; // OM 12.02.2013
    bool contentDelivered; //OM
    int numberOfBufferStarvations; // OM
    std::vector<double> bufferSizeVector; //OM
    double codecRate; // OM
    bool dynamicPlayRate; //OM
    simtime_t starvationStart; //OM
    std::vector<double> packetSizeVector; //OM
    std::vector<double> starvationDurationVector; //OM
    const char* contentServerAddress; //OM
    simtime_t starvationFreqStart; //OM
    std::vector<double> starvationFrequencyVector; //OM
    int bytesToReceive; // OM
    simtime_t playTimeStart; //OM
    simtime_t playTime; //OM
    bool restart;
    bool scheduleRestart;

	 // Statistics OM
	 StatisticsCollector *stats;
	 cModule *modp;

	typedef std::vector<cMessage*> PlayerControl; //Play/Pause message pointers are stored here
	PlayerControl playerControl;
//	PlayerControl::iterator tcpAppListIt;

	enum PlayBufferState {
		RED,
		ORANGE,
		YELLOW,
		GREEN
	};

	enum PlayBufferState playBufferState;
	int currentBufferState; //keeps track of the current state of the playBuffer

	//PlayBuffer Thresholds (in terms of number of chunks in playbuffer)
	int threshYO; //
	int threshGY;

	cModule *fs4vp;
	CliAppContext *cliAppContext; //a message for carrying client's context info such as client ID, bufferstate etc.

  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void playContent();
    virtual void pauseContent();
    virtual void createPlayTimer();
    virtual void createPauseTimer();
    virtual void determinePlayBufferState(int playBufferSize);
    virtual void verifyBufferStateTransition(int _playBufferState);
   // virtual void confirmStateTransition(enum PlayBufferState BufferState);

    //statistics
    cOutVector segmentsReceivedVec;
    cOutVector playBufferSizeVec;
    cOutVector playBreakVec;
    cOutVector packetSizeVec;
    cOutVector playTimeRemainingVec;


    cOutVector playBufferSizeByteVec;
    cOutVector playBufferDepletionVec;


    int playBufferIncreaseRate;
    int playBufferDepletionRate;

    simsignal_t totalBytesReceived; //Signal to be emitted towards the listener (e.g., FS4VP module in the remote server)
};

#endif


