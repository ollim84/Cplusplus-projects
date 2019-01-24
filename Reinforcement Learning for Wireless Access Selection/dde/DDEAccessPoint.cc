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


#include "DDEAccessPoint.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include "GenericAppMsg_m.h"
#include "ClientMsg_m.h"
#include "IPAddressResolver.h"
#include <vector>
#include <algorithm>
#include "InterfaceTableAccess.h"
#include <stdio.h>
#include <stdlib.h>
#include "AppContext_m.h"


#define BEV	EV << "[DDE AP]:"

//TODO: these could be defined differently, use different numbers?
#define MSGKIND_CONNECT		10
#define MSGKIND_REQUEST     11
#define MSGKIND_SEND        12
#define MSGKIND_INPUT       13
#define MSGKIND_RESPONSE    14
#define MSGKIND_FINISH      15
#define MSGKIND_SLEEP		16
//#define MSGKIND_SLEEP_RESPONSE		17
#define INTERNAL_LOAD_TIMER  17
#define INTERNAL_PKT_LOSS_TIMER 18
#define INTERNAL_CLASSIFY_TIMER 19

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
#define INTERNAL_ADD_CLIENT_THREAD_MSG   27
#define INTERNAL_SEND_VIDEO_DATA_MSG 28


Register_Class(DDEAccessPointClientHandler);
Define_Module(DDEAccessPoint);

/**
 * Constructor.
 */

DDEAccessPoint::DDEAccessPoint()
{
	setMsgsRcvd(0);
	setMsgsSent(0);
	setBytesRcvd(0);
	setBytesSent(0);

}

/**
 * Destructor.
 */

DDEAccessPoint::~DDEAccessPoint()
{
	//cancelAndDelete(pktLossTimer);
	//cancelAndDelete(loadTimer);
	pktLossTimer = NULL;
	loadTimer = NULL;

}

std::ostream& operator<<(std::ostream& os, const DDEAccessPoint::clientInfo& client)
{
    os << "ID=" << client.ID
       << " Address=" << client.clientAddress
       << " userClass=" << client.userClass;
    return os;
}


void DDEAccessPoint::initialize()
{
	// parent initialize()
	TCPSrvHostApp::initialize();

    delay = par("replyDelay"); //not needed
    maxMsgDelay = 0; //not needed
    cachedContent_var = par("cachedContent");
    fileServer_var = par("fileServer");
    packetLoss_var = 0.0;
    load_var = 0.0;
    consumedBW_var = 0.0;
    avBW_var = 0.0;
    maxBW_var = par("MaximumBandwidth");
    classification_var = par("classification");
    classifyInterval_var = par("classifyInterval");

    //packetlossVector.setName("pkt loss (%)");

    if(fileServer_var == false)
    {
		//pktLossTimer = new ddeInternalMsg("INTERNAL_PKT_LOSS_TIMER", INTERNAL_PKT_LOSS_TIMER);
		//scheduleAt(simTime(), pktLossTimer);

		//loadTimer = new ddeInternalMsg("INTERNAL_LOAD_TIMER", INTERNAL_LOAD_TIMER);
		//scheduleAt(simTime(), loadTimer);

    	// Do not classify if default is activated
    	if(classification_var == true)
    	{
    		apClassifyTimer = new ddeInternalMsg("INTERNAL_CLASSIFY_TIMER", INTERNAL_CLASSIFY_TIMER);
			scheduleAt(simTime(), apClassifyTimer);
    	}
    }

    WATCH(msgsRcvd_var);
    WATCH(msgsSent_var);
    WATCH(bytesRcvd_var);
    WATCH(bytesSent_var);
    WATCH(packetLoss_var);
    WATCH(load_var);
    WATCH_LIST(cliList);

}


// Handles messages coming to this module
void DDEAccessPoint::handleMessage(cMessage* msg)
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
		//TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();
		// default handling
		TCPSrvHostApp::handleMessage(msg);


	}

}

