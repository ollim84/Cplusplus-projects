//
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


#include "FS4VP.h"
#include "TCP.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include <limits> // for infinity
#include <iostream>
#include <fstream>
//#include "INotifiable.h"



#define MK_TRIGGER_MSG 1002
#define MK_TX_TRIGGER_TIMER 1003
#define MK_REGISTER_REQ_MSG 1004
#define MK_DE_REGISTER_MSG 1005
#define MK_VIRT_PLAY_ADV_TIMER 1006
#define MK_AGENT_TRIGGER_TIMER 1007 //OM 08.02.2013

#define MK_CLIENT_STATS 10002 //ZY 16.07.2012
#define MK_START_AGENT_MSG 10003 //OM 07.02.2013
#define MK_FINISH_AGENT_MSG 10004 //OM
#define MK_WRITE_QMATRIX_MSG 10005 //OM



Define_Module(FS4VP);

static std::ostream& operator<<(std::ostream& os, const TCP::SockPair& sp)
{
    os << "loc=" << IPvXAddress(sp.localAddr) << ":" << sp.localPort << " "
       << "rem=" << IPvXAddress(sp.remoteAddr) << ":" << sp.remotePort;
    return os;
}

static std::ostream& operator<<(std::ostream& os, const TCP::AppConnKey& app)
{
    os << "connId=" << app.connId << " appGateIndex=" << app.appGateIndex;
    return os;
}

static std::ostream& operator<<(std::ostream& os, const TCPConnection& conn)
{
    os << "connId=" << conn.connId << " " << TCPConnection::stateName(conn.getFsmState())
       << " state={" << const_cast<TCPConnection&>(conn).getState()->info() << "}";
    return os;
}

 std::ostream& operator<<(std::ostream& os, const FS4VP::ContextInfoStruct& c)
{
    os <<"\n"<< "App_Id= " << c.appId <<"Connect Port Id: "<<c.connectPortId<<": App_FileSize=" <<c.fileSize<<" B"<<": Remaining_Bytes= "<<c.remainingBytes
    	<<"\n" <<": Chunk Size= "<<c.chunkSize<<"B: "<< " Codec Rate = "<<c.codecRate<<": Tx-Rate= "<<c.txRate<<"s"
    	<<"\n" <<"Playout Buffer Size ="<<c.playoutBufferSize<<" bits:" <<"Play Buffer State: "<<c.playBufferState
    	<<"\n" <<"Initial play started ="<<c.initialPlayStarted <<" userClass =" << c.userClass << " playFinished =" << c.playFinished;

    return os;
}

 std::ostream& operator<<(std::ostream& os, const FS4VP::QInfo& qInfo)
 {
     os << "N1_fraction=" << qInfo.N1_fraction
        << " N2_fraction=" << qInfo.N2_fraction
        << " N3_fraction=" << qInfo.N3_fraction
        << " N1_action=" << qInfo.N1_action
        << " N2_action=" << qInfo.N2_action
        << " N3_action=" << qInfo.N3_action
        << " Qvalue=" << qInfo.Qvalue;
     return os;
 }

 std::ostream& operator<<(std::ostream& os, const FS4VP::stateInfo& state)
 {
     os << "N1_fraction=" << state.N1_fraction
        << " N2_fraction=" << state.N2_fraction
        << " N3_fraction=" << state.N3_fraction
        << " N1_action=" << state.N1_action
        << " N2_action=" << state.N2_action
        << " N3_action=" << state.N3_action;
     return os;
 }
#include <string.h>
#include <iostream>
#include <map>
#include <utility>
#include <algorithm> //for max_element
#include <sys/time.h>
FS4VP::FS4VP()
{};

FS4VP::~FS4VP()
{};

void FS4VP::initialize(int stage)
{
    EV<<"This is the initialize function of the FS4VP module"<<endl;
    numOfTcpApps = par("numTcpApps");
//    polledAppIndex =0; //initializing the flag
    EV<<"Number of TCP Apps Registered with FS4VP = " << numOfTcpApps <<endl;

    listener = new cListener();
    subscribe("Total_Bytes_Received", listener);
    simulation.getSystemModule()->subscribe("Total_Bytes_Received", listener);

    if (isSubscribed("Total_Bytes_Received", listener))
    	EV<<"FS4VP Module is subscribed to a signal"<<endl;
    else
    	EV<<"FS4VP: No signal subscription "<<endl;

    if(stage==3)
    {

    	//nb = NotificationBoardAccess().get();
    	//The tx parameters are assigned statically as specified in *.ini file. TODO teh algo should be able to dynamcially determine these parameters
    	bytesToSend = par("bytesToSend");
    	chunkSize = par("chunkSize");
    	txRate = par("txRate");
    	iatChunk = par("iatChunk");
    	bufferEstimation = par("bufferEstimation");
    	alpha = par("alpha");
    	mss = par("maxSegSize");

    	//------------------ RL AGENT----------------- OM Add 08.02.2013
    	rlAgentStarted = false;
    	agentControlTime = par("agentControlTime");
    	gamma = par("gamma");
    	alphaRL = par("alphaRL");
    	beeta = par("beeta");
    	learningEnabled = par("learningEnabled");
    	burst = par("burst");

    	// Coefficients for Reward function
    	uN1Gold = par("uN1Gold");
    	uN2Gold = par("uN2Gold");
    	uN3Gold = par("uN3Gold");
    	uN1Silver = par("uN1Silver");
    	uN2Silver = par("uN2Silver");
    	uN3Silver = par("uN3Silver");
    	uN1Bronze = par("uN1Bronze");
    	uN2Bronze = par("uN2Bronze");
    	uN3Bronze = par("uN3Bronze");

    	// Increase/decrease percentages
    	goldIncPercentage = par("goldIncPercentage");
    	silverIncPercentage = par("silverIncPercentage");
    	bronzeIncPercentage = par("bronzeIncPercentage");

    	goldDecPercentage = par("goldDecPercentage");
    	silverDecPercentage = par("silverDecPercentage");
    	bronzeDecPercentage = par("bronzeDecPercentage");

    	changeChunksize = par("changeChunksize");

    	// OM add: vector recording for chunkSize
    	chunkSize_UE1.setName("Chunksize UE1");
    	chunkSize_UE2.setName("Chunksize UE2");
    	chunkSize_UE3.setName("Chunksize UE3");
    	chunkSize_UE4.setName("Chunksize UE4");
    	chunkSize_UE5.setName("Chunksize UE5");
    	chunkSize_UE6.setName("Chunksize UE6");
    	chunkSize_UE7.setName("Chunksize UE7");
    	chunkSize_UE8.setName("Chunksize UE8");
    	chunkSize_UE9.setName("Chunksize UE9");
    	chunkSize_UE10.setName("Chunksize UE10");

    	txRate_UE1.setName("txRate UE1");
    	txRate_UE2.setName("txRate UE2");
    	txRate_UE3.setName("txRate UE3");
    	txRate_UE4.setName("txRate UE4");
    	txRate_UE5.setName("txRate UE5");

    	N1_action_vec.setName("N1_action");
    	N2_action_vec.setName("N2_action");
    	N3_action_vec.setName("N3_action");

    	numN1Gold_vec.setName("numN1Gold");
    	numN2Gold_vec.setName("numN2Gold");
    	numN3Gold_vec.setName("numN3Gold");

    	numN1Silver_vec.setName("numN1Silver");
    	numN2Silver_vec.setName("numN2Silver");
    	numN3Silver_vec.setName("numN3Silver");

    	numN1Bronze_vec.setName("numN1Bronze");
    	numN2Bronze_vec.setName("numN2Bronze");
    	numN3Bronze_vec.setName("numN3Bronze");

    	reward_vec.setName("RL agent Reward");
    	readQmatrixFromFile = par("readQmatrixFromFile");
    	writeQmatrixToFile = par("writeQmatrixToFile");
    	initialPolicy = par("initialPolicy");
    	scheduleRestart_par = par("scheduleRestart");
    	idlePeriod = par("idlePeriod");
    	randomQTable = par("randomQTable");



    	if(readQmatrixFromFile == false)
    	{
    		// Initialize the Q matrix
    		initializeQmatrix();
    	}
    	else
    	{
    		// Read the Q-matrix from file
    		readQmatrix();
    	}


    	int number = 0;

        for (QMatrix::iterator it=qMatrix.begin(); it!=qMatrix.end(); ++it)
        {
            if (it->Qvalue != -(std::numeric_limits<double>::infinity()))
            {
                number++;
            }
        }
        std::cerr << "Number of relevant entries: " << number << endl;

       	WATCH_PTRVECTOR(tcpAppList);
       	WATCH_MAP(appMap);
       	// OM Add 11.02.2013
       	WATCH_LIST(qMatrix);
       	WATCH(agentState);
//       	WATCH_VECTOR(simTimeVec);
//       	WATCH_VECTOR(tcpAppStatVec);
 //      	WATCH_MAP(tcpStatMap);



// Determining the number of TCP application instances and putting them in a pointer vector.
    	cModule *compoundmod = getParentModule(); //get the pointer to the parent module of the FS4VP in order to trace the tcpApp modules

    	// identify the tcp apps and push them to the "vector" list.... NOT NEEDED AT THE MOMENT
    	for (int x=0; x!=numOfTcpApps; x++)
    	{
    		EV <<"The id of TCP App #"<<x<<" is"<<compoundmod->findSubmodule("tcpApp", x) <<endl;
    		tcpAppList.push_back(compoundmod->getSubmodule("tcpApp",x));
    	}
//	DEBUG
//    	for (tcpAppListIt = tcpAppList.begin(); tcpAppListIt < tcpAppList.end(); tcpAppListIt++)
//    	   		EV <<"Iterating thru vector: "<<*tcpAppListIt<<endl;
//	DEBUG
//    	for(int x=0; x<tcpAppList.size(); x++)
//    	{
//    		EV <<"The id of TCP App "<<x<<" is"<< tcpAppList[x]->getId()<<endl;
//
//    	}

       	//================== EXPERIMENTAL (16.07.2012 ==============================

       	cModule *sysmod = getParentModule()->getParentModule();

       	EV<<"The system module is :"<< sysmod->getFullName()<<endl;
       	EV<<"The Client Module Id is: "<< sysmod->findSubmodule("UE_1")<<" and full name is "<<sysmod->getSubmodule("UE_1")->getFullName()<<endl;
       	EV<<"The TCP Host Application is: "<< sysmod->findSubmodule("tcpHostApp", 0)<<endl;

       	//================== EXPERIMENTAL (16.07.2012 ==============================



    	WATCH_PTRMAP(tcpConnMap);

    }
}

