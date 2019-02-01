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
// // Copyright 2013 Olli Mämmelä (VTT)

#ifndef __INET_InfoServer_H
#define __INET_InfoServer_H

#include <omnetpp.h>
#include "INETDefs.h"
#include <vector>
#include <list>
#include "TCPSocket.h"
#include "TCPSrvHostApp.h"
#include "ddeMsg_m.h"
#include "MACAddress.h"
#include <AbstractRadioExtended.h>
#include <nsfis.h>
#include "LoadBalancing.h"
#include "event.h"
//#include "StatisticsCollector.h"



/**
 * A thread based TCP-model subclassed from TCPSrvHostApp.
 * Connection handlers are subclassed from TCPServerThreadBase.
 *
 */

class INET_API InfoServer : public TCPSrvHostApp
{

  public:

    InfoServer();
    virtual ~InfoServer();

	 /** Describes an AP */
	 struct apInfo {
		int ID;
		string stringID;
		IPvXAddress apAddress;
		int numUsers;
		float load;
		float packetLoss;
		float classifyValue;
		bool cachedContent;
		TCPServerThreadBase* thread;
		MACAddress macAddress;
		bool congestion;
		double maxBW;
		double avBW;
		double consumedBW;
		int numGold;
		int numSilver;
		int numBronze;
		apInfo() {
			ID = -1; classifyValue = -1, load = -1, packetLoss = -1, numGold = 0, numSilver = 0, numBronze = 0;
		}
	 };

	 struct clientAPInfo {
		 int ID;
		 float networkClassification;
		 double signalStrength;
		 float mnClassification;
		 float apClassification;
		 MACAddress macAddress;
		 int numberOfUsers;
		 clientAPInfo() {
				ID = -1, networkClassification = -1;
		 }
	 };

 	 typedef std::list<clientAPInfo> ClientAPList;


	 /** Describes a client */
	 struct clientInfo {
		std::string ID;
		IPvXAddress clientAddress;
		float mnClassifyValue;
		float signalStrength;
		float nsClassifyValue;
		std::string assocAP;
		double client_x;
		double client_y;
		ClientAPList clientAPList;
		vector<clientAPInfo> clientAPListVector;
		int userClass;
		MACAddress clientMAC;
		double BW_requirement;
		float rssiDBM;
		TCPServerThreadBase* thread;
		float noiseLevelScaled;
		float noiseLevelDBM;
		clientInfo() {
			ID = "", mnClassifyValue = -1, signalStrength = -1;
		}
	 };

	 // String methods
	 string intToString(int value);
	 string floatToString(float value);

	 // milliwatts to dBm
	  static double milliwattToDBM(float mw){
	      return (10 * log10(mw/1));
	  }


    // GET/SET method
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
    int getNumberOfThreads();
    double getLoad();
    double getPacketLoss();
    int getRequestLength();
    void setRequestLength(int);

    //AP methods
    float getAPclassification();
    int getNumAPs();
    float getAPclassification(int ID);
    IPvXAddress getAPaddress(int ID);
    int getAPNumUsers(int ID);
    float getAPLoad(int ID);
    float getAPPacketLoss(int ID);
    bool getAPCachedContent(int ID);
    MACAddress getAPMacAddress(int ID);
    TCPServerThreadBase* getAPThread(int);
    void setAPThread(int, TCPServerThreadBase*);
    string getAPSSIDByMAC(MACAddress macAddr);
    void addAPinfo(int ID, float classifyValue, int numUsers, float load, float packetLoss, IPvXAddress apAddr,
    		bool cachedContent, const MACAddress& macAddress, double, double, double, int, int, int);


    //Client methods
    void addClientInfo(IPvXAddress, const char*, float, const char*, float, double, double, int, float, MACAddress, double, float, float, float, TCPServerThreadBase*);
    void addClientInitially(IPvXAddress clientAddr, TCPServerThreadBase* thread);
    void deleteClient(IPvXAddress);


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

     int numAPs_var;
     double startTime_var;
     int requestLength_var;
     int receivedClassify;

     // Load balancing interval
     double loadBalancing_interval;
     double congestionThreshold;

 	 // A linked list for the AP structure
 	 typedef std::list<apInfo> APList;
 	 APList apList; ///< list of APs

 	 // A linked list for the client structure
 	 typedef std::list<clientInfo> ClientList;
 	 ClientList cliList; ///< list of clients

 	 bool loadBalancing_var;

	struct SortDescending
	{
	   bool operator()(const clientAPInfo &a, const clientAPInfo &b) const
	   {
		   return b.networkClassification < a.networkClassification;
	   }
	};

// 	 // Statistics
// 	 StatisticsCollector *stats;
// 	 cModule *modp;
//
//     StatisticsCollector *dcStats;
//     cModule *dcModp;

 	 //Load balancing module
 	 LoadBalancing *loadBalancing;
 	 cModule *lbModp;


  protected:

	 /* Redefined methods from TCPSrvHostApp */
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void handleSelfMessage(cMessage *msg);
	virtual void handleThreadMessage(cMessage *msg);
	virtual void removeThread(TCPServerThreadBase *);
	virtual void finish();

	// Added functions for load balancing
	void doLoadBalancing();
	float computeNetworkClassification(float apClassification, float mn);
	double computeEstimatedSignalStrength(const char* apSSID, double client_x, double client_y);
	double getEstimatedSignalStrength(const char* apSSID, double client_x, double client_y);
	void callAlgorithm();
	pair<string, string> ParseHandoverEvent(string str) const;
	vector <string> ParseAPName(string str) const;


	/** Utility function: looks up client in our server list. Returns NULL if not found. */
	virtual clientInfo *lookupClientID(const char* ID);
	virtual clientInfo *lookupClientIP(IPvXAddress ID);
	virtual apInfo *lookupAP(int ID);
	virtual clientAPInfo *lookupClientAP(int clientAP_ID, ClientAPList list);
	virtual apInfo *lookupAPByString(string stringID);


};

/**
 * The connection handler thread for clients.
 *
 */
class INET_API InfoServerClientHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		InfoServerClientHandler();
		/* Destructor */
		virtual ~InfoServerClientHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		InfoServer* hostModule();

		void sendHandoverRecommendation(MACAddress apMAC, string apSSID);
		void setThread(TCPServerThreadBase* thread);
		TCPServerThreadBase* getThread();

		// GET/SET methods



	protected:

		//Variables
		cMessage* evtTout;	// session timeout
		//cMessage* delThreadMsg;		// delete thread
		TCPServerThreadBase* clientThread;

		//Reflection methods
		//virtual void sendRequest();
		virtual void sendMessage(cMessage *msg);
		void sendClassifyMN();

	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void closed();
		virtual void close();

};

/**
 * The connection handler thread for clients.
 *
 */
class INET_API InfoServerAPHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		InfoServerAPHandler();
		/* Destructor */
		virtual ~InfoServerAPHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		void sendClassify();
		/* Redefined method from TCPServerThreadBase */
		InfoServer* hostModule();

		// GET/SET methods
		void setID(int);
		int getID();



	protected:

		//Variables
		cMessage* evtTout;	// session timeout
		//cMessage* delThreadMsg;		// delete thread
		cMessage *internalMsg; // internal msg
		int ID_var;

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


