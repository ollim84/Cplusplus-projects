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
 * SuperComUSO.cc
 *
 *  Created on: Jun 13, 2011
 *      Author: Olli Mämmelä
 */



#include "SuperComUSO.h"
#include "SuperComRmsApp.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include "GenericAppMsg_m.h"
#include "SuperComMsg_m.h"
#include "IPAddressResolver.h"
#include <vector>
#include <algorithm>
#include "StatisticsCollector.h"
#include "InterfaceTableAccess.h"
#include <stdio.h>
#include <stdlib.h>

#define BEV	EV << "[UNICORE Service Orchestrator]:"

#define MSGKIND_CONNECT		10
#define MSGKIND_REQUEST     11
#define MSGKIND_SEND        12
#define MSGKIND_INPUT       13
#define MSGKIND_RESPONSE    14
#define MSGKIND_FINISH      15
#define MSGKIND_SLEEP		16
#define MSGKIND_SLEEP_RESPONSE		17
#define MSGKIND_WAKEUP  18
#define MSGKIND_WAKEUP_RESPONSE 19

//Self messages for base module
#define INTERNAL_INIT_CONNECTION_MSG    20
#define INTERNAL_FINISH_MSG    21
#define INTERNAL_SLEEP_MSG    22
#define INTERNAL_JOB_MSG	23
#define INTERNAL_WAKEUP_MSG  24
//Self messages for handlers
#define EVT_TOUT    25
//Self messages for threads
#define INTERNAL_REMOVE_THREAD_MSG   26



Register_Class(SuperComUSOClientHandler);
Register_Class(SuperComUSORmsHandler);
Define_Module(SuperComUSO);

/**
 * Constructor.
 */

SuperComUSO::SuperComUSO()
{
	setMsgsRcvd(0);
	setMsgsSent(0);
	setBytesRcvd(0);
	setBytesSent(0);


}

/**
 * Destructor.
 */

SuperComUSO::~SuperComUSO()
{
	//cancelAndDelete(evtSleep);

}

//This function is needed for the WATCH_LIST(rmsList) command
//Prints the server struct in the GUI
std::ostream& operator<<(std::ostream& os, const SuperComUSO::RMSInfo& rms)
{
    os << " ID=" << rms.ID
       << " queueLength=" << rms.queueLength
       << " startTime=" << rms.startTime;
    return os;
}

//This function is needed for the WATCH_LIST(cliList) command
//Prints the client struct in the GUI
std::ostream& operator<<(std::ostream& os, const SuperComUSO::clientInfo& client)
{
    os << "ID=" << client.ID
       << " Address=" << client.clientAddress
       << " finishedServers=" << client.finishedServers
       << " nodes=" << client.nodes
       << " wakeupNodes= " << client.wakeupNodes
       << " needwakeupNodes= " << client.needwakeupNodes
       << " jobPriority= " << client.jobPriority
       << " outputData= " << client.OutputData;
    return os;
}

//This function is needed for the WATCH_LIST(rjbList) command
//Prints the runjobs struct in the GUI
std::ostream& operator<<(std::ostream& os, const SuperComUSO::runjobInfo& runjob)
{
   os  << "nodes=" << runjob.nodes
	   << " needMemory=" << runjob.needMemory
	   << " runTime=" << runjob.runTime
       << " address=" << runjob.clientAddress
       << " subTime=" << runjob.subTime
       << " compTime=" << runjob.compTime;
    return os;
}


std::ostream& operator<<(std::ostream& os, const SuperComUSO::iFaceInfo& iFace)
{
   os  << "ID=" << iFace.ID
	   << " prevSent=" << iFace.prevSent
	   << " prevRcvd=" << iFace.prevRcvd;
    return os;
}


void SuperComUSO::initialize()
{
	// parent initialize()
	TCPSrvHostApp::initialize();

    delay = par("replyDelay"); //not needed
    maxMsgDelay = 0; //not needed

	last_calculated = 0;//the last time energy consumption for the network was calculated
	E_network = 0.0;//total energy consumpted by the network

//	requestsSent = 0;
//	jobsCompleted = 0;

    numRMS_var = par("numRMS");  // RMS in the network
    fit4green_var = par("fit4green");
    previousRMS_var = -1;

    // Statistics
	//modp = simulation.getModuleByPath("datacenter.stats");
//	modp = simulation.getModuleByPath("HPCNetwork.stats");
//	stats = check_and_cast<StatisticsCollector *>(modp);
//*.numClients
//
//	if (stats == NULL)
//			opp_error("Wrong statisticsModulePath configuration");

    // Initialize the RMS structure
    // Create RMS modules based on the numRMS_var
    // server structure ---> information about a node!

    for (int i = 0; i < numRMS_var; i++) {

        RMSInfo *rms;
        rmsList.push_back(RMSInfo());
        rms = &rmsList.back();
        rms->ID = i; //ID = 0, 1, 2, ...

		char string[80];
		sprintf(string, "datacenter%d.rms.tcpApp[0]", i);
		const char *rmsString = string;

        rms->rmsModule = simulation.getModuleByPath(rmsString);
        rms->rms = check_and_cast<SuperComRmsApp *>(rms->rmsModule);

    	//modp = simulation.getModuleByPath("HPCNetwork.stats");
    	//stats = check_and_cast<StatisticsCollector *>(modp);

        //Generate a message for forming the connections to the server
        rms->evtConn = new SuperComInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
        rms->evtConn->setServerIndex(rms->ID); //Set the rms number into the msg
        scheduleAt(simTime(), rms->evtConn);

    }

	IInterfaceTable *inet_ift;
	inet_ift = InterfaceTableAccess().get();

	// Create list of RMS interfaces
	//NOTE: at initialization, only loopback interface is considered,
	//afterwards there are 3 interfaces: eth1, eth2 and loopback
	for(int i = 0; i < (inet_ift->getNumInterfaces() + 1); i++)
	{
        iFaceInfo *iFace;
        ifList.push_back(iFaceInfo());
        iFace = &ifList.back();
        iFace->ID = i; //ID = 0, 1, 2, ...

	}

    WATCH(msgsRcvd_var);
    WATCH(msgsSent_var);
    WATCH(bytesRcvd_var);
    WATCH(bytesSent_var);
    WATCH_LIST(rmsList);
    WATCH_LIST(cliList);
    //WATCH_LIST(rjbList);
    WATCH_LIST(ifList);


}