void FS4VP::handleMessage(cMessage *msg)
{
	if(msg->getKind()== MK_REGISTER_REQ_MSG) //this message is rxed from the TCP_App when its state changes to ESTABLISH
	{
		EV<<"Request Message Received from Application to Register Context Data. "<<endl;
		registerAppContext(msg); //call function to register the app context data in a map data structure
	}
	else 	if(msg->getKind() == MK_TX_TRIGGER_TIMER)//This is the internal timer message generated to send a tx_trigger to the relevant app
	{
		EV<<"Received triggerMsg - firing tx_triggers to the application modules !" <<endl;
		sendTxTrigger(msg);
	}
	else if(msg->getKind() == MK_VIRT_PLAY_ADV_TIMER)
		virtualPlayAdvance(msg);

	else if(msg->getKind()== MK_CLIENT_STATS)
	{
		EV<<"Received Client Buffer Stats via direct message"<<endl;
		processCliApplicationContextInfo(msg);
	}
	//OM ADD 08.02.2013
	else if(msg->getKind() == MK_START_AGENT_MSG)
	{
		EV <<"Received Client play start via direct message." << endl;
		processCliPlayStart(msg);
	}
	//OM ADD
	else if(msg->getKind() == MK_FINISH_AGENT_MSG)
	{
		EV <<"Received Client play finish via direct message." << endl;
		processCliPlayFinish(msg);
	}
	else if(msg->getKind() == MK_AGENT_TRIGGER_TIMER)
	{
		EV<<"Received agent triggerMsg - updating Q-matrix and selecting actions!" <<endl;

		// Check if there are clients in the system
		bool clients = false;

		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{

			EV<<"DEBUG: KEY-1: "<<it1->first<<" Port_id: "<<(it1->second).connectPortId<<endl;
			EV <<"play buffer state " << (it1->second).playBufferState << endl;
			EV <<"initial play started: " << (it1->second).initialPlayStarted << endl;
			EV << "play finished: " << (it1->second).playFinished << endl;
			if((it1->second).playFinished == false && (it1->second).initialPlayStarted == true)
			{
				clients = true;
				break;

			}

		}

		if(clients == true)
		{
			// Get the actions
			int N1_action;
			int N2_action;
			int N3_action;
			if(simTime() > 0)
			{
				getRLAgentAction(N1_action, N2_action, N3_action);
//				std::cerr << "N1_action: " << N1_action << endl;
//				std::cerr << "N2_action: " << N2_action << endl;
//				std::cerr << "N3_action: " << N3_action << endl;

				// Carry out the actions
				doActions(N1_action, N2_action, N3_action);
			}

			// Schedule another agent timer
			cMessage *agentTimer = new cMessage("agentTimer", MK_AGENT_TRIGGER_TIMER);
			scheduleAt(simTime()+agentControlTime, agentTimer); // schedule it within the control time
		}

		delete (msg);
	}
	else if(msg->getKind() == MK_WRITE_QMATRIX_MSG)
	{
		if(writeQmatrixToFile == true)
		{
			EV <<"Received Write Q-matrix via direct message." << endl;
			writeQmatrix();
		}
		else
		{
			//endSimulation();
		}
		delete msg;
	}

}

//OM Add: 08.02.2013
void FS4VP::processCliPlayStart(cMessage *msg)
{
	EV<<"Processing Client Play start!"<<endl;

	// Update the AppContextDataMap that the client play has started
	CliAppContext *cliAppContext = check_and_cast<CliAppContext*> (msg->removeControlInfo());
	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
	{
	//	EV<<"DEBUG: KEY-1: "<<it1->first<<" Port_id: "<<(it1->second).connectPortId<<endl;
		if((it1->second).connectPortId == cliAppContext->getPortId())
		{
			(it1->second).initialPlayStarted = true;
			(it1->second).userClass = cliAppContext->getUserClass();
			//(it1->second).playBufferState2 = printBufferState(cliAppContext->getPlayBufferState());
			break;
		}

	}

	// If learning is enabled Check if the agent has started and generate a RL agent timer
	if(learningEnabled == true)
	{
		if(rlAgentStarted == false)
		{
			EV << "Agent has not started yet, starting the timer." << endl;
			//Creating a trigger timer for the RL agent
			cMessage *agentTimer = new cMessage("agentTimer", MK_AGENT_TRIGGER_TIMER);
			scheduleAt(simTime(), agentTimer); // schedule it immediately
			rlAgentStarted = true; // set the agent as started
		}
	}

	delete(cliAppContext);
	delete (msg);
}

//OM Add
void FS4VP::processCliPlayFinish(cMessage *msg)
{
	EV<<"Processing Client Play finish!"<<endl;

	// Update the AppContextDataMap that the client play has started
	CliAppContext *cliAppContext = check_and_cast<CliAppContext*> (msg->removeControlInfo());
	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
	{
	//	EV<<"DEBUG: KEY-1: "<<it1->first<<" Port_id: "<<(it1->second).connectPortId<<endl;
		if((it1->second).connectPortId == cliAppContext->getPortId())
		{
			(it1->second).playFinished = true;
			break;
		}

	}

	delete(cliAppContext);
	delete (msg);
}

void FS4VP::processCliApplicationContextInfo(cMessage *msg)
{
	EV<<"Processing Client Application Context Info!"<<endl;
	CliAppContext *cliAppContext = check_and_cast<CliAppContext*> (msg->removeControlInfo());
		//EV<<"The Size of the Play Buffer of UE with ID: "<< cliContextInfo->getSenderModuleId() <<" is: "<<(long)cliContextInfo->par("bytesRxed")<<" segments!"<<endl;
	EV<<"The following Client stats are received: "<<endl; //DEBUG Statement
	EV<<"Port ID: "<< cliAppContext->getPortId()<<endl; //DEBUG Statement
	EV<<"PlayBuffer Size: "<< cliAppContext->getPlayBufferSize()<<endl; //DEBUG Statement
	EV<<"Play Buffer Status: "<<cliAppContext->getPlayBufferState()<<endl; //DEBUG Statement

//	std::cerr << "simtime: " << simTime() << endl;
//	std::cerr <<"The following Client stats are received: "<<endl; //DEBUG Statement
//	std::cerr <<"Port ID: "<< cliAppContext->getPortId()<<endl; //DEBUG Statement
//	std::cerr <<"PlayBuffer Size: "<< cliAppContext->getPlayBufferSize()<<endl; //DEBUG Statement
//	std::cerr <<"Play Buffer Status: "<<cliAppContext->getPlayBufferState()<<endl; //DEBUG Statement
//	std::cerr << " " << endl;

	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
	{
	//	EV<<"DEBUG: KEY-1: "<<it1->first<<" Port_id: "<<(it1->second).connectPortId<<endl;
		if((it1->second).connectPortId == cliAppContext->getPortId())
		{
			(it1->second).playBufferState = cliAppContext->getPlayBufferState();
			//(it1->second).playBufferState2 = printBufferState(cliAppContext->getPlayBufferState());
			break;
		}

	}
	delete(cliAppContext);
	delete (msg);
}

//char* FS4VP::printBufferState(int _cliBufferState)
//{
//	switch(_cliBufferState)
//	{
//	case 0: return "RED";
//	case 1:	return "ORANGE";
//	case 2: return "YELLOW";
//	case 3: return "GREEN";
//	default: return "STATE_NOT_KNOWN";
//	}
//}