//Handles self messages
void DDEAccessPoint::handleSelfMessage(cMessage *msg)
{
	switch(msg->getKind())
	{
		// Initiates a connection with a server
		// This is based on the peer-wire module of the BT simulation model
		case INTERNAL_INIT_CONNECTION_MSG:
		{

			delete msg;
			break;

		}
		// This message is received when a load is calculated
		case INTERNAL_LOAD_TIMER:
		{
			double load = 0.0;
			load = computeLoad();
			setLoad(load);

			loadTimer = new ddeMsg("INTERNAL_LOAD_TIMER", INTERNAL_LOAD_TIMER);
			scheduleAt(simTime()+0.5, loadTimer);

			delete msg;
			break;
		}
		// This message is received when a packet loss is calculated
		case INTERNAL_PKT_LOSS_TIMER:
		{

			double pktLoss = 0.0;
			pktLoss = computePacketLoss();
			setPacketLoss(pktLoss);

			//packetlossVector.recordWithTimestamp(simTime(), pktLoss*100);

			pktLossTimer = new ddeMsg("INTERNAL_PKT_LOSS_TIMER", INTERNAL_PKT_LOSS_TIMER);
			scheduleAt(simTime()+0.5, pktLossTimer);

			delete msg;
			break;
		}

		case INTERNAL_CLASSIFY_TIMER:
		{
			double pktLoss = 0.0;
			pktLoss = computePacketLoss();
			setPacketLoss(pktLoss);

			double load = 0.0;
			load = computeLoad();
			setLoad(load);

	    	float nu, lo, pl, classifyValue = 0.0;
	    	getAPclassification(nu, lo, pl, classifyValue);

	    	int numGold, numSilver, numBronze = 0;
	    	getUserClassDistribution(numGold, numSilver, numBronze);

	    	//TODO: send only if value changes
	    	sendInfoToClients(nu, lo, pl, classifyValue, numGold, numSilver, numBronze);


	    	apClassifyTimer = new ddeMsg("INTERNAL_CLASSIFY_TIMER", INTERNAL_CLASSIFY_TIMER);
			scheduleAt(simTime()+ getClassifyInterval(), apClassifyTimer);


			delete msg;
			break;
		}

//		case INTERNAL_INFO_CLASSIFY_TIMER:
//		{
//	    	float nu, lo, pl, classifyValue = 0.0;
//	    	getAPclassification(nu, lo, pl, classifyValue);
//
//	    	int numGold, numSilver, numBronze = 0;
//	    	getUserClassDistribution(numGold, numSilver, numBronze);
//
//	    	//TODO: send only if value changes
//	    	sendInfoToInfoServer(nu, lo, pl, classifyValue, numGold, numSilver, numBronze);
//
//
//	    	infoClassifyTimer = new ddeMsg("INTERNAL_INFO_CLASSIFY_TIMER", INTERNAL_INFO_CLASSIFY_TIMER);
//			scheduleAt(simTime()+ getClassifyInterval(), infoClassifyTimer);
//		}

		default:
		{
			delete msg;
			break;
		}
	}

}

//Handles self messages concerning a thread
void DDEAccessPoint::handleThreadMessage(cMessage *msg)
{

	// Get a pointer for the thread
	TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

	// This would get a handler for the thread. Handler could be used to call functions
	//DDEAccessPointClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

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

//Adds client info to the client struct
void DDEAccessPoint::addClientInfo(IPvXAddress clientAddr, TCPServerThreadBase* thread, bool infoServer)
{

    clientInfo *client = lookupClient(clientAddr);
    if (client)
    {
        EV << "Client address=" << clientAddr  << " already in our client list, refreshing the info" << endl;
    }
    else
    {
        EV << "Inserting Client address=" << clientAddr << " into the Client list" << endl;
        cliList.push_back(clientInfo());
        client = &cliList.back();
    }

	client->clientAddress = clientAddr;
	client->thread = thread;
	client->infoServer = infoServer;

}

void DDEAccessPoint::addClientUserClass(int userClass, IPvXAddress clientAddr)
{
    clientInfo *client = lookupClient(clientAddr);
    if (client)
    {
        EV << "Client address=" << clientAddr  << " already in our client list, refreshing the info" << endl;
    }
    else
    {
    	error("DDEAccessPoint: Client not found while adding userClass!");

    }

    client->userClass = userClass;

}

//Looks up client from the client list
DDEAccessPoint::clientInfo *DDEAccessPoint::lookupClient(IPvXAddress clientAddress)
{
    for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
        if (it->clientAddress == clientAddress)
            return &(*it);
    return NULL;
}

// Deletes a client according to a IP address
void DDEAccessPoint::deleteClient(IPvXAddress clientAddress)
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

//Removes a thread from the socket map
void DDEAccessPoint::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->getSocket());
    updateDisplay();
}