// Handles messages coming to this module
void SuperComUSO::handleMessage(cMessage* msg)
{
	//If it's a self message
	if(msg->isSelfMessage())
	{
		if(msg->getKind() < EVT_TOUT)
		{
			//Handles self-messages concerning the base module
			handleSelfMessage(msg);
		}
		else
		{
			if(msg->getKind() == INTERNAL_REMOVE_THREAD_MSG)
			{
				//Handles self-messages concerning a thread
				handleThreadMessage(msg);
			}
			//Else: the self-message concerns a handler
			//E.g. EVT_TOUT
			else
			{
				// default handling
				// This should call timerExpired()
				TCPSrvHostApp::handleMessage(msg);
			}
		}
	}
	//Else: the message is socket data, dataArrived() of a handler should be called
	else
	{
		// default handling
//        TCPSocket *socket = socketMap.findSocketFor(msg);
//        if (!socket)
//        {
//            // new connection -- create new socket object and server process
//            socket = new TCPSocket(msg);
//            socket->setOutputGate(gate("tcpOut"));
//
//            const char *serverThreadClass = par("serverThreadClass");
//            TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));
//
//            socket->setCallbackObject(proc);
//            proc->init(this, socket);
//
//            socketMap.addSocket(socket);
//
//            updateDisplay();
//        }
//        socket->processMessage(msg);

		TCPSrvHostApp::handleMessage(msg);

	}

}

//Handles self messages
void SuperComUSO::handleSelfMessage(cMessage *msg)
{
	switch(msg->getKind())
	{
		// Initiates a connection with a server
		// This is based on the peer-wire module of the BT simulation model
		case INTERNAL_INIT_CONNECTION_MSG:
		{
			SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
			int rmsNumber = intMsg->getServerIndex();

			char string[80];
			sprintf(string, "datacenter%d.rms", rmsNumber);
			const char *connectAddress = string;

			BEV << "Establishing a connection with rms: " << connectAddress << endl;
			// new connection -- create new socket object and server process
			TCPSocket *newsocket = new TCPSocket();

			//unsigned short port = tcp->getEphemeralPort();
			int port = par("port");

			//NOTE: adding this (TCP.h etc.) causes warnings
			//while ((port==connectPort))
				//port = tcp->getEphemeralPort();

			newsocket->bind(port);

			//const char *serverThreadClass = (const char*)par("serverThreadClass");
			const char *serverThreadClass = "SuperComUSORmsHandler";

			TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

			newsocket->setCallbackObject(proc);

			//Set thread into server struct
			setRMSThread(rmsNumber, proc);

			//Creates a thread object to the server handler
			//By this we can call methods from concerning this thread
			SuperComUSORmsHandler* myProc = (SuperComUSORmsHandler*) proc;


			//Set the needed parameters for the job
			myProc->init(this, newsocket); // This initializes "SuperComUSORmsHandler"
			//myProc->setRuntime(intMsg->getRunTime());
			//myProc->setClientAddress(intMsg->getClientAddress());
			//myProc->setNeedMemory(intMsg->getNeedMemory());
			myProc->setRMSIndex(rmsNumber);

			newsocket->setOutputGate(gate("tcpOut"));

			//IP address of the server is retrieved by the IPAddressresolver,
			// based on the name of the server
			newsocket->connect(IPAddressResolver().resolve(connectAddress), port);
			socketMap.addSocket(newsocket);

			updateDisplay();

			delete msg;
			break;

		}
		// This message is received when a finish msg needs to be sent to the client
		// Initiates a thread to the client
		case INTERNAL_FINISH_MSG:
		{

			SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
			IPvXAddress connectAddress = intMsg->getClientAddress();
			BEV << "Job finished, Establishing a connection with client: " << connectAddress << endl;
			BEV << "Amount of output data: " << intMsg->getOutputData() << endl;

			// new connection -- create new socket object and server process
			TCPSocket *newsocket = new TCPSocket();

			//unsigned short port = tcp->getEphemeralPort();
			int port = par("port");

			//NOTE: adding this (TCP.h etc.) causes warnings
			//while ((port==connectPort))
				//port = tcp->getEphemeralPort();

			newsocket->bind(port);

			//const char *serverThreadClass = (const char*)par("serverThreadClass");
			const char *rmsThreadClass = "SuperComUSOClientHandler";

			TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(rmsThreadClass));

			newsocket->setCallbackObject(proc);

			SuperComUSOClientHandler* myProc = (SuperComUSOClientHandler*) proc;

			myProc->init(this, newsocket); // This initializes "SuperComUSOClientHandler"
			myProc->setFinish(true); //Set that the job has been finished, needed for the established() function
			myProc->setOutputData(intMsg->getOutputData());

			newsocket->setOutputGate(gate("tcpOut"));

			newsocket->connect(connectAddress, port);
			socketMap.addSocket(newsocket);

			updateDisplay();
			delete msg;
			break;
		}

		case INTERNAL_JOB_MSG:
		{
			//SuperComInternalMsg* jobMsg = (SuperComInternalMsg*)msg;
			int rmsNumber = ((SuperComInternalMsg*)msg)->getServerIndex();
			TCPServerThreadBase *proc = getRMSThread(rmsNumber);

			if(proc->getSocket()->getState() == TCPSocket::CONNECTED)
			{
				BEV << "Sending client's request to RMS." << endl;
				SuperComUSORmsHandler* myProc = (SuperComUSORmsHandler*) proc;
				myProc->sendRequest(((SuperComInternalMsg*)msg)->getClientAddress());
			}
			else
			{
				std::cerr << "USO not connected to this RMS." << endl;
				error("USO not connected to this RMS.");
			}
			//delete jobMsg;
			delete msg;
			break;
		}

		default:
		{
			delete msg;
			break;
		}
	}

}