void FS4VP::virtualPlayAdvance(cMessage *msg)
{
	EV<<"Virtual Play Advance Timer message received for application index Id: "<<(long)msg->par("appIndex")<<" at time "<<simTime()<<endl;
	EV<<"Play Rate is:"<<floor(alpha)<<endl;
	/**
	 * 1. get the app index id
	 * 1.a: using the index, get the codec-rate
	 * 1.b: get the mss (its a global variable set to me equal to the mss of TCP layer)
	 * 1.c: calculate the pseuodPlayBackRate = mss/codec-rate
	 *
	 * 2. using the index as the key, search the corresponding entry in the appMap
	 * 3. get the transmit frequency (f) (i.e., Tx-Rate) for the corresponding app instance
	 * 4. get the chunk size (c) "in bits" for the corresponding app instance
	 * 5. calculate the transmit interval (i.e., c/f)
	 * 6. get the mss from the TCP layer. At the moment just declare it as a static value.
	 * 6. calculate the playrate (mss and nextPlayEvent and reschedule the playAdvanceTimer to that time
	 */
	//experimenting with map and data acquisiton from map based on key value
	//appMapIt = appMap.find((long)msg->par("appIndex"));
	double userCodec = appMap.find((long)msg->par("appIndex"))->second.codecRate;
	EV<<"The codec value is "<< userCodec<<endl;

	double playRate = (mss*8)/(userCodec); //calculating the play rate of the relevant UE
	EV<<"Virtual PlayTimer value is:"<<playRate<<" sec."<<endl;

	double estPlayBufferSize = appMap.find((long)msg->par("appIndex"))->second.playoutBufferSize;
	EV<<"Estimated Playout Buffer Size is: "<<estPlayBufferSize<<" bits"<<endl;

	scheduleAt(simTime()+ playRate, msg);

}
/*
 ** This function registers the application context received from the respective application modules
 ** to the Application Map. This function gets invoked when the module recieves a registeration message
 ** from an application module.
 */
void FS4VP::registerAppContext(cMessage *msg)
{
	AppContext *appCi =check_and_cast<AppContext*> (msg->removeControlInfo()); //removing the app context data from the registration request message
	EV<<"Registration request message from an application module."<<endl;// with id: "<< appCi->getAppId()<<" index: "<< appCi->getAppIndex()<<" and filesize: "<< appCi->getFileSize()<<endl;
	EV<<"Registration request message from an application module with id: "<< appCi->getAppId()<<" index: "<< appCi->getAppIndex()<<" filesize: "<< appCi->getFileSize()<<" and Tx-window: "<< appCi->getTxDuration()<<endl;
	ContextInfoStruct &appContext = appMap[appCi->getAppIndex()]; //assigning a key to the context reference
	appContext.appId = appCi->getAppId();
	appContext.connectPortId = appCi->getPortId();
	appContext.fileSize = appCi->getFileSize();
	appContext.remainingBytes = appCi->getFileSize();
	appContext.txRate = appCi->getTxDuration();
	appContext.chunkSize = chunkSize; //appCi->getChunkSize(); //TODO: this parameter has to be dynamically calculated for each app.
	appContext.codecRate = appCi->getCodecRate();
	appContext.initialPlayStarted = false; // OM Add 11.0.2013
	appContext.playFinished = false; // OM add
	appContext.origTxRate = appCi->getTxDuration(); // OM Add
	//appContext.chunkSize = appCi->getChunkSize();

	//int *appIndex = &(appCi->getAppIndex());

	//============== EXPERIMENTAL CODE: START (July) ==============================================
	//initializing the map for collecting tcp statistics
	EV<<"Initializing the statistics map for app index:"<<appCi->getAppIndex()<<endl;
	ApplicationStatStruct &appStats = tcpStatMap[appCi->getAppIndex()];
	appStats.simTimeVec.push_back(simTime());
	appStats.tcpAppStatVec.push_back(0);

	TCPStatMap::iterator statMapIt;

	for(statMapIt = tcpStatMap.begin(); statMapIt != tcpStatMap.end(); ++statMapIt)
	{
		EV<<"STAT MAP STATS:"<<endl;
	}

	//============== EXPERIMENTAL CODE: END (July) ==============================================

	delete(msg);  //deleting the registration request message
	//delete(appCi); //deleting the app context data appended to the registration request message

	//=============================== PER APP TIMER ROUTINE : START ================================================
	//Creating a new trigger timer for each app that registers with the FS4VP module, and this trigger_timer will carry the necessary context info of the app.
	cMessage *triggerTimer = new cMessage("initialTriggerTimer", MK_TX_TRIGGER_TIMER);
	triggerTimer->setContextPointer(appCi); //appending the AppContext Info with the timer trigger.

	//create a timer message that will be used to run a virtual play function for each application
	cMessage *virtualPlayAdvanceTimer = new cMessage("playAdvacnetimer", MK_VIRT_PLAY_ADV_TIMER); //ZY (11.06.2012)

	virtualPlayAdvanceTimer -> addPar("appIndex").setLongValue(appCi->getAppIndex());
	//virtualPlayAdvanceTimer -> setContextPointer(appCi);
	//testing the virtualPlayAdvanceTimer
	scheduleAt(simTime(), virtualPlayAdvanceTimer);

	simtime_t nextScheduledTime = appCi->getTxDuration(); //determine the time to schedule the timer
	scheduleAt(simTime() + nextScheduledTime, triggerTimer); //scheduling the app trigger timer


	//=============================== PER APP TIMER  ROUTINE : END =================================================

//=============================== SINGLE TIMER FOR ALL APPS ROUTINE : START =================================================
	//check if all the apps have been registered. Only those apps will register that have an ESTABLSIHED TCP cx state.
	// The initial tx trigger timer will be created after all the apps have been registered.
	// NOTE: This is a quick-fix but later the generation of tx-trigger_timer has to be decoupled from this condition,
	// as it may be that a TCP app may be present but not connected to any cli.
	// TODO
//	if(appMap.size() == numOfTcpApps)
//	{
//    	cMessage *triggerTimer = new cMessage("initialTriggerTimer", MK_TX_TRIGGER_TIMER);
//    	scheduleAt(simTime(), triggerTimer);
//	}
//=============================== SINGLE TIMER FOR ALL APPS ROUTINE : END =================================================

	return;
}
/*
 ** This function sends a trigger to the corresponding application modules.
 */
