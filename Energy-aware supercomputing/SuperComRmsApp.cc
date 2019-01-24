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

#include "SuperComServerApp.h" //MiM 8.9.2011 for dynamicPUE

#define BEV	EV << "[Resource Management System]:"

//TODO: these could be defined differently, use different numbers?
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
//Self message to update PUE_var (MiM 5.9.2011)
#define INTERNAL_DYNAMIC_PUE_MSG 27



Register_Class(SuperComRmsAppClientHandler);
Register_Class(SuperComRmsAppServerHandler);
Define_Module(SuperComRmsApp);

/**
 * Constructor.
 */

SuperComRmsApp::SuperComRmsApp()
{
	setMsgsRcvd(0);
	setMsgsSent(0);
	setBytesRcvd(0);
	setBytesSent(0);

	QueueTime.clear();
	meanQueueTime = 0;

}

/**
 * Destructor.
 */

SuperComRmsApp::~SuperComRmsApp()
{
	//cancelAndDelete(evtSleep);

}

//This function is needed for the WATCH_LIST(srvList) command
//Prints the server struct in the GUI
std::ostream& operator<<(std::ostream& os, const SuperComRmsApp::serverInfo& server)
{
    os << "state=" << server.state
       << " memory=" << server.memory
       << " ID=" << server.ID
       << " cores=" << server.numCores
       << " coresInUse=" << server.coresInUse
       << " reserved=" << server.reserved
       << " connected=" << server.connected
       << " compTime=" << server.compTime
       << " jobList= " << (server.jobs).length();
    return os;
}

//This function is needed for the WATCH_LIST(cliList) command
//Prints the client struct in the GUI
std::ostream& operator<<(std::ostream& os, const SuperComRmsApp::clientInfo& client)
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
std::ostream& operator<<(std::ostream& os, const SuperComRmsApp::runjobInfo& runjob)
{
   os  << "nodes=" << runjob.nodes
	   << " needMemory=" << runjob.needMemory
	   << " runTime=" << runjob.runTime
       << " address=" << runjob.clientAddress
       << " subTime=" << runjob.subTime
       << " compTime=" << runjob.compTime
       << " startTime=" << runjob.startTime
       << " cores=" << runjob.cores;
    return os;
}


std::ostream& operator<<(std::ostream& os, const SuperComRmsApp::iFaceInfo& iFace)
{
   os  << "ID=" << iFace.ID
	   << " prevSent=" << iFace.prevSent
	   << " prevRcvd=" << iFace.prevRcvd;
    return os;
}