//Handles self messages concerning a thread
void SuperComUSO::handleThreadMessage(cMessage *msg)
{

	// Get a pointer for the thread
	TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

	// This would get a handler for the thread. Handler could be used to call functions
	//SuperComUSOClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

	switch(msg->getKind())
	{
		case INTERNAL_REMOVE_THREAD_MSG:
		{
			// If the socket has not been closed, schedule again
			if ((thread->getSocket()->getState()!= TCPSocket::CLOSED) && (thread->getSocket()->getState()!= TCPSocket::SOCKERROR))
			{
				scheduleAt(simTime()+100,msg);
			}
			else
			{
				removeThread(thread);
				delete msg;
			}
			break;
		}
		default:
		{
			delete msg;
			break;
		}
	}
}

//Removes a thread from the socket map
void SuperComUSO::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->getSocket());
    updateDisplay();
}

//This function should cover all the statistics
void SuperComUSO::finish()
{

}

//Looks up server from the server list
SuperComUSO::RMSInfo *SuperComUSO::lookupRMS(int index)
{
    for (RMSList::iterator it=rmsList.begin(); it!=rmsList.end(); ++it)
        if (it->ID == index)
            return &(*it);
    return NULL;
}

//Looks up client from the client list
SuperComUSO::clientInfo *SuperComUSO::lookupClient(IPvXAddress clientAddr)
{
    for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
        if (it->clientAddress == clientAddr)
            return &(*it);
    return NULL;
}

//Looks up interface from the interface list
SuperComUSO::iFaceInfo *SuperComUSO::lookupInterface(int index)
{
    for (iFaceList::iterator it=ifList.begin(); it!=ifList.end(); ++it)
        if (it->ID == index)
            return &(*it);
    return NULL;
}

void SuperComUSO::setIfacePrevSent(int index, long prevSent)
{
	iFaceInfo *iFace = lookupInterface(index);
	iFace->prevSent = prevSent;
}

void SuperComUSO::setIfacePrevRcvd(int index, long prevRcvd)
{
	iFaceInfo *iFace = lookupInterface(index);
	iFace->prevRcvd = prevRcvd;
}

long SuperComUSO::getIfacePrevSent(int index)
{
	iFaceInfo *iFace = lookupInterface(index);
	return iFace->prevSent;
}

long SuperComUSO::getIfacePrevRcvd(int index)
{
	iFaceInfo *iFace = lookupInterface(index);
	return iFace->prevRcvd;
}

/***** GET/SET methods *****/

int SuperComUSO::getNumRMS()
{
	return numRMS_var;
}

void SuperComUSO::setNumRMS(int numRMS)
{
	numRMS_var = numRMS;
}

int SuperComUSO::getClientNodes(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->nodes;
}


int SuperComUSO::getClientRuntime(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->runTime;
}

int SuperComUSO::getClientNeedMemory(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->needMemory;
}

int SuperComUSO::getClientCores(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->cores;
}

int SuperComUSO::getClientLoad(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->load;
}

int SuperComUSO::getClientInputData(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->InputData;
}

double SuperComUSO::getClientLoadedTime(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->loadedTime;
}

void SuperComUSO::setClientRMSNumber(int rmsNumber, IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	client->rmsNmb = rmsNumber;

}

int SuperComUSO::getClientRMSNumber(IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	return client->rmsNmb;
}

void SuperComUSO::incrementClientOutputData(IPvXAddress clientAddress, long data)
{
	clientInfo *client = lookupClient(clientAddress);
	if(data <= 0)
	{
		error("Increment client: Cannot add negative Output data.");
	}
	long output = 0;
	if(client == NULL)
		error("Increment client outputdata: client is null");
	output = client->OutputData;
	client->OutputData = output + data;
	if(client->OutputData <= 0)
	{
		error("Increment client: Client output data is now negative: %d. Client amount of servers: %d", client->OutputData, client->nodes);
	}
}

long SuperComUSO::getClientOutputData(IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	if((client->OutputData) < 0)
	{
		error("get client: Client output data is now negative: %d. Client amount of servers: %d", client->OutputData, client->nodes);

	}
	else if(client->OutputData == 0)
	{
		error("get client: outputdata is zero.");
	}
	else if(client == NULL)
	{
		error("get client: client is NULL.");
	}
	return client->OutputData;

}

void SuperComUSO::clearClientOutputData(IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	client->OutputData = 0;

}


// Deletes a client according to a IP address
void SuperComUSO::deleteClient(IPvXAddress clientAddress)
{

	for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
	{
		if(it->clientAddress == clientAddress)
		{
			cliList.erase(it);
			return;
		}

	}
}

TCPServerThreadBase* SuperComUSO::getRMSThread(int rmsIndex)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	return rms->thread;
}

void SuperComUSO::setRMSThread(int rmsIndex, TCPServerThreadBase* thread)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	rms->thread = thread;
}

IPvXAddress SuperComUSO::getRMSAddress(int rmsNumber)
{
	char string[80];
	sprintf(string, "datacenter%d.rms", rmsNumber);
	const char *connectAddress = string;
	IPvXAddress rmsAddr = IPAddressResolver().resolve(connectAddress);

	return rmsAddr;

}


//Function to initiate connections to RMS
void SuperComUSO::initRMSConnection(int nodes, int serverNumber, double runTime, int needMemory, IPvXAddress clientAddr, int cores, int load, long InputData, double loadedTime)
{

	//Set the selected servers for the client struct
	setClientRMSNumber(serverNumber, clientAddr);
	sendClientRequests(clientAddr);

}

void SuperComUSO::sendClientRequests(IPvXAddress clientAddr)
{

	clientInfo *client = lookupClient(clientAddr);
	int nodes = client->nodes;
	int needMemory = client->needMemory;
	double runTime = client->runTime;
	int cores = client->cores;
	int load = client->load;
	long InputData = client->InputData;
	double loadedTime = client->loadedTime;
	simtime_t subTime = client->subTime;

	simtime_t compTime = runTime + simTime();

	//Compute Network Energy
	computeNetworkEnergy(getLastCalculated(), simTime());

	// Generate a self message for initiating a connection with a RMS
	SuperComInternalMsg *jobMsg = new SuperComInternalMsg("INTERNAL_JOB_MSG", INTERNAL_JOB_MSG);
	jobMsg->setServerIndex(client->rmsNmb);
	jobMsg->setRunTime(runTime);
	jobMsg->setClientAddress(clientAddr);
	jobMsg->setNeedMemory(needMemory);
	jobMsg->setCores(cores);
	jobMsg->setLoad(load);
	jobMsg->setInputData(InputData);
	jobMsg->setLoadedTime(loadedTime);
	jobMsg->setNodes(nodes);
	jobMsg->setTimeOfStart(simTime());

	scheduleAt(simTime(), jobMsg);

}

