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


#include "InfoServer.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include "GenericAppMsg_m.h"
#include "ClientMsg_m.h"
#include "ddeMsg_m.h"
#include "IPAddressResolver.h"
#include <vector>
#include <algorithm>
#include "InterfaceTableAccess.h"
#include <stdio.h>
#include <stdlib.h>
#include "fisNS.h"
#include "cmn_utils.h"



#define BEV	EV << "[InfoServer]:"

//TODO: these could be defined differently, use different numbers?
//#define MSGKIND_CONNECT		10
//#define MSGKIND_REQUEST     11
//#define MSGKIND_SEND        12
//#define MSGKIND_INPUT       13
//#define MSGKIND_RESPONSE    14
//#define MSGKIND_FINISH      15
//#define MSGKIND_SLEEP		16
//#define MSGKIND_SLEEP_RESPONSE		17
//#define MSGKIND_WAKEUP  18
//#define MSGKIND_WAKEUP_RESPONSE 19

//Self messages for base module
#define INTERNAL_DECISION_LOCK 11
#define INTERNAL_INIT_CONNECTION_MSG    20
#define INTERNAL_CLASSIFY_MSG    21
#define INTERNAL_LOAD_BALANCING_MSG    22
#define INTERNAL_JOB_MSG	23
#define INTERNAL_WAKEUP_MSG  24
//Self messages for handlers
#define EVT_TOUT    25
//Self messages for threads
#define INTERNAL_REMOVE_THREAD_MSG   26
#define INTERNAL_THREAD_CLASSIFY_MSG   27
#define INTERNAL_THREAD_MN_CLASSIFY_MSG   28
#define INTERNAL_ADD_CLIENT_THREAD_MSG   29

// sendDirect message
#define MK_AP_LIST 30


Register_Class(InfoServerClientHandler);
Register_Class(InfoServerAPHandler);
Define_Module(InfoServer);

/**
 * Constructor.
 */

InfoServer::InfoServer()
{
	setMsgsRcvd(0);
	setMsgsSent(0);
	setBytesRcvd(0);
	setBytesSent(0);

}

/**
 * Destructor.
 */

InfoServer::~InfoServer()
{
	//cancelAndDelete(evtSleep);

}

//This function is needed for the WATCH_LIST(apList) command
//Prints the AP struct in the GUI
std::ostream& operator<<(std::ostream& os, const InfoServer::apInfo& ap)
{
    os << "ID=" << ap.ID
       << " stringID=" << ap.stringID
       << " Address=" << ap.apAddress
       << " classifyValue=" << ap.classifyValue
       << " numUsers=" << ap.numUsers
       << " load=" << ap.load
       << " loss=" << ap.packetLoss
       << " congestion=" << ap.congestion
       << " maxBW=" << ap.maxBW
       << " avBW=" << ap.avBW
       << " consumedBW=" << ap.consumedBW
       << " numGold=" << ap.numGold
       << " numSilver=" << ap.numSilver
       << " numBronze=" << ap.numBronze;
    return os;
}

std::ostream& operator<<(std::ostream& os, const InfoServer::clientInfo& client)
{
    os << "ID=" << client.ID
       << " Address=" << client.clientAddress
       << " mnClassifyValue=" << client.mnClassifyValue
       << " signalStrength=" << client.signalStrength
       << " assocAP=" << client.assocAP
       << " X=" << client.client_x
       << " Y=" << client.client_y
       << " userClass=" << client.userClass
       << " rssiDBM=" << client.rssiDBM
       << " noiseLevelScaled=" << client.noiseLevelScaled
       << " noiseLevelDBM=" << client.noiseLevelDBM
       << " thread=" << client.thread;
    return os;
}


void InfoServer::initialize()
{
	// parent initialize()
	TCPSrvHostApp::initialize();

    delay = par("replyDelay"); //not needed
    maxMsgDelay = 0; //not needed
    numAPs_var = par("numAPs");
    startTime_var = par("startTime");
    requestLength_var = par("requestLength");
    receivedClassify = 0;
    loadBalancing_interval = par("loadBalancing_interval");
    congestionThreshold = par("congestionThreshold");
    loadBalancing_var = par("loadBalancing");

    WATCH(msgsRcvd_var);
    WATCH(msgsSent_var);
    WATCH(bytesRcvd_var);
    WATCH(bytesSent_var);
    WATCH_LIST(apList);
    WATCH_LIST(cliList);

    string string1 = "ap";
    string string2;
    string string3;
    stringstream convert;
    //form connections with all APs in the beginning and keep connections open
    for (int i = 0; i < numAPs_var; i++)
    {

    	apInfo *ap;
    	apList.push_back(apInfo());
    	ap = &apList.back();
    	ap->ID = (i+1);
    	ap->classifyValue = 0;
    	ap->apAddress = IPvXAddress("0.0.0.0");
    	ap->congestion = false;

    	convert << (i+1);
    	string2 = convert.str();
    	string3 = string1 + string2; // Will contain "ap%"
    	ap->stringID = string3;

    	convert.str(std::string());
    	string3.clear();

        //Generate a message for forming the connections to the AP
    	ddeInternalMsg *msg = new ddeInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
    	msg->setApIndex(i+1); // set the AP index into the msg

    	scheduleAt(simTime()+startTime_var, msg);

    }

    //form internal timer for contacting the load balancing algorithm
    if(loadBalancing_var == true)
    {
		cMessage *internalLB = new cMessage("INTERNAL_LOAD_BALANCING_MSG", INTERNAL_LOAD_BALANCING_MSG);
		scheduleAt(simTime()+loadBalancing_interval, internalLB);
    }

    lbModp = getParentModule()->getSubmodule("loadBalancing");
    loadBalancing = check_and_cast<LoadBalancing *>(lbModp);





}

