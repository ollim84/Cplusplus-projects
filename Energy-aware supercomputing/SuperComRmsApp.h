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

#ifndef __INET_SUPERCOMRMSAPP_H
#define __INET_SUPERCOMRMSAPP_H

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


/**
 * A thread based TCP-model of the Resource management system subclassed from TCPSrvHostApp.
 * It serves requests coming in SuperComMsg messages.
 * Connection handlers are subclassed from TCPServerThreadBase.
 *
 */

class INET_API SuperComRmsApp : public TCPSrvHostApp
{

  public:

    SuperComRmsApp();
    virtual ~SuperComRmsApp();

    // Enumerated server states
    enum serverState {IDLE, RUNNING, DOWN, STANDBY, HYBERNATED};

    /** Describes a Server */
	 struct serverInfo {
		serverState state;
		int ID;
		int memory;
		int numCores;
		int coresInUse;
		SuperComInternalMsg *evtSleep;
		SuperComInternalMsg *evtConn;
		TCPServerThreadBase* thread;
		cQueue jobs;
		bool reserved; // needed for the search algorithm
		bool connected; // whether there is a connection between rms and server
		simtime_t compTime; //when the server finishes a job
		serverInfo() {
			state = IDLE; memory = -1; numCores = -1, reserved = false, connected = false, compTime = 0, coresInUse = 0;
		}
	 };

	 /** Describes a client */
	 struct clientInfo {
		int ID;
		IPvXAddress clientAddress;
		IPvXAddress usoAddress;
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
		std::vector<int> serverNmb;
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
		simtime_t startTime;
		IPvXAddress clientAddress;
		int cores;
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

	 /** Describes a optimal job */
	 struct optjobInfo {
		int freeNodes;
		int freeMemory;
		int freeCores;
		int jobIndex;
		int nodes;
		int needMemory;
		int cores;
		double runTime;
		int load;
		long InputData;
		double loadedTime;
		simtime_t subTime;
		std::vector<int> serverNmb;
		IPvXAddress clientAddress;
		optjobInfo() {
			freeMemory = -1; freeNodes = -1, runTime = -1,
			freeCores = -1;
		}
	 };


    // GET/SET methods
    int getNumServers();
    void setNumServers(int);
    int getMemory();
    void setMemory(int);
    int getServerMemory(int);
    void setServerMemory(int, int);
    int getServerCores(int);
    void setServerCores(int, int);
    int getServerCoresInUse(int);
    void setServerCoresInUse(int, int);
    int getServerState(int);
    void setServerState(int, serverState);
    simtime_t getServerCompTime(int serverIndex);
    void setServerCompTime(int serverIndex, simtime_t compTime);
    bool getServerReserved(int);
    void setServerReserved(int, bool);
    bool getServerConnected(int);
    void setServerConnected(int, bool);
    TCPServerThreadBase* getServerThread(int);
    void setServerThread(int, TCPServerThreadBase*);
    TCPServerThreadBase* getUSOThread();
    void setUSOThread(TCPServerThreadBase*);
    void addServerJobs(int serverIndex, SuperComInternalMsg *msg);
    void removeServerJobs(int serverIndex, IPvXAddress clientAddr);
    int getServerJobsLength(int serverIndex);
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
    bool getEnergyAware();
    int getScheduling();
    int getClientFinishedServers(IPvXAddress);
    void setClientFinishedServers(IPvXAddress, int);
    void incrementClientFinishedServers(IPvXAddress);
    bool getFederated();
    int getQueueLength();
    double getPUE();
    double getCUE();
    double getAverageQueueTime();
    double getEstimatedWaitTime();
    double getRunningJobsTime();
    double getQueuedJobsTime();
    void addReceivedJobs();

    void setUSOIPAddr(IPvXAddress usoAddr);
    IPvXAddress getUSOIPAddr();

    int getClientWakeupServers(IPvXAddress);
    void setClientWakeupServers(IPvXAddress, int);
    void incrementClientWakeupServers(IPvXAddress);

    int getClientNeedWakeupServers(IPvXAddress);
    void setClientNeedWakeupServers(IPvXAddress, int);
    void incrementClientNeedWakeupServers(IPvXAddress);