//Adds client info to the client struct
void SuperComUSO::addClientInfo(IPvXAddress clientAddr, int nodes, int needMemory, double runTime, int cores, int load, long InputData, double loadedTime, int jobPriority, simtime_t subTime)
{
	clientInfo *client;
	cliList.push_back(clientInfo());
	client = &cliList.back();
	client->clientAddress = clientAddr;
	client->nodes = nodes;
	client->needMemory = needMemory;
	client->runTime = runTime;
	client->cores = cores;
	client->load = load;
	client->InputData = InputData;
	client->loadedTime = loadedTime;
	client->jobPriority = jobPriority;
	client->OutputData = 0;
	client->subTime = subTime;

}


simtime_t SuperComUSO::getLastCalculated()
{
	return last_calculated;
}

void SuperComUSO::setLastCalculated(simtime_t time)
{
	last_calculated = time;
}


long SuperComUSO::getMsgsRcvd()
{
	return msgsRcvd_var;
}

void SuperComUSO::setMsgsRcvd(long msgsRcvd)
{
	msgsRcvd_var = msgsRcvd;
}

void SuperComUSO::incrementMsgsRcvd()
{
	msgsRcvd_var++;
}

long SuperComUSO::getMsgsSent()
{
	return msgsSent_var;
}

void SuperComUSO::setMsgsSent(long msgsSent)
{
	msgsSent_var = msgsSent;
}

void SuperComUSO::incrementMsgsSent()
{
	msgsSent_var++;
}


long SuperComUSO::getBytesRcvd()
{
	return bytesRcvd_var;
}

void SuperComUSO::setBytesRcvd(long bytesRcvd)
{
	bytesRcvd_var = bytesRcvd;
}

void SuperComUSO::incrementBytesRcvd(long byteLength)
{
	bytesRcvd_var += byteLength;
}

long SuperComUSO::getBytesSent()
{
	return bytesSent_var;
}

void SuperComUSO::setBytesSent(long bytesSent)
{
	bytesSent_var = bytesSent;
}

void SuperComUSO::incrementBytesSent(long byteLength)
{
	bytesSent_var += byteLength;
}

bool SuperComUSO::getFit4Green()
{
	return fit4green_var;
}
/**
 * Get the session timeout value (in seconds).
 */
simtime_t SuperComUSO::getSessionTimeout() const
{
	return sessionTimeout_var;
}

/**
 * Set the session timeout value (in seconds).
 */
void SuperComUSO::setSessionTimeout(simtime_t sessionTimeout)
{
	sessionTimeout_var = sessionTimeout;
}


/**
 * Function to get the packets per second from a given
 * time interval. Received and sent packets are retrieved
 * from the physical ethernet module.
 *
 */

double SuperComUSO::getPacketsPerSecond(simtime_t from, simtime_t to, cModule *module, int index)
{
	long CurSent = 0;
	long CurRcvd = 0;
	long Rcvd = 0;
	long Sent = 0;

	double pps = 0.0;
	EtherMAC *ethMac;

	ethMac = check_and_cast<EtherMAC*> (module);
	CurSent= ethMac->getSent();
	CurRcvd = ethMac->getRcvd();

	Rcvd = CurRcvd - getIfacePrevRcvd(index);
	Sent = CurSent - getIfacePrevSent(index);
	EV << " PrevSent: " << getIfacePrevSent(index) << " PrevRcvd: " << getIfacePrevRcvd(index) << endl;

	setIfacePrevRcvd(index, CurRcvd);
	setIfacePrevSent(index, CurSent);

	EV << " CurSent: " << CurSent << " CurRcvd: " << CurRcvd << endl;
    EV << " Packets sent: " << Sent << " Packets rcvd: " << Rcvd << endl;

    pps = (Rcvd + Sent) / (SIMTIME_DBL(to-from));

	if(pps < 0)
		error("SuperComUSO: pps cannot be below zero!");

    EV << " Packets per second: " << pps << endl;

    return pps;

}

/**
 * Computes the energy of the Network Interface of the RMS.
 * First the power of all NICs are computed, and the power
 * due to switching operations is added.
 */

void SuperComUSO::computeNetworkEnergy(simtime_t from, simtime_t to)
{
  double E = 0.0;
  /* Servers (NICs) :
   P_max = 0.090934950000000
   P_idle = 0.090807

   PC-based Routers:
   P_max = 69.316105370000002;
   P_idle = 69.316;
   */

 // double roo = 700; //max power consumption in Watts
  //0 < alpha < beta <= 1
  //double beta = 0.9; //full load power consumption = beta * roo
  //double alpha = 0.1; //idle power consumption = alpha * roo
  //double bw = 500000.0; //router bandwidth, in packets/s ???
  double totallambda = 0.0;
  double P_max = 69.316105370000002;
  double P_idle = 69.316;
  double E_switch = 0.0;
  double totalPps = 0.0;

  //TODO: USO has only a single interface, no switching?

  if(from != to)
  {
	  cModule *EthModule;
	  IInterfaceTable *inet_ift;
	  inet_ift = InterfaceTableAccess ().get();

	  // Compute the NIC power of each interface and increment total PPS
	  for(int i = 0; i < (inet_ift->getNumInterfaces() - 1); i++)
	  {
	  		EthModule = getParentModule()->getSubmodule("eth", i)->getSubmodule("mac");
	  		totallambda = getPacketsPerSecond(from, to, EthModule, i); // packets per second for the interface
	  		totalPps += totallambda;
	  		E += P_idle + (P_max - P_idle) * totallambda; // Power of the NIC
	  		BEV << "NIC power: " << E << endl;

	  }

	  BEV << "Total pps: " << totalPps << endl;

	  setLastCalculated(simTime());

	  // Compute power due to switching operations
	  E_switch = P_idle + (P_max - P_idle) * totalPps;
	  BEV << "switch power: " << E_switch << endl;
	  E += E_switch;

	 // E = roo * (alpha + (beta - alpha)/bw * totallambda);

	  E = E * SIMTIME_DBL(to-from);


	  E_network += E;
  }

  //stats->recordNetworkEnergyVector(E);
  //stats->recordTotalEnergyVector(E);

  BEV << "Network energy: " << E << " From: " << from << " to: " << to << endl;
  BEV << "Total Network energy: " << E_network  << endl;

}