void FS4VP::sendTxTrigger(cMessage *triggerTimer)
{
	AppContext *timerContext = (AppContext *)triggerTimer->getContextPointer();
	EV<<"Triggering App Module Id: "<<timerContext->getAppId()<<" App_Index: "<<timerContext->getAppIndex()<<" to transmit chunk Size: "<< chunkSize <<endl; //timerContext->getChunkSize()<<endl;

//	if(timerContext->getRemainingFileSize()!=0)

		AppContextDataMap::iterator it = appMap.find(timerContext->getAppIndex());
		if((it->second).remainingBytes != 0)
		{

			EV<<"Creating an app-polling trigger msg !!" <<endl;
			cMessage *txTrigger2App = new cMessage("triggerMsg", MK_TRIGGER_MSG); //NOTE: this is not a self-message and is sent (via sendDirect()) to the app module.

			//initializing context information
			ci = new ContextInformation();
			ci -> setBytesToSend((it->second).remainingBytes);
			ci -> setChunkSize((it->second).chunkSize);
//			ci -> setChunkSize(timerContext->getChunkSize());


			//ci -> setTxRate(timerContext->getTxDuration()); //the time for which the application is allowed to transmit chunks
			ci -> setTxRate((it->second).txRate); //the time for which the application is allowed to transmit chunks
			ci -> setIatChunk(iatChunk);

			// ChunkSize vector recording.
			// TODO: use parameters
			if((it->second).connectPortId == 1000)
			{
				chunkSize_UE1.record((it->second).chunkSize);
				txRate_UE1.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1001)
			{
				chunkSize_UE2.record((it->second).chunkSize);
				txRate_UE2.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1002)
			{
				chunkSize_UE3.record((it->second).chunkSize);
				txRate_UE3.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1003)
			{
				chunkSize_UE4.record((it->second).chunkSize);
				txRate_UE4.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1004)
			{
				chunkSize_UE5.record((it->second).chunkSize);
				txRate_UE5.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1005)
			{
				chunkSize_UE6.record((it->second).chunkSize);
				//txRate_UE5.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1006)
			{
				chunkSize_UE7.record((it->second).chunkSize);
				//txRate_UE5.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1007)
			{
				chunkSize_UE8.record((it->second).chunkSize);
				//txRate_UE5.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1008)
			{
				chunkSize_UE9.record((it->second).chunkSize);
				//txRate_UE5.record((it->second).txRate);
			}
			else if((it->second).connectPortId == 1009)
			{
				chunkSize_UE10.record((it->second).chunkSize);
				//txRate_UE5.record((it->second).txRate);
			}

			//std::cerr <<"Sending chunk Size: "<< (it->second).chunkSize <<endl;

			//appending context info to the app-polling trigger message
			txTrigger2App->setControlInfo(ci);

			EV<<"Txing via the sendDirect() to TCP App "<< timerContext->getAppIndex() <<endl;
			sendDirect(txTrigger2App,tcpAppList[timerContext->getAppIndex()],"directInput");
			(it->second).remainingBytes -= (it->second).chunkSize; //timerContext->getChunkSize(); //decrement the remaining number of bytes
			(it->second).playoutBufferSize += ((it->second).chunkSize)*8; //updating the playbuffer size in bits

			EV<<"The Remaining Filesize is"<<(it->second).remainingBytes<<endl;
			EV<<"Total Bits sent: "<<(it->second).playoutBufferSize<<endl;


			//re-scheduling the trigger_timer
			if((it->second).remainingBytes !=0 && (it->second).remainingBytes > 0)
			{
				//scheduleAt(simTime() + timerContext->getTxDuration(), triggerTimer);

				//Set a new txTrigger based on the chunksize:
				//(it->second).txRate = ((it->second).chunkSize * 8) / ((it->second).codecRate + 0.005 * (it->second).codecRate);

				scheduleAt(simTime() + (it->second).txRate, triggerTimer);
			}
			else if((it->second).remainingBytes < 0)
			{
				EV<<"No more bytes to send - canceling the triggerTimer for App_id: "<<timerContext->getAppIndex()<<endl;

				std::cerr <<"No more bytes to send: " << simTime() << " - canceling the triggerTimer for App_id: "<<timerContext->getAppIndex()<<endl;



				// Generate a cMessage for starting the sending again
				//if(scheduleRestart_par == true && (it->second).userClass == GOLD)
				if(scheduleRestart_par == true && ((it->second).connectPortId == 1000 || (it->second).connectPortId == 1001 || (it->second).connectPortId == 1002))
				{

					CliAppContext *cliAppContext;
					cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
					cliAppContext -> setPortId((it->second).connectPortId);

					scheduleRestart(cliAppContext, triggerTimer);


				}
				else
				{

					// OM add: generate a cMessage for informing the agent that the content has been delivered
					cMessage *finishAgent = new cMessage("finishAgent", MK_FINISH_AGENT_MSG);
					EV <<"Informing the FS4VP module that client's content has been sent. " << endl;
					CliAppContext *cliAppContext;
					cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
					cliAppContext -> setPortId((it->second).connectPortId);
					finishAgent->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
					scheduleAt(simTime(), finishAgent);

					cancelAndDelete(triggerTimer);
				}


			}

		}


//=================================NEW CODE ABOVE ===============================
//		AppContextDataMap::iterator it = appMap.find(polledAppIndex);
////		ContextInfoStruct & ciStruct = it->second;
////		EV<<"The tcpApp["<<polledAppIndex<<"] has App_ID: "<<ciStruct.appId<<endl;
//		EV<<"The tcpApp["<<polledAppIndex<<"] has "<<(it->second).remainingBytes<<" remaining bytes"<<endl;
//
//		if((polledAppIndex <= numOfTcpApps-1) && ((it->second).remainingBytes != 0))
//		{
//			EV<<"Creating an app-polling trigger msg !!" <<endl;
//			cMessage *txTrigger = new cMessage("triggerMsg", MK_TRIGGER_MSG); //NOTE: this is not a self-message and is sent (via sendDirect()) to the app module.
//
//			//initializing context information
//			ci = new ContextInformation();
//			ci -> setBytesToSend((it->second).remainingBytes);
//			ci -> setChunkSize(chunkSize);
//			ci -> setTxRate(txWindow); //the time for which the application is allowed to transmit chunks
//			ci -> setIatChunk(iatChunk);
//			//appending context info to the app-polling trigger message
//			txTrigger->setControlInfo(ci);
//
//			EV<<"Txing via the sendDirect() to TCP App "<< polledAppIndex <<endl;
//			sendDirect(txTrigger,tcpAppList[polledAppIndex],"directInput");
//
//			(it->second).remainingBytes -= bytesToSend; //decrement the remaining number of bytes
//			EV<<"The Remaining Filesize is"<<(it->second).remainingBytes<<endl;
//
//			polledAppIndex++; //increment the flag to select the next app in the vector list to transmit.
//			if(polledAppIndex > numOfTcpApps-1)
//			{
//				EV <<"Resetting the TCP App Flag from: "<< polledAppIndex <<endl;
//				polledAppIndex = 0; //reset the flag and return to app[0]
//			}
//
//			EV<<"Scheduling next Tx Timer !!"<<endl;
//			scheduleAt(simTime()+ txWindow, triggerTimer); //re-initiate the trigger-timer self-message
//		}
//		else //if remaining bytes are zero then the next app module should be polled.
//		{
//
//			polledAppIndex++; //increment the flag to select the next app in the vector list to transmit.
//			if(polledAppIndex > numOfTcpApps-1) //check if the polled app is not greater than the available number of apps
//				{
//					EV <<"Resetting the TCP App Flag from: "<< polledAppIndex <<endl;
//					polledAppIndex = 0; //reset the flag and return to app[0]
//				}
//			//if((polledAppIndex <= numOfTcpApps-1) && ((it->second).remainingBytes != 0))
//
//		//		{
//					EV<<"Scheduling next Tx Timer !!"<<endl;
//					scheduleAt(simTime()+ txWindow, triggerTimer); //re-initiate the trigger-timer self-message
//			//	}
//
//		}

}

void FS4VP::getRLAgentAction(int &N1_action, int &N2_action, int &N3_action)
{
	// Previous state
	EV << "Previous state and actions selected: " << endl;
	EV << "N1_portion: " << agentState.N1_fraction << endl;
	EV << "N2_portion: " << agentState.N2_fraction << endl;
	EV << "N3_portion: " << agentState.N3_fraction << endl;
	EV << "N1_action: " << agentState.N1_action << endl;
	EV << "N2_action: " << agentState.N2_action << endl;
	EV << "N3_action: " << agentState.N3_action << endl;


	// First action to be selected by the agent
	if((agentState.N1_fraction == -1) && (agentState.N2_fraction == -1 ) &&
			(agentState.N3_fraction == -1) && (agentState.N1_action == -1) && (agentState.N2_action == -1) && (agentState.N3_action == -1))
	{


		if(initialPolicy == false)
		{
			int N3action = intuniform(0,2);
			agentState.N1_action = nochange;
			agentState.N2_action = nochange;

			if(N3action == 0)
				agentState.N3_action = nochange;
			else if(N3action == 1)
				agentState.N3_action = increase;
			else if(N3action == 2)
				agentState.N3_action = decrease;
		}
		else
		{
			// Initially do not change anything
			agentState.N1_action = nochange;
		    agentState.N2_action = nochange;
		    agentState.N3_action = nochange;
		}


	    // Update agent state
		updateAgentState();

		EV << "NEW state and actions selected: " << endl;
		EV << "N1_portion: " << agentState.N1_fraction << endl;
		EV << "N2_portion: " << agentState.N2_fraction << endl;
		EV << "N3_portion: " << agentState.N3_fraction << endl;
		EV << "N1_action: " << agentState.N1_action << endl;
		EV << "N2_action: " << agentState.N2_action << endl;
		EV << "N3_action: " << agentState.N3_action << endl;
	}

	else
	{

		// 1. get the qInfo of the last state
		QInfo *qInfo = lookupQInfo(agentState.N1_fraction, agentState.N2_fraction, agentState.N3_fraction, agentState.N1_action, agentState.N2_action, agentState.N3_action);

		if(qInfo == 0)
		{
			std::cerr << "N1_portion: " << agentState.N1_fraction << endl;
			std::cerr << "N2_portion: " << agentState.N2_fraction << endl;
			std::cerr << "N3_portion: " << agentState.N3_fraction << endl;
			std::cerr << "N1_action: " << agentState.N1_action << endl;
			std::cerr << "N2_action: " << agentState.N2_action << endl;
			std::cerr << "N3_action: " << agentState.N3_action << endl;
			opp_error("Error! qInfo not found by the RL agent");
		}

		// Max future value if action is chosen: Q(next state, all actions)
		// action set: increase, nochange, decrease
		//TODO: how to know the next state? next state could be all possible states?
		double reward = 0.0;
		reward = getInstantReward();

		reward_vec.record(reward);

		std::cerr << "NAME: " << getParentModule()->getName() << endl;
		std::cerr << "simTime: " << simTime() << endl;
		std::cerr << "instant reward: " << reward << endl;

		double maxFuture = 0.0;
		int futureActionN1, futureActionN2, futureActionN3;

		EV << "MAX FUTURE: " << endl;
		getMaxFuture(agentState.N1_fraction, agentState.N2_fraction, agentState.N3_fraction, maxFuture, futureActionN1, futureActionN2, futureActionN3);

		EV << "Old Q value: " << qInfo->Qvalue << endl;
		EV << "instant reward received from last move: " << reward << endl;
		EV << "max future Q value: " << maxFuture << endl;
		EV << "max future action N1: " << futureActionN1 << endl;
		EV << "max future action N2: " << futureActionN2 << endl;
		EV << "max future action N3: " << futureActionN3 << endl;

		//update transfer Q value based on the instant reward that was retrieved from previous action
		// Q-value --> estimated reward
		// new Q value = (1-alpha) * old Q value + alpha * ( instantReward + gamma * (max future value if action is chosen))
		qInfo->Qvalue = (1-alphaRL) * qInfo->Qvalue + alphaRL * (reward + gamma * maxFuture);

		EV << "New Q value: " << qInfo->Qvalue << endl;

		std::cerr << "New Q value: " << qInfo->Qvalue << endl;


		//Update agent state
		updateAgentState();

		// Select NEW actions
		int actionN1, actionN2, actionN3;
		selectAction(actionN1, actionN2, actionN3);

		agentState.N1_action = actionN1;
	    agentState.N2_action = actionN2;
	    agentState.N3_action = actionN3;

		EV << "NEW state and actions selected: " << endl;
		EV << "N1_portion: " << agentState.N1_fraction << endl;
		EV << "N2_portion: " << agentState.N2_fraction << endl;
		EV << "N3_portion: " << agentState.N3_fraction << endl;
		EV << "N1_action: " << agentState.N1_action << endl;
		EV << "N2_action: " << agentState.N2_action << endl;
		EV << "N3_action: " << agentState.N3_action << endl;

	}

	// Return values
	N1_action = agentState.N1_action;
	N2_action = agentState.N2_action;
	N3_action = agentState.N3_action;

	// N1_action recording
	if(N1_action == decrease)
	{
		N1_action_vec.record(-1);
	}
	else if (N1_action == nochange)
	{
		N1_action_vec.record(0);
	}
	else if (N1_action == increase)
	{
		N1_action_vec.record(1);
	}

	// N2_action recording
	if(N2_action == decrease)
	{
		N2_action_vec.record(-1);
	}
	else if (N2_action == nochange)
	{
		N2_action_vec.record(0);
	}
	else if (N2_action == increase)
	{
		N2_action_vec.record(1);
	}

	// N3_action recording
	if(N3_action == decrease)
	{
		N3_action_vec.record(-1);
	}
	else if (N3_action == nochange)
	{
		N3_action_vec.record(0);
	}
	else if (N3_action == increase)
	{
		N3_action_vec.record(1);
	}



	return;
}

//Looks up qInfo from the Q-matrix
FS4VP::QInfo *FS4VP::lookupQInfo(int N1_fraction, int N2_fraction, int N3_fraction, int N1_action, int N2_action, int N3_action)
{

    for (QMatrix::iterator it=qMatrix.begin(); it!=qMatrix.end(); ++it)
    {
        if (it->N1_fraction == N1_fraction && it->N2_fraction == N2_fraction && it->N3_fraction == N3_fraction && it->N1_action == N1_action && it->N2_action == N2_action && it->N3_action == N3_action)
        {
            return &(*it);
        }
    }
    return NULL;
}

void FS4VP::getMaxFuture(int N1_fraction, int N2_fraction, int N3_fraction, double &maxFuture, int &futureActionN1, int &futureActionN2, int &futureActionN3)
{

	std::vector <double> qVector;
	double candidateValue = -1;
	double qvalue = 0.0;
	int maxFutureActionN1;
	int maxFutureActionN2;
	int maxFutureActionN3;
	QInfo *futureQ;

    // 27 action permutations
	// Find the action set with the highest Q-value

	for(int N1_action = 0; N1_action < countActions; N1_action++)
	{
		for(int N2_action = 0; N2_action < countActions; N2_action++)
		{
			for(int N3_action = 0; N3_action < countActions; N3_action++)
			{
				futureQ = lookupQInfo(N1_fraction, N2_fraction, N3_fraction, N1_action, N2_action, N3_action);
				qvalue = futureQ->Qvalue;
				if(candidateValue == -1 || qvalue > candidateValue)
				{
					candidateValue = qvalue;
					maxFutureActionN1 = N1_action;
					maxFutureActionN2 = N2_action;
					maxFutureActionN3 = N3_action;
				}
			}
		}
	}


	//Searching the whole Q-matrix

//    for (QMatrix::iterator it=qMatrix.begin(); it!=qMatrix.end(); ++it)
//    {
//    	qvalue = it->Qvalue;
//		if(candidateValue == -1 || qvalue > candidateValue)
//		{
//			candidateValue = qvalue;
//			maxFutureActionN1 = it->N1_action;
//			maxFutureActionN2 = it->N2_action;
//			maxFutureActionN3 = it->N3_action;
//		}
//
//    }



//	for(int N1_frac = 0; N1_frac < countFractionSpace; N1_frac++)
//	{
//		for(int N2_frac = 0; N2_frac < countFractionSpace; N2_frac++)
//		{
//			for(int N3_frac = 0; N3_frac < countFractionSpace; N3_frac++)
//			{
//				for(int N1_action = 0; N1_action < countActions; N1_action++)
//				{
//					for(int N2_action = 0; N2_action < countActions; N2_action++)
//					{
//						for(int N3_action = 0; N3_action < countActions; N3_action++)
//						{
//							futureQ = lookupQInfo(N1_frac, N2_frac, N3_frac, N1_action, N2_action, N3_action);
//							qvalue = futureQ->Qvalue;
//							if(candidateValue == -1 || qvalue > candidateValue)
//							{
//								candidateValue = qvalue;
//								maxFutureActionN1 = N1_action;
//								maxFutureActionN2 = N2_action;
//								maxFutureActionN3 = N3_action;
//							}
//						}
//					}
//				}
//			}
//		}
//	}

	// Return values
	maxFuture = candidateValue;
	futureActionN1 = maxFutureActionN1;
	futureActionN2 = maxFutureActionN2;
	futureActionN3 = maxFutureActionN3;



	return;
}
/* Function that gets the instant reward
 * based on the buffer states of the users
 *
 */

double FS4VP::getInstantReward()
{
	double reward = 0.0;

	// Num of total users
	int numUsers = 0;

	// Gold users
	int numN1Gold = 0;
	int numN2Gold = 0;
	int numN3Gold = 0;

	double percN1Gold = 0.0;
	double percN2Gold = 0.0;
	double percN3Gold = 0.0;

	// Silver users
	int numN1Silver = 0;
	int numN2Silver = 0;
	int numN3Silver = 0;

	double percN1Silver = 0.0;
	double percN2Silver = 0.0;
	double percN3Silver = 0.0;

	// Bronze users
	int numN1Bronze = 0;
	int numN2Bronze = 0;
	int numN3Bronze = 0;

	double percN1Bronze = 0.0;
	double percN2Bronze = 0.0;
	double percN3Bronze = 0.0;

	int state;
	int userClass;

	// Get amount of users in different zone and class
	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
	{
		// Check that the initial play has started and play has not finished
		if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
		{
			numUsers++; // Count the total number of users
			state = (it1->second).playBufferState; // Get the playBuffer state
			userClass = (it1->second).userClass; // Get the user class

			if (userClass == GOLD)
			{
				//RED = N1
				if(state == 0)
				{
					numN1Gold++;
				}
				//ORANGE = N1
				else if(state == 1)
				{
					numN1Gold++;
				}
				//YELLOW = N2
				else if(state == 2)
				{
					numN2Gold++;
				}
				//GREEN = N3
				else if(state == 3)
				{
					numN3Gold++;
				}
			}
			else if (userClass == SILVER)
			{
				//RED = N1
				if(state == 0)
				{
					numN1Silver++;
				}
				//ORANGE = N1
				else if(state == 1)
				{
					numN1Silver++;
				}
				//YELLOW = N2
				else if(state == 2)
				{
					numN2Silver++;
				}
				//GREEN = N3
				else if(state == 3)
				{
					numN3Silver++;
				}
			}
			else if(userClass == BRONZE)
			{
				//RED = N1
				if(state == 0)
				{
					numN1Bronze++;
				}
				//ORANGE = N1
				else if(state == 1)
				{
					numN1Bronze++;
				}
				//YELLOW = N2
				else if(state == 2)
				{
					numN2Bronze++;
				}
				//GREEN = N3
				else if(state == 3)
				{
					numN3Bronze++;
				}
			}
		}
	}

	//Get reward based on percentages of user classes

	percN1Gold = (double)numN1Gold/numUsers;
	percN2Gold = (double)numN2Gold/numUsers;
	percN3Gold = (double)numN3Gold/numUsers;

	percN1Silver = (double)numN1Silver/numUsers;
	percN2Silver = (double)numN2Silver/numUsers;
	percN3Silver = (double)numN3Silver/numUsers;

	percN1Bronze = (double)numN1Bronze/numUsers;
	percN2Bronze = (double)numN2Bronze/numUsers;
	percN3Bronze = (double)numN3Bronze/numUsers;

	numN1Gold_vec.record(numN1Gold);
	numN2Gold_vec.record(numN2Gold);
	numN3Gold_vec.record(numN3Gold);

	numN1Silver_vec.record(numN1Silver);
	numN2Silver_vec.record(numN2Silver);
	numN3Silver_vec.record(numN3Silver);

	numN1Bronze_vec.record(numN1Bronze);
	numN2Bronze_vec.record(numN2Bronze);
	numN3Bronze_vec.record(numN3Bronze);



//	if(numN1Gold != 0)
//	{
//		percN1Gold = 1;
//	}
//	else
//	{
//		percN1Gold = 0;
//	}
//
//	if(numN2Gold != 0)
//	{
//		percN2Gold = 1;
//	}
//	else
//	{
//		percN2Gold = 0;
//	}
//
//	if(numN3Gold != 0)
//	{
//		percN3Gold = 1;
//	}
//	else
//	{
//		percN3Gold = 0;
//	}


	reward = -uN1Gold * percN1Gold  + uN2Gold * percN2Gold + uN3Gold * percN3Gold
			-uN1Silver * percN1Silver + uN2Silver * percN2Silver + uN3Silver * percN3Silver
			- uN1Bronze * percN1Bronze + uN2Bronze * percN2Bronze + uN3Bronze * percN3Bronze;

//	// Reward function: weighted sum
//	reward = -uN1Gold * numN1Gold + uN2Gold * numN2Gold + uN3Gold * numN3Gold
//			-uN1Silver * numN1Silver + uN2Silver * numN2Silver + uN3Silver * numN3Silver
//			- uN1Bronze * numN1Bronze + uN2Bronze * numN2Bronze + uN3Bronze * numN3Bronze;

	std::cerr << "NumN1Gold: " << numN1Gold << ", NumN2Gold: " << numN2Gold << ", numN3Gold: " << numN3Gold << endl;
	std::cerr << "NumN1Silver: " << numN1Silver << ", NumN2Silver: " << numN2Silver << ", numN3Silver: " << numN3Silver << endl;
	std::cerr << "NumN1Bronze: " << numN1Bronze << ", NumN2Bronze: " << numN2Bronze << ", numN3Bronze: " << numN3Bronze << endl;
	//std::cerr << "Number of active users: " << numUsers << endl;



	return reward;

}

/* Function that updates the agent state
 *
 */

void FS4VP::updateAgentState()
{

	int numUsers = 0;
	int state;
	int numN1 = 0;
	int numN2 = 0;
	int numN3 = 0;
	double N1_fraction = 0.0;
	double N2_fraction = 0.0;
	double N3_fraction = 0.0;

	// Get amount of users in different zone
	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
	{
		// Check that the initial play has started and play has not finished
		if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
		{
			numUsers++; // Count the total number of users
			state = (it1->second).playBufferState; // Get the playBuffer state

			//RED = N1
			if(state == 0)
			{
				numN1++;
			}
			//ORANGE = N1
			else if(state == 1)
			{
				numN1++;
			}
			//YELLOW = N2
			else if(state == 2)
			{
				numN2++;
			}
			//GREEN = N3
			else if(state == 3)
			{
				numN3++;
			}
		}
	}
	N1_fraction = (double) numN1/numUsers;
	N2_fraction = (double) numN2/numUsers;
	N3_fraction = (double) numN3/numUsers;

	double sum = 0.0;
	sum = N1_fraction + N2_fraction + N3_fraction;

//	std::cerr << "simTime: " << simTime() << endl;
//	std::cerr << "N1_fraction: " << N1_fraction << endl;
//	std::cerr << "N2_fraction: " << N2_fraction << endl;
//	std::cerr << "N3_fraction: " << N3_fraction << endl;


	if(sum != 1)
	{
		std::cerr << "N1_fraction: " << N1_fraction << endl;
		std::cerr << "N2_fraction: " << N2_fraction << endl;
		std::cerr << "N3_fraction: " << N3_fraction << endl;
		std::cerr << "sum: " << sum << endl;
		error("N1_fraction + N2_fraction + N3_fraction != 1");
	}

	// Update new values

	// N1 fraction
	if(N1_fraction == 0)
	{
		agentState.N1_fraction = zero;
	}
	else if(N1_fraction > 0 && N1_fraction <= 0.25)
	{
		agentState.N1_fraction = zeroTo25;
	}
	else if (N1_fraction > 0.25 && N1_fraction <= 0.50)
	{
		agentState.N1_fraction = Twenty5ToFifty;
	}
	else if (N1_fraction > 0.50 && N1_fraction <= 0.75)
	{
		agentState.N1_fraction = FiftyTo75;
	}
	else if (N1_fraction > 0.75 && N1_fraction <= 1)
	{
		agentState.N1_fraction = Seventy5To100;
	}

	// N2 fraction
	if(N2_fraction == 0)
	{
		agentState.N2_fraction = zero;
	}
	else if(N2_fraction > 0 && N2_fraction <= 0.25)
	{
		agentState.N2_fraction = zeroTo25;
	}
	else if (N2_fraction > 0.25 && N2_fraction <= 0.50)
	{
		agentState.N2_fraction = Twenty5ToFifty;
	}
	else if (N2_fraction > 0.50 && N2_fraction <= 0.75)
	{
		agentState.N2_fraction = FiftyTo75;
	}
	else if (N2_fraction > 0.75 && N2_fraction <= 1)
	{
		agentState.N2_fraction = Seventy5To100;
	}

	// N3 fraction
	if(N3_fraction == 0)
	{
		agentState.N3_fraction = zero;
	}
	else if(N3_fraction > 0 && N3_fraction <= 0.25)
	{
		agentState.N3_fraction = zeroTo25;
	}
	else if (N3_fraction > 0.25 && N3_fraction <= 0.50)
	{
		agentState.N3_fraction = Twenty5ToFifty;
	}
	else if (N3_fraction > 0.50 && N3_fraction <= 0.75)
	{
		agentState.N3_fraction = FiftyTo75;
	}
	else if (N3_fraction > 0.75 && N3_fraction <= 1)
	{
		agentState.N3_fraction = Seventy5To100;
	}

}

/* Does the actual action selection
 * Based on the Boltzmann exploration
 */

void FS4VP::selectAction(int &actionN1, int &actionN2, int &actionN3)
{
	QInfo *qInfo;
	double probability;
	double sum = 0.0;
	double value = 0.0;
	//double beeta = 0.8;

	int N1_fraction = agentState.N1_fraction;
	int N2_fraction = agentState.N2_fraction;
	int N3_fraction = agentState.N3_fraction;



 	ProbList probList; // a list containing the possible action permutations and their probabilities

 	//TODO: use Q-matrix instead
 	// Go through different set of actions to compute the sum: exp(beeta * qvalue) for all possible actions
	for(int N1_action = 0; N1_action < countActions; N1_action++)
	{
		for(int N2_action = 0; N2_action < countActions; N2_action++)
		{
			for(int N3_action = 0; N3_action < countActions; N3_action++)
			{
				value = 0.0;
				qInfo = lookupQInfo(N1_fraction, N2_fraction, N3_fraction, N1_action, N2_action, N3_action);
				value = exp(beeta * (qInfo->Qvalue));
				sum += value;
//				std::cerr << "Action set: " << N1_action << ", " << N2_action << ", " << N3_action << endl;
//				std::cerr << "value: " << value << endl;


			}
		}
	}
	//std::cerr << "sum: " << sum << endl;


	// Calculate probability for each action set and add it into the linked list
	for(int N1_action = 0; N1_action < countActions; N1_action++)
	{
		for(int N2_action = 0; N2_action < countActions; N2_action++)
		{
			for(int N3_action = 0; N3_action < countActions; N3_action++)
			{
				value = 0.0;
				probability = 0.0;
				qInfo = lookupQInfo(N1_fraction, N2_fraction, N3_fraction, N1_action, N2_action, N3_action);
				value = exp(beeta * (qInfo->Qvalue));
				probability = value / sum;

//				std::cerr << "value: " << value << endl;
//				std::cerr << "sum: " << sum << endl;
//				std::cerr << "Action set: " << N1_action << ", " << N2_action << ", " << N3_action << endl;
//				std::cerr << "qInfo->Qvalue: " << qInfo->Qvalue << endl;
//				std::cerr << "probability: " << probability << endl;

				probInfo *pInfo;
				probList.push_back(probInfo());
				pInfo = &probList.back();

				pInfo->N1_action = N1_action;
				pInfo->N2_action = N2_action;
				pInfo->N3_action = N3_action;
				pInfo->probability = probability;
				pInfo->qvalue = qInfo->Qvalue;

			}
		}
	}


	// Select the actual actions-> select an actionSet from the probList

	//std::cerr << "Simtime: " << simTime() << endl;
    double rnd = 0;

	rnd = dblrand();

	//std::cerr << "rnd : " << rnd  << endl;

	for (ProbList::iterator it=probList.begin(); it!=probList.end(); ++it)
	{

	//	std::cerr << "rnd : " << rnd  << endl;
	//	std::cerr << "Action set: " << it->N1_action << ", " << it->N2_action << ", " << it->N3_action << endl;
	//	std::cerr << "Probability: " << it->probability << endl;
		if(rnd < it->probability)
		{
			std::cerr << "Current state: " << N1_fraction << ", " << N2_fraction << ", " << N3_fraction << endl;
    		std::cerr << "Selected Action set: " << it->N1_action << ", " << it->N2_action << ", " << it->N3_action << endl;
    		std::cerr << "Probability: " << it->probability << endl;
    		//std::cerr << "qvalue: " << it->qvalue << endl;
    		std::cerr << " "<< endl;
			actionN1 = it->N1_action;
			actionN2 = it->N2_action;
			actionN3 = it->N3_action;
			break;
		}
		rnd -= it->probability;
	}

	if(actionN1 > decrease || actionN2 > decrease || actionN3 > decrease
			|| actionN2 < nochange || actionN2 < nochange || actionN2 < nochange)
	{
		std::cerr << "Wrong Selected Action set: " << actionN1 << ", " << actionN2 << ", " << actionN3 << endl;
			actionN1 = nochange;
			actionN2 = nochange;
			actionN3 = nochange;

		//error("Wrong action selected");
	}


//	actionN1 = nochange;
//	actionN2 = nochange;
//	actionN3 = nochange;

//	actionN1 = increase;
//	actionN2 = increase;
//	actionN3 = increase;

//	actionN1 = decrease;
//	actionN2 = decrease;
//	actionN3 = decrease;

}

/* Carry out the actions
 *
 */

void FS4VP::doActions(int N1_action, int N2_action, int N3_action)
{
	int state;
	int userClass;

	if(burst == true)
	{
		//Omit chunkSize changes in last phase -> to make changes only a temporary boost
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{

			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				if(changeChunksize == true)
				{
					if ((it1->second).chunkSize != chunkSize )
					{
						(it1->second).chunkSize = chunkSize;
					}
				}
				else
				{
					if ((it1->second).txRate != (it1->second).origTxRate )
					{
						(it1->second).txRate = (it1->second).origTxRate;
					}

				}
			}
		}
	}

//	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
//	{
//
//		if((it1->second).codecRate == 4000000)
//		{
//			(it1->second).incRate = 0.7;
//			(it1->second).decRate = 0.0125;
//		}
//		else if((it1->second).codecRate == 2500000)
//		{
//			(it1->second).incRate = 0.3;
//			(it1->second).decRate = 0.05;
//		}
//		else if((it1->second).codecRate == 800000)
//		{
//			(it1->second).incRate = 0.1;
//			(it1->second).decRate = 0.4;
//		}
//		else if((it1->second).codecRate == 400000)
//		{
//			(it1->second).incRate = 0.05;
//			(it1->second).decRate = 0.7;
//		}
//	}


	//NOTE: when txRate is used
	// increase = decrease
	// decrease = increase
	// Due to problems with determining initial policy

	// N1 == RED
	if(N1_action == nochange)
	{
		// Do nothing
		EV << "N1_action :" << N1_action << ", do nothing. " << endl;
	}
	else if (N1_action == increase)
	{
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{
			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				state = (it1->second).playBufferState; // Get the playBuffer state
				userClass = (it1->second).userClass; // Get the user class

				//RED = N1
				if(state == 0 || state == 1)
				{
					// Increase the chunk size according to percentage parameter
					if(userClass == GOLD)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + goldIncPercentage);
							//(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + (it1->second).incRate);


						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - goldIncPercentage);
						}
					}
					else if (userClass == SILVER)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + silverIncPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - silverIncPercentage);
						}
					}
					else if (userClass == BRONZE)
					{

						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + bronzeIncPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - bronzeIncPercentage);
						}
					}

				}
			}
		}
	}
	else if (N1_action == decrease)
	{
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{
			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				state = (it1->second).playBufferState; // Get the playBuffer state
				userClass = (it1->second).userClass; // Get the user class

				//RED or ORANGE= N1
				if(state == 0 || state == 1)
				{
					// Decrease the chunk size according to percentage parameter
					if(userClass == GOLD)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - goldDecPercentage);
							//(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - (it1->second).decRate);

						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + goldDecPercentage);
						}
					}
					else if (userClass == SILVER)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - silverDecPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + silverDecPercentage);
						}
					}
					else if (userClass == BRONZE)
					{

						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - bronzeDecPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + bronzeDecPercentage);
						}
					}
				}
			}
		}
	}

	// N2 == YELLOW
	if(N2_action == nochange)
	{
		// Do nothing
		EV << "N2_action :" << N2_action << ", do nothing. " << endl;
	}
	else if (N2_action == increase)
	{
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{
			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				state = (it1->second).playBufferState; // Get the playBuffer state
				userClass = (it1->second).userClass; // Get the user class

				//YELLOW = N2
				if(state == 2)
				{
					// Increase the chunk size
					if(userClass == GOLD)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + goldIncPercentage);
							//(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + (it1->second).incRate);

						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - goldIncPercentage);
						}
					}
					else if (userClass == SILVER)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + silverIncPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - silverIncPercentage);
						}
					}
					else if (userClass == BRONZE)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + bronzeIncPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - bronzeIncPercentage);
						}
					}
				}
			}
		}
	}
	else if (N2_action == decrease)
	{
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{
			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				state = (it1->second).playBufferState; // Get the playBuffer state
				userClass = (it1->second).userClass; // Get the user class

				//YELLOW = N2
				if(state == 2)
				{
					// Decrease the chunk size
					if(userClass == GOLD)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - goldDecPercentage);
							//(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - (it1->second).decRate);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + goldDecPercentage); // 10 % decrease
						}
					}
					else if (userClass == SILVER)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - silverDecPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + silverDecPercentage);
						}
					}
					else if (userClass == BRONZE)
					{

						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - bronzeDecPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + bronzeDecPercentage);
						}
					}
				}
			}
		}
	}

	// N3 == GREEN
	if(N3_action == nochange)
	{
		// Do nothing
		EV << "N3_action :" << N3_action << ", do nothing. " << endl;
	}
	else if (N3_action == increase)
	{
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{
			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				state = (it1->second).playBufferState; // Get the playBuffer state
				userClass = (it1->second).userClass; // Get the user class

				//GREEN = N3
				if(state == 3 )
				{
					// Increase the chunk size
					if(userClass == GOLD)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + goldIncPercentage);
							//(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + (it1->second).incRate);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - goldIncPercentage);
						}
					}
					else if (userClass == SILVER)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + silverIncPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - silverIncPercentage);
						}
					}
					else if (userClass == BRONZE)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 + bronzeIncPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 - bronzeIncPercentage);
						}
					}
				}
			}
		}
	}
	else if (N3_action == decrease)
	{
		for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
		{
			// Check that the initial play has started and play has not finished
			if((it1->second).initialPlayStarted == true && (it1->second).playFinished == false)
			{
				state = (it1->second).playBufferState; // Get the playBuffer state
				userClass = (it1->second).userClass; // Get the user class

				//GREEN = N3
				if(state == 3 )
				{
					// Decrease the chunk size
					if(userClass == GOLD)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - goldDecPercentage);
							//(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - (it1->second).decRate);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + goldDecPercentage);
						}
					}
					else if (userClass == SILVER)
					{
						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - silverDecPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + silverDecPercentage);
						}
					}
					else if (userClass == BRONZE)
					{

						if(changeChunksize == true)
						{
							(it1->second).chunkSize = ((it1->second).chunkSize) * (1 - bronzeDecPercentage);
						}
						else
						{
							(it1->second).txRate = ((it1->second).txRate) * (1 + bronzeDecPercentage);
						}
					}
				}
			}
		}
	}

}

