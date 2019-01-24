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

/*
 * SuperComUSO.h
 *
 *  Created on: Jun 13, 2011
 *      Author: Olli Mämmelä
 */

#ifndef __INET_SUPERCOMUSO_H
#define __INET_SUPERCOMUSO_H

#include <omnetpp.h>
#include "INETDefs.h"
#include <vector>
#include <list>
#include "TCPSocket.h"
#include "TCPSrvHostApp.h"
#include "SuperComMsg_m.h"
#include "StatisticsCollector.h"
#include <EtherMACBase.h>
#include <EtherMAC.h>
#include "SuperComRmsApp.h"


/**
 * A thread based TCP-model of the Resource management system subclassed from TCPSrvHostApp.
 * It serves requests coming in SuperComMsg messages.
 * Connection handlers are subclassed from TCPServerThreadBase.
 *
 */

class INET_API SuperComUSO : public TCPSrvHostApp
{

  public:

    SuperComUSO();
    virtual ~SuperComUSO();


    /** Describes a RMS */
	 struct RMSInfo {
		int ID;
		int queueLength;
		int startTime;
		SuperComInternalMsg *evtConn;
		TCPServerThreadBase* thread;
	 	SuperComRmsApp *rms;
	 	cModule *rmsModule;
		RMSInfo() {
			queueLength = -1, startTime = -1;
		}
	 };

	 /** Describes a client */
	 struct clientInfo {
		int ID;
		IPvXAddress clientAddress;
		int finishedServers;
		int nodes;
		int cores;
		int needMemory;
		double runTime;
		double load;
		double loadedTime;
		long InputData;
		long OutputData; //amount of output data needed to send to the client
		int wakeupNodes;
		int needwakeupNodes;
		int rmsNmb;
		//std::vector<int> serverNmb;
		int jobPriority;
		simtime_t subTime;
		clientInfo() {
			ID = -1; finishedServers = 0; nodes = 0; wakeupNodes = 0; needwakeupNodes = 0, jobPriority = 1, OutputData = 0;
		}
	 };

	 /** Describes a running job */
	 struct runjobInfo {
		int nodes;
		int needMemory;
		double runTime;
		simtime_t subTime;
		simtime_t compTime;
		IPvXAddress clientAddress;
		runjobInfo() {
			needMemory = 0; nodes = 0, compTime = 0;
		}
	 };

	 /**
	  * Structure describing rcvd/sent packets
	  * for network interface
	  */
	 struct iFaceInfo {
		long prevRcvd;
		long prevSent;
		int ID;
		iFaceInfo() {
			ID = -1; prevRcvd = 0; prevSent = 0;
		}
	 };


    // GET/SET methods
	int getNumRMS();
	void setNumRMS(int);

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
    simtime_t getSessionTimeout() const;
    void setSessionTimeout(simtime_t);

    int getClientNodes(IPvXAddress);
    int getClientCores(IPvXAddress);
    int getClientRuntime(IPvXAddress);
    int getClientLoad(IPvXAddress);
    int getClientInputData(IPvXAddress);
    double getClientLoadedTime(IPvXAddress);
    int getClientNeedMemory(IPvXAddress);

    bool getFit4Green();
    int getPreviousRMS();
    void setPreviousRMS(int rmsIndex);


    void setClientRMSNumber(int serverNumber, IPvXAddress clientAddr);
    int getClientRMSNumber(IPvXAddress clientAddr);

    void incrementClientOutputData(IPvXAddress clientAddr, long);
    void clearClientOutputData(IPvXAddress clientAddr);
    long getClientOutputData(IPvXAddress clientAddr);

    void setIfacePrevSent(int index, long prevSent);
    void setIfacePrevRcvd(int index, long prevRcvd);
    long getIfacePrevSent(int index);
    long getIfacePrevRcvd(int index);

    void sendClientRequests(IPvXAddress);

    TCPServerThreadBase* getRMSThread(int);
    void setRMSThread(int, TCPServerThreadBase*);

    int selectRMS(bool, IPvXAddress, int, double, int);
    int getRMSQueueLength(int);
    double getRMSCUE(int rmsIndex);
    bool searchRMSServers(int rmsIndex, IPvXAddress clientAddr);
    IPvXAddress getRMSAddress(int rmsNumber);
    double getRMSQueueTime(int rmsIndex);
    double getRMSEstimatedWaitTime(int rmsIndex);
    int getRMSNumServers(int rmsIndex);
    double getRMSEstimate(int rmsIndex, IPvXAddress clientAddr);

    //Network energy
    simtime_t getLastCalculated();
    void setLastCalculated(simtime_t);
    void computeNetworkEnergy(simtime_t from, simtime_t to); //computes the total network energy consumption on a given time window
    double getPacketsPerSecond(simtime_t from, simtime_t to, cModule *module, int index);


    //Search function for servers
    int* searchServers(int nodes, int needMemory, int cores, int serverNumber[], bool &serversFound);