int SuperComUSO::selectRMS(bool energyAware, IPvXAddress clientAddr, int nodes, double runTime, int cores)
{

	int selectedRMS = -1;
	int queueLength = -1;
	//int candidateRMS = -1;
	bool serversFound = false;
	int previousRMS = getPreviousRMS();
	double queueTime = -1;
	int priority = 0;
//	priority = nodes * runTime * cores;
	//priority = nodes;
	priority = runTime;


	/** Default scheduling strategy for selecting the cluster
	 *  Uses round-robin for selection.
	 **/
	if(getFit4Green() == false)
	{
		BEV << "Standard round-robin scheduling strategy is used." << endl;
		if(previousRMS == -1)
		{
			selectedRMS = 0;
			setPreviousRMS(selectedRMS);
		}
		else
		{
			if(previousRMS + 1 > (getNumRMS() - 1))
			{
				selectedRMS = 0;
				setPreviousRMS(selectedRMS);
			}
			else
			{
				selectedRMS = previousRMS + 1;
				setPreviousRMS(selectedRMS);
			}
		}
	}

	/**
	 * Fit4Green scheduling strategy for selecting the cluster (F4G algorithm)
	 * User can decide either to
	 * 1. Schedule the job as soon as possible (energy unaware)
	 * 2. Use energy aware scheduling
	 */
	else
	{
		if(energyAware == false)
		{
			BEV << "Client is requesting F4G energy unaware scheduling." << endl;

			for(int i = 0; i < getNumRMS(); i++)
			{

				BEV << "Cluster: " << i << " average queueTime: " << getRMSQueueTime(i) << " queue size: " << getRMSQueueLength(i) << endl;
				serversFound = searchRMSServers(i, clientAddr);

				double maxTime = 0.0;
				maxTime = getRMSEstimatedWaitTime(i);
				BEV << "RMS: " << i << ", Estimated max waiting time: " << maxTime << endl;
				//std::cerr << "RMS: " << i << ", Estimated max waiting time: " << maxTime << endl;

				//1. Finds the first cluster that can execute the job immediately
				if(getRMSQueueLength(i) == 0 && serversFound == true)
				{
					BEV << "Servers found for the job, selecting cluster: " << i << endl;
					selectedRMS = i;
					break;
				}
				//2. Finds the cluster that can start job execution in a minimal delay
				else if(getRMSQueueTime(i) <= queueTime || queueTime == -1)
				{
					if(getRMSQueueLength(i) < queueLength || queueLength == -1)
					{
						BEV << "No servers found, Selecting RMS index: " << i << " , queue time: " << getRMSQueueTime(i) << endl;
						queueTime = getRMSQueueTime(i);
						queueLength = getRMSQueueLength(i);
						selectedRMS = i;
					}
				}


			}


		}
		/**
		 * Energy aware decisions:
		 * 1. Minimize emissions, (ICTEnergy * CUE)
		 * 2. Minimize energy, (ICTEnergy * PUE)
		 */
		else
		{
			BEV << "Client is requesting F4G energy aware scheduling." << endl;
			selectedRMS = 0;
			//double CUE = -1;
			int length = -1;
			int numServers = 0;
			double threshold = 0.0;
			bool clusterFound = false;
			double candidateThreshold = -1;
			double estimate = 0.0;
			double candidateEstimate = -1;


			for(int i = 0; i < getNumRMS(); i++)
			{
				length = getRMSQueueLength(i);
				numServers = getRMSNumServers(i);
				threshold = (double)length/numServers;
				estimate = getRMSEstimate(i, clientAddr);

				BEV << "Threshold: " << threshold << endl;
				BEV << "queue length: " << length << endl;
				BEV << "numServers: " << numServers << endl;
				BEV << "estimate: " << estimate << endl;

				if(threshold < 0.7)
				{
					//if(getRMSCUE(i) < CUE || CUE == -1)
					if(estimate < candidateEstimate || candidateEstimate == -1)
					{
						//CUE = getRMSCUE(i);
						candidateEstimate = estimate;
						BEV << "RMS: " << i << " estimate: " << estimate << endl;
						selectedRMS = i;
						clusterFound = true;
					}
				}

			}
			if(clusterFound == false)
			{
				for(int i = 0; i < getNumRMS(); i++)
				{
					length = getRMSQueueLength(i);
					numServers = getRMSNumServers(i);
					threshold = (double)length/numServers;
					if(threshold < candidateThreshold || candidateThreshold == -1)
					{
						selectedRMS = i;
						candidateThreshold = threshold;

					}
				}

			}

		}
	}

	return selectedRMS;
}

int SuperComUSO::getRMSQueueLength(int rmsIndex)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	return rmsModule->getQueueLength();
}

int SuperComUSO::getRMSNumServers(int rmsIndex)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	return rmsModule->getNumServers();
}

double SuperComUSO::getRMSCUE(int rmsIndex)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	return rmsModule->getCUE();
}

double SuperComUSO::getRMSQueueTime(int rmsIndex)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	return rmsModule->getAverageQueueTime();
}

bool SuperComUSO::searchRMSServers(int rmsIndex, IPvXAddress clientAddr)
{
	int serverNumber[getClientNodes(clientAddr)];
	bool serversFound = true;

	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	rmsModule->searchServers(getClientNodes(clientAddr), getClientNeedMemory(clientAddr), getClientCores(clientAddr), serverNumber, serversFound);

	return serversFound;

}

double SuperComUSO::getRMSEstimate(int rmsIndex, IPvXAddress clientAddr)
{
	double estimate = 0.0;
	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	estimate = rmsModule->getEnergyEstimate(getClientCores(clientAddr), getClientNeedMemory(clientAddr), getClientLoad(clientAddr), getClientLoadedTime(clientAddr), getClientRuntime(clientAddr));

	return estimate;

}

int SuperComUSO::getPreviousRMS()
{
	return previousRMS_var;
}