void FS4VP::initializeQmatrix()
{
	// Initialize Q-matrix. Added 11.02.2013

	// 1. States
	/* 		zero (0), // 0
	zeroTo25 (1), // 0 - 25
	Twenty5ToFifty (2), // 25 - 50
	FiftyTo75 (3), // 50 - 75
	Seventy5To100 (4), // 75 - 100
	*/
	for(int N1fractionIndex = 0 ; N1fractionIndex < countFractionSpace ; N1fractionIndex++) // Portion of users in N1
	{
		for(int N2fractionIndex = 0; N2fractionIndex < countFractionSpace; N2fractionIndex++) // Portion of users in N2
		{
			for(int N3fractionIndex = 0; N3fractionIndex < countFractionSpace; N3fractionIndex++) // // Portion of users in N3
			{
				//2.  Actions
				// 		increase (0),
				// nochange (1),
				// decrease (2).
				for(int N1_actionIndex = 0; N1_actionIndex < countActions; N1_actionIndex++) // N1 actions
				{
					for(int N2_actionIndex = 0; N2_actionIndex < countActions ; N2_actionIndex++) // N2 actions
					{
						for(int N3_actionIndex = 0; N3_actionIndex< countActions; N3_actionIndex++) // N3 actions
						{
//    							QInfo *qInfo;
//								qMatrix.push_back(QInfo());
//								qInfo = &qMatrix.back();
//
//								qInfo->N1_fraction = N1fractionIndex;
//								qInfo->N2_fraction = N2fractionIndex;
//								qInfo->N3_fraction = N3fractionIndex;
//								qInfo->N1_action = N1_actionIndex;
//								qInfo->N2_action = N2_actionIndex;
//								qInfo->N3_action = N3_actionIndex;


							int numberN1 = 0;
							int numberN2 = 0;
							int numberN3 = 0;
							int sum = 0;

							if(N1fractionIndex == zero)
							{
								numberN1 = 0;
							}
							else if (N1fractionIndex == zeroTo25)
							{
								numberN1 = 1;
							}
							else if (N1fractionIndex == Twenty5ToFifty)
							{
								numberN1 = 25;
							}
							else if (N1fractionIndex == FiftyTo75)
							{
								numberN1 = 50;
							}
							else if (N1fractionIndex == Seventy5To100)
							{
								numberN1 = 75;
							}

							if(N2fractionIndex == zero)
							{
								numberN2 = 0;
							}
							else if (N2fractionIndex == zeroTo25)
							{
								numberN2 = 1;
							}
							else if (N2fractionIndex == Twenty5ToFifty)
							{
								numberN2 = 25;
							}
							else if (N2fractionIndex == FiftyTo75)
							{
								numberN2 = 50;
							}
							else if (N2fractionIndex == Seventy5To100)
							{
								numberN2 = 75;
							}

 							if(N3fractionIndex == zero)
							{
								numberN3 = 0;
							}
							else if (N3fractionIndex == zeroTo25)
							{
								numberN3 = 1;
							}
							else if (N3fractionIndex == Twenty5ToFifty)
							{
								numberN3 = 25;
							}
							else if (N3fractionIndex == FiftyTo75)
							{
								numberN3 = 50;
							}
							else if (N3fractionIndex == Seventy5To100)
							{
								numberN3 = 75;
							}



 							// ----------- Not possible states -----------------

							sum = numberN1 + numberN2 + numberN3;
							if (sum < 25 || sum > 100)
							{
								// Do nothing

							}

							// ----------- Possible states -----------------
							else
							{

    							QInfo *qInfo;
								qMatrix.push_back(QInfo());
								qInfo = &qMatrix.back();

								qInfo->N1_fraction = N1fractionIndex;
								qInfo->N2_fraction = N2fractionIndex;
								qInfo->N3_fraction = N3fractionIndex;
								qInfo->N1_action = N1_actionIndex;
								qInfo->N2_action = N2_actionIndex;
								qInfo->N3_action = N3_actionIndex;


								// --------------Initial policy------------------

								// chunkSize is used N1_actionIndex == decrease || N3_actionIndex == increase
								if(N3_actionIndex == increase && initialPolicy == true)
								{

									qInfo->Qvalue = -(std::numeric_limits<double>::infinity());


								}

								else
								{
									if(N1fractionIndex == zero && N2fractionIndex != zero && N3fractionIndex != zero)
									{
										//if(N1_actionIndex == increase || N1_actionIndex == decrease || N3_actionIndex == increase)
										if(N1_actionIndex == increase || N1_actionIndex == decrease)
										{
											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
										}
										else
										{
											if(randomQTable == false)
											{
												qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = uniform(-100,100);
											}

										}

									}
									else if (N2fractionIndex == zero && N1fractionIndex != zero && N3fractionIndex != zero)
									{
										//if(N2_actionIndex == increase || N2_actionIndex == decrease || N1_actionIndex == decrease || N3_actionIndex == increase)
										if(N2_actionIndex == increase || N2_actionIndex == decrease)
										{
											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
										}
										else
										{
											if(randomQTable == false)
											{
												qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = uniform(-100,100);
											}
										}


									}
									else if (N3fractionIndex == zero && N1fractionIndex != zero && N2fractionIndex != zero)
									{
										//if(N3_actionIndex == increase || N3_actionIndex == decrease || N1_actionIndex == decrease)
										if(N3_actionIndex == increase || N3_actionIndex == decrease)
										{
											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
										}
										else
										{
											if(randomQTable == false)
											{
												qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = uniform(-100,100);
											}
										}

									}
									else if (N1fractionIndex == zero && N2fractionIndex == zero && N3fractionIndex != zero)
									{

										// Do nothing when all users are in green
										if (N3fractionIndex == Seventy5To100 && initialPolicy == true)
										{
											if(N1_actionIndex == nochange && N2_actionIndex == nochange && N3_actionIndex == nochange)
											{
												qInfo->Qvalue = 0.0;
											}
											else if (N1_actionIndex != nochange || N2_actionIndex != nochange)
											{
												qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
												//qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
												//qInfo->Qvalue = 0.0;
											}

										}
										else
										{

											if(N1_actionIndex == increase || N1_actionIndex == decrease || N2_actionIndex == increase || N2_actionIndex == decrease)
											{
												qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
											}
											else
											{
												if(randomQTable == false)
												{
													qInfo->Qvalue = 0.0;
												}
												else
												{
													qInfo->Qvalue = uniform(-100,100);
												}
											}
										}
									}
									else if (N1fractionIndex == zero && N2fractionIndex != zero && N3fractionIndex == zero)
									{
										if(N1_actionIndex == increase || N1_actionIndex == decrease || N3_actionIndex == increase || N3_actionIndex == decrease)
										{
											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
										}
										else
										{
											if(randomQTable == false)
											{
												qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = uniform(-100,100);
											}
										}
									}
									else if (N1fractionIndex != zero && N2fractionIndex == zero && N3fractionIndex == zero)
									{
										//if(N2_actionIndex == increase || N2_actionIndex == decrease || N3_actionIndex == increase || N3_actionIndex == decrease || N1_actionIndex == decrease)
										if(N2_actionIndex == increase || N2_actionIndex == decrease || N3_actionIndex == increase || N3_actionIndex == decrease)
										{
											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
										}
										else
										{
											if(randomQTable == false)
											{
												qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = uniform(-100,100);
											}
										}
									}
									else if (N1fractionIndex == zero && N2fractionIndex == zero && N3fractionIndex == zero)
									{
										if(N1_actionIndex == increase || N1_actionIndex == decrease || N2_actionIndex == increase || N2_actionIndex == decrease || N3_actionIndex == increase || N3_actionIndex == decrease)
										{
											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
										}
										else
										{
											if(randomQTable == false)
											{
												qInfo->Qvalue = 0.0;
											}
											else
											{
												qInfo->Qvalue = uniform(-100,100);
											}
										}
									}

//									else if (N1fractionIndex != zero && N2fractionIndex != zero && N3fractionIndex != zero)
//									{
//										if(N1_actionIndex == decrease || N3_actionIndex == increase)
//										{
//											qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
//										}
//										else
//										{
//											qInfo->Qvalue = 0.0;
//										}
//
//									}

									else
									{
										// Initialize Q value as zero
										if(randomQTable == false)
										{
											qInfo->Qvalue = 0.0;
										}
										else
										{
											qInfo->Qvalue = uniform(-100,100);
										}
									}
								}


							}
						}
					}
				}
			}
		}
	}


}