void SuperComRmsApp::initialize()
{
	// parent initialize()
	TCPSrvHostApp::initialize();

    delay = par("replyDelay"); //not needed
    maxMsgDelay = 0; //not needed

	last_calculated = 0;//the last time energy consumption for the network was calculated
	E_network = 0.0;//total energy consumpted by the network

	requestsSent = 0;
	jobsCompleted = 0;
	usoThread = NULL;

    numServers_var = par("numServers");  // Servers in the data center
    memory_var = par("memory"); // Memory each server holds
    numCores_var = par("numCores"); //Cores each server has
    sessionTimeout_var 	= par("sessionTimeout");
    sleepTimeout_var = par("sleepTimeout");
    energyAware_var = par("energyAware");
    federated_var = par("federated");
    scheduling_var = par("scheduling");

    PUE_var = par("PUE");
    ESC_var = par("ESC");

    //Dynamic PUE (MiM 5.9.2011):
    dynamicPUEstartTime_var = par("dynamicPUEstartTime");
    dynamicPUEupdateInterval_var = par("dynamicPUEupdateInterval");
    dynamicPUEchange_var = par("dynamicPUEchange");
    if(dynamicPUEupdateInterval_var > 0){
    	cMessage *PUEmsg = new cMessage("INTERNAL_DYNAMIC_PUE_MSG ", INTERNAL_DYNAMIC_PUE_MSG );
    	scheduleAt(dynamicPUEstartTime_var + dynamicPUEupdateInterval_var, PUEmsg);
    }

    jobQueue.setName("jobQueue");

//    const char* ch1 = getParentModule()->getParentModule()->getFullName();
//    char str1[200];
//    strcpy(str1, ch1);
//
//    char str2[80];
//    sprintf(str2, ".server[%d]", 1);
//    strcat(str1, str2);
//
//    std::cerr << "Master module string: " << str1 << endl;


    // Statistics

    const char* ch1 = getParentModule()->getParentModule()->getFullName();
    char str1[200];
    strcpy(str1, ch1);

    char str2[80];
    sprintf(str2, ".stats");
    strcat(str1, str2);

    const char *statsString = str1;

    dcModp = simulation.getModuleByPath(statsString);
    dcStats = check_and_cast<StatisticsCollector *>(dcModp);

    dcStats->setCUE(ESC_var * PUE_var);

	//modp = simulation.getModuleByPath("datacenter.stats");
	modp = simulation.getModuleByPath("HPCNetwork.stats");
	stats = check_and_cast<StatisticsCollector *>(modp);


	if (stats == NULL)
			opp_error("Wrong statisticsModulePath configuration");

    // Initialize the server structure
    // Create servers based on the numServers_var
    // server structure ---> information about a node!
    // Other parameters should be added
    for (int i = 0; i < numServers_var; i++) {

        serverInfo *server;
        srvList.push_back(serverInfo());
        server = &srvList.back();
        server->ID = i; //ID = 0, 1, 2, ...
        server->memory = memory_var;
        server->state = IDLE;

        server->numCores = numCores_var;
        server->reserved = false;
        (server->jobs).clear();

        char result[100];
        int num = i;
        sprintf(result, "%d", num);

        (server->jobs).setName(result);

        // Generate a sleep timer
        if(getEnergyAware() == true)
        {
        	server->evtSleep = new SuperComInternalMsg("INTERNAL_SLEEP_MSG", INTERNAL_SLEEP_MSG);
        	server->evtSleep->setServerIndex(server->ID); //Set the server number into the msg
        }
        //Generate a message for forming the connections to the server
        server->evtConn = new SuperComInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
        server->evtConn->setServerIndex(server->ID); //Set the server number into the msg
        scheduleAt(simTime(), server->evtConn);

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
    WATCH_LIST(srvList);
    WATCH_LIST(cliList);
    WATCH_LIST(rjbList);
    WATCH_LIST(ifList);


}


// Handles messages coming to this module
void SuperComRmsApp::handleMessage(cMessage* msg)
{
	//If it's a self message
	if(msg->isSelfMessage())
	{
		if(msg->getKind() < EVT_TOUT)
		{
			//Handles self-messages concerning the base module
			handleSelfMessage(msg);
		}
		else if(msg->getKind() == INTERNAL_DYNAMIC_PUE_MSG)
			handleSelfMessage(msg); //INTERNAL_DYNAMIC_PUE_MSG==27, EVT_TOUT=25... (MiM 5.9.2011)
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
		if(getFederated() == true)
		{
	        TCPSocket *socket = socketMap.findSocketFor(msg);
	        //If there's no socket created, the message is coming from the USO
	        if (!socket)
	        {
				BEV << "No socket found for this message." << endl;
	            // new connection -- create new socket object and server process
	            socket = new TCPSocket(msg);
	            socket->setOutputGate(gate("tcpOut"));

	            const char *serverThreadClass = par("serverThreadClass"); //This is SuperComRmsClientHandler
	            TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

				//In federated case, set the USO thread
				if(getUSOThread() == 0)
				{
					BEV << "No USO thread set, setting it now." << endl;
					setUSOThread(proc);
				}

	            socket->setCallbackObject(proc);
	            proc->init(this, socket);

	            socketMap.addSocket(socket);

	            updateDisplay();
	        }
	        socket->processMessage(msg);

		}
		else
		{
			TCPSrvHostApp::handleMessage(msg);
		}

	}

}

//Handles self messages
void SuperComRmsApp::handleSelfMessage(cMessage *msg)
{
	switch(msg->getKind())
	{
		// Initiates a connection with a server
		// This is based on the peer-wire module of the BT simulation model
		case INTERNAL_INIT_CONNECTION_MSG:
		{
			SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
			int serverNumber = intMsg->getServerIndex();

			/** Federated case **/
		    const char* ch1 = getParentModule()->getParentModule()->getFullName();
		    char str1[200];
		    strcpy(str1, ch1);

		    char str2[80];
		    sprintf(str2, ".server[%d]", serverNumber);
		    strcat(str1, str2);

		    const char *connectAddress = str1;

			/** Single-site case **/
		   // char string[80];
			//sprintf(string, "datacenter.server[%d]", serverNumber);
			//const char *connectAddress = string;



			BEV << "Establishing a connection with server: " << connectAddress << endl;
			// new connection -- create new socket object and server process
			TCPSocket *newsocket = new TCPSocket();

			//unsigned short port = tcp->getEphemeralPort();
			int port = par("port");

			//NOTE: adding this (TCP.h etc.) causes warnings
			//while ((port==connectPort))
				//port = tcp->getEphemeralPort();

			newsocket->bind(port);

			//const char *serverThreadClass = (const char*)par("serverThreadClass");
			const char *serverThreadClass = "SuperComRmsAppServerHandler";

			TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

			newsocket->setCallbackObject(proc);

			//Set thread into server struct
			setServerThread(serverNumber, proc);

			//Creates a thread object to the server handler
			//By this we can call methods from concerning this thread
			SuperComRmsAppServerHandler* myProc = (SuperComRmsAppServerHandler*) proc;


			//Set the needed parameters for the job
			myProc->init(this, newsocket); // This initializes "SuperComRmsAppServerHandler"
			//myProc->setRuntime(intMsg->getRunTime());
			//myProc->setClientAddress(intMsg->getClientAddress());
			//myProc->setNeedMemory(intMsg->getNeedMemory());
			myProc->setServerIndex(serverNumber);

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
			IPvXAddress connectAddress;

			if(getFederated() == true)
			{
				connectAddress = intMsg->getUsoAddress();

				TCPServerThreadBase *proc = getUSOThread();
				//std::cerr << " USO thread: " << proc << endl;
				if(proc->getSocket()->getState() == TCPSocket::CONNECTED)
				{
					BEV << "Sending a finish msg to the USO." << endl;
					SuperComRmsAppClientHandler* myProc = (SuperComRmsAppClientHandler*) proc;
					myProc->setOutputData(intMsg->getOutputData());
					myProc->sendFinish(intMsg->getClientAddress());
				}
				else
				{
					std::cerr << "RMS not connected to the USO. State: " << proc->getSocket()->getState() << endl;
					error("RMS not connected to the USO.");
				}


			}
			else
			{
				connectAddress = intMsg->getClientAddress();
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
				const char *serverThreadClass = "SuperComRmsAppClientHandler";

				TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

				newsocket->setCallbackObject(proc);

				SuperComRmsAppClientHandler* myProc = (SuperComRmsAppClientHandler*) proc;

				myProc->init(this, newsocket); // This initializes "SuperComRmsAppClientHandler"
				myProc->setFinish(true); //Set that the job has been finished, needed for the established() function
				myProc->setOutputData(intMsg->getOutputData());

				newsocket->setOutputGate(gate("tcpOut"));

				newsocket->connect(connectAddress, port);
				socketMap.addSocket(newsocket);

				updateDisplay();
			}

			delete msg;
			break;
		}
		case INTERNAL_SLEEP_MSG:
		{
			//Get the server number
			SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
			int serverNumber = intMsg->getServerIndex();
			int sleepMode = intMsg->getSleepMode();

			TCPServerThreadBase *proc = getServerThread(serverNumber);
			if(proc->getSocket()->getState() == TCPSocket::CONNECTED)
			{
				BEV << "Sending a sleep msg to the server." << endl;
				SuperComRmsAppServerHandler* myProc = (SuperComRmsAppServerHandler*) proc;
				myProc->sendSleep(sleepMode);
			}
			else
			{
				std::cerr << "RMS not connected to this server. State: " << proc->getSocket()->getState() << endl;
				error("RMS not connected to this server.");
			}

			delete msg;
			break;
		}
		case INTERNAL_WAKEUP_MSG:
		{
			//Get the server number
			SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
			int serverNumber = intMsg->getServerIndex();

			TCPServerThreadBase *proc = getServerThread(serverNumber);
			if(proc->getSocket()->getState() == TCPSocket::CONNECTED)
			{
				BEV << "Sending a wakeup msg to the server." << endl;
				SuperComRmsAppServerHandler* myProc = (SuperComRmsAppServerHandler*) proc;
//				myProc->setRuntime(intMsg->getRunTime());
//				myProc->setClientAddress(intMsg->getClientAddress());
//				myProc->setNeedMemory(intMsg->getNeedMemory());
//				myProc->setCores(intMsg->getCores());
//				myProc->setServerIndex(serverNumber);
//				myProc->setLoad(intMsg->getLoad());
				myProc->sendWakeup(intMsg->getClientAddress());
			}
			else
			{
				std::cerr << "RMS not connected to this server. State: " << proc->getSocket()->getState() << endl;
				error("RMS not connected to this server.");
			}

			delete msg;
			break;
		}
		case INTERNAL_JOB_MSG:
		{
			SuperComInternalMsg* jobMsg = (SuperComInternalMsg*)msg;
			int serverNumber = jobMsg->getServerIndex();
			TCPServerThreadBase *proc = getServerThread(serverNumber);

			if(proc->getSocket()->getState() == TCPSocket::CONNECTED)
			{
				BEV << "Sending client's request." << endl;
				//TODO: with multiple jobs per node, these should be retrieved
				// based on the client's ip address, fetch these from the client
				// structure
				SuperComRmsAppServerHandler* myProc = (SuperComRmsAppServerHandler*) proc;
				if(serverNumber == 26)
				{
//					std::cerr << " Server index at INTERNAL JOB MSG: " << serverNumber << endl;
//					std::cerr << " cores in use: " << getServerCoresInUse(serverNumber) << endl;
//					std::cerr << " total: " << getServerCores(serverNumber) << endl;
//					std::cerr << " server state: " << getServerState(serverNumber) << endl;
//					serverInfo *server = lookupServer(serverNumber);
//					std::cerr << " server jobs: " << (server->jobs).length() << endl;
				}

				myProc->sendRequest(jobMsg->getClientAddress());
			}
			else
			{
				std::cerr << "RMS not connected to this server. State: " << proc->getSocket()->getState() << endl;
				error("RMS not connected to this server.");
			}
			delete jobMsg;
			break;
		}
		case INTERNAL_DYNAMIC_PUE_MSG:
		{
			BEV << "INTERNAL_DYNAMIC_PUE_MSG received" << endl;
			dynamicPUEupdate();

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
void SuperComRmsApp::handleThreadMessage(cMessage *msg)
{

	// Get a pointer for the thread
	TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

	// This would get a handler for the thread. Handler could be used to call functions
	//SuperComRmsAppClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

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
				//TODO: the server connection flag should be set to false here
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
void SuperComRmsApp::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->getSocket());
    updateDisplay();
}

//This function should cover all the statistics
void SuperComRmsApp::finish()
{

	double sum = 0;
	for(int i = 0; i < (int) QueueTime.size(); i++)
	{
		sum = sum + SIMTIME_DBL(QueueTime[i]);
	}

	if(sum != 0)
	{
		meanQueueTime = sum/(QueueTime.size());
	}
	else
	{
		meanQueueTime = 0;
	}


	//Compute network energy
	computeNetworkEnergy(getLastCalculated(), simTime());

	//Add the total energy to the global statistics
	stats->addTotalEnergy(E_network);
	stats->addNetworkEnergy(E_network);
	stats->addQueueTime(meanQueueTime);

	dcStats->addTotalEnergy(E_network);
	dcStats->addQueueTime(meanQueueTime);
	dcStats->addNetworkEnergy(E_network);
	//stats->addJobsCompleted(getJobsCompleted());
	//stats->addRequestsSent(getRequestsSent());


    BEV << getFullPath() << ": sent " << bytesSent_var << " bytes in " << msgsSent_var << " packets\n";
    BEV << getFullPath() << ": received " << bytesRcvd_var << " bytes in " << msgsRcvd_var << " packets\n";
    BEV << getFullPath() << ": Mean Queue Time: " << meanQueueTime << endl;
    BEV << getFullPath() << ": Network energy: " << E_network << endl;
    BEV << getFullPath() << ": Sim time: " << simTime() << endl;

    //recordScalar("Mean Queue Time", meanQueueTime);
    //recordScalar("Network energy", E_network);
    recordScalar("packets sent", msgsSent_var);
    recordScalar("packets rcvd", msgsRcvd_var);
    recordScalar("bytes sent", bytesSent_var);
    recordScalar("bytes rcvd", bytesRcvd_var);
    recordScalar("Requests sent", getRequestsSent());
    recordScalar("Jobs completed", getJobsCompleted());
}

//Looks up server from the server list
SuperComRmsApp::serverInfo *SuperComRmsApp::lookupServer(int index)
{
    for (ServerList::iterator it=srvList.begin(); it!=srvList.end(); ++it)
        if (it->ID == index)
            return &(*it);
    return NULL;
}

//Looks up client from the client list
SuperComRmsApp::clientInfo *SuperComRmsApp::lookupClient(IPvXAddress clientAddr)
{
    for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
        if (it->clientAddress == clientAddr)
            return &(*it);
    return NULL;
}

//Looks up interface from the interface list
SuperComRmsApp::iFaceInfo *SuperComRmsApp::lookupInterface(int index)
{
    for (iFaceList::iterator it=ifList.begin(); it!=ifList.end(); ++it)
        if (it->ID == index)
            return &(*it);
    return NULL;
}

void SuperComRmsApp::setIfacePrevSent(int index, long prevSent)
{
	iFaceInfo *iFace = lookupInterface(index);
	iFace->prevSent = prevSent;
}

void SuperComRmsApp::setIfacePrevRcvd(int index, long prevRcvd)
{
	iFaceInfo *iFace = lookupInterface(index);
	iFace->prevRcvd = prevRcvd;
}

long SuperComRmsApp::getIfacePrevSent(int index)
{
	iFaceInfo *iFace = lookupInterface(index);
	return iFace->prevSent;
}

long SuperComRmsApp::getIfacePrevRcvd(int index)
{
	iFaceInfo *iFace = lookupInterface(index);
	return iFace->prevRcvd;
}

/***** GET/SET methods *****/

int SuperComRmsApp::getNumServers()
{
	return numServers_var;
}

void SuperComRmsApp::setNumServers(int numServers)
{
	numServers_var = numServers;
}

int SuperComRmsApp::getMemory()
{
	return memory_var;
}

void SuperComRmsApp::setMemory(int memory)
{
	memory_var = memory;
}

int SuperComRmsApp::getServerMemory(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->memory;
}

void SuperComRmsApp::setServerMemory(int serverIndex, int memory)
{
	serverInfo *server = lookupServer(serverIndex);
	server->memory = memory;
}

int SuperComRmsApp::getServerCores(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->numCores;
}

void SuperComRmsApp::setServerCores(int serverIndex, int cores)
{
	serverInfo *server = lookupServer(serverIndex);
	server->numCores = cores;
}

int SuperComRmsApp::getServerCoresInUse(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->coresInUse;
}

void SuperComRmsApp::setServerCoresInUse(int serverIndex, int coresInUse)
{
	serverInfo *server = lookupServer(serverIndex);
	server->coresInUse = coresInUse;
}

int SuperComRmsApp::getServerState(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->state;
}

void SuperComRmsApp::setServerState(int serverIndex, serverState state)
{
	serverInfo *server = lookupServer(serverIndex);
	server->state = state;
}

simtime_t SuperComRmsApp::getServerCompTime(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->compTime;
}

void SuperComRmsApp::setServerCompTime(int serverIndex, simtime_t compTime)
{
	serverInfo *server = lookupServer(serverIndex);
	server->compTime = compTime;
}

bool SuperComRmsApp::getServerReserved(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->reserved;
}

void SuperComRmsApp::setServerReserved(int serverIndex, bool reserved)
{
	serverInfo *server = lookupServer(serverIndex);
	server->reserved = reserved;
}

bool SuperComRmsApp::getServerConnected(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->connected;
}

void SuperComRmsApp::setServerConnected(int serverIndex, bool connected)
{
	serverInfo *server = lookupServer(serverIndex);
	server->connected = connected;
}

TCPServerThreadBase* SuperComRmsApp::getServerThread(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return server->thread;
}

void SuperComRmsApp::setServerThread(int serverIndex, TCPServerThreadBase* thread)
{
	serverInfo *server = lookupServer(serverIndex);
	server->thread = thread;
}

TCPServerThreadBase* SuperComRmsApp::getUSOThread()
{
	return usoThread;
}

void SuperComRmsApp::setUSOThread(TCPServerThreadBase* thread)
{
	usoThread = thread;
}

void SuperComRmsApp::addServerJobs(int serverIndex, SuperComInternalMsg *msg)
{
	serverInfo *server = lookupServer(serverIndex);
	(server->jobs).insert(msg);

}

int SuperComRmsApp::getServerJobsLength(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	return (server->jobs).length();

}

void SuperComRmsApp::setUSOIPAddr(IPvXAddress usoAddr)
{
	usoIP_var = usoAddr;
}

IPvXAddress SuperComRmsApp::getUSOIPAddr()
{
	return usoIP_var;
}

void SuperComRmsApp::removeServerJobs(int serverIndex, IPvXAddress clientAddr)
{
	serverInfo *server = lookupServer(serverIndex);

	//Go through the jobs of the server
	for(int i = 0; i < (server->jobs).length(); i++)
	{
		//Get the job from queue: 1, 2, 3,...
		//cMessage *msg = (cMessage *)(server->jobs).get(i);
		//SuperComMsg *jobMsg = dynamic_cast<SuperComMsg*>(msg);
		SuperComInternalMsg *jobMsg = (SuperComInternalMsg *)(server->jobs).get(i);
		IPvXAddress tempAddr = jobMsg->getClientAddress();

		//If the job has the same clientAddress, remove the job from queue
		if(tempAddr == clientAddr)
		{
			(server->jobs).remove((server->jobs).get(i));
		}
		//delete jobMsg;
	}

}


long SuperComRmsApp::getMsgsRcvd()
{
	return msgsRcvd_var;
}

void SuperComRmsApp::setMsgsRcvd(long msgsRcvd)
{
	msgsRcvd_var = msgsRcvd;
}

void SuperComRmsApp::incrementMsgsRcvd()
{
	msgsRcvd_var++;
}

long SuperComRmsApp::getMsgsSent()
{
	return msgsSent_var;
}

void SuperComRmsApp::setMsgsSent(long msgsSent)
{
	msgsSent_var = msgsSent;
}

void SuperComRmsApp::incrementMsgsSent()
{
	msgsSent_var++;
}


long SuperComRmsApp::getBytesRcvd()
{
	return bytesRcvd_var;
}

void SuperComRmsApp::setBytesRcvd(long bytesRcvd)
{
	bytesRcvd_var = bytesRcvd;
}

void SuperComRmsApp::incrementBytesRcvd(long byteLength)
{
	bytesRcvd_var += byteLength;
}

long SuperComRmsApp::getBytesSent()
{
	return bytesSent_var;
}

void SuperComRmsApp::setBytesSent(long bytesSent)
{
	bytesSent_var = bytesSent;
}

void SuperComRmsApp::incrementBytesSent(long byteLength)
{
	bytesSent_var += byteLength;
}

/**
 * Get the session timeout value (in seconds).
 */
simtime_t SuperComRmsApp::getSessionTimeout() const
{
	return sessionTimeout_var;
}

/**
 * Set the session timeout value (in seconds).
 */
void SuperComRmsApp::setSessionTimeout(simtime_t sessionTimeout)
{
	sessionTimeout_var = sessionTimeout;
}

bool SuperComRmsApp::getEnergyAware()
{
	return energyAware_var;
}

bool SuperComRmsApp::getFederated()
{
	return federated_var;
}

int SuperComRmsApp::getScheduling()
{
	return scheduling_var;
}

int SuperComRmsApp::getClientFinishedServers(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->finishedServers;
}

void SuperComRmsApp::setClientFinishedServers(IPvXAddress clientAddress, int finishedServers)
{
	clientInfo *client = lookupClient(clientAddress);
	client->finishedServers = finishedServers;
}

void SuperComRmsApp::incrementClientFinishedServers(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	(client->finishedServers)++;
}

int SuperComRmsApp::getClientWakeupServers(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->wakeupNodes;
}

void SuperComRmsApp::setClientWakeupServers(IPvXAddress clientAddress, int wakeupNodes)
{
	clientInfo *client = lookupClient(clientAddress);
	client->wakeupNodes = wakeupNodes;
}

void SuperComRmsApp::incrementClientWakeupServers(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	(client->wakeupNodes)++;
}

int SuperComRmsApp::getClientNeedWakeupServers(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->needwakeupNodes;
}

void SuperComRmsApp::incrementClientNeedWakeupServers(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	(client->needwakeupNodes)++;
}

void SuperComRmsApp::setClientNeedWakeupServers(IPvXAddress clientAddress, int needwakeupNodes)
{
	clientInfo *client = lookupClient(clientAddress);
	client->needwakeupNodes = needwakeupNodes;
}


int SuperComRmsApp::getClientNodes(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->nodes;
}

IPvXAddress SuperComRmsApp::getClientUsoAddress(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->usoAddress;
}


int SuperComRmsApp::getClientRuntime(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->runTime;
}

int SuperComRmsApp::getClientNeedMemory(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->needMemory;
}

int SuperComRmsApp::getClientCores(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->cores;
}

int SuperComRmsApp::getClientLoad(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->load;
}

int SuperComRmsApp::getClientInputData(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->InputData;
}

double SuperComRmsApp::getClientLoadedTime(IPvXAddress clientAddress)
{
	clientInfo *client = lookupClient(clientAddress);
	return client->loadedTime;
}



void SuperComRmsApp::setClientServerNumber(int serverNumber[], IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	for(int i = 0; i < (client->nodes) ; i++)
	{
		client->serverNmb.insert(client->serverNmb.end(), serverNumber[i]);
	}

}

void SuperComRmsApp::getClientServerNumber(IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	for(int i = 0; i < client->nodes ; i++)
	{
		EV << "Client server number: " << client->serverNmb[i] << endl;
	}
}

void SuperComRmsApp::incrementClientOutputData(IPvXAddress clientAddress, long data)
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

long SuperComRmsApp::getClientOutputData(IPvXAddress clientAddr)
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

void SuperComRmsApp::clearClientOutputData(IPvXAddress clientAddr)
{
	clientInfo *client = lookupClient(clientAddr);
	client->OutputData = 0;

}

// Deletes a client according to a IP address
void SuperComRmsApp::deleteClient(IPvXAddress clientAddress)
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

// Deletes a running job according to a client IP address
void SuperComRmsApp::deleteRunjob(IPvXAddress clientAddress)
{

	for (RunjobList::iterator it=rjbList.begin(); it!=rjbList.end(); ++it)
	{
		if(it->clientAddress == clientAddress)
		{
			rjbList.erase(it);
			return;
		}

	}

}

// Deletes a running job according to a client IP address
int SuperComRmsApp::getRunjobNodes(simtime_t compTime)
{

	for (RunjobList::iterator it=rjbList.begin(); it!=rjbList.end(); ++it)
	{
		if(it->compTime == compTime)
		{
			return it->nodes;
		}

	}
	return NULL;

}

//Generates an internal_job_msg and puts it into the queue
void SuperComRmsApp::enqueueJob(double runtime, IPvXAddress clientAddr, int needMemory, int nodes, int cores, simtime_t subTime, int load, long InputData, double loadedTime, int jobPriority)
{
	//Generate a message containing information about the job
	SuperComInternalMsg *jobMsg = new SuperComInternalMsg("INTERNAL_JOB_MSG", INTERNAL_JOB_MSG);
	jobMsg->setRunTime(runtime);
	jobMsg->setClientAddress(clientAddr);
	jobMsg->setNeedMemory(needMemory);
	jobMsg->setNodes(nodes);
	jobMsg->setCores(cores);
	jobMsg->setSubTime(subTime);
	jobMsg->setLoad(load);
	jobMsg->setInputData(InputData);
	jobMsg->setLoadedTime(loadedTime);
	jobMsg->setPriority(jobPriority);

	//For interactive jobs, place the request as the first element
	if(jobPriority == 0)
	{
		enqueue(jobMsg, true, true, jobPriority);
	}
	// Place the message into the queue, queueBegin = false --> place it as the last element
	else
	{
		enqueue(jobMsg, false, false, jobPriority);
	}


}

simtime_t SuperComRmsApp::getEstimatedStartTime()
{
	cMessage *msg = (cMessage *)jobQueue.get(0);
	SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(msg);

	int nodes = jobMsg->getNodes(); //nodes needed for the job
	int cores = jobMsg->getCores(); //cores per node needed for the job
	int availableNodes = 0;
	simtime_t startTime = 0;
	simtime_t serverStartTime = 0;
	std::vector <simtime_t> serverCompTime;
	//Get the completion times of the running jobs into a vector
	CompletionTimeVector compTime;

	BEV << "First job nodes: " << nodes << " cores: " << cores << endl;
	//std::cerr << "First job nodes: " << nodes << " cores: " << cores << endl;

	std::vector<SuperComInternalMsg*> tempJobList;

	//1. Go through the server list and check what servers are IDLE
	for (ServerList::iterator it=srvList.begin(); it!=srvList.end(); ++it)
	{
		if (it->state == IDLE)
		{
			availableNodes++;
			compTime.insert(compTime.end(), 0);
			if(availableNodes == nodes)
				break;
		}
	}

	//2. Check which servers are in standby mode
	if(availableNodes < nodes)
	{
		for (ServerList::iterator it=srvList.begin(); it!=srvList.end(); ++it)
		{
			if (it->state == STANDBY)
			{
				availableNodes++;
				compTime.insert(compTime.end(), 5);
				if(availableNodes == nodes)
					break;
			}
		}
	}

	//3. Check which servers are in hybernated mode
	if(availableNodes < nodes)
	{
		for (ServerList::iterator it=srvList.begin(); it!=srvList.end(); ++it)
		{
			if (it->state == HYBERNATED)
			{
				availableNodes++;
				compTime.insert(compTime.end(), 90);
				if(availableNodes == nodes)
					break;
			}
		}
	}

	//4. Check servers running a job and when they would have enough resources
	//TODO: add a cQueue structure to servers, and add jobs to it
	int availableCores = 0;
	if(availableNodes < nodes)
	{
		for (ServerList::iterator it=srvList.begin(); it!=srvList.end(); ++it)
		{
			serverStartTime = 0;
			if (it->state == RUNNING || it->reserved == true)
			{
				//Go through the jobs that the server is running
				availableCores = 0;
				availableCores = (it->numCores) - (it->coresInUse);

				//If the server has enough available cores for the job
				if(availableCores >= cores)
				{
					BEV << "The server has enough cores available: " << it->ID << endl;
					//If the server is running and it is not reserved, i.e.
					//it is not in a waking up state and available immediately
					if(it->state == RUNNING && it->reserved == false)
					{
						serverCompTime.insert(serverCompTime.end(), 0);
					}
					//If the server is running and it is reserved, i.e.
					//it's in a waking up state, the server will be available
					//in approximately 90 seconds
					//TODO: should add different loops for stby/hyb
					else if(it->state == RUNNING && it->reserved == true)
					{
						serverCompTime.insert(serverCompTime.end(), 90);
					}

				}
				//Else: the server does not have enough free available cores
				else
				{
					cQueue serverJobs = it->jobs;
					BEV << "The server does not have enough cores available: " << it->ID  << " Number of jobs: " << serverJobs.length() << endl;

					//availableCores = (it->numCores) - (it->coresInUse);
//					std::cerr << "Server ID: " << it->ID << endl;
//					std::cerr << "Server available cores: " << availableCores << endl;
//					std::cerr << "Server cores in use: " << it->coresInUse << endl;
//					std::cerr << "Server total cores: " << it->numCores << endl;
//					std::cerr << "Server state: " << it->state << " reserved: " << it->reserved << endl;
					//Go through the jobs that the server holds and form a temporary job list
					// which is sorted according to the completion times of the jobs
				//	std::cerr << "Server jobs length: " << serverJobs.length() << endl;
					for(int i = 0; i < serverJobs.length(); i++)
					{
						//cMessage *msg = (cMessage *)jobs.get(i);
						//SuperComMsg *jobMsg = dynamic_cast<SuperComMsg*>(msg);
						SuperComInternalMsg *serverJobMsg = (SuperComInternalMsg*)serverJobs.get(i);
						bool jobInserted = false;
						simtime_t serverJobComptime = 0;

					//	std::cerr << "Server job cores: " << serverJobMsg->getCores() << endl;

						if(it->state == RUNNING && it->reserved == false)
						{
							serverJobComptime = serverJobMsg->getTimeOfStart() + serverJobMsg->getRunTime();
						}
						else if(it->state == RUNNING && it->reserved == true)
						{
							serverJobComptime = serverJobMsg->getTimeOfStart() + serverJobMsg->getRunTime() + 90;
						}

						if(tempJobList.empty())
						{
					//		std::cerr << "tempJobList is empty." << endl;
							tempJobList.insert(tempJobList.end(), serverJobMsg);
						}
						else
						{
						//	std::cerr << "tempJobList is NOT empty." << endl;
							//std::cerr << "temp job list size: " << (int)tempJobList.size() << endl;
							simtime_t tempCompTime = 0;
							for(int j = 0; j < (int)tempJobList.size(); j++)
							{
								//std::cerr << "temp job list index: " << j << endl;
								//cMessage *tempMsg = (cMessage *)tempJobList.get(i);
								SuperComInternalMsg *tempJobMsg = tempJobList[j];
//								std::cerr << "serverJobComptime: " << serverJobComptime << endl;
//								std::cerr << "tempjob value: " << tempJobList[j] << endl;
//								std::cerr << "tempjobStart: " << tempJobMsg->getTimeOfStart() << endl;
//								std::cerr << "tempjob runtime: " << tempJobMsg->getRunTime() << endl;
								//SuperComMsg *tempJobMsg = dynamic_cast<SuperComMsg*>(tempMsg);
								tempCompTime = tempJobMsg->getTimeOfStart() + tempJobMsg->getRunTime();
							//	std::cerr << "tempcomptime: " << tempCompTime << endl;
								if(serverJobComptime < tempCompTime)
								{
							//		std::cerr << "inserting at index: " << j << endl;
									tempJobList.insert(tempJobList.begin()+j, serverJobMsg);
									jobInserted = true;
									break;
								}
							}
						//	std::cerr << "temp job list size after for: " << (int)tempJobList.size() << endl;
							if(jobInserted == false)
							{
								tempJobList.insert(tempJobList.end(), serverJobMsg);
							}
						//	std::cerr << "temp job list size after: " << (int)tempJobList.size() << endl;
						}
					}

//					for(int k = 0; k < (int)tempJobList.size(); k++)
//					{
//						//cMessage *tempMsg = tempJobList[k];
//						//SuperComMsg *tempJobMsg = dynamic_cast<SuperComMsg*>(tempMsg);
//						SuperComMsg *tempJobMsg  = tempJobList[k];
//						EV << "TempJob Index: " << k << " Cores: " << tempJobMsg->getCores() << " Completion time: " << SIMTIME_DBL(tempJobMsg->getTimeOfStart()) + tempJobMsg->getRunTime() << endl;
//
//					}

					//Go through the temporary job list and add the number of cores the job holds
					// to the amount of available cores
			//		std::cerr << "cores available before: " << availableCores << endl;
					for(int k = 0; k < (int)tempJobList.size(); k++)
					{
						//cMessage *tempMsg = tempJobList[k];
						//SuperComMsg *tempJobMsg = dynamic_cast<SuperComMsg*>(tempMsg);
						SuperComInternalMsg *tempMsg  = tempJobList[k];
						//EV << "TempJob Index: " << k << " Cores: " << tempJobMsg->getCores() << " Completion time: " << SIMTIME_DBL(tempJobMsg->getTimeOfStart()) + tempJobMsg->getRunTime() << endl;

						availableCores = availableCores + tempMsg->getCores();
						serverStartTime = serverStartTime + ((tempMsg->getTimeOfStart() + tempMsg->getRunTime()) - serverStartTime);

					//	std::cerr << "cores available: " << availableCores << endl;

						//If there are enough cores available the node is considered free at this moment in time
						if(availableCores >= cores)
						{
						//	std::cerr << "enough cores available: " << availableCores << endl;
							serverCompTime.insert(serverCompTime.end(), serverStartTime);
							tempJobList.clear();
							break;
						}
					}
					//std::cerr << "temjoblist size: " << (int)tempJobList.size() << endl;
					//tempJobList.clear();
				}
			}
		}

		// Sort the server comptime list with smallest values first
		// and count the number of available nodes
		// add the values to the comptime vector
		sort(serverCompTime.begin(), serverCompTime.end());

		//std::cerr << "server compTime size: " << (int)serverCompTime.size() << endl;
		for(int i = 0; i < (int)serverCompTime.size(); i++)
		{
			EV << "serverCompTime Index: " << i << " value: " << serverCompTime[i] << endl;
			compTime.insert(compTime.end(), serverCompTime[i]);
			availableNodes++;
			if(availableNodes == nodes)
				break;
		}

	}



	//Sort the running jobs in descending order
	sort(compTime.begin(), compTime.end());

	int neededNodes = 0;
	//Get the nodes number of the running jobs and increase estimated start time
	//If all needed nodes are found, stop the loop
	for (unsigned int i=0; i<compTime.size(); i++)
	{
		neededNodes++;
		EV << "CompTime Index: " << i << " value: " << compTime[i] << endl;
		startTime = startTime + (compTime[i]-startTime);
		if(neededNodes == nodes)
					break;
	}
	compTime.clear();
	serverCompTime.clear();
	return startTime;

}

/**
 * Gets the average queue time of the cluster.
 */

double SuperComRmsApp::getAverageQueueTime()
{

	double sum = 0;
	double averageQueueTime = 0;
	for(int i = 0; i < (int) QueueTime.size(); i++)
	{
		sum = sum + SIMTIME_DBL(QueueTime[i]);
	}

	if(sum != 0)
	{
		averageQueueTime = sum/(QueueTime.size());
	}
	else
	{
		averageQueueTime = 0;
	}

	return averageQueueTime;

}

double SuperComRmsApp::getEstimatedWaitTime()
{
	double waitTime = 0.0;
	double runningJobs = 0.0;
	double queuedJobs = 0.0;
	int totalCores = 0;
	totalCores = getServerCores(0) * getNumServers();
	//BEV << "Total Cores: " << totalCores << endl;

	runningJobs = getRunningJobsTime();
	queuedJobs = getQueuedJobsTime();

	waitTime = (runningJobs + queuedJobs) / totalCores;

	return waitTime;

}

double SuperComRmsApp::getRunningJobsTime()
{
	double runJobTime = 0.0;
	double temp = 0.0;
    for (RunjobList::iterator it=rjbList.begin(); it!=rjbList.end(); ++it)
    {
    	temp = 0.0;
    	temp = (it->nodes * it->cores) * (SIMTIME_DBL((it->compTime) - simTime()));
    	runJobTime += temp;
    	EV << "Running job nodes: " << it->nodes << endl;
    	EV << "Running job cores: " << it->cores << endl;
    	EV << "Running job time remaining: " << SIMTIME_DBL((it->compTime) - simTime()) << endl;

    }
   // BEV << "Running jobs: " << runJobTime << endl;
    return runJobTime;

}

double SuperComRmsApp::getQueuedJobsTime()
{
	double queuedJobTime = 0.0;
	double tempTime = 0.0;
	cQueue tempQueue = jobQueue;
	int nodes = 0;
	double runTime = 0.0;
	int cores = 0;

	if(!tempQueue.empty())
	{
		do
		{
			tempTime = 0.0;

			//Get the job message from queue
			cMessage *msg = (cMessage *)tempQueue.pop();
			SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(msg);

			nodes = jobMsg->getNodes(); //nodes needed for the job
			runTime = jobMsg->getRunTime();
			cores = jobMsg->getCores();

	    	EV << "Queued job nodes: " << nodes << endl;
	    	EV << "Queued job cores: " << cores << endl;
	    	EV << "Queued job wall time: " << runTime << endl;

			tempTime = runTime * nodes * cores;
			queuedJobTime += tempTime;


		}while(!tempQueue.empty());
	}

	//BEV << "Queued jobs time remaining: " << queuedJobTime << endl;

    return queuedJobTime;

}

// Puts a message into a queue
void SuperComRmsApp::enqueue(cMessage *msg, bool queueBegin, bool interActive, int jobPriority)
{
	int priority;

	//If the queue is not empty
	if(!jobQueue.empty())
	{
		//Insert the message as the first element if it is not an interactive job
		//This is the case when no jobs were found for a popped request
		if(queueBegin == true && interActive == false)
		{
			jobQueue.insertBefore(jobQueue.front(), msg);
		}
		//Interactive job
		else if(queueBegin == true && interActive == true)
		{

			//Go through the queue and find the first job that is not an interactive job
			for(int index = 0; index < jobQueue.getLength(); index++)
			{
				//Get the job from queue: 0, 1, 2, 3,...
				cMessage *mmsg = (cMessage *)jobQueue.get(index);
				SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(mmsg);
				priority = jobMsg->getPriority(); //Check the priority of the job

				if(priority != 0) //If it is not an interactive job, place the incoming job before it
				{
					jobQueue.insertBefore(jobQueue.get(index), msg);
					break;
				}
				// Else, if the end of the queue is reached place the job at the end
				else if(index == (jobQueue.getLength() - 1) )
				{
					jobQueue.insert(msg);
					break;
				}

			}

		}
		//Non-interactive job, arrange the queue according to the priority
		else
		{
			//Go through the queue
			for(int index = 0; index < jobQueue.getLength(); index++)
			{
				//Get the job from queue: 0, 1, 2, 3,...
				cMessage *mmsg = (cMessage *)jobQueue.get(index);
				SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(mmsg);
				priority = jobMsg->getPriority(); //Check the priority of the job

				//Place the job before this job, if it not interactive or has smaller priority
				if(priority != 0 && priority < jobPriority)
				{
					jobQueue.insertBefore(jobQueue.get(index), msg);
					break;
				}
				// Else, if the end of the queue is reached place the job at the end
				else if(index == (jobQueue.getLength() - 1) )
				{
					jobQueue.insert(msg);
					break;
				}

			}

		}
	}
	//Else: the queue is empty, just place the request in queue
	else
	{
		jobQueue.insert(msg);
	}
}

// Pops a message from queue
//NOTE: this queue holds SuperComInternalMsgs, which are used for contacting a server
// The messages have parameters such as needed memory ---> information about a job!
// Other parameters can be added, such as walltime, time of submission, etc.

// 2. Job Finishes execution

void SuperComRmsApp::dequeue()
{
	//If the queue is not empty
	if(!jobQueue.empty())
	{
		//FIFO
		if(getScheduling() == 1)
		{
			//Find resources for the first job on the queue
			findResources();
			BEV << "Finished checking the queue." <<endl;
			//Check for idle resources
			if(getEnergyAware() == true)
			{
				BEV << "Checking for idle resources.." << endl;
				checkServers();
			}

		}
		// BACKFILL FIRST FIT
		if(getScheduling() == 2)
		{
			//Find resources for the first job on the queue
			findResources();

			//If the queue is still empty and there are more than 1 jobs, and free resources, continue to search for jobs
			if(!jobQueue.empty() && jobQueue.getLength() > 1)
			{

				BEV << "Finished checking the queue. Now searching for jobs that can be executed..." <<endl;

				//1. Go through all the jobs in the queue before the 1st one
				//2. Find the first job that can be executed with the available resources
				//and before the estimated start time of the first job
				//3. If resources are still free, go through the search again

				int index = 1; //index starts from 1

				//Search for jobs until end of the queue is reached
				// or there are no free resources
				do
				{

					//Get the job from queue: 1, 2, 3,...
					cMessage *msg = (cMessage *)jobQueue.get(index);

					SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(msg);
					int nodes = jobMsg->getNodes(); //nodes needed for the job
					int needMemory = jobMsg->getNeedMemory();
					int serverNumber[nodes];
					double runTime = jobMsg->getRunTime();
					int cores = jobMsg->getCores();
					IPvXAddress clientAddr = jobMsg->getClientAddress();
					int load = jobMsg->getLoad();
					long InputData = jobMsg->getInputData();
					double loadedTime = jobMsg->getLoadedTime();
					int jobPriority = jobMsg->getPriority();

					bool serversFound = true;
					searchServers(nodes, needMemory, cores, serverNumber, serversFound);

					if(serversFound == false)
					{
						//No servers found, continue search
						BEV << "No servers found for the job index: " << index << " client address: " << clientAddr << endl;
						//delete jobMsg;

					}
					else
					{
						BEV << "Servers found for the job index: " << index << " client address: " << clientAddr << endl;
						// 1. Check the estimated completion time of the new job
						simtime_t compTime = runTime + simTime();
						BEV << "Completion time of the new job: " << compTime << endl;

						// Check the estimated start time of the 1st job in the queue
						simtime_t startTime;
						startTime = getEstimatedStartTime(); //TODO_ causes seg fault
						BEV << "Estimated start time of the 1st job: " << startTime << endl;

						//The new job can be executed if it is able to complete before the 1st job is estimated to start
						if(startTime > compTime || jobPriority == 0)
						{
							BEV << "The job can be executed before the 1st element in the queue." << endl;
							//Remove job from queue
							jobQueue.remove(jobQueue.get(index));

							//Insert the queue time into the vector
							//QueueTime.insert(QueueTime.end(), simTime()-(jobMsg->getSubTime()));

							// Set the index as zero, and start the search again (from index = 1)
							//index = 0; //start from the beginning
							index--; // start from the next element
							initServerConnection(nodes, serverNumber, runTime, needMemory, clientAddr, cores, load, InputData, loadedTime);

						}
						//Else: job cannot be executed, continue search
						else
						{
							BEV << "The job cannot be executed before the 1st element in the queue." << endl;;

						}

					}
					//Increment the index
					index++;
				} while(!jobQueue.empty() && index != jobQueue.getLength() && isResourcesAvailable() == true);
				BEV << "Backfill first fit finished..." <<endl;

				//Check for idle resources
				if(getEnergyAware() == true)
				{
					BEV << "Checking for idle resources.." << endl;
					checkServers();
				}

			}

		}
		// BACKFILL BEST FIT
		if(getScheduling() == 3)
		{
			//Find resources for the first job on the queue
			findResources();

			//If the queue is still empty and more than 1, continue to search for jobs
			if(!jobQueue.empty() && jobQueue.getLength() > 1)
			{

				BEV << "Finished checking the queue. Now searching for jobs that can be executed using backfill best..." <<endl;


				//1. Check if the job can be executed with the free resources
				// - what resources are needed
				// - can the job be executed before the first job on the queue

				//2. Check what resources are left free after the job is allocated
				// - How many nodes are still free
				// - How much memory is still free
				// - How many cores are still free

				//3. Go through the queue and compare "spare parts"

				//4. Select the job that leaves the less free resources

				//5. Repeat parts 1-4 until no suitable job is found.



				//Search for jobs until end of the queue is reached
				// or there are no free resources

				bool stopSearch;
				do
				{

					bool jobFound = false;
					bool interActiveJobFound = false;

					int index = 1; //index starts from 1

					// Add info about the optimal job
					optjobInfo *optjob;
					ojbList.push_back(optjobInfo());
					optjob = &ojbList.back();

					do
					{

						//Free resources
						int freeNodes = 0;
						int freeMemory = 0;
						int freeCores = 0;

						//Get the job from queue: 1, 2, 3,...
						//cMessage *msg = (cMessage *)jobQueue.get(index);
						//SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(msg);
						SuperComInternalMsg *jobMsg = (SuperComInternalMsg *)jobQueue.get(index);
//						if(SIMTIME_DBL(simTime()) > 500000)
//						{
//							//std::cerr << "Index: " << index << endl;
//							//std::cerr << "Queue length: " << jobQueue.getLength() << endl;
//						}
						int nodes = jobMsg->getNodes(); //nodes needed for the job
						int needMemory = jobMsg->getNeedMemory();
						int serverNumber[nodes];
						double runTime = jobMsg->getRunTime();
						int cores = jobMsg->getCores();
						IPvXAddress clientAddr = jobMsg->getClientAddress();
						int load = jobMsg->getLoad();
						simtime_t subTime = jobMsg->getSubTime();

						long InputData = jobMsg->getInputData();
						double loadedTime = jobMsg->getLoadedTime();
						int jobPriority = jobMsg->getPriority();
						bool serversFound = true;
						searchServers(nodes, needMemory, cores, serverNumber, serversFound);

						if(serversFound == false)
						{
							//No servers found, continue search
							BEV << "No servers found for the job index: " << index << " client address: " << clientAddr << endl;
							//delete msg;

						}
						else
						{
							BEV << "Servers found for the job index: " << index << " client address: " << clientAddr << endl;
							// 1. Check the estimated completion time of the new job
							simtime_t compTime = runTime + simTime();
							BEV << "Completion time of the new job: " << compTime << endl;

							// Check the estimated start time of the 1st job in the queue
							simtime_t startTime;
							startTime = getEstimatedStartTime();

							BEV << "Estimated start time of the 1st job: " << startTime << endl;

							//The new job can be executed if it is able to complete before the 1st job is estimated to start
							if(startTime > compTime && jobPriority != 0)
							{
								BEV << "The job can be executed before the 1st element in the queue." << endl;
								jobFound = true; //At least one job was found

								//Calculate what resources are left free if the job is executed
								calculateFreeResources(nodes, needMemory, cores, freeNodes, freeMemory, freeCores);

								//Set this job as the optimal job if it has less free resources than the optimal job
								if(freeNodes < optjob->freeNodes || optjob->freeNodes == -1)
								{
									BEV << "The job is selected as the optimal job. Nodes." << endl;
									optjob->jobIndex = index;
									optjob->freeNodes = freeNodes;
									optjob->freeMemory = freeMemory;
									optjob->freeCores = freeCores;
									addOptimalJob(optjob, nodes, needMemory, cores, runTime, clientAddr, serverNumber, load, subTime, InputData, loadedTime);
								}
								else if(freeNodes == optjob->freeNodes)
								{
									if(freeCores < optjob->freeCores || optjob->freeCores == -1)
									{
										BEV << "The job is selected as the optimal job. Cores." << endl;
										optjob->jobIndex = index;
										optjob->freeNodes = freeNodes;
										optjob->freeMemory = freeMemory;
										optjob->freeCores = freeCores;
										addOptimalJob(optjob, nodes, needMemory, cores, runTime, clientAddr, serverNumber, load, subTime, InputData, loadedTime);
									}
									else if(freeCores == optjob->freeCores)
									{
										if(freeMemory < optjob->freeMemory || optjob->freeMemory == -1)
										{
											BEV << "The job is selected as the optimal job. Memory." << endl;
											optjob->jobIndex = index;
											optjob->freeNodes = freeNodes;
											optjob->freeMemory = freeMemory;
											optjob->freeCores = freeCores;
											addOptimalJob(optjob, nodes, needMemory, cores, runTime, clientAddr, serverNumber, load, subTime, InputData, loadedTime);

										}
									}

								}

							}
							//Interactive job can be executed right away
							else if(jobPriority == 0)
							{
								jobFound = true;
								interActiveJobFound = true; //interactive job was found
								BEV << "Interactive job can be executed right away." << endl;
								//Remove job from queue
								jobQueue.remove(jobQueue.get(index));

								//Insert the queue time into the vector
								//QueueTime.insert(QueueTime.end(), simTime()-(jobMsg->getSubTime()));

								// Set the index as zero, and start the search again (from index = 1)
								//index = 0; //start from the beginning
								index--; // start from the next element
								initServerConnection(nodes, serverNumber, runTime, needMemory, clientAddr, cores, load, InputData, loadedTime);
							}

							//Else: job cannot be executed, continue search
							else
							{
								BEV << "The job cannot be executed before the 1st element in the queue." << endl;

							}

						}
						//Increment the index
						index++;
					} while(!jobQueue.empty() && index != jobQueue.getLength()); //Go through the queue until the end is reached

					//Remove the optimal job from queue and initiate connections
					if(jobFound == true)
					{
						//If an interactive job was found, the search needs to be done again to ensure that there are free servers
						if(interActiveJobFound == false)
						{
							int serverNumber[optjob->nodes];
							for(int i = 0; i < (optjob->nodes); i++)
							{
								serverNumber[i] = optjob->serverNmb[i];
							}

							//Insert the queue time into the vector
							//QueueTime.insert(QueueTime.end(), simTime()-(optjob->subTime));

							initServerConnection(optjob->nodes, serverNumber, optjob->runTime, optjob->needMemory, optjob->clientAddress, optjob->cores, optjob->load, optjob->InputData, optjob->loadedTime);

							jobQueue.remove(jobQueue.get(optjob->jobIndex));
						}

						stopSearch = false;

						ojbList.clear();

					}
					else
					{
						stopSearch = true;
						ojbList.clear();
					}

				} while(isResourcesAvailable() == true && stopSearch == false && jobQueue.getLength() > 1); //Search for jobs until there are no resources or no job is found

				BEV << "Backfill best fit finished..." <<endl;
				//Check for idle resources
				if(getEnergyAware() == true)
				{
					BEV << "Checking for idle resources.." << endl;
					checkServers();
				}
			}
		}
	}
	else
	{
		BEV << "Queue empty." << endl;
		//Check for idle resources
		if(getEnergyAware() == true)
		{
			BEV << "Checking for idle resources.." << endl;
			checkServers();
		}
	}

}

void SuperComRmsApp::addOptimalJob(optjobInfo *optjob, int nodes, int needMemory, int cores, double runTime, IPvXAddress clientAddr, int serverNumber[], int load, simtime_t subTime, long InputData, double loadedTime)
{
	optjob->serverNmb.clear();

	optjob->nodes = nodes;
	optjob->needMemory = needMemory;
	optjob->runTime = runTime;
	optjob->clientAddress = clientAddr;
	optjob->cores = cores;
	optjob->load = load;
	optjob->InputData = InputData;
	optjob->loadedTime = loadedTime;
	optjob->subTime = subTime;

	for(int i = 0; i < nodes; i++)
	{
		optjob->serverNmb.insert(optjob->serverNmb.end(), serverNumber[i]);
	}

}

void SuperComRmsApp::findResources()
{

	// Extract jobs from the queue until the queue becomes empty
	// or no suitable servers are found for the first job in the queue
	bool serversFound;
	do {

		BEV << "Popping a job from queue." <<endl;

		//NOTE: FIFO gets the first element from the queue
		// backfill algorithm should search for a job

		//Get the job message from queue
		cMessage *msg = (cMessage *)jobQueue.pop();
		SuperComInternalMsg *jobMsg = dynamic_cast<SuperComInternalMsg*>(msg);

		int nodes = jobMsg->getNodes(); //nodes needed for the job
		int needMemory = jobMsg->getNeedMemory();
		int serverNumber[nodes];
		double runTime = jobMsg->getRunTime();
		int cores = jobMsg->getCores();
		int load = jobMsg->getLoad();
		long InputData = jobMsg->getInputData();
		double loadedTime = jobMsg->getLoadedTime();
		int jobPriority = jobMsg->getPriority();


		serversFound = true;
		//Search for servers again
		searchServers(nodes, needMemory, cores, serverNumber, serversFound);

		//No suitable server(s) were found, place the job into a queue
		//When the queue is popped, the server who finished a certain job is contacted
		//Else: put the message back in a queue
		//NOTE: the job should be placed on top of the queue, as the first object
		if(serversFound == false)
		{

			BEV << "Suitable servers were not found, placing the popped request back into the queue." <<endl;
			// Place the message into the queue, queueBegin = true --> place it as the first element
			//False -> not an interactive job, negligible in this case because the job is the 1st element in the queue
			enqueue(jobMsg, true, false, jobPriority);

		}
		//All needed servers were found, initiate connections
		else
		{
			BEV << "Suitable servers found. Initiate connections." <<endl;

			//Insert the queue time into the vector
			//QueueTime.insert(QueueTime.end(), simTime()-(jobMsg->getSubTime()));

			// Initiate message, but do not initiate connection, send the request message instead right away
			initServerConnection(nodes, serverNumber, runTime, needMemory, jobMsg->getClientAddress(), cores, load, InputData, loadedTime);
			delete jobMsg;

		}

	} while(!jobQueue.empty() && serversFound == true);

}

/** Checks if the queue is empty
 *
 */
bool SuperComRmsApp::isQueueEmpty()
{
	if(jobQueue.empty())
	{
		return true;

	}
	else
	{
		return false;
	}

}

/**Checks the server list if there are available resources.
 * Server is considered idle if it has enough cores
 * available
 */
bool SuperComRmsApp::isResourcesAvailable()
{
	bool available = false;
	int coresInUse = 0;
	int availableCores = 0;

	for (int i = 0; i < getNumServers(); i++)
	{
		coresInUse = getServerCoresInUse(i);
		availableCores = getServerCores(i) - coresInUse;
		if((getServerState(i) == IDLE) || (getServerState(i) == DOWN) || (getServerState(i) == STANDBY) || (getServerState(i) == HYBERNATED))
		{
			available = true;
			break;

		}
		else if(availableCores > 0)
		{
			available = true;
			break;
		}

	}

	return available;

}

void SuperComRmsApp::calculateFreeResources(int nodes, int needMemory, int cores, int &freeNodes, int &freeMemory, int &freeCores)
{
	int coresAvailable = 0;
	//What resources are free currently
	for (int i = 0; i < getNumServers(); i++)
	{
		//If the server does not have all cores in use, it is available
		if(getServerCoresInUse(i) < getServerCores(i))
		{
			coresAvailable = getServerCores(i) - getServerCoresInUse(i);
			//If there are enough cores, the node is considered free
			if((coresAvailable - cores) > 0)
			{
				freeNodes++;
				freeCores = freeCores + coresAvailable;
				freeMemory = freeMemory + getServerMemory(i);
			}
		}
	}

	//What resources are free after job allocation
	freeNodes = freeNodes - nodes;
	freeMemory = freeMemory - (needMemory * nodes);
	freeCores = freeCores - (cores * nodes);

	BEV << "Free nodes: " << freeNodes << endl;
	BEV << "Free memory: " << freeMemory << endl;
	BEV << "Free cores: " << freeCores << endl;

	return;
}

//Function for searching servers
int* SuperComRmsApp::searchServers(int nodes, int needMemory, int cores, int serverNumber[], bool &serversFound)
{
	//Init the serverNumber
	for (int i = 0; i < nodes; i++)
	{
		serverNumber[i] = -1;
	}

	int coresInUse = 0;
	int availableCores = 0;
	int candidateCores;
	serverState candidateState;

	for (int j = 0; j < nodes; j++)
	{
		candidateCores = -1;

		for (int i = 0; i < getNumServers(); i++)
		{

			coresInUse = getServerCoresInUse(i);
			availableCores = getServerCores(i) - coresInUse; // The amount of available cores at the server

//			EV << "Server cores: " << getServerCores(i) << endl;
//			EV << "availableCores: " << availableCores << endl;
//			EV << "coresInUse: " << coresInUse << endl;

			// The server must have enough memory and free cores
			// status can be even running
			// Select a server that has enough memory, is not reserved and has enough cores
			if(getServerMemory(i) >= needMemory && getServerReserved(i) == false && availableCores >= cores)
			{
				//Select the server that has the minimum amount of needed cores
				if(availableCores < candidateCores || candidateCores == -1)
				{
					serverNumber[j] = i;
					candidateCores = availableCores;
					candidateState = (serverState)getServerState(i);
					EV << "Selecting server: " << i << " cores in use: " << (coresInUse + cores) << " state: " << candidateState << endl;
					continue;
				}
				//Selects a server that is in idle state if the previous server's state was down/standby/hybernated
				//This selection favours idle servers that have all cores available
				else if(availableCores == candidateCores && (candidateState == DOWN || candidateState == STANDBY || candidateState == HYBERNATED))
				{
					serverNumber[j] = i;
					candidateCores = availableCores;
					candidateState = (serverState)getServerState(i);
					EV << "Selecting server: " << i << " cores in use: " << (coresInUse + cores) << " state: " << candidateState << endl;
					continue;
				}

			}

		} //for i
		if(serverNumber[j] != -1)
		{
			setServerReserved(serverNumber[j], true);
		}

	} //for j

	// Check if servers were found
	for (int k = 0; k < nodes; k++ ) {

		if(serverNumber[k] == -1) {
			serversFound = false;
		}
		else {
			// Free the reserved server
			setServerReserved(serverNumber[k], false);
		}
	}

	// Return the servers
	return serverNumber;

}

//Function to initiate connections to servers
void SuperComRmsApp::initServerConnection(int nodes, int serverNumber[], double runTime, int needMemory, IPvXAddress clientAddr, int cores, int load, long InputData, double loadedTime)
{

	//Set the selected servers for the client struct
	setClientServerNumber(serverNumber, clientAddr);

	bool wakeUp = false;

	//Go through the selected servers and check if any servers need to be waken up
	for (int k = 0; k < nodes; k++)
	{
		//setServerReserved(serverNumber[k], true); //reserve the server, this is needed for servers that have to be waken up
		//reserve the cores of the server
		if(serverNumber[k] == 26)
		{
			//std::cerr << " Server index before setcoresInUse: " << serverNumber[k] << endl;
			//std::cerr << " cores in use: " << getServerCoresInUse(serverNumber[k]) << endl;
			//std::cerr << " total: " << getServerCores(serverNumber[k]) << endl;
//			std::cerr << " server state: " << getServerState(serverNumber[k]) << endl;
//			serverInfo *server = lookupServer(serverNumber[k]);
			//std::cerr << " server jobs: " << (server->jobs).length() << endl;
		}
		setServerCoresInUse(serverNumber[k], (getServerCoresInUse(serverNumber[k])) + cores);

		if(getServerCoresInUse(serverNumber[k]) > getServerCores(serverNumber[k]))
		{
			error("initServerConnection: More cores in use than total amount.");
		}
		if(serverNumber[k] == 26)
		{
//			std::cerr << " Server index after: " << serverNumber[k] << endl;
//			std::cerr << " cores in use: " << getServerCoresInUse(serverNumber[k]) << endl;
//			std::cerr << " total: " << getServerCores(serverNumber[k]) << endl;
//			std::cerr << " server state: " << getServerState(serverNumber[k]) << endl;
//			serverInfo *server = lookupServer(serverNumber[k]);
//			std::cerr << " server jobs: " << (server->jobs).length() << endl;
//			std::cerr << " simtime: " << simTime() << endl;
//			std::cerr << " client: " << clientAddr << endl;
		}


		if (getServerState(serverNumber[k]) == DOWN || (getServerState(serverNumber[k]) == STANDBY) || (getServerState(serverNumber[k]) == HYBERNATED))
		{
			incrementClientNeedWakeupServers(clientAddr);
			wakeUp = true;
			setServerReserved(serverNumber[k], true); //reserve the server, this is needed for servers that have to be waken up

			BEV << "Generate a wakeup message to the server: " << serverNumber[k] << endl;
			//std::cerr << "Generate a wakeup message to the server: " << serverNumber[k]  << " at simtime: " << simTime() << endl;
			//std::cerr << " client: " << clientAddr << endl;
			// Generate a self message for waking up a server
			SuperComInternalMsg *wuMsg = new SuperComInternalMsg("INTERNAL_WAKEUP_MSG", INTERNAL_WAKEUP_MSG);
			wuMsg->setServerIndex(serverNumber[k]);
			wuMsg->setRunTime(runTime);
			wuMsg->setClientAddress(clientAddr);
			wuMsg->setNeedMemory(needMemory);
			wuMsg->setCores(cores);
			wuMsg->setLoad(load);
			scheduleAt(simTime(), wuMsg);

		}
		else if(getServerState(serverNumber[k]) == IDLE)
		{
			setServerState(serverNumber[k], RUNNING);
		}

	}

	//Go through the selected servers and add jobs to the server queue
	for (int k = 0; k < nodes; k++)
	{
		// Generate a self message for the server queue
		SuperComInternalMsg *jobMsg = new SuperComInternalMsg("INTERNAL_JOB_MSG", INTERNAL_JOB_MSG);
		jobMsg->setServerIndex(serverNumber[k]);
		jobMsg->setRunTime(runTime);
		jobMsg->setClientAddress(clientAddr);
		jobMsg->setNeedMemory(needMemory);
		jobMsg->setCores(cores);
		jobMsg->setLoad(load);
		jobMsg->setInputData(InputData);
		jobMsg->setLoadedTime(loadedTime);
		jobMsg->setNodes(nodes);

		//If there are servers that need to wakeup, the time of start is after 90seconds
		if(wakeUp == true)
			jobMsg->setTimeOfStart(simTime() + 90);
		else
			jobMsg->setTimeOfStart(simTime());

		//Add the job as a running job into the server structure
		addServerJobs(serverNumber[k], jobMsg);

	}

	if(wakeUp == false)
	{
		BEV << "All the needed servers are awake, sending the requests immediately." << endl;
		//std::cerr << "All the needed servers are awake, sending the requests immediately." << endl;
		sendClientRequests(clientAddr);

	}
	else
	{
		BEV << "Some servers need to be waken up, delaying the request until all are awake." << endl;
		//std::cerr << "Some servers need to be waken up, delaying the request until all are awake." << endl;

	}

}

void SuperComRmsApp::sendClientRequests(IPvXAddress clientAddr)
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

	//Insert the queue time into the vector
	QueueTime.insert(QueueTime.end(), simTime()-subTime);
	double waitTime = 0.0;
	waitTime = SIMTIME_DBL(simTime()-subTime);
	stats->addWaitTime(waitTime);

	simtime_t compTime = runTime + simTime();

	//Add the job into the running jobs structure
	runjobInfo *runjob;
	rjbList.push_back(runjobInfo());
	runjob = &rjbList.back();
	runjob->nodes = nodes;
	runjob->needMemory = needMemory;
	runjob->runTime = runTime;
	runjob->clientAddress = clientAddr;
	runjob->subTime = subTime;
	runjob->compTime = runTime + simTime();
	runjob->cores = cores;
	runjob->startTime = simTime();

	//Compute Network Energy
	computeNetworkEnergy(getLastCalculated(), simTime());


	for (int k = 0; k < nodes; k++)
	{

		// Generate a self message for initiating a connection with a server
		SuperComInternalMsg *jobMsg = new SuperComInternalMsg("INTERNAL_JOB_MSG", INTERNAL_JOB_MSG);
		jobMsg->setServerIndex(client->serverNmb[k]);
		jobMsg->setRunTime(runTime);
		jobMsg->setClientAddress(clientAddr);
		jobMsg->setNeedMemory(needMemory);
		jobMsg->setCores(cores);
		jobMsg->setLoad(load);
		jobMsg->setInputData(InputData);
		jobMsg->setLoadedTime(loadedTime);
		jobMsg->setNodes(nodes);
		jobMsg->setTimeOfStart(simTime());

		//Add the job as a running job into the server structure
		//addServerJobs(client->serverNmb[k], jobMsg->dup());

		if(client->serverNmb[k] == 26)
		{
//			std::cerr << " Server index at send client requests: " << client->serverNmb[k]  << endl;
//			std::cerr << " cores in use: " << getServerCoresInUse(client->serverNmb[k] ) << endl;
//			std::cerr << " total: " << getServerCores(client->serverNmb[k] ) << endl;
////			std::cerr << " server state: " << getServerState(client->serverNmb[k] ) << endl;
//			serverInfo *server = lookupServer(client->serverNmb[k] );
//			std::cerr << " server jobs: " << (server->jobs).length() << endl;
//			std::cerr << " simTime: " << simTime() << endl;
		}

		// Allocate memory from the selected server and set state as RUNNING
		setServerMemory(client->serverNmb[k], (getServerMemory(client->serverNmb[k])) - needMemory);
		setServerState(client->serverNmb[k], RUNNING);
		setServerCompTime(client->serverNmb[k], compTime);
		setServerReserved(client->serverNmb[k], false); //Free the server
		//setServerCoresInUse(client->serverNmb[k], (getServerCoresInUse(client->serverNmb[k])) + cores);

		BEV << "Selected server for the client: " << client->serverNmb[k] << endl;
		BEV << "Runtime: " << runTime << endl;

		scheduleAt(simTime(), jobMsg);

	}

}

//Adds client info to the client struct
void SuperComRmsApp::addClientInfo(IPvXAddress usoAddr, IPvXAddress clientAddr, int nodes, int needMemory, double runTime, int cores, int load, long InputData, double loadedTime, int jobPriority, simtime_t subTime)
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
	client->usoAddress = usoAddr;

}

/**
 * Checks for idle resources and shuts them down
 */

void SuperComRmsApp::checkServers()
{
	//Loop through all servers
	for (int i = 0; i < getNumServers(); i++)
	{
		//If the server is idle and not reserved, shut it down
		if(getServerState(i) == IDLE && getServerReserved(i) == false)
		{

			//Avoid suspending servers before all clients have made their request
			if(simTime() > 20)
			{
				if(!jobQueue.empty())
				{
					simtime_t startTime = getEstimatedStartTime() - simTime();
					BEV << "Check Idle servers, Estimated startTime: " << startTime << endl;
					if(startTime > 50)
					{
						BEV << "Generating a suspend action to server: " << i << endl;
						setServerState(i, HYBERNATED);
						setServerReserved(i, true);
						setServerCompTime(i, 90);
						scheduleSleep(i, 2);
					}
				}
				else
				{
					BEV << "Generating a suspend action to server: " << i << endl;
					setServerState(i, HYBERNATED);
					setServerReserved(i, true);
					setServerCompTime(i, 90);
					scheduleSleep(i, 2);
				}
			}

//			if(!jobQueue.empty())
//			{
//				simtime_t startTime = getEstimatedStartTime();
//				BEV << "Estimated startTime: " << startTime << endl;
//
//				BEV << "Generating a standby action to server: " << i << endl;
//				setServerState(i, STANDBY);
//				setServerReserved(i, true);
//				setServerCompTime(i, 5);
//				scheduleSleep(i, 1);
//
//				if(startTime <= 16.5)
//				{
//					BEV << "Do nothing, since job will start in a short time." << endl;
//				}
//				else if((startTime > 16.5) && (startTime <= 127.5))
//				{
//					BEV << "Generating a standby action to server: " << i << endl;
//					setServerState(i, STANDBY);
//					setServerReserved(i, true);
//					setServerCompTime(i, 5);
//					scheduleSleep(i, 1);
//				}
//				else if(startTime > 127.5)
//				{
//					BEV << "Generating a suspend action to server: " << i << endl;
//					setServerState(i, HYBERNATED);
//					setServerReserved(i, true);
//					setServerCompTime(i, 90);
//					scheduleSleep(i, 2);
//				}
//
//			}
//			else
//			{
//				BEV << "Generating a suspend action to server: " << i << endl;
//				setServerState(i, STANDBY);
//				setServerReserved(i, true);
//				setServerCompTime(i, 5);
//				scheduleSleep(i, 1);
//			}

		}

	}

}

//Function for scheduling the sleep timers according to the server index and sleepMode
void SuperComRmsApp::scheduleSleep(int serverIndex, int sleepMode)
{
	serverInfo *server = lookupServer(serverIndex);
	server->evtSleep = new SuperComInternalMsg("INTERNAL_SLEEP_MSG", INTERNAL_SLEEP_MSG);
	server->evtSleep->setServerIndex(server->ID); //Set the server number into the msg
	server->evtSleep->setSleepMode(sleepMode);
	scheduleAt(simTime(), server->evtSleep);

}

//Function for canceling the sleep timers according to the server index
void SuperComRmsApp::cancelSleep(int serverIndex)
{
	serverInfo *server = lookupServer(serverIndex);
	//If a sleep-timer has been scheduled, delete it
	if((server->evtSleep)->isScheduled())
	{
		cancelAndDelete(server->evtSleep);
	}
}

void SuperComRmsApp::insertQueueTime(simtime_t time)
{
	QueueTime.insert(QueueTime.end(), time);
}

simtime_t SuperComRmsApp::getLastCalculated()
{
	return last_calculated;
}

void SuperComRmsApp::setLastCalculated(simtime_t time)
{
	last_calculated = time;
}

long SuperComRmsApp::getRequestsSent()
{
	return requestsSent;
}

void SuperComRmsApp::incrementRequestsSent()
{
	requestsSent++;
}

long SuperComRmsApp::getJobsCompleted()
{
	return jobsCompleted;
}

void SuperComRmsApp::incrementJobsCompleted()
{
	jobsCompleted++;
}


/**
 * Function to get the packets per second from a given
 * time interval. Received and sent packets are retrieved
 * from the physical ethernet module.
 *
 */

double SuperComRmsApp::getPacketsPerSecond(simtime_t from, simtime_t to, cModule *module, int index)
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
		error("SuperComRmsApp: pps cannot be below zero!");

    EV << " Packets per second: " << pps << endl;

    return pps;

}