void SuperComUSO::setPreviousRMS(int rmsIndex)
{
	previousRMS_var = rmsIndex;
}

double SuperComUSO::getRMSEstimatedWaitTime(int rmsIndex)
{
	RMSInfo *rms = lookupRMS(rmsIndex);
	SuperComRmsApp *rmsModule = rms->rms;

	return rmsModule->getEstimatedWaitTime();

}


/**
 * Constructor.
 */

SuperComUSOClientHandler::SuperComUSOClientHandler()
{
	evtTout		= new cMessage("timeoutClient", EVT_TOUT);
	delThreadMsg = new SuperComInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);
	finish_var = false;
	outputData_var = 0;
}

/**
 * Destructor.
 */

SuperComUSOClientHandler::~SuperComUSOClientHandler()
{
	// cancel any pending messages
	hostModule()->cancelAndDelete(evtTout);
	hostModule()->cancelAndDelete(delThreadMsg);
	evtTout = NULL;
	delThreadMsg = NULL;

}

void SuperComUSOClientHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}


void SuperComUSOClientHandler::sendMessage(cMessage *msg)
{

	SuperComMsg *appmsg = dynamic_cast<SuperComMsg*>(msg);

	// If appmsg
	if (appmsg)
	{
		hostModule()->incrementMsgsSent();
		hostModule()->incrementBytesSent(appmsg->getByteLength());

		BEV << "sending \"" << appmsg->getName() << "\" to TCP, " << appmsg->getByteLength() << " bytes\n";
	}

	else
	{
		BEV << "sending \"" << msg->getName() << "\" to TCP\n";
	}

    getSocket()->send(msg);
}


/**
 * Initializes the session variables and performs some logging.
 *
 */
void SuperComUSOClientHandler::established()
{
	// start the session timer
	//NOTE: this causes errors
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << "connection with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

	// If the job has finished, send a finish message
	if(getFinish() == true)
	{
		BEV << " The job client requested has finished. Informing client and sending output data. " << endl;
		BEV << " Output data: " << getOutputData() << endl;
		SuperComMsg* finishMsg = new SuperComMsg("finish");
		finishMsg->setType(MSGKIND_FINISH);
		finishMsg->setByteLength(getOutputData()); //Set the size as the amount of output data
		sendMessage(finishMsg);
		//Delete the client from the cliList
		BEV << "Deleting client info.." << endl;
		hostModule()->deleteClient(getSocket()->getRemoteAddress());

	}

}

/**
 * Handles the reception of a message
 *
 */
void SuperComUSOClientHandler::dataArrived(cMessage* msg, bool urgent)
{

	SuperComMsg *appmsg = dynamic_cast<SuperComMsg*>(msg);

	if (!appmsg)
		hostModule()->error("Message (%s)%s is not a SuperComMsg -- "
			  "probably wrong client app, or wrong setting of TCP's "
			  "sendQueueClass/receiveQueueClass parameters "
			  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
			  msg->getClassName(), msg->getName());

	//NOTE: getType() is defined by the SuperComMsg
	// to use getKind() we should use encapsulation
	switch(appmsg->getType())
	{
		case MSGKIND_REQUEST:
		{

			hostModule()->incrementMsgsRcvd();
			hostModule()->incrementBytesRcvd(appmsg->getByteLength());

			BEV << "Received a request message from client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]" << endl;

			//Increment the number of requests
			//hostModule()->incrementRequestsSent();

			long requestedBytes = appmsg->getExpectedReplyLength();

			simtime_t msgDelay = appmsg->getReplyDelay();

			if (requestedBytes==0)
			{
				delete appmsg;
			}
			else
			{

				SuperComMsg *rmsg = new SuperComMsg("response");

				//NOTE: TCP_I_DATA causes an error
				// since it is used only for receiving
				rmsg->setKind(TCP_C_SEND);
				rmsg->setType(MSGKIND_RESPONSE);


				//These are needed for server connection
				setRuntime(appmsg->getRunTime());
				setNeedMemory(appmsg->getMemory());
				setNodes(appmsg->getNodes());
				setCores(appmsg->getCores());
				setSubTime(simTime()); //submission time of the job, needed for queueing time statistics
				setLoad(appmsg->getLoad());
				setInputData(appmsg->getInputData());
				setLoadedTime(appmsg->getLoadedTime());
				setPriority(appmsg->getPriority());
				setEnergyAware(appmsg->getEnergyAware());

				int jobPriority = 1;
				//Add client info
				hostModule()->addClientInfo(getSocket()->getRemoteAddress(), getNodes(), getNeedMemory(), getRuntime(), getCores(), getLoad(), getInputData(), getLoadedTime(), jobPriority, simTime());

				// Expected reply
				rmsg->setByteLength(requestedBytes);
				int rmsNumber = 0;
				rmsNumber = hostModule()->selectRMS(getEnergyAware(), getSocket()->getRemoteAddress(), getNodes(), getRuntime(), getCores());

				BEV << "Selected RMS: " << rmsNumber << endl;;

				IPvXAddress rmsAddr = hostModule()->getRMSAddress(rmsNumber);
				rmsg->setRmsAddress(rmsAddr);

				int nodes = getNodes();
				//int jobPriority = getPriority() * getNodes() * getRuntime(); // Change this to something else

				BEV << "Needed memory: " << getNeedMemory() << endl;
				BEV << "Needed nodes: " << nodes << endl;
				BEV << "Needed cores: " << getCores() << endl;
				BEV << "Job load: " << getLoad() << endl;;
				BEV << "Job priority: " << jobPriority << endl;
				BEV << "Job loaded time: " << getLoadedTime() << endl;

				BEV << "Input data in get: " << getInputData() << endl;

				//Send the response message to the client
				sendMessage(rmsg);

				//Set the selected rms for the client struct
				hostModule()->setClientRMSNumber(rmsNumber, getSocket()->getRemoteAddress());

				//Send client request
				hostModule()->sendClientRequests(getSocket()->getRemoteAddress());

				//Close the connection. To be opened again when server finishes job execution
				//close();

				delete appmsg;
			}

			break;
		}
		case MSGKIND_INPUT:
		{

			hostModule()->error("USO should not receive input data.");

			//std::cerr << "Input data received in RMS. Simtime: " << simTime() << endl;

			BEV << "Received an input message from client[address=" << getSocket()->getRemoteAddress() << "]" << endl;

			hostModule()->incrementMsgsRcvd();
			hostModule()->incrementBytesRcvd(appmsg->getByteLength());

			//Add client info
//			hostModule()->addClientInfo(getSocket()->getRemoteAddress(), getNodes(), getNeedMemory(), getRuntime(), getCores(), getLoad(), getInputData(), getLoadedTime(), jobPriority, simTime());
//
//			int rmsNumber = 0;
//			rmsNumber = hostModule()->selectRMS(getEnergyAware(), getSocket()->getRemoteAddress());
//
//			BEV << "Selected RMS: " << rmsNumber << endl;;
//
//			//Set the selected rms for the client struct
//			hostModule()->setClientRMSNumber(rmsNumber, getSocket()->getRemoteAddress());
//
//			//Send client request
//			hostModule()->sendClientRequests(getSocket()->getRemoteAddress());

			//hostModule()->initRMSConnection(nodes, serverNumber, getRuntime(), getNeedMemory(), getSocket()->getRemoteAddress(), getCores(), getLoad(), getInputData(), getLoadedTime());

			//Close the connection. To be opened again when server finishes job execution
			close();
			delete appmsg;

			break;
		}

		default:
		{
			// some indication -- ignore
			delete appmsg;
			break;
		}
	}//Switch

}