//Sends classification to clients
void DDEAccessPoint::sendInfoToClients(float nu, float lo, float pl, float classifyValue, int numGold, int numSilver, int numBronze)
{
	//TODO: do not send info to non-learning clients
	TCPServerThreadBase* thread;
	for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
	{

		//if(it->clientAddress == IPvXAddress("10.0.3.1"))

		//if(it->infoServer == false)

		thread = it->thread;
		DDEAccessPointClientHandler* myProc = (DDEAccessPointClientHandler*) thread;

		if(myProc->getSocket()->getState() == TCPSocket::CONNECTED)
		{
			myProc->sendInfo(nu, lo, pl, classifyValue, numGold, numSilver, numBronze);
		}
	}

}

//void DDEAccessPoint::sendInfoToInfoServer(float nu, float lo, float pl, float classifyValue, int numGold, int numSilver, int numBronze)
//{
//	//TODO: do not send info to non-learning clients
//	TCPServerThreadBase* thread;
//	for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
//	{
//
//		//if(it->clientAddress == IPvXAddress("10.0.3.1"))
//		if(it->infoServer == true)
//		{
//
//			thread = it->thread;
//			DDEAccessPointClientHandler* myProc = (DDEAccessPointClientHandler*) thread;
//
//			if(myProc->getSocket()->getState() == TCPSocket::CONNECTED)
//			{
//				myProc->sendInfo(nu, lo, pl, classifyValue, numGold, numSilver, numBronze);
//			}
//		}
//	}
//}

//This function should cover all the statistics
void DDEAccessPoint::finish()
{

    BEV << getFullPath() << ": sent " << bytesSent_var << " bytes in " << msgsSent_var << " packets\n";
    BEV << getFullPath() << ": received " << bytesRcvd_var << " bytes in " << msgsRcvd_var << " packets\n";
    BEV << getFullPath() << ": Sim time: " << simTime() << endl;

    recordScalar("packets sent", msgsSent_var);
    recordScalar("packets rcvd", msgsRcvd_var);
    recordScalar("bytes sent", bytesSent_var);
    recordScalar("bytes rcvd", bytesRcvd_var);
}


/***** GET/SET methods *****/


long DDEAccessPoint::getMsgsRcvd()
{
	return msgsRcvd_var;
}

void DDEAccessPoint::setMsgsRcvd(long msgsRcvd)
{
	msgsRcvd_var = msgsRcvd;
}

void DDEAccessPoint::incrementMsgsRcvd()
{
	msgsRcvd_var++;
}

long DDEAccessPoint::getMsgsSent()
{
	return msgsSent_var;
}

void DDEAccessPoint::setMsgsSent(long msgsSent)
{
	msgsSent_var = msgsSent;
}

void DDEAccessPoint::incrementMsgsSent()
{
	msgsSent_var++;
}


long DDEAccessPoint::getBytesRcvd()
{
	return bytesRcvd_var;
}

void DDEAccessPoint::setBytesRcvd(long bytesRcvd)
{
	bytesRcvd_var = bytesRcvd;
}

void DDEAccessPoint::incrementBytesRcvd(long byteLength)
{
	bytesRcvd_var += byteLength;
}

long DDEAccessPoint::getBytesSent()
{
	return bytesSent_var;
}

void DDEAccessPoint::setBytesSent(long bytesSent)
{
	bytesSent_var = bytesSent;
}

void DDEAccessPoint::incrementBytesSent(long byteLength)
{
	bytesSent_var += byteLength;
}

bool DDEAccessPoint::getCachedContent()
{
	return cachedContent_var;
}

void DDEAccessPoint::setCachedContent(bool cachedContent)
{
	cachedContent_var = cachedContent;
}

const MACAddress& DDEAccessPoint::getMacAddress()
{
	//return macAddress_var;

    IInterfaceTable* ift = InterfaceTableAccess().get();
    const MACAddress& address = ift->getInterfaceByName("wlan")->getMacAddress();

    return address;
}

int DDEAccessPoint::getNumberOfUsers()
{
	module = getParentModule()->getSubmodule("wlan")->getSubmodule("mgmt");
	MgmtAP = check_and_cast<Ieee80211MgmtAP*> (module);

	//exclude interfering traffic?
	int numberOfUsers = 0;
	numberOfUsers = (MgmtAP->getStaListSize());

	// -1 to omit the information server
//	if(cliList.size() != 0)
//		numberOfUsers = cliList.size() - 1;

	return numberOfUsers;

}
/**
 * Returns the number of gold, silver, and bronze users in the AP
 */