/**
 * Computes the energy of the Network Interface of the RMS.
 * First the power of all NICs are computed, and the power
 * due to switching operations is added.
 */

void SuperComRmsApp::computeNetworkEnergy(simtime_t from, simtime_t to)
{
  double E = 0.0;
  /* Servers (NICs) :
   P_max = 0.090934950000000
   P_idle = 0.090807

   PC-based Routers:
   P_max = 69.316105370000002;
   P_idle = 69.316;
   */

  //TODO: correct values missing! And make these parameters
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

  stats->recordNetworkEnergyVector(E);
  stats->recordTotalEnergyVector(E);

  BEV << "Network energy: " << E << " From: " << from << " to: " << to << endl;
  BEV << "Total Network energy: " << E_network  << endl;

}

int SuperComRmsApp::getQueueLength()
{
	return jobQueue.getLength();
}

double SuperComRmsApp::getPUE()
{
	return PUE_var;
}

/**
 * Computes an estimate for the energy of the job
 * by using a single server from the cluster.
 */

double SuperComRmsApp::getEnergyEstimate(int cores, int memory, int coreLoad, double loadedTime, int runTime)
{
	BEV << "getEnergyEstimate() calling server->computeEstimate()" << endl;
	double estimate = 0.0;

	cModule *Servermodule = getParentModule()->getParentModule()->getSubmodule("server", 0)->getSubmodule("tcpApp", 0);
	SuperComServerApp *serverapplication = check_and_cast<SuperComServerApp*>(Servermodule);

	estimate = serverapplication->computeEstimate(cores, memory, coreLoad, loadedTime, runTime);

	//Compute estimate with PUE to get total ICT energy
	estimate = estimate * PUE_var;

	//Compute with ESC to get emissions
	estimate = (estimate * ESC_var)/3600000;
	BEV << " Estimate with CO2: " << estimate << endl;

	return estimate;

}