/**
 * Handles self-messages regarding this handler.
 *
 */
void SuperComUSOClientHandler::timerExpired(cMessage* msg)
{

	switch(msg->getKind())
	{
		// timer expired while waiting
		case EVT_TOUT:
		{
			// logging
			BEV << "session with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] expired\n";
			// perform close()
			close();
			delete msg;
		}
		default:
		{
			delete msg;
		}
	}

}


void SuperComUSOClientHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void SuperComUSOClientHandler::closed()
{

	BEV << "Connection closed." << endl;

	//Schedule a self message for removing the thread
	scheduleAt(simTime(), delThreadMsg);


}

/**
 * Closes an active connection
 */
void SuperComUSOClientHandler::close()
{

	// cancel the session timeout event and call default close()
	//cancelEvent(evtTout);
	hostModule()->cancelAndDelete(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}

/**
 * Get the host module (i.e., the instance of SuperComUSO which spawns the thread).
 */
SuperComUSO* SuperComUSOClientHandler::hostModule()
{
	// get the host module and check its type
	SuperComUSO* hostMod = dynamic_cast<SuperComUSO*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/

bool SuperComUSOClientHandler::getFinish()
{
	return finish_var;
}

void SuperComUSOClientHandler::setFinish(bool finish)
{
	finish_var = finish;
}

double SuperComUSOClientHandler::getRuntime()
{
	return runTime_var;
}

void SuperComUSOClientHandler::setRuntime(double runTime)
{
	runTime_var = runTime;
}

int SuperComUSOClientHandler::getNeedMemory()
{
	return needMemory_var;
}

void SuperComUSOClientHandler::setNeedMemory(int needMemory)
{
	needMemory_var = needMemory;
}

int SuperComUSOClientHandler::getNodes()
{
	return nodes_var;
}

void SuperComUSOClientHandler::setNodes(int nodes)
{
	nodes_var = nodes;
}

int SuperComUSOClientHandler::getCores()
{
	return cores_var;
}

void SuperComUSOClientHandler::setCores(int cores)
{
	cores_var = cores;
}

simtime_t SuperComUSOClientHandler::getSubTime()
{
	return subTime_var;
}

void SuperComUSOClientHandler::setSubTime(simtime_t subTime)
{
	subTime_var = subTime;
}

int SuperComUSOClientHandler::getLoad()
{
	return load_var;
}

void SuperComUSOClientHandler::setLoad(int load)
{
	load_var = load;
}

long SuperComUSOClientHandler::getInputData()
{
	if(inputData_var <= 0)
	{
		hostModule()->error("Input data cannot be negative.");
	}
	return inputData_var;
}

void SuperComUSOClientHandler::setInputData(long data)
{
	if(data <= 0)
	{
		hostModule()->error("Input data cannot be negative.");
	}
	inputData_var = data;
}

double SuperComUSOClientHandler::getLoadedTime()
{
	return loadedTime_var;
}

void SuperComUSOClientHandler::setLoadedTime(double loadedTime)
{
	loadedTime_var = loadedTime;
}

int SuperComUSOClientHandler::getPriority()
{
	return priority_var;
}

void SuperComUSOClientHandler::setPriority(int priority)
{
	priority_var = priority;
}

bool SuperComUSOClientHandler::getEnergyAware()
{
	return energyAware_var;
}

void SuperComUSOClientHandler::setEnergyAware(bool energyAware)
{
	energyAware_var = energyAware;
}

void SuperComUSOClientHandler::setOutputData(long data)
{
	if(data <= 0)
	{
		hostModule()->error("Cannot add negative Output data.");
	}
	outputData_var += data;
}

long SuperComUSOClientHandler::getOutputData()
{
	if(outputData_var <= 0)
	{
		hostModule()->error("Output data cannot be negative.");
	}
	return outputData_var;
}


void SuperComUSOClientHandler::clearOutputData()
{
	outputData_var = 0;
}

/**
 * Constructor.
 */

SuperComUSORmsHandler::SuperComUSORmsHandler()
{
	evtTout		= new cMessage("timeoutServer", EVT_TOUT);
	delThreadMsg = new SuperComInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

	last_calculated = 0;//the last time energy consumption for the network was calculated
	E_network = 0.0;//total energy consumpted by the network

}

/**
 * Destructor.
 */

SuperComUSORmsHandler::~SuperComUSORmsHandler()
{
	// cancel any pending messages
	hostModule()->cancelAndDelete(evtTout);
	hostModule()->cancelAndDelete(delThreadMsg);
	evtTout = NULL;
	delThreadMsg = NULL;;
}

void SuperComUSORmsHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}

void SuperComUSORmsHandler::sendMessage(cMessage *msg)
{

	SuperComMsg *appmsg = dynamic_cast<SuperComMsg*>(msg);


	// If appmsg
	if (appmsg)
	{
		hostModule()->incrementMsgsSent();
		hostModule()->incrementBytesSent(appmsg->getByteLength());

		BEV << "sending \"" << appmsg->getName() << "\" to TCP, " << appmsg->getByteLength() << " bytes\n";
	}

	else
	{
		BEV << "sending \"" << msg->getName() << "\" to TCP\n";
	}

    getSocket()->send(msg);

}

void SuperComUSORmsHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [RMSHandler]: connection with RMS[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

}

/**
 * Handles the reception of a message
 * The server can only send a finish message to the RMS
 *
 */
void SuperComUSORmsHandler::dataArrived(cMessage* msg, bool urgent)
{
	SuperComMsg *appmsg = dynamic_cast<SuperComMsg*>(msg);

	if (!appmsg)
		hostModule()->error("Message (%s)%s is not a SuperComMsg -- "
				  "probably wrong client app, or wrong setting of TCP's "
				  "sendQueueClass/receiveQueueClass parameters "
				  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
				  msg->getClassName(), msg->getName());

	hostModule()->incrementMsgsRcvd();
	hostModule()->incrementBytesRcvd(appmsg->getByteLength());

	switch(appmsg->getType())
	{
		case MSGKIND_FINISH:
		{
			BEV << " [RMSHandler]: RMS[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] finished executing the job.\n";

			BEV << " Amount of output data received from the server: " << appmsg->getByteLength() << endl;

			if(appmsg->getByteLength() <= 0)
				hostModule()->error("Finish message in USO, cannot receive negative amount of bytes");

			IPvXAddress clientAddr = appmsg->getClientAddress();

			//Increment amount of output data to be sent to the client
			long data = 0;
			data = (long)appmsg->getByteLength();
			//hostModule()->incrementClientOutputData(clientAddr, data);

			//Generate an internal finish msg
			SuperComInternalMsg *intMsg = new SuperComInternalMsg("INTERNAL_FINISH_MSG", INTERNAL_FINISH_MSG);
			intMsg->setClientAddress(clientAddr);
			intMsg->setOutputData(data); //error
			scheduleAt(simTime(), intMsg);

			//Calculate network energy
			//hostModule()->computeNetworkEnergy(hostModule()->getLastCalculated(), simTime());


			delete appmsg;
			break;

		}
		case MSGKIND_RESPONSE:
		{
			BEV << "Received a response from the RMS." <<  endl;
			delete appmsg;;
			break;

		}

		default:
		{
			delete appmsg;
			break;
		}

	}


}

/**
 * Handles a session timeout event.
 *
 */
void SuperComUSORmsHandler::timerExpired(cMessage* msg)
{
	switch(msg->getKind())
	{
		// timer expired while waiting
		case EVT_TOUT:
		{
			// logging
			BEV << "session with server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] expired\n";
			// perform close()
			close();
			delete msg;
		}
		default:
		{
			delete msg;
		}
	}

}

void SuperComUSORmsHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void SuperComUSORmsHandler::closed()
{

	BEV << "Connection closed." << endl;
	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void SuperComUSORmsHandler::close()
{

	// cancel the session timeout event and call default close()
//	cancelEvent(evtTout);
	//hostModule()->cancelAndDelete(evtTout);
	hostModule()->cancelEvent(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}

void SuperComUSORmsHandler::sendRequest(IPvXAddress clientAddr)
{
	BEV <<"Sending client's request to the RMS."<< endl;

	BEV << "Runtime: " << hostModule()->getClientRuntime(clientAddr) << endl;
	BEV << "Needed memory: " << hostModule()->getClientNeedMemory(clientAddr) << endl;
	BEV << "Needed nodes: " << hostModule()->getClientNodes(clientAddr) << endl;
	BEV << "Needed cores: " << hostModule()->getClientCores(clientAddr) << endl;
	BEV << "Job load: " << hostModule()->getClientLoad(clientAddr) << endl;;
	BEV << "Job loaded time: " << hostModule()->getClientLoadedTime(clientAddr) << endl;

	SuperComMsg *msg = new SuperComMsg("request");
	msg->setType(MSGKIND_REQUEST);
	msg->setRunTime(hostModule()->getClientRuntime(clientAddr));
	msg->setMemory(hostModule()->getClientNeedMemory(clientAddr));
	msg->setCores(hostModule()->getClientCores(clientAddr));
	msg->setLoad(hostModule()->getClientLoad(clientAddr));
	msg->setInputData(hostModule()->getClientInputData(clientAddr));
	msg->setLoadedTime(hostModule()->getClientLoadedTime(clientAddr));
	msg->setByteLength(350);
	msg->setNodes(hostModule()->getClientNodes(clientAddr));
	msg->setClientAddress(clientAddr);
	msg->setTimeOfStart(simTime());

	//Add the job as a running job into the server structure
	//int serverIndex = getServerIndex();
	//hostModule()->addServerJobs(serverIndex, msg->dup());


	//generate a input data msg
//	SuperComMsg *inputMsg = new SuperComMsg("inputData");
//	inputMsg->setType(MSGKIND_INPUT);
//	inputMsg->setClientAddress(clientAddr);
//	//Input data to be sent to the server is a uniform proportion of the whole input data
//	//TODO: a better way for this should be done.
//
//	int inputDataSize = 0;
//	inputDataSize = (int)(hostModule()->getClientInputData(clientAddr)) / (int)(hostModule()->getClientNodes(clientAddr));
//
//	//Set the input data as the size of the message
//	inputMsg->setByteLength(inputDataSize);
//	inputMsg->setExpectedReplyLength(350);

	//Cancel the timeout event
	if(evtTout->isScheduled())
	{
		hostModule()->cancelEvent(evtTout);
	}

	sendMessage(msg);
	//sendMessage(inputMsg);

}


/**
 * Get the host module (i.e., the instance of SuperComUSO which spawns the thread).
 */
SuperComUSO* SuperComUSORmsHandler::hostModule()
{
	// get the host module and check its type
	SuperComUSO* hostMod = dynamic_cast<SuperComUSO*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/
int SuperComUSORmsHandler::getRMSIndex()
{
	return rmsIndex_var;
}

void SuperComUSORmsHandler::setRMSIndex(int rmsIndex)
{
	rmsIndex_var = rmsIndex;
}