void DDEAccessPoint::getUserClassDistribution(int &numGold, int &numSilver, int &numBronze)
{

	int gold = 0;
	int silver = 0;
	int bronze = 0;

	for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
	{
		if(it->userClass == 3)
		{
			gold++;
		}
		else if(it->userClass == 2)
		{
			silver++;
		}
		else if(it->userClass == 1)
		{
			bronze++;
		}

	}

	numGold = gold;
	numSilver = silver;
	numBronze = bronze;


	return;

}

double DDEAccessPoint::getClassifyInterval()
{
	return classifyInterval_var;
}

int DDEAccessPoint::getNumberOfDownloaders()
{
	return (socketMap.size()-1);
}

double DDEAccessPoint::computeLoad()
{
	double load = 0.0;
	double uploadThruput = 0.0;
	double downloadThruput = 0.0;

//	double maxThruput = 11000000; // 11 mbps
//	double coefficient = 0.4656035385; // 5121638.924/11000000

	double consumedBW = 0.0;
	//double avBW = 0.0;
	double maxBW = 0.0;
	maxBW = getMaxBW();

	//5.1216389245 mbps = 5 121 600

	cModule *moduleUL;
	cModule *moduleDL;

	moduleUL = getParentModule()->getSubmodule("wlan")->getSubmodule("ULthruputMeter");
	moduleDL = getParentModule()->getSubmodule("wlan")->getSubmodule("DLthruputMeter");
	ThruputMeter *UL = check_and_cast<ThruputMeter*> (moduleUL);
	ThruputMeter *DL = check_and_cast<ThruputMeter*> (moduleDL);

	uploadThruput = UL->getThruput();
	downloadThruput = DL->getThruput();

	//uploadThruput = UL->getCurrentThruput();
	//downloadThruput = DL->getCurrentThruput();

	//std::cout << "consumed bandwidth: " << (uploadThruput + downloadThruput)<< endl;

	//TODO: what if available bandwidth is larger than max bandwidth
//	if((uploadThruput + downloadThruput) > (maxThruput * coefficient))
//		coefficient = 1.0;

	consumedBW = uploadThruput + downloadThruput;
	//maxBW = maxThruput * coefficient;


	//setMaxBW(maxBW);

//	if(strcmp(getParentModule()->getName(), "ap2") == 0)
//	{
//		std::cout << "Upload throughbut: " << uploadThruput << endl;
//		std::cout << "Download throughbut: " << downloadThruput << endl;
//		std::cout << "consumed bandwidth: " << (uploadThruput + downloadThruput)<< endl;
//		std::cout << "maxBW: " << maxBW << endl;
//	}


	if((maxBW - consumedBW) < 0)
	{
		//std::cerr << "ConsumedBW: " << consumedBW << endl;
		//error("Available BW below zero!");
		setAvBW(0);
		setConsumedBW(maxBW);
	}
	else
	{
		setAvBW((maxBW - consumedBW));
		setConsumedBW(consumedBW);
	}


	int users = getNumberOfUsers();
	//int downloaders = getNumberOfDownloaders();

	//TODO: 3 options
	//a) AP is free, available bandwidth is high! --> load should be high?
	//b) AP has users, but packet loss is high and consumed bandwidth is low --> load should be low?
	//c) AP has users, packet loss is zero, but bandwidth is spread between users --> load should be computed based on shared bandwidth?

	double value1 = 0.0;
	double value2 = 0.0;
	double value3 = 0.0;

	if(users == 0)
	{
		value1 = 1/(double)(users+1);
	}
	else
	{
		value1 = 1/(double)users;
	}

	value2 = (maxBW - consumedBW)/maxBW;
	value3 = consumedBW/maxBW;


	load = max(value1, value2);

	//load = value2;


//	if(users == 0 || users == 1)
//	{
//		avBW = maxBW - consumedBW;
//	}
//	else
//	{
//		avBW = maxBW - consumedBW;
//	}
//
//	load = avBW / maxBW;



//	float val = 37.777779;
//
//	float rounded_down = floorf(val * 100) / 100;   /* Result: 37.77 */
//	float rounded_up = ceilf(val * 100) / 100;

	// Rounding
	float nearest = floorf(load * 100 + 0.5) / 100;

	load = nearest;



//	if(strcmp(getParentModule()->getName(), "ap2") == 0)
//	{
//		std::cout << "value1: " << value1 << endl;
//		std::cout << "value2: " << value2 << endl;
//		std::cout << "value3: " << value3 << endl;
//		std::cout << "load: " << load << endl;
//
//	}

//	std::cout << "consumedBW: " << consumedBW << endl;
//	std::cout << "max bandwidth: " << avBW << endl;
//	std::cout << "load: " << load << endl;

	//load = (uploadThruput + downloadThruput) / (maxThruput * coefficient);


	return load;
}

