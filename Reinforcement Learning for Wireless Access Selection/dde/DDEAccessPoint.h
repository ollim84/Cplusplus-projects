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

#ifndef __INET_DDEAccessPoint_H
#define __INET_DDEAccessPoint_H

#include <omnetpp.h>
#include "INETDefs.h"
#include <vector>
#include <list>
#include "TCPSocket.h"
#include "TCPSrvHostApp.h"

#include <Ieee80211MgmtAP.h>
#include <AbstractRadioExtended.h>
#include <ThruputMeter.h>
#include <nsfis.h>
#include "ddeMsg_m.h"
//#include "StatisticsCollector.h"



/**
 * A thread based TCP-model subclassed from TCPSrvHostApp.
 * Connection handlers are subclassed from TCPServerThreadBase.
 *
 */

class INET_API DDEAccessPoint : public TCPSrvHostApp
{

  public:

    DDEAccessPoint();
    virtual ~DDEAccessPoint();

    // GET/SET methods
    TCPServerThreadBase* getServerThread(int);
    void setServerThread(int, TCPServerThreadBase*);

    long getMsgsRcvd();
    void setMsgsRcvd(long);
    void incrementMsgsRcvd();
    long getMsgsSent();
    void setMsgsSent(long);
    void incrementMsgsSent();
    long getBytesRcvd();
    void setBytesRcvd(long);
    void incrementBytesRcvd(long);
    long getBytesSent();
    void setBytesSent(long);
    void incrementBytesSent(long);
    int getNumberOfUsers();
    int getNumberOfDownloaders();

    void getAPclassification(float &nu, float &lo, float &pl, float &classifyValue);
    bool getCachedContent();
    void setCachedContent(bool);

    double getClassifyInterval();

    //Packet loss
    double computePacketLoss();
    double getPacketLoss();
    void setPacketLoss(double);

    //Load
    double computeLoad();
    double getLoad();
    void setLoad(double);
    double getConsumedBW();
    void setConsumedBW(double);
    double getMaxBW();
    void setMaxBW(double);
    double getAvBW();
    void setAvBW(double );

    //Client methods
    void addClientInfo(IPvXAddress, TCPServerThreadBase*, bool);
    void addClientUserClass(int userClass, IPvXAddress clientAddr);
    void deleteClient(IPvXAddress);
    void sendInfoToClients(float nu, float lo, float pl, float classifyValue, int numGold, int numSilver, int numBronze);
   // void sendInfoToInfoServer(float nu, float lo, float pl, float classifyValue, int numGold, int numSilver, int numBronze);
    void getUserClassDistribution(int &numGold, int &numSilver, int &numBronze);
    bool isClientRegistered(IPvXAddress clientAddr);

    const MACAddress& getMacAddress();

	 /** Describes a client */
	 struct clientInfo {
		std::string ID;
		IPvXAddress clientAddress;
		TCPServerThreadBase* thread;
		int userClass;
		bool infoServer;
		clientInfo() {
			ID = "", userClass = -1;
		}
	 };

  protected:
     simtime_t delay;
     simtime_t maxMsgDelay;
     simtime_t sessionTimeout_var;
     simtime_t sleepTimeout_var;

     // Statistics
     long msgsRcvd_var;
     long msgsSent_var;
     long bytesRcvd_var;
     long bytesSent_var;

	 cModule *module;
	 Ieee80211MgmtAP *MgmtAP;
	 AbstractRadioExtended *radio;

	 //cOutVector packetlossVector;

	 bool cachedContent_var;
	 double packetLoss_var;
	 double load_var;
	 bool fileServer_var;
	 double consumedBW_var;
	 bool classification_var;
	 double avBW_var;
	 double maxBW_var;

	 //Timers
	 cMessage* pktLossTimer;
	 cMessage* loadTimer;
	 cMessage* apClassifyTimer;

	 double classifyInterval_var;


 	 // A linked list for the client structure
 	 typedef std::list<clientInfo> ClientList;
 	 ClientList cliList; ///< list of clients

// 	 // Statistics
// 	 StatisticsCollector *stats;
// 	 cModule *modp;
//
//     StatisticsCollector *dcStats;
//     cModule *dcModp;


  protected:

	 /* Redefined methods from TCPSrvHostApp */
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void handleSelfMessage(cMessage *msg);
	virtual void handleThreadMessage(cMessage *msg);
	virtual void removeThread(TCPServerThreadBase *);
	virtual void finish();

	virtual clientInfo *lookupClient(IPvXAddress addr);

};

/**
 * The connection handler thread for servers.
 *
 */
class INET_API DDEAccessPointClientHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		DDEAccessPointClientHandler();
		/* Destructor */
		virtual ~DDEAccessPointClientHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		DDEAccessPoint* hostModule();

		// GET/SET methods
		double getPacketLoss();
		void setNumPrevGiven(long);
		long getNumPrevGiven();
		void sendInfo(float nu, float lo, float pl, float classifyValue, int numGold, int numSilver, int numBronze);
		void sendVideoData();



	protected:

		//Variables
		cMessage* evtTout;	// session timeout
		//cMessage* delThreadMsg;		// delete thread
		long numPrevOK;
		long numPrevGiven;

		// Video client information
		struct ContextInfoStruct
		{
			int fileSize; //total file size in Bytes
			int remainingBytes; //remaining content size in Bytes
			int chunkSize; //size in Bytes of the content segment
			double txRate; //the rate at which an app is allowed to transmit content data of specified chunk size
			double playoutBufferSize; //estiamted size of the playOut buffer size at the client end. Used when estimating slient buffer size
			double codecRate;
			int playBufferState;
			bool initialPlayStarted; // OM add 11.02.2013
			int userClass; // OM 12.02.2013
			int playFinished;
			double origTxRate; // Original txRate, needed for burst
		};

		ContextInfoStruct appContext;

		//Reflection methods
		//virtual void sendRequest();
		virtual void sendMessage(cMessage *msg);

	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void closed();
		virtual void close();

};

#endif