/**
 * Writes the Q-matrix into a text file
 */

void FS4VP::writeQmatrix()
{

	std::ofstream myfile ("test.txt");
	if(myfile.is_open())
	{

		for (QMatrix::iterator it=qMatrix.begin(); it!=qMatrix.end(); ++it)
		{
			myfile << it->N1_fraction << "," <<  it->N2_fraction << ","  << it->N3_fraction << "," <<  it->N1_action << "," <<  it->N2_action << ","  << it->N3_action << "," << it->Qvalue << endl;
		}
		myfile.close();

	}
	else
	{
		error("unable to open file!");
	}

	//EV << getFullPath() << ": Reached " << goldCompleted << " completed gold downloads, ending simulation. " << endl;
	endSimulation();

}

/**
 * Reads data from a text file and
 * forms the Q-matrix
 */

void FS4VP::readQmatrix()
{

	std::string line;
	std::string tempString;
	std::string inf = "-inf";
	std::ifstream myfile ("test.txt");
	std::string delimiter = ",";
	std::vector<std::string> strVec;
	std::string token;
	size_t pos = 0;

	if(myfile.is_open())
	{
		while(getline (myfile, line))
		{

			pos = 0;
			tempString = line;

			while ((pos = tempString.find(delimiter)) != std::string::npos) {
			    token = tempString.substr(0, pos);
			    tempString.erase(0, pos + delimiter.length());
			    strVec.push_back(token);
			}
			strVec.push_back(tempString);
			pushDataToQmatrix(strVec);

			strVec.clear();

		}

		myfile.close();

	}
	else
	{
		initializeQmatrix();
		//error("unable to open file!");
	}

}
/**
 * Pushes data into the Q-matrix
 */