double DDEAccessPoint::getLoad()
{
	return load_var;
}

void DDEAccessPoint::setLoad(double load)
{
	load_var = load;
}

double DDEAccessPoint::getConsumedBW()
{
	return consumedBW_var;
}

void DDEAccessPoint::setConsumedBW(double value)
{
	consumedBW_var = value;
}

double DDEAccessPoint::getMaxBW()
{
	return maxBW_var;
}

void DDEAccessPoint::setMaxBW(double value)
{
	maxBW_var = value;
}

double DDEAccessPoint::getAvBW()
{
	return avBW_var;
}

void DDEAccessPoint::setAvBW(double value)
{
	avBW_var = value;
}


double DDEAccessPoint::computePacketLoss()
{
	module = getParentModule()->getSubmodule("wlan")->getSubmodule("radio");
	radio = check_and_cast<AbstractRadioExtended*> (module);
	double packetLoss = 0.0;
	packetLoss = radio->getPacketLoss();

	return packetLoss;

}

double DDEAccessPoint::getPacketLoss()
{
	return packetLoss_var;
}

void DDEAccessPoint::setPacketLoss(double packetLoss)
{
	packetLoss_var = packetLoss;
}

//float DDEAccessPoint::getAPclassification()
//{
//	int ap = 0;
//	float nu = 0.0;
//	float lo = 0.0;
//	float pl = 0.0;
//	float classifyValue = 0.0;
//
//	nu = getNumberOfUsers();
//	lo = getLoad();
//	pl = getPacketLoss()*100;
//
//	BEV << "Number of users: " << nu << endl;
//	BEV  << "load: " << lo << endl;
//	BEV  << "packet loss: " << pl << endl;
//
////	std::cout << "Number of users: " << nu << endl;
////	std::cout   << "load: " << lo << endl;
////	std::cout   << "packet loss: " << pl << endl;
//
//	//float classifyValue = nsfis(0,10,0.3,2.0);
////	arg1 = number of clients connected to the access point  0..24
////	arg2 = available bandwidth / maximum bandwidth  0..1
////	arg3 = % packet losses  0..100
//
//	classifyValue = nsfis(ap,nu,lo,pl);
//
//	//classifyValue = nsfis(ap,1.0,0.0,0.0);
//
//	//std::cout << "AP classification: " << classifyValue << endl;
//
//	return classifyValue;
//
//
//}

void DDEAccessPoint::getAPclassification(float &nu, float &lo, float &pl, float &classifyValue)
{

	int ap = 0;
	nu = 0.0;
	lo = 0.0;
	pl = 0.0;
	classifyValue = 0.0;

	nu = getNumberOfUsers();
	lo = getLoad();
	pl = getPacketLoss()*100;

	BEV << "Number of users: " << nu << endl;
	BEV  << "load: " << lo << endl;
	BEV  << "packet loss: " << pl << endl;

//	std::cout << "Number of users: " << nu << endl;
//	std::cout   << "load: " << lo << endl;
//	std::cout   << "packet loss: " << pl << endl;

	//float classifyValue = nsfis(0,10,0.3,2.0);
//	arg1 = number of clients connected to the access point  0..24
//	arg2 = available bandwidth / maximum bandwidth  0..1
//	arg3 = % packet losses  0..100

	//TODO:nsfis
	classifyValue = nsfis(ap,nu,lo,pl);
	//classifyValue = 4.0;

	//std::cout << "AP classification: " << classifyValue << endl;

	return;
}

/**
 * Check if the client is registered, i.e. it is in the client list
 */

bool DDEAccessPoint::isClientRegistered(IPvXAddress clientAddr)
{
	bool clientRegistered = false;
	clientInfo *client = lookupClient(clientAddr);

	if(client)
	{
		clientRegistered = true;
	}
	else
	{
		clientRegistered = false;
	}

	return clientRegistered;
}


/**
 * Constructor.
 */

DDEAccessPointClientHandler::DDEAccessPointClientHandler()
{
	//evtTout		= new cMessage("timeoutServer", EVT_TOUT);
	//delThreadMsg = new ClientInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

}

/**
 * Destructor.
 */