    //Initiates connections to servers
    void initRMSConnection(int nodes, int serverNumber, double runTime, int needMemory, IPvXAddress clientAddr, int cores, int load, long InputData, double loadedTime);

    //Adds client info to the struct
    void addClientInfo(IPvXAddress clientAddr, int nodes, int needMemory, double runTime, int cores, int load, long InputData, double loadedTime, int jobPriority, simtime_t subTime);

    //Calculates free resources if a job is allocated
    void calculateFreeResources(int nodes, int needMemory, int cores, int &freeNodes, int &freeMemory, int &freeCores);

    //Deletes client info
    void deleteClient(IPvXAddress);


  protected:
     simtime_t delay;
     simtime_t maxMsgDelay;
     int numRMS_var;
     long msgsRcvd_var;
     long msgsSent_var;
     long bytesRcvd_var;
     long bytesSent_var;
     simtime_t sessionTimeout_var;

     bool fit4green_var;
     int previousRMS_var;

     //MiM 18.9.2010: for calculating the network energy consumption
	 simtime_t last_calculated;//the last time energy consumption for the network was calculated
	 double E_network;//total energy consumpted by the network

	 // PPP interface
	 cModule *module;
	 EtherMAC *ethMac;

     // A linked list for the rms structure
 	 typedef std::list<RMSInfo> RMSList;
 	 RMSList rmsList; ///< list of rms

 	 // RMS module
 	 SuperComRmsApp *rms;
 	 cModule *rmsModule;

 	 // A linked list for the client structure
 	 typedef std::list<clientInfo> ClientList;
 	 ClientList cliList; ///< list of clients

	 // A linked list for the interface structure
	 typedef std::list<iFaceInfo> iFaceList;
	 iFaceList ifList;

 	 // Statistics
 	 StatisticsCollector *stats;
 	 cModule *modp;


  protected:

	 /* Redefined methods from TCPSrvHostApp */
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void handleSelfMessage(cMessage *msg);
	virtual void handleThreadMessage(cMessage *msg);
	virtual void removeThread(TCPServerThreadBase *);
	virtual void finish();

	/** Utility function: looks up server in our server list. Returns NULL if not found. */
	virtual RMSInfo *lookupRMS(int index);
	/** Utility function: looks up client in our server list. Returns NULL if not found. */
	virtual clientInfo *lookupClient(IPvXAddress clientAddr);
	virtual iFaceInfo *lookupInterface(int index);

};

/*
 * The connection handler thread for clients.
 *
 */
class INET_API SuperComUSOClientHandler : public TCPServerThreadBase
{
	protected:
		cMessage* evtTout;	// session timeout
		cMessage* delThreadMsg;		// delete thread
		bool finish_var;
		double runTime_var;
		IPvXAddress clientAddress_var;
		int needMemory_var;
		int serverIndex_var;
		int nodes_var;
		int cores_var;
		simtime_t subTime_var;
		int load_var;
		double inputData_var;
		double loadedTime_var;
		int priority_var;
		long outputData_var;
		bool energyAware_var;


		virtual void sendMessage(cMessage *msg);


	public:
		/* Constructor */
		SuperComUSOClientHandler();
		/* Destructor */
		virtual ~SuperComUSOClientHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		SuperComUSO* hostModule();

		//GET/SET methods
		bool getFinish();
		void setFinish(bool);
		double getRuntime();
		void setRuntime(double);
		int getNeedMemory();
		void setNeedMemory(int);
		int getNodes();
		void setNodes(int);
		int getCores();
		void setCores(int);
		simtime_t getSubTime();
		void setSubTime(simtime_t);
		int getLoad();
		void setLoad(int);
		long getInputData();
		void setInputData(long);
		double getLoadedTime();
		void setLoadedTime(double);
		int getPriority();
		void setPriority(int);
		void setOutputData(long);
		long getOutputData();
		void clearOutputData();
		bool getEnergyAware();
		void setEnergyAware(bool);


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
 * The connection handler thread for servers.
 *
 */
class INET_API SuperComUSORmsHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		SuperComUSORmsHandler();
		/* Destructor */
		virtual ~SuperComUSORmsHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		SuperComUSO* hostModule();

		// GET/SET methods
		int getRMSIndex();
		void setRMSIndex(int);
		void sendRequest(IPvXAddress);


	protected:

		//Variables
		double runTime_var;
		IPvXAddress clientAddress_var;
		int needMemory_var;
		int rmsIndex_var;
		cMessage* evtTout;	// session timeout
		cMessage* delThreadMsg;		// delete thread
		bool sleep_var;
		int cores_var;
		int load_var;
		double inputData_var; //packets/s the job creates, MiM 18.9.2010
		double loadedTime_var;
		int nodes_var;

		//MiM 18.9.2010: for calculating the network energy consumption
		simtime_t last_calculated;//the last time energy consumption for the network was calculated
		double E_network;//total energy consumpted by the network
		double computeNetworkEnergy(simtime_t from, simtime_t to); //computes the total network energy consumption on a given time window



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