void FS4VP::pushDataToQmatrix(std::vector<std::string> strVec)
{

	QInfo *qInfo;
	qMatrix.push_back(QInfo());
	qInfo = &qMatrix.back();

	qInfo->N1_fraction = atoi(strVec[0].c_str());
	qInfo->N2_fraction = atoi(strVec[1].c_str());
	qInfo->N3_fraction = atoi(strVec[2].c_str());
	qInfo->N1_action = atoi(strVec[3].c_str());
	qInfo->N2_action = atoi(strVec[4].c_str());
	qInfo->N3_action = atoi(strVec[5].c_str());

	if(strncmp(strVec[6].c_str(), "-inf", 4) == 0)
	{
		qInfo->Qvalue = -(std::numeric_limits<double>::infinity());
	}
	else
	{
		qInfo->Qvalue = atof(strVec[6].c_str());
	}

}

void FS4VP::scheduleRestart(CliAppContext *cliAppContext, cMessage *triggerTimer)
{

	for (AppContextDataMap::iterator it1 = appMap.begin(); it1 != appMap.end(); ++it1)
	{
	//	EV<<"DEBUG: KEY-1: "<<it1->first<<" Port_id: "<<(it1->second).connectPortId<<endl;
		if((it1->second).connectPortId == cliAppContext->getPortId())
		{
			std::cerr << "Scheduling restart for client port id: " << cliAppContext->getPortId() << ", simtime: " << simTime() << endl;
			(it1->second).initialPlayStarted = false;
			(it1->second).playFinished = false;
			(it1->second).remainingBytes = bytesToSend;
			(it1->second).chunkSize = chunkSize;
			(it1->second).playoutBufferSize = 0;
			(it1->second).playBufferState = 3;

			break;
		}

	}

	scheduleAt(simTime() + idlePeriod, triggerTimer);
}