DDEAccessPointClientHandler::~DDEAccessPointClientHandler()
{
	// cancel any pending messages
//	hostModule()->cancelAndDelete(evtTout);
//	hostModule()->cancelAndDelete(delThreadMsg);
//	evtTout = NULL;
//	delThreadMsg = NULL;
	numPrevOK = 0;
	numPrevGiven = 0;
}

void DDEAccessPointClientHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
	numPrevOK = 0;
	numPrevGiven = 0;
}

void DDEAccessPointClientHandler::sendMessage(cMessage *msg)
{

	GenericAppMsg *appmsg = dynamic_cast<GenericAppMsg *>(msg);

	// If appmsg
	if (appmsg)
	{
		hostModule()->incrementMsgsSent();
		hostModule()->incrementBytesSent(appmsg->getByteLength());

		BEV << "sending \"" << appmsg->getName() << "\" to TCP, " << appmsg->getByteLength() << " bytes\n";
	}

	else
	{
		hostModule()->incrementMsgsSent();
		hostModule()->incrementBytesSent(appmsg->getByteLength());
		BEV << "sending \"" << appmsg->getName() << "\" to TCP, " << appmsg->getByteLength() << " bytes\n";
	}

    getSocket()->send(msg);

}

void DDEAccessPointClientHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [DDEAccessPointClientHandler]: connection with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

	// Add a timer for adding the client thread
	cMessage* addClient = new cMessage("addClient", INTERNAL_ADD_CLIENT_THREAD_MSG);
	scheduleAt(simTime(), addClient);


	//add client to a list (do not include infoServer)
//	if(getSocket()->getRemoteAddress() != IPvXAddress("10.0.3.1"))
//		hostModule()->addClientInfo(getSocket()->getRemoteAddress());

	//std::cout << " Number of connected users: " << hostModule()->getNumberOfUsers() << endl;
	//std::cout << " Number of TCP threads: " << hostModule()->getNumberOfThreads() << endl;
	//hostModule()->getAPclassification();
	//sendRequest();

}

/**
 * Handles the reception of a message
 *
 *
 */