void SuperComRmsApp::dynamicPUEupdate()
{
	//MiM 5.9.2011 for dynamic PUE
	BEV << "dynamicPUEupdate()" << endl;
	simtime_t now = simTime();

	//1. call every servers' computeEnergy() so that the energy consumption so far will be calculated using the old PUE
	for(int i=0;i<getNumServers();i++)
	{
		BEV << "dynamicPUEupdate() calling server->computeEnergy(), i= " <<i << endl;
		cModule *Servermodule = getParentModule()->getParentModule()->getSubmodule("server", i)->getSubmodule("tcpApp", 0);
		SuperComServerApp *serverapplication = check_and_cast<SuperComServerApp*>(Servermodule);
		serverapplication->computeEnergy();
	}
	//2. Then define the new PUE value:


	//if(now >= dynPUEstartTime && now < dynPUEstopTime){
		//For one year simulation, PUE changes according to the outside temperature;
		//increment by 0.05 each month and after 6 months start decrementing it by 0.05


	//if(now < 60*60*24*180){ //too big value to simtime_t the half year in seconds !!!
			//par("PUE") += 0.05;
			//PUE_var -= 0.05; //does not affect the GUI ???
			PUE_var += dynamicPUEchange_var;
			if(PUE_var <= 1.1)
				PUE_var = 1.1; //do not make PUE smaller than 1.1 (not realistic, <1.0 impossible)
		//}
		//else{
			//PUE_var += 0.05;
		//}
	//}
	BEV << "dynamicPUEupdate(), PUE_var = " << PUE_var <<endl;

	//re-schedule the timer:
	cMessage *PUEmsg = new cMessage("INTERNAL_DYNAMIC_PUE_MSG ", INTERNAL_DYNAMIC_PUE_MSG);
	scheduleAt(now + dynamicPUEupdateInterval_var, PUEmsg);



}