    int getClientNodes(IPvXAddress);
    int getClientCores(IPvXAddress);
    int getClientRuntime(IPvXAddress);
    int getClientLoad(IPvXAddress);
    int getClientInputData(IPvXAddress);
    double getClientLoadedTime(IPvXAddress);
    int getClientNeedMemory(IPvXAddress);
    IPvXAddress getClientUsoAddress(IPvXAddress);


    void setClientServerNumber(int serverNumber[], IPvXAddress clientAddr);
    void getClientServerNumber(IPvXAddress clientAddr);

    void incrementClientOutputData(IPvXAddress clientAddr, long);
    void clearClientOutputData(IPvXAddress clientAddr);
    long getClientOutputData(IPvXAddress clientAddr);

    void setIfacePrevSent(int index, long prevSent);
    void setIfacePrevRcvd(int index, long prevRcvd);
    long getIfacePrevSent(int index);
    long getIfacePrevRcvd(int index);

    void incrementRequestsSent();
    long getRequestsSent();
    void incrementJobsCompleted();
    long getJobsCompleted();

    void sendClientRequests(IPvXAddress);

    //Network energy
    simtime_t getLastCalculated();
    void setLastCalculated(simtime_t);
    void computeNetworkEnergy(simtime_t from, simtime_t to); //computes the total network energy consumption on a given time window
    double getPacketsPerSecond(simtime_t from, simtime_t to, cModule *module, int index);

    void insertQueueTime(simtime_t);

    // Methods for the queue
    void enqueueJob(double runtime, IPvXAddress clientAddr, int needMemory, int nodes, int cores, simtime_t subTime, int load, long InputData, double loadedTime, int jobPriority);
    void enqueue(cMessage *msg, bool queueBegin, bool interActive, int jobPriority);
    void dequeue();
    bool isQueueEmpty();
    void findResources();

    //Search function for servers
    int* searchServers(int nodes, int needMemory, int cores, int serverNumber[], bool &serversFound);

    //Initiates connections to servers
    void initServerConnection(int nodes, int serverNumber[], double runTime, int needMemory, IPvXAddress clientAddr, int cores, int load, long InputData, double loadedTime);

    //Adds client info to the struct
    void addClientInfo(IPvXAddress usoAddr, IPvXAddress clientAddr, int nodes, int needMemory, double runTime, int cores, int load, long InputData, double loadedTime, int jobPriority, simtime_t subTime);

    //Checks for idle resources
    void checkServers();
    ////Checks if there are available resources
    bool isResourcesAvailable();

    //Calculates free resources if a job is allocated
    void calculateFreeResources(int nodes, int needMemory, int cores, int &freeNodes, int &freeMemory, int &freeCores);

    //Methods for sleep
    void scheduleSleep(int serverIndex, int sleepMode);
    void cancelSleep(int serverIndex);

    //Deletes client info
    void deleteClient(IPvXAddress);

    //Running jobs methods
    void deleteRunjob(IPvXAddress clientAddress);
    int getRunjobNodes(simtime_t compTime);
    simtime_t getEstimatedStartTime();

    //Add info about an optimal job
    void addOptimalJob(optjobInfo *optjob, int nodes, int needMemory, int cores, double runtime, IPvXAddress clientAddr, int serverNumber[], int load, simtime_t subTime, long InputData, double loadedTime);

    //Methods for energy estimate
    double getEnergyEstimate(int cores, int memory, int coreLoad, double loadedTime, int runTime);

  protected:
     simtime_t delay;
     simtime_t maxMsgDelay;
     simtime_t sessionTimeout_var;
     simtime_t sleepTimeout_var;

     long msgsRcvd_var;
     long msgsSent_var;
     long bytesRcvd_var;
     long bytesSent_var;
     int numServers_var;
     int memory_var;
     int numCores_var;
     bool energyAware_var;
     int scheduling_var;
     bool federated_var;
     IPvXAddress usoIP_var;
     TCPServerThreadBase* usoThread;
     double PUE_var;
     double ESC_var;