void InfoServer::callAlgorithm()
{
	vector<Event> events;
	SampleEx poa_sample;
	SampleEx client_sample;
	Event evnt;
	map<string, int> subscription_types;
	map<string, string> poa_types;
	bool congestion;
	MACAddress apMac;
	string macString = "";
	string strStatus = "";
	string strUsers = "";
	string commaSpace = ", ";

	string string1 = "WLAN_AP";
	string string2;
	string string3;
	string maxBW = "";
	string avBW = "";
	string consumedBW = "";
	string capacityString = "";
	string numGold ="";
	string numSilver ="";
	string numBronze ="";



	// 2. PoA sample
    for (APList::iterator it=apList.begin(); it!=apList.end(); ++it)
    {

    	macString = (it->macAddress).str();

    	// 3. PoA types
    	string2 = intToString(it->ID);
    	string3 = string1 + string2; // Will contain "WLAN_AP%d"

    	std::cout << "string3: " << string3 << endl;

    	poa_types[macString] = string3;

    	string3.clear();

    	// 1. Congestion
    	congestion = it->congestion;
    	if(congestion == false)
    	{
    		evnt = Event("Congestion", macString, 0, 0, "0");
    		events.push_back(evnt);
    	}
    	else
    	{
    		evnt = Event("Congestion", macString, 0, 0, "1");
    		events.push_back(evnt);
    	}
    	// 2. PoA status
    	strStatus = floatToString(it->classifyValue);

    	evnt = Event("PoA status", macString, 0, 0, strStatus);
    	events.push_back(evnt);

    	std::cout << "macString: " << macString << endl;
    	std::cout << "PoA status: " << strStatus << endl;

    	// 3. Network type
    	evnt = Event("Network type", macString, 0, 0, "WLAN");
    	events.push_back(evnt);

    	// 4. Nb of active clients
    	strUsers = intToString(it->numUsers);

    	evnt = Event("Nb of active clients", macString, 0, 0, strUsers);
    	events.push_back(evnt);

    	std::cout << "Nb of active clients: " << strUsers << endl;

    	// 5. Capacity
    	maxBW = floatToString(it->maxBW);
    	avBW = floatToString(it->avBW);
    	consumedBW = floatToString(it->consumedBW);
    	capacityString = maxBW + commaSpace + consumedBW + commaSpace + avBW;

    	evnt = Event("Capacity", macString, 0, 0, capacityString);
    	events.push_back(evnt);

    	std::cout << "Capacity: " << capacityString << endl;

    	capacityString.clear();

    	//6. User class distribution
    	numGold = intToString(it->numGold);
    	numSilver = intToString(it->numSilver);
    	numBronze = intToString(it->numBronze);
    	evnt = Event("NumGold", macString, 0, 0, numGold);
    	events.push_back(evnt);
    	evnt = Event("NumSilver", macString, 0, 0, numSilver);
    	events.push_back(evnt);
    	evnt = Event("NumBronze", macString, 0, 0, numBronze);
    	events.push_back(evnt);

    	// Add all events to PoA sample
    	poa_sample.AddItem(macString, events);
    	events.clear();
    }

    string assocAP;
    apInfo* ap;
    string clientAP_MAC;
    string clientMAC;
    string connectionState;
    string comma = ",";
    string RSSI_NOISE = "";
    string strClientStatus = "";
    string semiColon = ";";
    string signalStrength = "";
    string RSSINOISE = "";
    string signalStrengthDBM = "";
    string noiseLevel = "0.0"; //TODO
    string noiseLevelDBM = "-110";
	string PoAList = "";
	string apInfo = "";

	string numAPs = "";
	int apNum = 0;
	string apInfoList = "";

	// 4. Client sample
    for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
    {

    	assocAP = it->assocAP;
    	ap = lookupAPByString(assocAP);

    	clientAP_MAC = (ap->macAddress).str();
    	clientMAC = (it->clientMAC).str();

    	std::cout << "clientAP_MAC: " << clientAP_MAC << endl;
    	std::cout << "clientMAC: " << clientMAC << endl;

    	// 5. Subscription types
    	subscription_types[it->ID] = it->userClass;

    	std::cout << "subscription_type: " << it->userClass << endl;

    	// RSSI/NOISE
    	signalStrength = floatToString(it->signalStrength);
    	signalStrengthDBM = floatToString(it->rssiDBM);

    	RSSINOISE = clientMAC + commaSpace + clientAP_MAC + commaSpace + signalStrength + commaSpace
    			+ signalStrengthDBM + commaSpace + noiseLevel + commaSpace + noiseLevelDBM;

    	//evnt = Event("RSSI/Noise", it->ID, 0, 0, "00:22:fb:68:0b:34, 00:1A:30:6A:0D:10, 72.5, -46, 50.0, -256");
    	evnt = Event("RSSI/Noise", it->ID, 0, 0, RSSINOISE);
    	events.push_back(evnt);



    	std::cout << "RSSINOISE: " << RSSINOISE << endl;

    	// BW requirement
    	evnt = Event("BW requirement", it->ID, 0, 0, floatToString(it->BW_requirement));
    	events.push_back(evnt);

    	std::cout << "BW requirement: " << floatToString(it->BW_requirement) << endl;

    	// PoA list
    	apNum = 0;
    	//numAPs = intToString((it->clientAPList).size()) + semiColon;
    	//PoAList += numAPs;


        for (ClientAPList::iterator list_it=(it->clientAPList).begin(); list_it!=(it->clientAPList).end(); ++list_it)
        {

    		//apInfo = floatToString(list_it->networkClassification) + semiColon + (list_it->macAddress).str() + comma;
    		apInfo = (list_it->macAddress).str() + comma + floatToString(list_it->networkClassification)  + semiColon;

    		//TODO: include only if classification is above threshold --> include as parameter
    		if(list_it->networkClassification > 1.0)
    		{
    			apInfoList += apInfo;
    			apNum++;
    		}
    		//PoAList += apInfo;

        }

        PoAList = intToString(apNum) + semiColon + apInfoList;

        std::cout << "PoAList: " << PoAList << endl;
    	//evnt = Event("PoA list", it->ID, 0, 0, "2;00:1A:30:6A:0D:10,4.73267728072;00:1A:30:6A:0A:70,2.61891891892");
        evnt = Event("PoA list", it->ID, 0, 0, PoAList);
    	events.push_back(evnt);
    	PoAList.clear();
    	apInfoList.clear();

    	// Connection state
    	strClientStatus = floatToString(it->nsClassifyValue);
    	connectionState = clientAP_MAC + comma + strClientStatus;

    	evnt = Event("Connection state", it->ID, 0, 0, connectionState);
    	events.push_back(evnt);
    	std::cout << "connectionState: " << connectionState << endl;

    	connectionState.clear();

    	// Add all events to client sample
    	client_sample.AddItem(it->ID, events);
    	events.clear();

    }

//	subscription_types["Gold1"] = 1;
//
//
//	poa_types["00:1A:30:6A:0A:70"] = "WLAN_AP1";
//	poa_types["00:1A:30:6A:0D:10"] = "WLAN_AP2";
//
//	//Step 1 (action)
//	evnt = Event("Congestion", "00:1A:30:6A:0D:10", 0, 0, "0");
//	events.push_back(evnt);
//	evnt = Event("PoA status", "00:1A:30:6A:0D:10", 0, 0, "5");
//	events.push_back(evnt);
//	evnt = Event("Network type", "00:1A:30:6A:0D:10", 0, 0, "WLAN");
//	events.push_back(evnt);
//	evnt = Event("Nb of active clients", "00:1A:30:6A:0D:10", 0, 0, "2");
//	events.push_back(evnt);
//	evnt = Event("Capacity", "00:1A:30:6A:0D:10", 0, 0, "3400.55, 541.786678679, 2858.76332132");
//	events.push_back(evnt);
//	poa_sample.AddItem("00:1A:30:6A:0D:10", events);
//
//	events.clear();
//	evnt = Event("Congestion", "00:1A:30:6A:0A:70", 0, 0, "1");
//	events.push_back(evnt);
//	evnt = Event("PoA status", "00:1A:30:6A:0A:70", 0, 0, "2");
//	events.push_back(evnt);
//	evnt = Event("Network type", "00:1A:30:6A:0A:70", 0, 0, "WLAN");
//	events.push_back(evnt);
//	evnt = Event("Nb of active clients", "00:1A:30:6A:0A:70", 0, 0, "3");
//	events.push_back(evnt);
//	evnt = Event("Capacity", "00:1A:30:6A:0A:70", 0, 0, "3918.85, 3570.60235425, 348.247645754");
//	events.push_back(evnt);
//	poa_sample.AddItem("00:1A:30:6A:0A:70", events);
//
//	events.clear();
//
//	evnt = Event("RSSI/Noise", "Gold1", 0, 0, "00:22:fb:68:0b:34, 00:1A:30:6A:0D:10, 72.5, -46, 50.0, -256");
//	events.push_back(evnt);
//	evnt = Event("BW requirement", "Gold1", 0, 0, "4000.000000");
//	events.push_back(evnt);
//	evnt = Event("PoA list", "Gold1", 0, 0, "2;00:1A:30:6A:0D:10,4.73267728072;00:1A:30:6A:0A:70,2.61891891892");
//	events.push_back(evnt);
//	evnt = Event("Connection state", "Gold1", 0, 0, "00:1A:30:6A:0A:70,2.0");
//	events.push_back(evnt);
//	client_sample.AddItem("Gold1", events);


	// Contact load balancing module to get return events
	vector<Event> returnEvents;

	returnEvents = loadBalancing->doLoadBalancing(poa_sample, client_sample, subscription_types, poa_types);

	//TODO:nsfis
	//returnEvents.clear();

	cout << "returnEvents: " << endl;
	for (unsigned int i = 0; i < returnEvents.size(); i++)
	{
		cout << returnEvents[i].GetName() << " " << returnEvents[i].GetValue() << endl;
	}

	// Parse client and ap names from the events

	string clientName;
	string APName;
	pair<string, string> ho_event;
	vector <string> apNameList;
	MACAddress apMAC;
	string apSSID ="";

	for (unsigned int i = 0; i < returnEvents.size(); i++)
	{

		ho_event = ParseHandoverEvent(returnEvents[i].GetValue());
		clientName = ho_event.first;
		APName = ho_event.second;

		cout << "ClientName: " << clientName << endl;
		//cout << "APName: " << APName << endl;
		apNameList = ParseAPName(APName); // parse AP name, since it may contain several APs

		//clientName = "wirelessHostLearning[0]";

		for(unsigned int k = 0; k < apNameList.size(); k++)
		{
			cout << "APName: " << apNameList[k]<< endl;
		}

		//TODO: select which AP from list?
		apMAC = MACAddress(apNameList[0].c_str());
		apSSID = getAPSSIDByMAC(apMAC);
		cout << "apSSID: " << apSSID << endl;

		// Get pointer for thread
		clientInfo* client = lookupClientID(clientName.c_str());
		if(!client)
			error("Client not found");

		TCPServerThreadBase *thread = client->thread;
		cout << "thread: " << thread << endl;
		InfoServerClientHandler* handler = (InfoServerClientHandler*)thread;
		handler->sendHandoverRecommendation(apMAC, apSSID);
	}



	// Send a message to all clients
//	cModule *clientModule;
//	if(returnEvents.size() == 0)
//	{
//		for(unsigned int n = 0 ; n < cliList.size() ; n++)
//		{
//
//			clientModule = getParentModule()->getParentModule()->getSubmodule("wirelessHostLearning", n)->getSubmodule("tcpApp", 0) ;
//
//			cMessage *decisionLock = new cMessage("decisionLock", INTERNAL_DECISION_LOCK);
//			sendDirect(decisionLock, clientModule, "directInput"); //sending info to the client module
//		}
//	}


}