double SuperComRmsApp::getCUE()
{
	double CUE = 0.0;
	//CUE = ESC_var * PUE_var;
	CUE = (ESC_var * PUE_var) / (3.6 * 1000000);
	return CUE;
}

void SuperComRmsApp::addReceivedJobs()
{
	dcStats->addReceivedJobs();
}



/**
 * Constructor.
 */

SuperComRmsAppClientHandler::SuperComRmsAppClientHandler()
{
	evtTout		= new cMessage("timeoutClient", EVT_TOUT);
	delThreadMsg = new SuperComInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);
	finish_var = false;
	outputData_var = 0;
}

/**
 * Destructor.
 */

SuperComRmsAppClientHandler::~SuperComRmsAppClientHandler()
{
	// cancel any pending messages
	hostModule()->cancelAndDelete(evtTout);
	hostModule()->cancelAndDelete(delThreadMsg);
	evtTout = NULL;
	delThreadMsg = NULL;

}

void SuperComRmsAppClientHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}


void SuperComRmsAppClientHandler::sendMessage(cMessage *msg)
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
void SuperComRmsAppClientHandler::established()
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
		sendFinish(getSocket()->getRemoteAddress());

//		BEV << " The job client requested has finished. Informing client and sending output data. " << endl;
//		BEV << " Output data: " << getOutputData() << endl;
//		SuperComMsg* finishMsg = new SuperComMsg("finish");
//		finishMsg->setType(MSGKIND_FINISH);
//		finishMsg->setByteLength(getOutputData()); //Set the size as the amount of output data
//		sendMessage(finishMsg);


		//Delete the client from the cliList in single site case
		if(hostModule()->getFederated() == false)
		{
			BEV << "Deleting client info.." << endl;
			hostModule()->deleteClient(getSocket()->getRemoteAddress());
		}

	}

}