void DDEAccessPointClientHandler::dataArrived(cMessage* msg, bool urgent)
{

    if(strcmp (msg->getName(),"CLASSIFY_AP") == 0)
    {
    	ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);
    	    if (!appmsg)
    	        opp_error("Message (%s)%s is not a ddeMsg -- "
    	                  "probably wrong client app, or wrong setting of TCP's "
    	                  "sendQueueClass/receiveQueueClass parameters "
    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	                  msg->getClassName(), msg->getName());

    	ddeMsg *msg = new ddeMsg("AP_CLASSIFICATION");

    	float nu, lo, pl, apClassification = 0.0;
    	//pl = getPacketLoss();

    	hostModule()->getAPclassification(nu, lo, pl, apClassification);

//    	if(strcmp(hostModule()->getParentModule()->getName(), "ap1") == 0)
//    	{
//				if(getSocket()->getRemoteAddress() == IPvXAddress("192.168.0.10"))
//					std::cout << "client: " << hostModule()->getParentModule()->getName() << ", packet loss: " << getPacketLoss() << endl;
//				else
//					std::cout << "info: " << hostModule()->getParentModule()->getName() << ", packet loss: " << getPacketLoss() << endl;
//    	}

    	const MACAddress& address = hostModule()->getMacAddress();

    	msg->setApClassifyValue(apClassification);
    	msg->setSeedsPerLeechers(100);
    	msg->setByteLength(appmsg->getByteLength());
    	msg->setNumUsers(nu);
    	msg->setLoad(lo);
    	msg->setPacketLoss(pl);
    	msg->setCachedContent(hostModule()->getCachedContent());
    	msg->setConsumedBW(hostModule()->getConsumedBW());
    	msg->setMacAddress(address);
    	msg->setAvBW(hostModule()->getAvBW());
    	msg->setMaxBW(hostModule()->getMaxBW());

    	sendMessage(msg);
    	delete appmsg;

    }
    //FTP request
    else if(strcmp (msg->getName(),"request") == 0)
    {
    	ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);
    	    	    if (!appmsg)
    	    	        opp_error("Message (%s)%s is not a ddeMsg -- "
    	    	                  "probably wrong client app, or wrong setting of TCP's "
    	    	                  "sendQueueClass/receiveQueueClass parameters "
    	    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	    	                  msg->getClassName(), msg->getName());

        long requestedBytes = appmsg->getExpectedReplyLength();
        bool doClose = appmsg->getServerClose();

        //int userClass = appmsg->getUserClass();

        hostModule()->addClientUserClass(appmsg->getUserClass(), getSocket()->getRemoteAddress());

        if (requestedBytes==0)
        {
            delete appmsg;
        }
        // Send only if connected
        else if(getSocket()->getState() == TCPSocket::CONNECTED)
        {
            appmsg->setByteLength(requestedBytes);
            delete appmsg->removeControlInfo();
            sendMessage(appmsg);
            //getSocket()->send(appmsg);
        }
        else
        {
        	delete appmsg;
        }

        if (doClose)
        {
            getSocket()->close();
        }

    }
    // Video request
    else if(strcmp (msg->getName(),"videoRequest") == 0)
    {
    	AppContext *appmsg = dynamic_cast<AppContext *>(msg);
    	    	    if (!appmsg)
    	    	        opp_error("Message (%s)%s is not a AppContext -- "
    	    	                  "probably wrong client app, or wrong setting of TCP's "
    	    	                  "sendQueueClass/receiveQueueClass parameters "
    	    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	    	                  msg->getClassName(), msg->getName());

    	// Update parameters to a struct
    	appContext.fileSize = appmsg->getFileSize();
    	appContext.remainingBytes = appmsg->getRemainingFileSize();
    	appContext.txRate = appmsg->getTxDuration();
    	appContext.chunkSize = appmsg->getChunkSize();
    	appContext.codecRate = appmsg->getCodecRate();
    	appContext.initialPlayStarted = false;
    	appContext.playFinished = false;
    	appContext.origTxRate = appmsg->getTxDuration();
    	appContext.userClass = appmsg->getUserClass();


    	hostModule()->addClientUserClass(appmsg->getUserClass(), getSocket()->getRemoteAddress());


        if (appContext.fileSize==0)
        {
            delete appmsg;
        }
        // Send only if connected
        else if(getSocket()->getState() == TCPSocket::CONNECTED)
        {
//        	// Send for 3 * MSS
//        	for(int i = 0; i < 3; i++)
//        	{
//				GenericAppMsg *videoData = new GenericAppMsg("Video packet");
//				videoData->setByteLength(appContext.chunkSize);
//				sendMessage(videoData);
//        	}

        	sendVideoData();

//            // Generate a timer for sending video data
//            cMessage* videoDataTimer = new cMessage ("videoTimer", INTERNAL_SEND_VIDEO_DATA_MSG);
//            scheduleAt(simTime()+appContext.txRate, videoDataTimer);

            delete appmsg;
        }
        else
        {
        	delete appmsg;
        }

    }
    else if(strcmp (msg->getName(),"unregister") == 0)
    {
    	BEV << "Received an unregister msg from Client IP: " << getSocket()->getRemoteAddress() << endl;
    	std::cout << "AP: Received an unregister msg from Client IP: " << getSocket()->getRemoteAddress() << ", " << simTime() << endl;
    	hostModule()->deleteClient(getSocket()->getRemoteAddress());

    	delete msg;
    }

}

double DDEAccessPointClientHandler::getPacketLoss()
{
	cModule* mod = hostModule()->getParentModule()->getSubmodule("wlan")->getSubmodule("radio");
	AbstractRadioExtended* radioMod = check_and_cast<AbstractRadioExtended*> (mod);

	long numReceivedCorrectlyTotal = 0;
	long numGivenUpTotal = 0;
	long intvlOK = 0;
	long intvlGiven = 0;
	double packetLoss = 0.0;

	numGivenUpTotal = radioMod->getNumGivenUp();
	numReceivedCorrectlyTotal = radioMod->getNumReceivedCorrectly();
//	std::cout << "numGivenUpTotal: " << numGivenUpTotal << endl;
//	std::cout << "numReceivedCorrectlyTotal: " << numReceivedCorrectlyTotal << endl;
//	std::cout << "numPrevGiven: " << numPrevGiven << endl;
//	std::cout << "numPrevOK: " << numPrevOK  << endl;

	intvlGiven = numGivenUpTotal - numPrevGiven;
	intvlOK = numReceivedCorrectlyTotal - numPrevOK;

//	std::cout << "intvlGiven: " << intvlGiven << endl;
//	std::cout << "intvlOK: " << intvlOK << endl;



	if((intvlOK+intvlGiven) == 0)
		packetLoss = 0;
	else
		packetLoss = (double)intvlGiven/((double)intvlOK+(double)intvlGiven);

	numPrevGiven = numGivenUpTotal;
	numPrevOK = numReceivedCorrectlyTotal;

//	std::cout << "new numPrevGiven: " << numPrevGiven << endl;
//	std::cout << "new numPrevOK: " << numPrevOK  << endl;

	return packetLoss*100;

}