string InfoServer::intToString(int value)
{
	stringstream ss;
	ss << value;
	string strValue = ss.str();

	return strValue;

}

string InfoServer::floatToString(float value)
{
	stringstream ss;
	ss << value;
	string strValue = ss.str();

	return strValue;

}

pair<string, string> InfoServer::ParseHandoverEvent(string str) const
{
	pair<string, string> ho_event;
	vector<string> vparts;
	::SplitString(vparts, str, ';');
	if (vparts.size() == 2)
	{
		ho_event = make_pair(vparts[0], vparts[1]);
	}
	else
	{
		stringstream smsg;
		smsg << "Cannot parse HO event.";
		throw logic_error(smsg.str());
	}
	return ho_event;
}

vector<string> InfoServer::ParseAPName(string str) const
{
	//pair<string, string> apName;

	vector<string> vparts;
	::SplitString(vparts, str, ',');

	if (vparts.size() != 0)
	{
		// Do nothing, everything OK.
	}

	else
	{
		stringstream smsg;
		smsg << "Cannot parse AP name.";
		throw logic_error(smsg.str());
	}

//	else if (vparts.size() == 2)
//	{
//		apName = make_pair(vparts[0], vparts[1]);
//	}

	return vparts;
}


// Handles messages coming to this module
void InfoServer::handleMessage(cMessage* msg)
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
		TCPSrvHostApp::handleMessage(msg);


	}

}