/**
 * Handles the reception of a message
 *
 */
void SuperComRmsAppClientHandler::dataArrived(cMessage* msg, bool urgent)
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

			hostModule()->addReceivedJobs();

			BEV << "Received a request message from client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]" << endl;

			//Increment the number of requests
			hostModule()->incrementRequestsSent();

			long requestedBytes = appmsg->getExpectedReplyLength();

			simtime_t msgDelay = appmsg->getReplyDelay();

			SuperComMsg *rmsg = new SuperComMsg("response");

			//NOTE: TCP_I_DATA causes an error
			// since it is used only for receiving
			rmsg->setKind(TCP_C_SEND);
			rmsg->setType(MSGKIND_RESPONSE);

			// Expected reply
			if (requestedBytes==0)
			{
				rmsg->setByteLength(appmsg->getByteLength());
			}
			else
			{
				rmsg->setByteLength(requestedBytes);
			}


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

			BEV << "Needed memory: " << getNeedMemory() << endl;
			BEV << "Needed nodes: " << getNodes() << endl;
			BEV << "Needed cores: " << getCores() << endl;
			BEV << "Job load: " << getLoad() << endl;
			BEV << "Job loaded time: " << getLoadedTime() << endl;
			BEV << "Client IP: " << appmsg->getClientAddress() << endl;
			BEV << "USO IP: " << getSocket()->getRemoteAddress() << endl;

			int jobPriority = 1;

			//Add client info
			hostModule()->addClientInfo(getSocket()->getRemoteAddress(), appmsg->getClientAddress(), getNodes(), getNeedMemory(), getRuntime(), getCores(), getLoad(), getInputData(), getLoadedTime(), jobPriority, simTime());

			//Check the queue if job can executed
			checkQueue(getSocket()->getRemoteAddress(), appmsg->getClientAddress(), getNodes(), getNeedMemory(), getRuntime(), getCores(), getLoad(), getInputData(), getLoadedTime(), jobPriority, simTime());

			if(hostModule()->getFederated() == true)
			{
				hostModule()->setUSOIPAddr(getSocket()->getRemoteAddress());

			}

			//Send the response message to the client
			sendMessage(rmsg);
			delete appmsg;


			break;
		}
		case MSGKIND_INPUT:
		{

			BEV << "Received an input message from client[address=" << getSocket()->getRemoteAddress() << "]" << endl;

			hostModule()->incrementMsgsRcvd();
			hostModule()->incrementBytesRcvd(appmsg->getByteLength());
			//int jobPriority = getPriority() * getNodes() * getRuntime(); //TODO: Change this to something else

			BEV << "Input data in msg: " << appmsg->getByteLength() << endl;


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

void SuperComRmsAppClientHandler::checkQueue(IPvXAddress usoAddr, IPvXAddress clientAddr, int nodes, int needMemory, double runTime, int cores, int load, long InputData, double loadedTime, int jobPriority, simtime_t subTime)
{

	int serverNumber[nodes];

	bool serversFound = true;

	//Check if queue is empty
	if(hostModule()->isQueueEmpty())
	{

		BEV << "Queue is empty." << endl;

		//Search for servers
		hostModule()->searchServers(nodes, needMemory, cores, serverNumber, serversFound);

		//No server was found, the place the job into a queue
		//When the queue is popped, the server who finished a certain job is contacted
		if(serversFound == false)
		{

			BEV << "No suitable servers found, placing the request into a queue." <<endl;
			hostModule()->enqueueJob(getRuntime(), clientAddr, needMemory, nodes, cores, getSubTime(), getLoad(), getInputData(), getLoadedTime(), jobPriority);


		}
		//All needed servers were found, initiate connections
		else
		{
			//Insert the queue time into the vector
			//hostModule()->insertQueueTime(0);
			hostModule()->initServerConnection(nodes, serverNumber, getRuntime(), needMemory, clientAddr, cores, getLoad(), getInputData(), getLoadedTime());

		}
		//After the servers have been selected, check for idle resources
		if(hostModule()->getEnergyAware() == true)
		{
			BEV << "Checking for idle resources.." << endl;
			hostModule()->checkServers();
		}

	}
	else
	{
		//FIFO scheduling
		if(hostModule()->getScheduling() ==  1)
		{
			//For interactive jobs, search for servers immediately
			if(jobPriority == 0)
			{
				//Search for servers
				hostModule()->searchServers(nodes, needMemory, cores, serverNumber, serversFound);
				//No server was found, the place the job into a queue
				//When the queue is popped, the server who finished a certain job is contacted
				//Interactive job shoulds be placed on top of the queue
				if(serversFound == false)
				{

					BEV << "No suitable servers found for the interactive job, placing the request into a queue." <<endl;
					hostModule()->enqueueJob(getRuntime(), clientAddr, needMemory, nodes, cores, getSubTime(), getLoad(), getInputData(), getLoadedTime(), jobPriority);


				}
				//All needed servers were found, initiate connections
				else
				{
					//Insert the queue time into the vector
					//hostModule()->insertQueueTime(0);
					hostModule()->initServerConnection(nodes, serverNumber, getRuntime(), needMemory, clientAddr, cores, getLoad(), getInputData(), getLoadedTime());

				}

			}
			//Non-interactive jobs are placed in the queue based on the jobPriority
			else
			{
				BEV << "Queue is not empty, placing the non-interactive job request into the queue." << endl;
				hostModule()->enqueueJob(getRuntime(), clientAddr, needMemory, nodes, cores, getSubTime(), getLoad(), getInputData(), getLoadedTime(), jobPriority);

			}

		}
		//Backfill scheduling (no difference between first fit and best fit?)
		//Interactive jobs, should we check if it influences the first N jobs?
		else
		{
			// 1. check whether there are enough resources for the new job
			//NOTE: if resources are not available, check if resources become available before 1st job starts??
			hostModule()->searchServers(nodes, needMemory, cores, serverNumber, serversFound);

			//Servers were not found
			if(serversFound == false)
			{
				BEV << "No suitable servers found, placing the request into a queue." <<endl;
				hostModule()->enqueueJob(getRuntime(), clientAddr, needMemory, nodes, cores, getSubTime(), getLoad(), getInputData(), getLoadedTime(), jobPriority);

			}
			//All needed servers were found
			else
			{
				// check if the job can be executed before the first job on the queue

				//  Check the estimated completion time of the new job
				simtime_t compTime = getRuntime() + simTime();
				BEV << "Completion time of the new job: " << compTime << endl;

				// Check the estimated start time of the 1st job in the queue
				simtime_t startTime;
				startTime = hostModule()->getEstimatedStartTime();
				BEV << "Estimated start time of the 1st job: " << startTime << endl;

				//The new job can be executed if it is able to complete before the 1st job is estimated to start
				//If it is an interactive job, execute it nevertheless
				if(startTime > compTime || jobPriority == 0)
				{
					BEV << "The job can be executed before the 1st element in the queue." << endl;
					//Insert the queue time into the vector
					//hostModule()->insertQueueTime(0);
					hostModule()->initServerConnection(nodes, serverNumber, getRuntime(), needMemory, clientAddr, cores, getLoad(), getInputData(), getLoadedTime());

				}
				//Else: place the request into the queue
				else
				{
					BEV << "The job cannot be executed before the 1st element in the queue. Adding it into queue." << endl;
					hostModule()->enqueueJob(getRuntime(), clientAddr, needMemory, nodes, cores, getSubTime(), getLoad(), getInputData(), getLoadedTime(), jobPriority);

				}

			}

		}

	}

}

/**
 * Handles self-messages regarding this handler.
 *
 */
void SuperComRmsAppClientHandler::timerExpired(cMessage* msg)
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

void SuperComRmsAppClientHandler::sendFinish(IPvXAddress clientAddr)
{
	BEV << " The job client requested has finished. Informing client and sending output data. " << endl;
	BEV << " Output data: " << getOutputData() << endl;
	SuperComMsg* finishMsg = new SuperComMsg("finish");
	finishMsg->setType(MSGKIND_FINISH);
	finishMsg->setByteLength(getOutputData()); //Set the size as the amount of output data
	finishMsg->setClientAddress(clientAddr);


	//Cancel the timeout event
	if(evtTout->isScheduled())
	{
		hostModule()->cancelEvent(evtTout);
	}

	sendMessage(finishMsg);

}


void SuperComRmsAppClientHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void SuperComRmsAppClientHandler::closed()
{

	BEV << "Connection closed." << endl;

	//Schedule a self message for removing the thread
	scheduleAt(simTime(), delThreadMsg);


}

/**
 * Closes an active connection
 */
void SuperComRmsAppClientHandler::close()
{

	// cancel the session timeout event and call default close()
	//cancelEvent(evtTout);
	hostModule()->cancelAndDelete(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}

/**
 * Get the host module (i.e., the instance of SuperComRmsApp which spawns the thread).
 */
SuperComRmsApp* SuperComRmsAppClientHandler::hostModule()
{
	// get the host module and check its type
	SuperComRmsApp* hostMod = dynamic_cast<SuperComRmsApp*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/

bool SuperComRmsAppClientHandler::getFinish()
{
	return finish_var;
}

void SuperComRmsAppClientHandler::setFinish(bool finish)
{
	finish_var = finish;
}

double SuperComRmsAppClientHandler::getRuntime()
{
	return runTime_var;
}

void SuperComRmsAppClientHandler::setRuntime(double runTime)
{
	runTime_var = runTime;
}

int SuperComRmsAppClientHandler::getNeedMemory()
{
	return needMemory_var;
}

void SuperComRmsAppClientHandler::setNeedMemory(int needMemory)
{
	needMemory_var = needMemory;
}

int SuperComRmsAppClientHandler::getNodes()
{
	return nodes_var;
}

void SuperComRmsAppClientHandler::setNodes(int nodes)
{
	nodes_var = nodes;
}

int SuperComRmsAppClientHandler::getCores()
{
	return cores_var;
}

void SuperComRmsAppClientHandler::setCores(int cores)
{
	cores_var = cores;
}

simtime_t SuperComRmsAppClientHandler::getSubTime()
{
	return subTime_var;
}

void SuperComRmsAppClientHandler::setSubTime(simtime_t subTime)
{
	subTime_var = subTime;
}

int SuperComRmsAppClientHandler::getLoad()
{
	return load_var;
}

void SuperComRmsAppClientHandler::setLoad(int load)
{
	load_var = load;
}

long SuperComRmsAppClientHandler::getInputData()
{
	if(inputData_var <= 0)
	{
		hostModule()->error("Input data cannot be negative.");
	}
	return inputData_var;
}

void SuperComRmsAppClientHandler::setInputData(long data)
{
	if(data <= 0)
	{
		hostModule()->error("Input data cannot be negative.");
	}
	inputData_var = data;
}

double SuperComRmsAppClientHandler::getLoadedTime()
{
	return loadedTime_var;
}

void SuperComRmsAppClientHandler::setLoadedTime(double loadedTime)
{
	loadedTime_var = loadedTime;
}

int SuperComRmsAppClientHandler::getPriority()
{
	return priority_var;
}

void SuperComRmsAppClientHandler::setPriority(int priority)
{
	priority_var = priority;
}

void SuperComRmsAppClientHandler::setOutputData(long data)
{
	if(data <= 0)
	{
		hostModule()->error("Cannot add negative Output data.");
	}
	outputData_var += data;
}

long SuperComRmsAppClientHandler::getOutputData()
{
	if(outputData_var <= 0)
	{
		hostModule()->error("Output data cannot be negative.");
	}
	return outputData_var;
}


void SuperComRmsAppClientHandler::clearOutputData()
{
	outputData_var = 0;
}

/**
 * Constructor.
 */

SuperComRmsAppServerHandler::SuperComRmsAppServerHandler()
{
	evtTout		= new cMessage("timeoutServer", EVT_TOUT);
	delThreadMsg = new SuperComInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

	last_calculated = 0;//the last time energy consumption for the network was calculated
	E_network = 0.0;//total energy consumpted by the network

}

/**
 * Destructor.
 */

SuperComRmsAppServerHandler::~SuperComRmsAppServerHandler()
{
	// cancel any pending messages
	hostModule()->cancelAndDelete(evtTout);
	hostModule()->cancelAndDelete(delThreadMsg);
	evtTout = NULL;
	delThreadMsg = NULL;;
}

void SuperComRmsAppServerHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}

void SuperComRmsAppServerHandler::sendMessage(cMessage *msg)
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

void SuperComRmsAppServerHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [ServerHandler]: connection with server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

	hostModule()->setServerConnected(getServerIndex(), true);

}

/**
 * Handles the reception of a message
 * The server can only send a finish message to the RMS
 *
 */
//TODO: should the server also send output data to the client?
void SuperComRmsAppServerHandler::dataArrived(cMessage* msg, bool urgent)
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
			BEV << " [ServerHandler]: Server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] finished executing the job.\n";

			BEV << " Amount of output data received from the server: " << appmsg->getByteLength() << endl;

			if(appmsg->getByteLength() <= 0)
				hostModule()->error("Finish message in RMS, cannot receive negative amount of bytes");

			IPvXAddress clientAddr = appmsg->getClientAddress();

			//Increment amount of output data to be sent to the client
			long data = 0;
			data = (long)appmsg->getByteLength();
			hostModule()->incrementClientOutputData(clientAddr, data);

			if(getServerIndex() == 26)
			{
//				std::cerr << " Server index before job finish: " << getServerIndex() << endl;
//				std::cerr << " cores in use: " << hostModule()->getServerCoresInUse(getServerIndex()) << endl;
//				std::cerr << " client cores: " << hostModule()->getClientCores(clientAddr) << endl;
//				std::cerr << " server cores: " << appmsg->getCores() << endl;
//				std::cerr << " total: " << hostModule()->getServerCores(getServerIndex()) << endl;
			}

			// Free memory from the server
			hostModule()->setServerMemory(getServerIndex(), (hostModule()->getServerMemory(getServerIndex()) + (hostModule()->getClientNeedMemory(clientAddr))));

			//Free cores from the server
			hostModule()->setServerCoresInUse(getServerIndex(), (hostModule()->getServerCoresInUse(getServerIndex()) - (hostModule()->getClientCores(clientAddr))));

			if(getServerIndex() == 26)
			{
//				std::cerr << " Server index after job finish: " << getServerIndex() << endl;
//				std::cerr << " cores in use: " << hostModule()->getServerCoresInUse(getServerIndex()) << endl;
//				std::cerr << " total: " << hostModule()->getServerCores(getServerIndex()) << endl;
			}

			// If there are no cores in use
			if(hostModule()->getServerCoresInUse(getServerIndex()) == 0)
			{
				//Set server state as idle
				hostModule()->setServerState(getServerIndex(), hostModule()->IDLE);

				//Set server completion time as zero
				hostModule()->setServerCompTime(getServerIndex(), 0);
			}

			//Remove the job from the servers job list
			hostModule()->removeServerJobs(getServerIndex(), clientAddr);

			//Increment finished servers number
			hostModule()->incrementClientFinishedServers(clientAddr);

			//Check if all servers have finished
			// FinishedServers = Nodes
			if(hostModule()->getClientFinishedServers(clientAddr) == hostModule()->getClientNodes(clientAddr))
			{
				//Delete the job from the running jobs list
				hostModule()->deleteRunjob(clientAddr);

				//Calculate network energy
				hostModule()->computeNetworkEnergy(hostModule()->getLastCalculated(), simTime());
				BEV << "All servers have finished their jobs. Informing client.." << endl;
				//BEV << " Output data: " << hostModule()->getClientOutputData(getClientAddress())<< endl;
				//std::cerr << "Output data received in RMS. Simtime: " << simTime() << endl;

				long outputData = 0;
				outputData = hostModule()->getClientOutputData(clientAddr);

				if(outputData < 0)
					hostModule()->error("Outputdata is below zero.");
				else if (outputData == 0)
					hostModule()->error("Outputdata is zero.");

				//Increment the number of jobs completed
				hostModule()->incrementJobsCompleted();

				//Dequeue any pending messages
				hostModule()->dequeue();

				//Generate an internal finish msg
				SuperComInternalMsg *intMsg = new SuperComInternalMsg("INTERNAL_FINISH_MSG", INTERNAL_FINISH_MSG);
				intMsg->setClientAddress(clientAddr);
				intMsg->setUsoAddress(hostModule()->getClientUsoAddress(clientAddr));
				intMsg->setOutputData(outputData); //error
				scheduleAt(simTime(), intMsg);
				//Clear the amount of output data
				hostModule()->clearClientOutputData(clientAddr);

				//TODO: in the federated case delete client info?
				//Delete the client from the cliList
				if(hostModule()->getFederated() == true)
				{
					BEV << "Deleting client info.." << endl;
					hostModule()->deleteClient(clientAddr);
				}

			}

			//NOTE: keep the server connections open

			delete appmsg;
			break;

		}
		case MSGKIND_SLEEP_RESPONSE:
		{
			BEV << "Received a sleep response msg from the Server." << endl;
			hostModule()->setServerReserved(getServerIndex(), false);
			delete appmsg;
			break;

		}
		case MSGKIND_WAKEUP_RESPONSE:
		{
			BEV << "Received a wakeup response msg from the Server." << endl;
			hostModule()->incrementClientWakeupServers(appmsg->getClientAddress());

			if(hostModule()->getClientWakeupServers(appmsg->getClientAddress()) == hostModule()->getClientNeedWakeupServers(appmsg->getClientAddress()))
			{
				BEV << "All the needed servers have now been waken up. Sending job requests.." << endl;
				hostModule()->sendClientRequests(appmsg->getClientAddress());

				//std::cerr << "All the needed servers have now been waken up. Sending job requests, at simtime: " << simTime() << endl;
				// Pop the queue to see if there are jobs that can be executed by the woken up servers
				hostModule()->dequeue();
			}


			delete appmsg;
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
void SuperComRmsAppServerHandler::timerExpired(cMessage* msg)
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

void SuperComRmsAppServerHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void SuperComRmsAppServerHandler::closed()
{

	BEV << "Connection closed." << endl;
	hostModule()->setServerConnected(getServerIndex(), false);
	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void SuperComRmsAppServerHandler::close()
{

	// cancel the session timeout event and call default close()
//	cancelEvent(evtTout);
	//hostModule()->cancelAndDelete(evtTout);
	hostModule()->cancelEvent(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}

void SuperComRmsAppServerHandler::sendRequest(IPvXAddress clientAddr)
{
	BEV <<"Sending client's request and input data to the server."<< endl;

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
	int serverIndex = getServerIndex();
	//hostModule()->addServerJobs(serverIndex, msg->dup());

	if(serverIndex == 26)
	{
//		std::cerr << " Server index at sendRequest: " << serverIndex << endl;
//		std::cerr << " cores in use: " << hostModule()->getServerCoresInUse(serverIndex) << endl;
//		std::cerr << " total: " << hostModule()->getServerCores(serverIndex) << endl;
////		std::cerr << " server state: " << hostModule()->getServerState(serverIndex) << endl;
//		std::cerr << " server jobs: " << hostModule()->getServerJobsLength(serverIndex) << endl;
	}

	//generate a input data msg
	SuperComMsg *inputMsg = new SuperComMsg("inputData");
	inputMsg->setType(MSGKIND_INPUT);
	//Input data to be sent to the server is a uniform proportion of the whole input data
	//TODO: a better way for this should be done.

	int inputDataSize = 0;
	inputDataSize = (int)(hostModule()->getClientInputData(clientAddr)) / (int)(hostModule()->getClientNodes(clientAddr));

	//Set the input data as the size of the message
	inputMsg->setByteLength(inputDataSize);
	inputMsg->setExpectedReplyLength(350);

	//Cancel the timeout event
	if(evtTout->isScheduled())
	{
		hostModule()->cancelEvent(evtTout);
	}

	sendMessage(msg);
	sendMessage(inputMsg);

}

void SuperComRmsAppServerHandler::sendSleep(int sleepMode)
{
	SuperComMsg *msg = new SuperComMsg("sleep");
	msg->setType(MSGKIND_SLEEP);

	//TODO: how to define these parameters?
	//Does server need only the runtime, i.e. no core/memory requirements?
	msg->setByteLength(350);
	msg->setExpectedReplyLength(350);
	msg->setSleepMode(sleepMode);

	//Cancel the timeout event
	if(evtTout->isScheduled())
	{
		hostModule()->cancelEvent(evtTout);
	}

	sendMessage(msg);

}

void SuperComRmsAppServerHandler::sendWakeup(IPvXAddress clientAddr)
{
	SuperComMsg *msg = new SuperComMsg("wakeUp");
	msg->setType(MSGKIND_WAKEUP);

	//TODO: how to define these parameters?
	//Does server need only the runtime, i.e. no core/memory requirements?
	msg->setByteLength(350);
	msg->setExpectedReplyLength(350);
	msg->setClientAddress(clientAddr);

	//Cancel the timeout event
	if(evtTout->isScheduled())
	{
		hostModule()->cancelEvent(evtTout);
	}

	sendMessage(msg);

}

/**
 * Get the host module (i.e., the instance of SuperComRmsApp which spawns the thread).
 */
SuperComRmsApp* SuperComRmsAppServerHandler::hostModule()
{
	// get the host module and check its type
	SuperComRmsApp* hostMod = dynamic_cast<SuperComRmsApp*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/

//double SuperComRmsAppServerHandler::getRuntime()
//{
//	return runTime_var;
//}
//
//void SuperComRmsAppServerHandler::setRuntime(double runTime)
//{
//	runTime_var = runTime;
//}

//IPvXAddress SuperComRmsAppServerHandler::getClientAddress()
//{
//	return clientAddress_var;
//}
//
//void SuperComRmsAppServerHandler::setClientAddress(IPvXAddress clientAddress)
//{
//	clientAddress_var = clientAddress;
//}
//
//int SuperComRmsAppServerHandler::getNeedMemory()
//{
//	return needMemory_var;
//}
//
//void SuperComRmsAppServerHandler::setNeedMemory(int needMemory)
//{
//	needMemory_var = needMemory;
//}

int SuperComRmsAppServerHandler::getServerIndex()
{
	return serverIndex_var;
}

void SuperComRmsAppServerHandler::setServerIndex(int serverIndex)
{
	serverIndex_var = serverIndex;
}

//int SuperComRmsAppServerHandler::getCores()
//{
//	return cores_var;
//}
//
//void SuperComRmsAppServerHandler::setCores(int cores)
//{
//	cores_var = cores;
//}

void SuperComRmsAppServerHandler::setSleep(bool sleep)
{
	sleep_var = sleep;

}

bool SuperComRmsAppServerHandler::getSleep()
{
	return sleep_var;
}

//int SuperComRmsAppServerHandler::getLoad()
//{
//	return load_var;
//}
//
//void SuperComRmsAppServerHandler::setLoad(int load)
//{
//	load_var = load;
//}
//
//void SuperComRmsAppServerHandler::setInputData(long data)
//{
//	if(data <= 0)
//	{
//		hostModule()->error("Input data cannot be negative.");
//	}
//	inputData_var = data;
//}
//
//long SuperComRmsAppServerHandler::getInputData()
//{
//	if(inputData_var <= 0)
//	{
//		hostModule()->error("Input data cannot be negative.");
//	}
//	return inputData_var;
//}
//
//void SuperComRmsAppServerHandler::setLoadedTime(double loadedTime)
//{
//	loadedTime_var = loadedTime;
//}
//
//double SuperComRmsAppServerHandler::getLoadedTime()
//{
//	return loadedTime_var;
//}
//
//void SuperComRmsAppServerHandler::setNodes(int nodes)
//{
//	nodes_var = nodes;
//}
//
//int SuperComRmsAppServerHandler::getNodes()
//{
//	return nodes_var;
//}