void FS4VP::finish()
{
    //empty
	if(writeQmatrixToFile == true)
	{
		writeQmatrix();
	}
}

void FS4VP::receiveChangeNotification(int category, const cPolymorphic *details)
{
	Enter_Method_Silent();
	EV <<"this is receieChangeNotification method"<<endl;
	//empty
}

void FS4VP::receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l)
{
	Enter_Method_Silent();
	EV <<"this is receiveSignal method"<<endl;
	//empty
}

//======================= TEST CODE =====================================================================
//    	EV <<"FS4VP:this is stage " << stage << endl;
//    	EV <<"1. this is module [ "<<this->getIndex()<<" ] in a vector of size [ "<< this->size()<<" ] "<<endl;
//    	EV <<"2. this is module [ "<<getIndex()<<" ] in a vector of size [ "<< size()<<" ] "<<endl;
//    	EV <<"the id of this module is "<<getId()<< " and its name is: "<<getFullName()<<endl;

//    	for (cSubModIterator iter(*getParentModule()); !iter.end(); iter++) //this for routine will iterate though all the submodules of hte parent module
//    	{
//    		EV <<"DEBUG: "<<iter()->getFullName()<<endl;
//    	}

//----------------------------------------------------------------
//    	EV<<"Experimenting with multiple timers"<<endl;
//    	cMessage *timerMsgMatrixPtr[numOfTcpApps];
//    	for(int y=0; y<= numOfTcpApps; y++)
//    	{
//    		timerMsgMatrixPtr[y] = new cMessage("timerMsg", MK_TEST_MSG);
//    		//timerMsgMatrixPtr[y] -> setControlInfo(ci);
////    		timerMsgMatrixPtr[y] -> setTcpAppId(tcpAppList[y]->getId());
//    		scheduleAt(simTime()+1.0, timerMsgMatrixPtr[y]);
//
//    	}