//Handles self messages
void InfoServer::handleSelfMessage(cMessage *msg)
{
	switch(msg->getKind())
	{

		//init a connection with AP, infoServer never initiates a connection with client, only AP
		case INTERNAL_INIT_CONNECTION_MSG:
		{
			ddeInternalMsg *appmsg = dynamic_cast<ddeInternalMsg *>(msg);
			int AP_index = appmsg->getApIndex();

			char string[80];
			sprintf(string, "ap%d", AP_index);
			const char *connectAddress = string;

		    // connect
		    //const char *connectAddress = appmsg->getConnectAddress();

			BEV << "Establishing a connection with AP: " << connectAddress << endl;
			// new connection -- create new socket object and server process
			TCPSocket *newsocket = new TCPSocket();

			//unsigned short port = tcp->getEphemeralPort();
			int port = par("port");

			//NOTE: adding this (TCP.h etc.) causes warnings
			//while ((port==connectPort))
				//port = tcp->getEphemeralPort();

			newsocket->bind(port);

			//const char *serverThreadClass = (const char*)par("serverThreadClass");
			const char *serverThreadClass = "InfoServerAPHandler";

			TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

			newsocket->setCallbackObject(proc);

			//Set thread into AP struct
			setAPThread(AP_index, proc);

			//Creates a thread object to the server handler
			//By this we can call methods from concerning this thread
			InfoServerAPHandler* myProc = (InfoServerAPHandler*) proc;

			myProc->init(this, newsocket); // This initializes "InfoServerAPHandler"
			myProc->setID(AP_index);

		    // find module
		    cModule *mod = simulation.getModuleByPath(connectAddress);
			newsocket->setOutputGate(gate("tcpOut"));

			//IP address of the server is retrieved by the IPAddressresolver,
			// based on the name of the server

			newsocket->connect(IPAddressResolver().routerIdOf(mod), port);
			//newsocket->connect(IPAddressResolver().resolve(connectAddress), port);
			socketMap.addSocket(newsocket);

			updateDisplay();

			delete msg;
			break;

		}
		case INTERNAL_LOAD_BALANCING_MSG:
		{
			BEV << "Received an internal load balancing msg. Contacting the Load Balancing algorithm." << endl;

			// Carry out load balancing if there are clients
			if(cliList.size() != 0)
			{
				doLoadBalancing();
			}
			else
			{
				std::cout << simTime() << ", no clients active." << endl;
			}


			delete msg;
			// Schedule another message
		    cMessage *internalLB = new cMessage("INTERNAL_LOAD_BALANCING_MSG", INTERNAL_LOAD_BALANCING_MSG);
		    scheduleAt(simTime()+loadBalancing_interval, internalLB);
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
void InfoServer::handleThreadMessage(cMessage *msg)
{

	// Get a pointer for the thread
	TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

	// This would get a handler for the thread. Handler could be used to call functions
	//InfoServerClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

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

void InfoServer::doLoadBalancing()
{
	BEV << "Doing load balancing!" << endl;

	for (ClientList::iterator clientIt=cliList.begin(); clientIt!=cliList.end(); ++clientIt)
	{
		BEV << "Client: " << clientIt->ID << endl;
		BEV << "X: " << clientIt->client_x << endl;
		BEV << "Y: " << clientIt->client_y << endl;


//	    for (ClientAPList::iterator clientAPit=(clientIt->clientAPList).begin(); clientAPit!=(clientIt->clientAPList).end(); ++clientAPit)
//	    {
//	    	BEV << "Client AP: " << clientAPit->ID << endl;
//	    	BEV << "Client classify: " << clientAPit->networkClassification << endl;
//	    }

	}

	callAlgorithm();
}

void InfoServer::addAPinfo(int ID, float classifyValue, int numUsers, float load, float packetLoss, IPvXAddress apAddr, bool cachedContent, const MACAddress& macAddress,
		double maxBW, double avBW, double consumedBW, int numGold, int numSilver, int numBronze)
{

	apInfo *ap = lookupAP(ID);
	ap->ID = ID;
	ap->classifyValue = classifyValue;
	ap->apAddress = apAddr;
	ap->numUsers = numUsers;
	ap->load = load;
	ap->packetLoss = packetLoss;
	ap->cachedContent = cachedContent;
	ap->macAddress = macAddress;
	ap->maxBW = (maxBW/1000) / 8; // kbps ---> kBps (kilobytes per second!)
	ap->avBW = (avBW/1000) / 8;
	ap->consumedBW = (consumedBW/1000) / 8;
	ap->numGold = numGold;
	ap->numSilver = numSilver;
	ap->numBronze = numBronze;

	// Congestion threshold based on the numGold
//	if(numGold == 0)
//	{
//		if(classifyValue < congestionThreshold)
//		{
//			ap->congestion = true;
//		}
//		else
//		{
//			ap->congestion = false;
//		}
//	}
//	else
//	{
//		if(classifyValue < 2.25)
//		{
//			ap->congestion = true;
//		}
//		else
//		{
//			ap->congestion = false;
//		}
//
//	}


	if(classifyValue < congestionThreshold)
	{
		ap->congestion = true;
	}
	else
	{
		ap->congestion = false;
	}

}

//Looks up AP from the client list
InfoServer::apInfo *InfoServer::lookupAP(int ID)
{
    for (APList::iterator it=apList.begin(); it!=apList.end(); ++it)
        if (it->ID == ID)
            return &(*it);
    return NULL;
}

InfoServer::apInfo *InfoServer::lookupAPByString(string stringID)
{
    for (APList::iterator it=apList.begin(); it!=apList.end(); ++it)
        if (strcmp (it->stringID.c_str(), stringID.c_str()) == 0)
            return &(*it);
    return NULL;
}

string InfoServer::getAPSSIDByMAC(MACAddress macAddr)
{
    for (APList::iterator it=apList.begin(); it!=apList.end(); ++it)
        if (it->macAddress == macAddr)
            return it->stringID;
    return NULL;
}

float InfoServer::getAPclassification(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->classifyValue;
}

IPvXAddress InfoServer::getAPaddress(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->apAddress;
}

int InfoServer::getAPNumUsers(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->numUsers;
}

float InfoServer::getAPLoad(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->load;
}

float InfoServer::getAPPacketLoss(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->packetLoss;
}

bool InfoServer::getAPCachedContent(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->cachedContent;
}

MACAddress InfoServer::getAPMacAddress(int ID)
{
	apInfo *ap = lookupAP(ID);
	return ap->macAddress;
}

TCPServerThreadBase* InfoServer::getAPThread(int apIndex)
{
	apInfo *ap = lookupAP(apIndex);
	return ap->thread;
}

void InfoServer::setAPThread(int apIndex, TCPServerThreadBase* thread)
{
	apInfo *ap = lookupAP(apIndex);
	ap->thread = thread;
}

//Adds client info to the client struct
void InfoServer::addClientInfo(IPvXAddress clientAddr, const char* hostID, float mnClassifyValue, const char* assocAP, float ss,
		double client_x, double client_y, int userClass, float nsClassifyValue, MACAddress clientMAC, double BW_requirement, float rssiDBM, float sensitivityValue, float noiseLevelDBM,
		TCPServerThreadBase* thread)
{

    clientInfo *client = lookupClientID(hostID);
    if (client)
    {
        EV << "Client address=" << clientAddr << ", SSID=" << hostID << " already in our client list, refreshing the info" << endl;

    }
    else
    {
        EV << "Inserting Client address=" << clientAddr << ", ID=" << hostID << " into the Client list" << endl;
        cliList.push_back(clientInfo());
        client = &cliList.back();

    }

	client->clientAddress = clientAddr;
	client->ID = hostID;
	client->mnClassifyValue = mnClassifyValue;
	client->assocAP = assocAP;
	client->signalStrength = ss;
	client->client_x = client_x;
	client->client_y = client_y;
	client->userClass = userClass;
	client->nsClassifyValue = nsClassifyValue;
	client->clientMAC = clientMAC;
	client->BW_requirement = BW_requirement;
	client->noiseLevelDBM = milliwattToDBM(noiseLevelDBM);
	client->noiseLevelScaled = (noiseLevelDBM - sensitivityValue) / noiseLevelDBM;
	client->thread = thread;

	// Convert the milliwatt value to dBm
	client->rssiDBM = milliwattToDBM(rssiDBM);


	clientAPInfo *tempAP;
	ClientAPList temp;

	client->clientAPList.clear();

	double estimatedSS = 0.0;
    int tempIndex = 0;
    char tempString[80];
    const char *tempSSID;

	float ec = 100; //energy consumption
	float bl = 100; //battery level
	int mn = 1;

	float MNclassification = 0.0;
	float NSclassification = 0.0;

	// Form list of PoAs the client sees
	for (APList::iterator it=apList.begin(); it!=apList.end(); ++it)
	{

		MNclassification = 0.0;
		NSclassification = 0.0;
		estimatedSS = 0.0;

		temp.push_back(clientAPInfo());
		tempAP = &temp.back();
		tempAP->ID = it->ID;

    	tempIndex = it->ID;
    	sprintf(tempString, "ap%d", tempIndex);
    	tempSSID = tempString;

		estimatedSS = getEstimatedSignalStrength(tempSSID, client->client_x, client->client_y);

		tempAP->signalStrength = estimatedSS;

		//TODO:nsfis
		MNclassification = nsfis(mn,estimatedSS,ec,bl);
		//MNclassification = 4.0;


		tempAP->mnClassification = MNclassification;
		tempAP->apClassification = it->classifyValue;
		tempAP->macAddress = it->macAddress;


		//NSclassification = 4.0;
		NSclassification = nsfis(2, it->classifyValue, MNclassification, 1.0);
		//NSclassification = computeNetworkClassification(it->classifyValue, MNclassification);
		tempAP->networkClassification = NSclassification;

		tempAP->numberOfUsers = it->numUsers;


		tempAP = NULL;


	}

	//Sort the list (best PoA first according to network classification value)
	temp.sort(SortDescending());


    client->clientAPList = temp;

    //std::cout << "Client List: " << endl;

    for (ClientAPList::iterator it=(client->clientAPList).begin(); it!=(client->clientAPList).end(); ++it)
    {
		std::cout << it->ID <<", network class: " << it->networkClassification << ", SS: " << it->signalStrength << ", mn class: " << it->mnClassification  << ", ap Class: " << it->apClassification
				<< ", numUsers: " << it->numberOfUsers << ", " << it->macAddress << endl;

    }

    temp.clear();
}


//Adds client initially to the client struct
void InfoServer::addClientInitially(IPvXAddress clientAddr, TCPServerThreadBase* thread)
{

    clientInfo *client = lookupClientIP(clientAddr);
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

}

float InfoServer::computeNetworkClassification(float apClassification, float MNclassification)
{
	float nsClassification = 0.0;
	float cache = 1.0;

	//TODO:nsfis

	nsClassification = nsfis(2, apClassification, MNclassification, cache);
	//nsClassification = 4.0;

	return nsClassification;
}

//Calculates an estimated signal strength based on distance
double InfoServer::computeEstimatedSignalStrength(const char* apSSID, double client_x, double client_y)
{

	float signalStrength = 0.0;
	float rssi = 0.0;


	Coord clientPos = new Coord();
	clientPos.x = client_x;
	clientPos.y = client_y;


    // Get AP position
	cModule* apMod = getParentModule()->getParentModule()->getSubmodule(apSSID)->getSubmodule("wlan",0)->getSubmodule("radio");
	AbstractRadioExtended* apRadio = check_and_cast<AbstractRadioExtended*> (apMod);
    const Coord& apPos = apRadio->getPosition();

    //Calculate the distance
    double distance = clientPos.distance(apPos);

    apRadio->getEstimatedSignalStrength(distance, signalStrength, rssi);

    //std::cout << "SSID: " << ssid << ",signal strength: " << signalStrength << ", distance: " << distance << endl;

    if(signalStrength < 0)
    	signalStrength = 0;

    return signalStrength;
}

// Gets an estimated signal strength
double InfoServer::getEstimatedSignalStrength(const char* ssid, double client_x, double client_y)
{
	double average = 0.0;
	double ss = 0.0;
	double sum = 0.0;
	//TODO:fix this
	for(int i = 0; i < 100 ; i++)
	{
		ss = computeEstimatedSignalStrength(ssid,  client_x, client_y) * 100;
		sum += ss;
	}

	average = sum/100;

//	ss = computeEstimatedSignalStrength(ssid,  client_x, client_y) * 100;
//	average = ss;

	return average;
}


//Looks up AP from the client AP list
InfoServer::clientAPInfo *InfoServer::lookupClientAP(int clientAP_ID, ClientAPList list)
{
    for (ClientAPList::iterator it=list.begin(); it!=list.end(); ++it)
        if (it->ID == clientAP_ID)
            return &(*it);
    return NULL;
}

//Looks up client from the client list by string ID
InfoServer::clientInfo *InfoServer::lookupClientID(const char* clientID)
{
    for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
       // if (it->ID == clientID)
        if (strcmp(it->ID.c_str(), clientID) == 0)
            return &(*it);
    return NULL;
}

//Looks up client from the client list by IP address
InfoServer::clientInfo *InfoServer::lookupClientIP(IPvXAddress clientAddress)
{
    for (ClientList::iterator it=cliList.begin(); it!=cliList.end(); ++it)
        if (it->clientAddress == clientAddress)
            return &(*it);
    return NULL;
}

// Deletes a client according to a IP address
void InfoServer::deleteClient(IPvXAddress clientAddress)
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
void InfoServer::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->getSocket());
    updateDisplay();
}

//This function should cover all the statistics
void InfoServer::finish()
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


long InfoServer::getMsgsRcvd()
{
	return msgsRcvd_var;
}

void InfoServer::setMsgsRcvd(long msgsRcvd)
{
	msgsRcvd_var = msgsRcvd;
}

void InfoServer::incrementMsgsRcvd()
{
	msgsRcvd_var++;
}

long InfoServer::getMsgsSent()
{
	return msgsSent_var;
}

void InfoServer::setMsgsSent(long msgsSent)
{
	msgsSent_var = msgsSent;
}

void InfoServer::incrementMsgsSent()
{
	msgsSent_var++;
}


long InfoServer::getBytesRcvd()
{
	return bytesRcvd_var;
}

void InfoServer::setBytesRcvd(long bytesRcvd)
{
	bytesRcvd_var = bytesRcvd;
}

void InfoServer::incrementBytesRcvd(long byteLength)
{
	bytesRcvd_var += byteLength;
}

long InfoServer::getBytesSent()
{
	return bytesSent_var;
}

void InfoServer::setBytesSent(long bytesSent)
{
	bytesSent_var = bytesSent;
}

void InfoServer::incrementBytesSent(long byteLength)
{
	bytesSent_var += byteLength;
}

int InfoServer::getRequestLength()
{
	return requestLength_var;
}

void InfoServer::setRequestLength(int requestLength)
{
	requestLength_var = requestLength;
}

int InfoServer::getNumAPs()
{
	return numAPs_var;
}


/**
 * Constructor.
 */

InfoServerClientHandler::InfoServerClientHandler()
{
	//evtTout		= new cMessage("timeoutServer", EVT_TOUT);
	//delThreadMsg = new ddeInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

}

/**
 * Destructor.
 */

InfoServerClientHandler::~InfoServerClientHandler()
{
	// cancel any pending messages
	//hostModule()->cancelAndDelete(evtTout);
//	hostModule()->cancelAndDelete(delThreadMsg);
	//evtTout = NULL;
//	delThreadMsg = NULL;;
}

void InfoServerClientHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}

void InfoServerClientHandler::sendMessage(cMessage *msg)
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

void InfoServerClientHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [InfoServerClientHandler]: connection with client[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//hostModule()->addClientInfo(getSocket()->getRemoteAddress());

	// Add a timer for adding the client thread
	cMessage* addClient = new cMessage("addClient", INTERNAL_ADD_CLIENT_THREAD_MSG);
	scheduleAt(simTime(), addClient);

	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

}

/**
 * Handles the reception of a message
 *
 *
 */

void InfoServerClientHandler::dataArrived(cMessage* msg, bool urgent)
{

	hostModule()->incrementMsgsRcvd();

    if(strcmp (msg->getName(),"CLASSIFY_AP") == 0)
    {
    	ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);
    	    if (!appmsg)
    	        opp_error("Message (%s)%s is not a ddeMsg -- "
    	                  "probably wrong client app, or wrong setting of TCP's "
    	                  "sendQueueClass/receiveQueueClass parameters "
    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	                  msg->getClassName(), msg->getName());

    	hostModule()->incrementBytesRcvd(appmsg->getByteLength());

    	//Retrieve AP information from list and send back to client
		//Write the list to the message
		::APList v;

		//for (APList::iterator it= temp.begin(); it!= temp.end(); ++it)
		for(int i = 0; i < hostModule()->getNumAPs(); i++)
		{
			::apInfo *ap;
			v.push_back(::apInfo());
			ap = &v.back();
			ap->ID = (i+1);
			ap->classifyValue = hostModule()->getAPclassification(i+1);
			ap->apAddress = hostModule()->getAPaddress(i+1);

			ap->numUsers = hostModule()->getAPNumUsers(i+1);
			ap->load = hostModule()->getAPLoad(i+1);
			ap->packetLoss = hostModule()->getAPPacketLoss(i+1);
			ap->cachedContent = hostModule()->getAPCachedContent(i+1);
			ap->macAddress = hostModule()->getAPMacAddress(i+1);
		}

    	ddeMsg *msg = new ddeMsg("INFO_CLASSIFICATION");
    	msg->setSeedsPerLeechers(100);
    	msg->setByteLength(appmsg->getByteLength());
    	msg->setApList(v);

		//How to read the list from a message
//		::APList test = msg->getApList();
//		for (::APList::iterator it=test.begin(); it!=test.end(); ++it)
//		{
//			std::cout << "test list ID: " << it->ID << endl;
//			std::cout << "test list classifyValue: " << it->classifyValue << endl;
//			std::cout << "test list apAddress: " << it->apAddress << endl;
//
//		}
    	delete appmsg;

		sendMessage(msg);


    }
    else if(strcmp (msg->getName(),"MN_CLASSIFICATION") == 0)
    {

    	ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);
    	    if (!appmsg)
    	        opp_error("Message (%s)%s is not a ddeMsg -- "
    	                  "probably wrong client app, or wrong setting of TCP's "
    	                  "sendQueueClass/receiveQueueClass parameters "
    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	                  msg->getClassName(), msg->getName());

    	hostModule()->incrementBytesRcvd(appmsg->getByteLength());
    	//do nothing
    	BEV <<"Received MN classification." << endl;
    	const char* hostID = appmsg->getHostID();
    	float mnClassify = appmsg->getMnClassifyValue();
    	const char* assocAP = appmsg->getAssocAP();
    	float ss = appmsg->getSignalStrength();
    	double client_x = appmsg->getClient_x();
    	double client_y = appmsg->getClient_y();
    	int userClass = appmsg->getUserClass();
    	float nsClassify = appmsg->getNsClassifyValue();
    	MACAddress clientMAC = appmsg->getMacAddress();
    	double BW_requirement = appmsg->getBW_requirement();
    	float rssiDBM = appmsg->getRssiDBM();
    	float sensitivityValue = appmsg->getSensitivity();
    	float noiseLevelDBM = appmsg->getNoiseLevelDBM();

    	std::cout << "InfoServer: received client info: " << hostID << ", "<< simTime() << endl;
//    	std::cout << "signal strength: " << ss << endl;
//    	std::cout << "rssiDBM: " << rssiDBM << endl;
//    	std::cout << "noiseLevelDBM: " << noiseLevelDBM << endl;

    	hostModule()->addClientInfo(getSocket()->getRemoteAddress(), hostID, mnClassify, assocAP, ss, client_x, client_y, userClass, nsClassify, clientMAC, BW_requirement, rssiDBM,
    			sensitivityValue, noiseLevelDBM, getThread());

    	std::cout << "InfoServer: added client info: " << hostID << ", "<< simTime() << endl;

    	//schedule next connection with client. Keep connection open? TODO: remove this from all configurations?
		//cMessage* classifyMN = new cMessage("INTERNAL_THREAD_MN_CLASSIFY_MSG", INTERNAL_THREAD_MN_CLASSIFY_MSG);
		//scheduleAt(simTime()+1.0, classifyMN);

    	delete appmsg;
    }
    else if(strcmp (msg->getName(),"unregister") == 0)
    {
    	BEV << "Received an unregister msg from Client IP: " << getSocket()->getRemoteAddress() << endl;
    	std::cout << "Info: Received an unregister msg from Client IP: " << getSocket()->getRemoteAddress() << ", " << simTime() << endl;
    	hostModule()->deleteClient(getSocket()->getRemoteAddress());

    	delete msg;

    }
    else
    {
    	GenericAppMsg *appmsg = dynamic_cast<GenericAppMsg *>(msg);
    	    	    if (!appmsg)
    	    	        opp_error("Message (%s)%s is not a GenericAppMsg -- "
    	    	                  "probably wrong client app, or wrong setting of TCP's "
    	    	                  "sendQueueClass/receiveQueueClass parameters "
    	    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	    	                  msg->getClassName(), msg->getName());

    	hostModule()->incrementBytesRcvd(appmsg->getByteLength());

        long requestedBytes = appmsg->getExpectedReplyLength();
        bool doClose = appmsg->getServerClose();

        if (requestedBytes==0)
        {
            delete appmsg;
        }
        else
        {
            appmsg->setByteLength(requestedBytes);
            delete appmsg->removeControlInfo();
            sendMessage(appmsg);
            //getSocket()->send(appmsg);
        }

        if (doClose)
        {
            getSocket()->close();
        }

    }


}