     //dynamic PUE (MiM 5.9.2011):
     double dynamicPUEupdateInterval_var; //how often PUE value is updated in seconds, e.g. once a week
     simtime_t dynamicPUEstartTime_var; //when to start PUE value updating
     simtime_t dynamicPUEstopTime_var; //when to stop PUE value updating
     double dynamicPUEchange_var; //how much PUE is incremented/decremented each time
     void dynamicPUEupdate(); //function called every dynamicPUEupdateInterval: defines the new PUE value and calls servers' computeEnergy()
     //end of dynamic PUE changes

     //long prevRcvd;
     //long prevSent;

     long requestsSent;
     long jobsCompleted;

     //MiM 18.9.2010: for calculating the network energy consumption
	 simtime_t last_calculated;//the last time energy consumption for the network was calculated
	 double E_network;//total energy consumpted by the network

	 // PPP interface
	 cModule *module;
	 EtherMAC *ethMac;

     // A linked list for the server structure
 	 typedef std::list<serverInfo> ServerList;
 	 ServerList srvList; ///< list of servers

 	 // A linked list for the client structure
 	 typedef std::list<clientInfo> ClientList;
 	 ClientList cliList; ///< list of clients

 	 // A linked list for the running jobs structure
 	 typedef std::list<runjobInfo> RunjobList;
 	 RunjobList rjbList; ///< list of running jobs

 	// A linked list for the running jobs structure
	 typedef std::list<optjobInfo> OptjobList;
	 OptjobList ojbList; ///< list of running jobs

	 // A linked list for the interface structure
	 typedef std::list<iFaceInfo> iFaceList;
	 iFaceList ifList;

 	 cQueue jobQueue; // queue for jobs
 	 typedef std::vector<simtime_t> CompletionTimeVector;

 	 std::vector<simtime_t> QueueTime;
 	 double meanQueueTime;

 	 // Statistics
 	 StatisticsCollector *stats;
 	 cModule *modp;

     StatisticsCollector *dcStats;
     cModule *dcModp;


  protected:

	 /* Redefined methods from TCPSrvHostApp */
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void handleSelfMessage(cMessage *msg);
	virtual void handleThreadMessage(cMessage *msg);
	virtual void removeThread(TCPServerThreadBase *);
	virtual void finish();

	/** Utility function: looks up server in our server list. Returns NULL if not found. */
	virtual serverInfo *lookupServer(int index);
	/** Utility function: looks up client in our server list. Returns NULL if not found. */
	virtual clientInfo *lookupClient(IPvXAddress clientAddr);
	virtual iFaceInfo *lookupInterface(int index);

};

/*
 * The connection handler thread for clients.
 *
 */
class INET_API SuperComRmsAppClientHandler : public TCPServerThreadBase
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


		virtual void sendMessage(cMessage *msg);


	public:
		/* Constructor */
		SuperComRmsAppClientHandler();
		/* Destructor */
		virtual ~SuperComRmsAppClientHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		SuperComRmsApp* hostModule();

		void sendFinish(IPvXAddress clientAddr);
		void checkQueue(IPvXAddress usoAddr, IPvXAddress clientAddr, int nodes, int needMemory, double runTime, int cores, int load, long InputData, double loadedTime, int jobPriority, simtime_t subTime);

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
class INET_API SuperComRmsAppServerHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		SuperComRmsAppServerHandler();
		/* Destructor */
		virtual ~SuperComRmsAppServerHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		SuperComRmsApp* hostModule();

		// GET/SET methods
		//double getRuntime();
		//void setRuntime(double);
		//IPvXAddress getClientAddress();
		//void setClientAddress(IPvXAddress);
		//int getNeedMemory();
		//void setNeedMemory(int);
		int getServerIndex();
		void setServerIndex(int);
		void sendRequest(IPvXAddress);
		void sendSleep(int);
		void sendWakeup(IPvXAddress);
		void setSleep(bool sleep);
		bool getSleep();
//		int getCores();
//		void setCores(int);
//		int getLoad();
//		void setLoad(int);
//		void setInputData(long);
//		long getInputData();
//		void setLoadedTime(double);
//		double getLoadedTime();
//		void setNodes(int);
//		int getNodes();


	protected:

		//Variables
		double runTime_var;
		IPvXAddress clientAddress_var;
		int needMemory_var;
		int serverIndex_var;
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