void DDEAccessPointClientHandler::setNumPrevGiven(long value)
{
	numPrevGiven = value;
}

long DDEAccessPointClientHandler::getNumPrevGiven()
{
	return numPrevGiven;
}

/**
 * Handles a session timeout event.
 *
 */
void DDEAccessPointClientHandler::timerExpired(cMessage* msg)
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
			break;
		}
		case INTERNAL_ADD_CLIENT_THREAD_MSG:
		{
			TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

			//add client to a list
			//if(getSocket()->getRemoteAddress() != IPvXAddress("10.0.3.1"))
			bool infoServer = false;
			if(getSocket()->getRemoteAddress() == IPvXAddress("10.0.3.1"))
			{
				infoServer = true;
			}

			hostModule()->addClientInfo(getSocket()->getRemoteAddress(), thread, infoServer);

			delete msg;
			break;

		}
		case INTERNAL_SEND_VIDEO_DATA_MSG:
		{
			bool clientRegistered = false;

			clientRegistered = hostModule()->isClientRegistered(getSocket()->getRemoteAddress());

		    // Send data only if client is registered
			if(getSocket()->getState() == TCPSocket::CONNECTED && clientRegistered == true)
			{
				BEV << "Sending video data to client. " << endl;
				sendVideoData();
			}

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

void DDEAccessPointClientHandler::sendVideoData()
{

    BEV << "Remaining bytes before: " << appContext.remainingBytes << endl;

	// Send for 3 * MSS
	for(int i = 0; i < 3; i++)
	{
		if(appContext.remainingBytes > 0)
		{
			GenericAppMsg *videoData = new GenericAppMsg("Video packet");
			videoData->setByteLength(appContext.chunkSize);
			appContext.remainingBytes -= videoData->getByteLength();
			sendMessage(videoData);

		}
	}


	BEV << "Remaining bytes after: " << appContext.remainingBytes << endl;

    //check if all data has been delivered
    if(appContext.remainingBytes > 0)
    {
		// Generate a timer for sending video data
		cMessage* videoDataTimer = new cMessage ("videoTimer", INTERNAL_SEND_VIDEO_DATA_MSG);
		scheduleAt(simTime()+appContext.txRate, videoDataTimer);
    }
    else
    {
    	BEV << "All content delivered! " << simTime() << endl;
    	std::cout << "All content delivered! " << simTime() << endl;
    }

}

void DDEAccessPointClientHandler::sendInfo(float nu, float lo, float pl, float apClassification, int numGold, int numSilver, int numBronze)
{
	ddeMsg *msg = new ddeMsg("AP_CLASSIFICATION");

	const MACAddress& address = hostModule()->getMacAddress();

	msg->setApClassifyValue(apClassification);
	msg->setSeedsPerLeechers(100);
	msg->setByteLength(20);
	msg->setNumUsers(nu);
	msg->setLoad(lo);
	msg->setPacketLoss(pl);
	msg->setCachedContent(hostModule()->getCachedContent());
	msg->setConsumedBW(hostModule()->getConsumedBW());
	msg->setMacAddress(address);
	msg->setAvBW(hostModule()->getAvBW());
	msg->setMaxBW(hostModule()->getMaxBW());
	msg->setNumGold(numGold);
	msg->setNumSilver(numSilver);
	msg->setNumBronze(numBronze);
	sendMessage(msg);
}

void DDEAccessPointClientHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void DDEAccessPointClientHandler::closed()
{

	BEV << "Connection closed." << endl;
	cMessage* delThreadMsg;		// delete thread
	delThreadMsg = new ddeInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void DDEAccessPointClientHandler::close()
{

	// cancel the session timeout event and call default close()
//	cancelEvent(evtTout);
	//hostModule()->cancelAndDelete(evtTout);
	//hostModule()->cancelEvent(evtTout);
	getSocket()->close();

	hostModule()->deleteClient(getSocket()->getRemoteAddress());
	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}


/**
 * Get the host module (i.e., the instance of DDEAccessPoint which spawns the thread).
 */
DDEAccessPoint* DDEAccessPointClientHandler::hostModule()
{
	// get the host module and check its type
	DDEAccessPoint* hostMod = dynamic_cast<DDEAccessPoint*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/