/**
 * Handles a session timeout event.
 *
 */
void InfoServerClientHandler::timerExpired(cMessage* msg)
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
		case INTERNAL_THREAD_MN_CLASSIFY_MSG:
		{
			if(this->getSocket()->getState() == TCPSocket::CONNECTED)
			{
				BEV << "Sending a classify operation to the client." << endl;
				sendClassifyMN();
			}

			delete msg;
			break;
		}
		case INTERNAL_ADD_CLIENT_THREAD_MSG:
		{
			TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

			InfoServerClientHandler* handler = (InfoServerClientHandler*)thread;
			handler->setThread(thread);

			//add client to a list initially
			//hostModule()->addClientInitially(getSocket()->getRemoteAddress(), thread);

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

void InfoServerClientHandler::sendClassifyMN()
{
	ddeMsg *ddeClassifyMsg = new ddeMsg("CLASSIFY_MN");
	ddeClassifyMsg->setByteLength(hostModule()->getRequestLength());
	ddeClassifyMsg->setClassifyOperation(1);
	sendMessage(ddeClassifyMsg);
}

void InfoServerClientHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void InfoServerClientHandler::sendHandoverRecommendation(MACAddress apMAC, string apSSID)
{

	ddeMsg *HO_recommendation = new ddeMsg("HANDOVER_RECOMMENDATION");
	HO_recommendation->setByteLength(hostModule()->getRequestLength());
	HO_recommendation->setHO_to_AP_MAC(apMAC);
	HO_recommendation->setHO_to_AP_SSID(apSSID.c_str());
	sendMessage(HO_recommendation);

}

void InfoServerClientHandler::setThread(TCPServerThreadBase* thread)
{
	clientThread = thread;
}

TCPServerThreadBase* InfoServerClientHandler::getThread()
{
	return clientThread;
}

void InfoServerClientHandler::closed()
{

	BEV << "Connection closed." << endl;

	cMessage* delThreadMsg;		// delete thread
	delThreadMsg = new ddeInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);
	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void InfoServerClientHandler::close()
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
 * Get the host module (i.e., the instance of InfoServer which spawns the thread).
 */
InfoServer* InfoServerClientHandler::hostModule()
{
	// get the host module and check its type
	InfoServer* hostMod = dynamic_cast<InfoServer*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/


/**
 * Constructor.
 */

InfoServerAPHandler::InfoServerAPHandler()
{
	//evtTout		= new cMessage("timeoutServer", EVT_TOUT);
//	delThreadMsg = new ddeInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

}

/**
 * Destructor.
 */

InfoServerAPHandler::~InfoServerAPHandler()
{
	// cancel any pending messages
//	hostModule()->cancelAndDelete(evtTout);
//	hostModule()->cancelAndDelete(delThreadMsg);
//	evtTout = NULL;
//	delThreadMsg = NULL;
	internalMsg = NULL;
	ID_var = -1;
}

void InfoServerAPHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}

void InfoServerAPHandler::sendMessage(cMessage *msg)
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

void InfoServerAPHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [InfoServerAPHandler]: connection with AP[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//sendClassify();
	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

}

void InfoServerAPHandler::sendClassify()
{
	ddeMsg *ddeClassifyMsg = new ddeMsg("CLASSIFY_AP");
	ddeClassifyMsg->setByteLength(hostModule()->getRequestLength());
	ddeClassifyMsg->setClassifyOperation(0);
	sendMessage(ddeClassifyMsg);
}

void InfoServerAPHandler::setID(int ID)
{
	ID_var = ID;
}

int InfoServerAPHandler::getID()
{
	return ID_var;
}

/**
 * Handles the reception of a message
 *
 *
 */

void InfoServerAPHandler::dataArrived(cMessage* msg, bool urgent)
{

	hostModule()->incrementMsgsRcvd();

    if(strcmp (msg->getName(), "AP_CLASSIFICATION") == 0)
	{

    	ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);
    	    if (!appmsg)
    	        opp_error("Message (%s)%s is not a ddeMsg -- "
    	                  "probably wrong client app, or wrong setting of TCP's "
    	                  "sendQueueClass/receiveQueueClass parameters "
    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	                  msg->getClassName(), msg->getName());

    	hostModule()->incrementBytesRcvd(appmsg->getByteLength());
		float APclassification = appmsg->getApClassifyValue();
		//float seedsPerLeechers = appmsg->getSeedsPerLeechers();
		BEV << "AP: " << getID() << ", Received AP classification: " << APclassification << endl;

		//if(getID() == 1)
			//std::cout << "InfoServer: AP: " << getID() << ", Received AP classification: " << APclassification << ", packet loss: " << appmsg->getPacketLoss() << ", load: " << appmsg->getLoad() << ", simTime: " << simTime() << endl;

		hostModule()->addAPinfo(getID(), APclassification, appmsg->getNumUsers(), appmsg->getLoad(), appmsg->getPacketLoss(), getSocket()->getRemoteAddress(), appmsg->getCachedContent(), appmsg->getMacAddress(),
				appmsg->getMaxBW(), appmsg->getAvBW(), appmsg->getConsumedBW(), appmsg->getNumGold(), appmsg->getNumSilver(), appmsg->getNumBronze());

		//TODO: Infoserver might not have up-to-date information about APs with high interval
		//TODO: send a classify on demand msg?
		// Schedule future classify event

		//internalMsg = new ddeMsg("INTERNAL_THREAD_CLASSIFY_MSG", INTERNAL_THREAD_CLASSIFY_MSG);
		//scheduleAt(simTime()+0.5, internalMsg);

		delete appmsg;


	}
    else
    {
    	GenericAppMsg *appmsg = dynamic_cast<GenericAppMsg *>(msg);
    	    	    if (!appmsg)
    	    	        opp_error("Message (%s)%s is not a GenericAppMsg -- "
    	    	                  "probably wrong client app, or wrong setting of TCP's "
    	    	                  "sendQueueClass/receiveQueueClass parameters "
    	    	                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
    	    	                  msg->getClassName(), msg->getName());

        long requestedBytes = appmsg->getExpectedReplyLength();
        hostModule()->incrementBytesRcvd(appmsg->getByteLength());
        bool doClose = appmsg->getServerClose();

        if (requestedBytes==0)
        {
            delete appmsg;
        }
        else
        {
            appmsg->setByteLength(requestedBytes);
            delete appmsg->removeControlInfo();
            sendMessage(appmsg);
            //getSocket()->send(appmsg);
        }

        if (doClose)
        {
            getSocket()->close();
        }

    }

}

/**
 * Handles a session timeout event.
 *
 */
void InfoServerAPHandler::timerExpired(cMessage* msg)
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
			//delete msg;
		}
		case INTERNAL_THREAD_CLASSIFY_MSG:
		{
			BEV << "Generating a classify operation to AP." << endl;
			sendClassify();
			//delete msg;
		}
		default:
		{
			//Do nothing
			//delete msg;
		}
	}
	delete msg;

}

void InfoServerAPHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void InfoServerAPHandler::closed()
{

	BEV << "Connection closed." << endl;
	cMessage* delThreadMsg;		// delete thread
	delThreadMsg = new ddeInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);
	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void InfoServerAPHandler::close()
{

	// cancel the session timeout event and call default close()
//	cancelEvent(evtTout);
	//hostModule()->cancelAndDelete(evtTout);
	//hostModule()->cancelEvent(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}


/**
 * Get the host module (i.e., the instance of InfoServer which spawns the thread).
 */
InfoServer* InfoServerAPHandler::hostModule()
{
	// get the host module and check its type
	InfoServer* hostMod = dynamic_cast<InfoServer*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}

/***** GET/SET methods *****/

