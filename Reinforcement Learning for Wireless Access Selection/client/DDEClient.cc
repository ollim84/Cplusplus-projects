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
// Copyright 2013 Olli Mämmelä (VTT)


#include "DDEClient.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include "GenericAppMsg_m.h"
#include "ClientMsg_m.h"
#include "IPAddressResolver.h"
#include "NotifierConsts.h"
#include <vector>
#include <algorithm>
#include "InterfaceTableAccess.h"
#include <stdio.h>
#include <stdlib.h>
#include "AppContext_m.h"




#define BEV	EV << "[DDE Client]:"

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
#define FTP_APP 0
#define VIDEO_APP 1

#define INTERNAL_THRUPUT_TIMER 18

//Self messages for base module
#define INTERNAL_DECISION_LOCK 11
#define INTERNAL_DECISION_TIMEOUT 12
#define INTERNAL_UNREGISTER_MSG    16
#define INTERNAL_MN_CLASSIFY_MSG	17
#define INTERNAL_INIT_CONNECTION_MSG    19
#define INTERNAL_ASSOCIATE_MSG    20
#define INTERNAL_CLOSE_CONNECTION_MSG    21
#define INTERNAL_INFO_CLASSIFY_MSG	22
#define INTERNAL_AP_CLASSIFY_MSG  23
#define INTERNAL_LOCK_TIMEOUT 24
#define INTERNAL_SUSPEND_TIMEOUT 25
#define INTERNAL_INFO_CLOSE_MSG    26
//Self messages for handlers
#define EVT_TOUT    27
//Self messages for threads
#define INTERNAL_REMOVE_THREAD_MSG   28
#define INTERNAL_CLASSIFY_MSG   29

// VIDEO
#define MK_PLAY_CONTENT 13 //ZY 25.10.2011
#define MK_PAUSE_CONTENT 14 //ZY 25.10.2011
#define MK_BUFFER_STATUS_MSG 15 //ZY 16.07.2012
//#define MK_START_AGENT_MSG 16 //OM 07.02.2013
//#define MK_FINISH_AGENT_MSG 17 //OM

// sendDirect message
#define MK_AP_LIST 30

Register_Class(DDEClientServerHandler);
Register_Class(DDEClientInfoHandler);
Define_Module(DDEClient);

/**
 * Constructor.
 */

DDEClient::DDEClient()
{
	setMsgsRcvd(0);
	setMsgsSent(0);
	setBytesRcvd(0);
	setBytesSent(0);
	handoverTime.clear();

}

/**
 * Destructor.
 */

DDEClient::~DDEClient()
{
	//cancelAndDelete(ThruputTimer);
	ThruputTimer = NULL;
	handoverTime.clear();

}

std::ostream& operator<<(std::ostream& os, const DDEClient::AssociatedAPInfo& assocAP)
{
    os << "ID=" << assocAP.ID
       << " Address=" << assocAP.apAddress
       << " classifyValue=" << assocAP.classifyValue
       << " numUsers=" << assocAP.numUsers
       << " load=" << assocAP.load
       << " loss=" << assocAP.packetLoss
       << " mac=" << assocAP.macAddress
       << " nsClassify=" << assocAP.nsClass;
    return os;
}

std::ostream& operator<<(std::ostream& os, const DDEClient::BestOtherAPInfo& bestOtherAp)
{
    os << "ID=" << bestOtherAp.ID
       << " Address=" << bestOtherAp.apAddress
       << " classifyValue=" << bestOtherAp.classifyValue
       << " numUsers=" << bestOtherAp.numUsers
       << " load=" << bestOtherAp.load
       << " loss=" << bestOtherAp.packetLoss
       << " mac=" << bestOtherAp.macAddress
       << " nsClassify=" << bestOtherAp.nsClass;
    return os;
}

//This function is needed for the WATCH_LIST(apList) command
//Prints the AP struct in the GUI
std::ostream& operator<<(std::ostream& os, const DDEClient::apInfo& ap)
{
    os << "ID=" << ap.ID
       << " Address=" << ap.apAddress
       << " suspended=" << ap.suspended;
    return os;
}


void DDEClient::initialize()
{
	// parent initialize()
	TCPSrvHostApp::initialize();

    delay = par("replyDelay"); //not needed
    maxMsgDelay = 0; //not needed

    sessionTimeout_var 	= par("sessionTimeout");
    suspendTimeout_var = par("suspendTimeout");
    startTime_var = par("startTime");
    numRequestsPerSession_var = par("numRequestsPerSession");
    requestLength_var = par("requestLength");
    replyLength_var = par("replyLength");
    classifyInterval_var = par("classifyInterval") ;
    idleInterval_var = par("idleInterval");
    decisionThreshold_var = par("decisionThreshold");

    numRequests_var = (int)fileSize_var/replyLength_var;
    numRequestsSent_var = 0;
    bytesDownloaded_var = 0;
    downloadComplete_var = false;
    changeStart = changeEnd = 0;
    dlThruput_var = 0.0;
    ulThruput_var = 0.0;
    totalBW_var = 0.0;
    isDownloadStarted_var = false;
    downloadStart = downloadEnd = 0.0;
    requestDelay_var = 0.0;
    numberOfHandovers = 0;
    newSSID = "";
    downloadTime_var = 0;
    ss_var = 0;
    rssi_var = 0;
    mnClass_var = 0;


    sessionLock_var = false;
    decisionLock_var = false;
    debuggedNode = par("debuggedNode");
    totalBW_vector.setName("Total BW vector");

    // Algorithms
    fuzzy_var = par("fuzzy");
    learning_var = par("learning");
    pureQLearning_var = par("pureQLearning");
    loadBalancing_var = par("loadBalancing");
    backgroundTraffic_var = par("backgroundTraffic");

	fileServerThread = infoServerThread = apThread = 0;

	cModule* tcpMod = getParentModule()->getSubmodule("tcp",0);
	tcp = check_and_cast<TCP*> (tcpMod);
	//tcp = new TCP();


	//timeoutAssoc = new ClientInternalMsg("INTERNAL_ASSOCIATE_MSG ", INTERNAL_ASSOCIATE_MSG );

	modp = simulation.getModuleByPath("WirelessAccess.stats");
	stats = check_and_cast<StatisticsCollector *>(modp);

	if (stats == NULL)
			opp_error("Wrong statisticsModulePath configuration");


    //bitpersecVector.setName("thruput (bit/sec)");

    BEV << "Num requests: " << numRequests_var  << endl;
    BEV << "file size: " << fileSize_var << endl;

    // Application type
    applicationType = par("applicationType");
    if(applicationType != FTP_APP && applicationType != VIDEO_APP)
    {
    	error("Wrong application type!");
    }


    handoverStart = 0;
    playTimeStart = 0;
    playTime_var = 0;

    handoverScan = par("handoverScan");
    closeTcpConnections_var = par("closeTcpConnections");

    // --------------- VIDEO Client parameters --------------- //
	//Signal creation
	totalBytesReceived = registerSignal("Total_Bytes_Received"); //registering a signal to be emitted and listened by the remote server

	starvationDurationVector.clear();
	starvationStart = 0; //OM
	pauseEvent = false;

	//const char *address = par("address");
    //port = par("port");

    pauseRate = par("pauseRate");
    initAdvBufferSize = par("initAdvBufferSize");
    minAdvBufferSize = par("minAdvBufferSize");
    //specifying playBuffer status informations
    threshYO = par("threshYO");
    threshGY = par("threshGY");

    playBuffer.setName("Playout_Buffer"); //cQueue used as a playout buffer

    // txRate = 3 x MSS (bits) / codecRate (bps) + 0.5% * codecRate (bps)
    // playRate = chunkSize (bits) / codecRate (bps)

    chunkSize = par ("chunkSize");
    codecRate = par("codecRate");
    txRate = (3 * chunkSize * 8) / (codecRate + 0.005 * codecRate);
    playRate = (chunkSize) * 8 / codecRate;

    // Video length
    videoLength_var = par("videoLength");


    //m_videoSize = (m_videoLength * m_videoEncodingRate)/8; // size of the video in Bytes

    //std::cout << "CodecRate: " << codecRate << ", chunkSize: " << chunkSize << " , txRate: " << txRate << ", playRate: " << playRate << endl;

    // For an FTP application, get the bw limit from ini-file
    if(applicationType == FTP_APP)
    {
    	bandwidthLimit_var = par("bandwidthLimit");
    	fileSize_var = par("fileSize");
    }
    // For a video application, the bw limit is the same as the codecRate
    else if(applicationType == VIDEO_APP)
    {
    	bandwidthLimit_var = codecRate/1000;
    	fileSize_var = (videoLength_var * codecRate)/8;

    	//fileSize_var = par("fileSize");
    	//videoLength_var = (fileSize_var * 8)/codecRate;

    	std::cout << "CodecRate: " << codecRate << ", chunkSize: " << chunkSize << " , txRate: " << txRate << ", playRate: " << playRate << endl;
    	std::cout << "Video length: " << videoLength_var << endl;
    	std::cout << "fileSize: " << fileSize_var << endl;
    }

    remainingFileSize = fileSize_var;
    playFinished = false;
    playStarted = false;

    userClass = par("userClass"); // OM 12.02.2013
    contentDelivered = false; //OM
    numberOfBufferStarvations = 0; //OM
    bufferSizeVector.clear();

    //Setting up of Statistics vectors
    segmentsReceivedVec.setName("Total_Segments_Rcvd");
    playBufferSizeVec.setName("Play_Buffer_Size");
    playBufferSizeByteVec.setName("Play_Buffer_Size_Bytes");
    playBreakVec.setName("Play_Break");

    WATCH(pauseEvent);

    bytesRcvd = 0;
    WATCH(bytesRcvd);

    playBufferSize = 0; //ZY 25.10.2011
    WATCH(playBufferSize); //ZY 25.10.2011

    segmentsRcvd = 0;

    WATCH(segmentsRcvd);
    WATCH(initAdvBufferSize);
    WATCH(minAdvBufferSize);
    WATCH(threshYO);
    WATCH(threshGY);
	WATCH_PTRVECTOR(playerControl);
	WATCH(remainingFileSize);
	WATCH(ss_var);
	WATCH(fileSize_var);

	currentBufferState = 0; //initializing the current playBufferState to 0 (= RED i.e., starved buffer )
	WATCH(currentBufferState); //to visually keep track of the playBufferState.


	// --------------- VIDEO Client parameters END --------------- //

//    std::cout << "Request length: " << requestLength_var << endl;
//    std::cout << "Reply length: " << replyLength_var << endl;

//	ThruputTimer = new ddeInternalMsg("INTERNAL_THRUPUT_TIMER", INTERNAL_THRUPUT_TIMER);
//	scheduleAt(simTime(), ThruputTimer);

    //set host position
    int Xposition = getParentModule()->getSubmodule("mobility")->par("x");
    int Yposition = getParentModule()->getSubmodule("mobility")->par("y");

	std::cout << getFullPath() << ": Position: " << "("  << Xposition << ", " <<  Yposition << ")" << endl;


    nb = NotificationBoardAccess().get();
    if (nb==NULL)
    {
        error("Notification board not found. Client needs a notification board to know when the DHCP has assigned an IP address.");
    }

    // for a wireless interface subscribe the DHCP event to start the TCP application
    nb->subscribe(this, NF_INTERFACE_IPv4CONFIG_CHANGED);

    //NF_INTERFACE_IPv4CONFIG_CHANGED
    //NF_L2_ASSOCIATED

    // Get the interface to configure
//    IInterfaceTable* ift = InterfaceTableAccess().get();
//    this->ie = ift->getInterfaceByName("wlan");
//
//    if (this->ie==NULL)
//    {
//        error("DHCP Interface does not exist. aborting.");
//        return;
//    }

//    ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
//    msg->setConnectAddress(par("connectAddressInterface1"));
//    msg->setInterfaceID(0);
//    scheduleAt(simTime()+startTime_var, msg);

//    ClientInternalMsg *msg2 = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
//    msg2->setConnectAddress(par("connectAddressInterface2"));
//    msg2->setInterfaceID(1);
//    scheduleAt(simTime()+startTime_var, msg2);


    WATCH(msgsRcvd_var);
    WATCH(msgsSent_var);
    WATCH(bytesRcvd_var);
    WATCH(bytesSent_var);
    WATCH(bytesDownloaded_var);
    WATCH(numRequestsSent_var);
    WATCH(dlThruput_var);
    WATCH(assocAP);
    WATCH(bestOtherAp);
    WATCH(downloadComplete_var);
    WATCH(requestDelay_var);
    WATCH_LIST(apList);

}


// Handles messages coming to this module
void DDEClient::handleMessage(cMessage* msg)
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
	// Handle directInput messages from the infoServer
	else if(msg->getKind() == INTERNAL_DECISION_LOCK)
	{
		handleSelfMessage(msg);
	}
	//Else: the message is socket data, dataArrived() of a handler should be called
	else
	{
		// default handling
		TCPSrvHostApp::handleMessage(msg);


	}

}

//Handles self messages
void DDEClient::handleSelfMessage(cMessage *msg)
{
	switch(msg->getKind())
	{
		// Initiates a connection with a server
		// This is based on the peer-wire module of the BT simulation model
		case INTERNAL_INIT_CONNECTION_MSG:
		{

        	if(isDownloadStarted_var == false)
        	{
        		downloadStart = simTime();
        		isDownloadStarted_var = true;

        		// Start the thruput timer, since download is started.
        		ThruputTimer = new ddeInternalMsg("INTERNAL_THRUPUT_TIMER", INTERNAL_THRUPUT_TIMER);
        		scheduleAt(simTime(), ThruputTimer);
        	}


			ClientInternalMsg *appmsg = dynamic_cast<ClientInternalMsg *>(msg);

			// connect
			const char *connectAddress = appmsg->getConnectAddress();
			//const char *connectAddress = par("connectAddress");

			bool connectToAp = appmsg->getConnectToAp();

			BEV << "Establishing a connection with server: " << connectAddress << endl;
			// new connection -- create new socket object and server process
			TCPSocket *newsocket = new TCPSocket();


			ushort localPort = tcp->getEphemeralPort();
			int remotePort = par("port");

			//NOTE: adding this (TCP.h etc.) causes warnings
			while ((localPort == remotePort))
				localPort = tcp->getEphemeralPort();

			//Bind to local port
			newsocket->bind(localPort);

			//const char *serverThreadClass = (const char*)par("serverThreadClass");

			//If we are connecting to AP
			if(connectToAp == true)
			{
				const char *serverThreadClass = "DDEClientServerHandler";

				TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

				newsocket->setCallbackObject(proc);

				//Creates a thread object to the server handler
				//By this we can call methods from concerning this thread
				DDEClientServerHandler* myProc = (DDEClientServerHandler*) proc;

				myProc->init(this, newsocket); // This initializes "DDEClientServerHandler"
				myProc->setInterfaceID(appmsg->getInterfaceID());
				if(appmsg->getConnectToFileServer() == true)
				{
					BEV << "Connecting to file server." << endl;
					myProc->setFileServer(true);
					setFileServerThread(proc);
				}
				else
				{
					BEV << "Connecting to AP." << endl;
					//std::cout  << "connectin to AP: " << simTime() << endl;
					setAPThread(proc);
				}
			}
			//If we are connecting to infoServer
			else
			{
				const char *serverThreadClass = "DDEClientInfoHandler";

				TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));
				setInfoServerThread(proc);

				newsocket->setCallbackObject(proc);

				//Creates a thread object to the server handler
				//By this we can call methods from concerning this thread
				DDEClientInfoHandler* myProc = (DDEClientInfoHandler*) proc;

				myProc->init(this, newsocket); // This initializes "DDEClientServerHandler"
				myProc->setInterfaceID(appmsg->getInterfaceID());

			}



			newsocket->setOutputGate(gate("tcpOut"));

			//IP address of the server is retrieved by the IPAddressresolver,
			// based on the name of the server
			//Connect to a remote port
			newsocket->connect(IPAddressResolver().resolve(connectAddress), remotePort);
			socketMap.addSocket(newsocket);

			updateDisplay();

			delete msg;
			break;

		}
		// Initiates association to an AP
		case INTERNAL_ASSOCIATE_MSG:
		{
			ClientInternalMsg *appmsg = dynamic_cast<ClientInternalMsg *>(msg);
			BEV << "Generating an associate request to: "<<  appmsg->getSsid() << ", MAC: " << appmsg->getNewMAC() << endl;
			std::cout << getParentModule() << ": Generating an associate request to: "<<  appmsg->getSsid() << ", MAC: " << appmsg->getNewMAC() << ", simTime: " << simTime() << endl;
			sendScanRequest(appmsg->getSsid(), appmsg->getMacAddress(), appmsg->getNewMAC());

			delete msg;
			break;
		}
		case INTERNAL_INFO_CLOSE_MSG:
		{
			BEV << "Closing connection to the infoServer." << endl;
			std::cout << "Closing connection to the infoServer." << endl;
			TCPServerThreadBase* thread = getInfoServerThread();
			if(thread == 0)
				error("Thread is zero!");
			DDEClientInfoHandler* myProc = (DDEClientInfoHandler*) thread;
			myProc->callClose();

			delete msg;
			break;

		}
		case INTERNAL_CLOSE_CONNECTION_MSG:
		{

			BEV << "Closing connection to the AP." << endl;
			std::cout << "Closing connection to the AP." << endl;
			TCPServerThreadBase* thread = getAPThread();
			if(thread == 0)
				error("Thread is zero!");
			DDEClientServerHandler* myProc = (DDEClientServerHandler*) thread;
			myProc->callClose();


//			if(getLoadBalancing() == true)
//			{
//				BEV << "Closing connection to the AP." << endl;
//				std::cout << "Closing connection to the AP." << endl;
//				TCPServerThreadBase* thread = getAPThread();
//				if(thread == 0)
//					error("Thread is zero!");
//				DDEClientServerHandler* myProc = (DDEClientServerHandler*) thread;
//				myProc->callClose();
//			}
//			else
//			{
//				BEV << "Closing connection to the infoServer." << endl;
//				std::cout << "Closing connection to the infoServer." << endl;
//				TCPServerThreadBase* thread = getInfoServerThread();
//				if(thread == 0)
//					error("Thread is zero!");
//				DDEClientInfoHandler* myProc = (DDEClientInfoHandler*) thread;
//				myProc->callClose();
//			}



			delete msg;
			break;
		}
		case INTERNAL_UNREGISTER_MSG:
		{


			BEV << "Unregistering from the AP." << endl;
			std::cout << "Unregistering from the AP." << endl;
			TCPServerThreadBase* thread = getAPThread();
			if(thread == 0)
				error("Thread is zero!");
			DDEClientServerHandler* myProc = (DDEClientServerHandler*) thread;
			myProc->unRegister();

//			if(getLoadBalancing() == true)
//			{
//				BEV << "Unregistering from the AP." << endl;
//				std::cout << "Unregistering from the AP." << endl;
//				TCPServerThreadBase* thread = getAPThread();
//				if(thread == 0)
//					error("Thread is zero!");
//				DDEClientServerHandler* myProc = (DDEClientServerHandler*) thread;
//				myProc->unRegister();
//			}
//			else
//			{
//				BEV << "Unregistering from the infoServer." << endl;
//				std::cout << "Unregistering from the infoServer." << endl;
//				TCPServerThreadBase* thread = getInfoServerThread();
//				if(thread == 0)
//					error("Thread is zero!");
//				DDEClientInfoHandler* myProc = (DDEClientInfoHandler*) thread;
//				myProc->unRegister();
//			}

		}
		case INTERNAL_INFO_CLASSIFY_MSG:
		{
			BEV << "Sending a classify operation to infoServer." << endl;
			TCPServerThreadBase* thread = getInfoServerThread();
			DDEClientInfoHandler* myProc = (DDEClientInfoHandler*) thread;
			myProc->sendClassifyAP();

//			if(getLoadBalancing() == false)
//			{
//
//			}
//			else
//			{
//				myProc->sendClassificationMN();
//			}


			delete msg;
			break;
		}
		case INTERNAL_MN_CLASSIFY_MSG:
		{
			BEV << "Sending a MN classify operation to infoServer." << endl;
			TCPServerThreadBase* thread = getInfoServerThread();
			DDEClientInfoHandler* myProc = (DDEClientInfoHandler*) thread;

			myProc->sendClassificationMN();
			delete msg;
			break;
		}
		case INTERNAL_AP_CLASSIFY_MSG:
		{
			BEV << "Sending a classify operation to AP." << endl;
			TCPServerThreadBase* thread = getAPThread();
			DDEClientServerHandler* myProc = (DDEClientServerHandler*) thread;
			myProc->sendClassify();

			delete msg;
			break;
		}
		case INTERNAL_THRUPUT_TIMER:
		{
			double dlThruput = 0.0;
			dlThruput = computeDownloadThruput();
			setDownloadThruput(dlThruput);

			double ulThruput = 0.0;
			ulThruput = computeUploadThruput();
			setUploadThruput(ulThruput);

			double totalBW = dlThruput + ulThruput;
			setTotalBandwidth(totalBW);

			if(backgroundTraffic_var == false)
			{
//				std::cout << getParentModule()->getFullName() << ": Download thruput kbps: " << dlThruput/1000 << endl;
//				std::cout << getParentModule()->getFullName() << ": Upload thruput kbps: " << ulThruput/1000 << endl;
			}

			//bitpersecVector.recordWithTimestamp(simTime(), dlThruput);


			if(getDownloadComplete() == false)
			{
				ThruputTimer = new ddeMsg("INTERNAL_THRUPUT_TIMER", INTERNAL_THRUPUT_TIMER);
				scheduleAt(simTime()+0.5, ThruputTimer);
			}

			delete msg;
			break;
		}
		case INTERNAL_LOCK_TIMEOUT:
		{
			//std::cout << "INTERNAL LOCK " << endl;
			setSessionLock(false);
			delete msg;
			break;
		}
		case INTERNAL_DECISION_LOCK:
		{

			BEV << "Received a decision lock from the infoServer." << endl;

	        // Set decision lock active
			setDecisionLock(true);

			// Schedule timeout for the decision lock
			cMessage* decisionLockTimeout = new cMessage("decisionLockTimeout", INTERNAL_DECISION_TIMEOUT);
			scheduleAt(simTime() + (simtime_t)getDecisionThreshold(), decisionLockTimeout);

			delete msg;
			break;
		}
		case INTERNAL_DECISION_TIMEOUT:
		{
			BEV << "Releasing decision lock." << endl;
			setDecisionLock(false);
			delete msg;
			break;
		}
		case INTERNAL_SUSPEND_TIMEOUT:
		{
			ClientInternalMsg *appmsg = dynamic_cast<ClientInternalMsg *>(msg);
			unSuspendAP(appmsg->getSsid());
			delete msg;
			break;
		}

		// Video methods START//
		case MK_PLAY_CONTENT:
		{
			EV<<"Play Timer Message Received !"<<endl;
			playContent();
			if(!pauseEvent && playFinished == false) //only schedule the message if the PAUSE event is FALSE
			{
				scheduleAt(simTime()+playRate, msg);
			}
			else
			{
				delete msg;
			}

			break;
		}
		case MK_PAUSE_CONTENT:
		{
	       	EV<<"Pause Timer Message Received !"<<endl;
	       	pauseContent();
	       	if(pauseEvent) //only schedule the message if PAUSE event is TRUE
	       	{
	       		scheduleAt(simTime()+pauseRate, msg);
	       	}
	       	else
	       	{
	       		delete msg;
	       	}

			break;
		}
		// Video methods END //

		default:
		{
			delete msg;
			break;
		}
	}

}

//Handles self messages concerning a thread
void DDEClient::handleThreadMessage(cMessage *msg)
{

	// Get a pointer for the thread
	TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->getContextPointer();

	// This would get a handler for the thread. Handler could be used to call functions
	//DDEClientClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

	switch(msg->getKind())
	{
		case INTERNAL_REMOVE_THREAD_MSG:
		{
			BEV << "Internal remove thread, socket state: " << thread->getSocket()->getState() << endl;
			// If the socket has not been closed, schedule again
			if ((thread->getSocket()->getState()!= TCPSocket::CLOSED) && (thread->getSocket()->getState()!= TCPSocket::SOCKERROR))
			{
				BEV << "Socket state is not close, scheduling again."  << endl;
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
void DDEClient::removeThread(TCPServerThreadBase *thread)
{
	BEV << "Removing socket from socketMap." << endl;
    socketMap.removeSocket(thread->getSocket());
    BEV << "socketMap size: " << socketMap.size() << endl;
    if(socketMap.size() == 0)
    {

    	if(getDownloadComplete() == false)
    	{
    		//TODO: do all threads need to be closed?

    		std::cout << getParentModule() << ": All threads are now closed. SimTime: " << simTime() << endl;
    		std::string newSSID = getNewAPssid();
    		double delay;

    		//TODO: HandoverDelay!
    		if(handoverScan == true)
    		{
    			delay = uniform(0,1);
    			//delay = 0;
    		}
    		else
    		{
    			delay = 0;
    		}

    		std::cout << "delay: " << delay << endl;
			ClientInternalMsg *assocmsg = new ClientInternalMsg("INTERNAL_ASSOCIATE_MSG ", INTERNAL_ASSOCIATE_MSG );
			assocmsg->setSsid(newSSID.c_str());
			assocmsg->setMacAddress(getAssocAPMacAddress());
			assocmsg->setNewMAC(getNewAPMac());
			scheduleAt(simTime()+delay, assocmsg);
    	}

    }
    updateDisplay();
}

void DDEClient::scheduleAssociation()
{
	module = getParentModule()->getSubmodule("wlan",0)->getSubmodule("agent");
	agent = check_and_cast<Ieee80211AgentSTA*> (module);

	//agent->doDisassociate(getAssocAPMacAddress());

	std::string newSSID = getNewAPssid();
	double delay = uniform(0.1,1.0);
	std::cout << "delay: " << delay << endl;
	ClientInternalMsg *assocmsg = new ClientInternalMsg("INTERNAL_ASSOCIATE_MSG ", INTERNAL_ASSOCIATE_MSG );
	assocmsg->setSsid(newSSID.c_str());
	assocmsg->setMacAddress(getAssocAPMacAddress());
	assocmsg->setNewMAC(getNewAPMac());
	scheduleAt(simTime()+delay, assocmsg);

}

void DDEClient::receiveChangeNotification(int category, const cPolymorphic *details)
{
    Enter_Method_Silent();
    printNotificationBanner(category, details);

    //3 notifications are generated from the DHPC client, 2 with details, one without details
    //this is also called when DHCP lease time expires --> set lease larger

    // host associated. Link is up. DHCP has assigned an IP address.
    if (category == NF_INTERFACE_IPv4CONFIG_CHANGED && simTime() > 0)
    {
        if (!details)
        {

        	double delay = 0.0;
        	if(isDownloadStarted_var == false)
        	{
        		delay = startTime_var;

        	}
        	else
        	{
        		double handoverDelay = 0.0;
        		handoverDelay = SIMTIME_DBL(simTime() - getHandoverStart());
        		handoverTime.insert(handoverTime.end(), handoverDelay);
        		std::cerr << "HANDOVER DELAY: " << handoverDelay << endl;
        		stats->recordHandoverTime(handoverDelay);

        		//error("HANDOVER DELAY");

        	}

        	getAPSignalStrengths();

        	BEV << "Interface has IP, starting application: " << simTime() << endl;
        	std::cout << getParentModule() << ": Interface has IP, starting application: " << simTime() << ". Delay: " << delay << endl;
        	std::cout << "socketmap size: " << socketMap.size() << endl;
        	std::cout << "getAPThread(): " << getAPThread() << endl;

        	//FIXME: socketMap should be empty when receiving a notification from the DHCP
        	if(socketMap.size() == 0)
        	{
				TCPServerThreadBase* apThread = getAPThread();

				if(apThread == 0)
				{

					//Connect to the AP
					ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
					// Get associated AP info
					//msg->setConnectAddress(par("connectAddressInterface1"));
					msg->setConnectAddress(getAssocApSSID());
					msg->setInterfaceID(0);
					msg->setConnectToAp(true); //connecting to AP
					msg->setConnectToFileServer(false);
					scheduleAt(simTime()+(simtime_t)delay, msg);

					// Do not connect to infoServer if fuzzy is not enabled
					//if(getFuzzy() == true)
					//{
						//Connect to the InfoServer
						ClientInternalMsg *infomsg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
						// Get associated AP info
						//msg->setConnectAddress(par("connectAddressInterface1"));
						infomsg->setConnectAddress("infoServer");
						infomsg->setInterfaceID(0);
						infomsg->setConnectToAp(false);
						infomsg->setConnectToFileServer(false);
						scheduleAt(simTime(), infomsg);
					//}
				}
				else
				{
					std::cout << "state: " << apThread->getSocket()->getState() << endl;
					if(apThread->getSocket()->getState() == TCPSocket::SOCKERROR)
					{
						std::cout << "Socket ERROR!" << endl;
						//error("Socket ERROR!");
					}

					else if(apThread->getSocket()->getState() == TCPSocket::CONNECTING)
					{
						error("Duplicate DHCP ack received!");
					}
					else
					{

						//Connect to the AP
						ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
						// Get associated AP info
						//msg->setConnectAddress(par("connectAddressInterface1"));
						msg->setConnectAddress(getAssocApSSID());
						msg->setInterfaceID(0);
						msg->setConnectToAp(true); //connecting to AP
						msg->setConnectToFileServer(false);
						scheduleAt(simTime()+(simtime_t)delay, msg);

						// Do not connect to infoServer if fuzzy is not enabled
						//if(getFuzzy() == true)
						//{
							//Connect to the InfoServer
							ClientInternalMsg *infomsg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
							// Get associated AP info
							//msg->setConnectAddress(par("connectAddressInterface1"));
							infomsg->setConnectAddress("infoServer");
							infomsg->setInterfaceID(0);
							infomsg->setConnectToAp(false);
							infomsg->setConnectToFileServer(false);
							scheduleAt(simTime(), infomsg);
						//}
					}
				}
        	}
        	else if (closeTcpConnections() == false)
        	{
        		setAPThread(0);

				//Connect to the AP
				ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );
				// Get associated AP info
				msg->setConnectAddress(getAssocApSSID());
				msg->setInterfaceID(0);
				msg->setConnectToAp(true); //connecting to AP
				msg->setConnectToFileServer(false);
				scheduleAt(simTime()+(simtime_t)delay, msg);


				//Connect to the InfoServer
				ClientInternalMsg *infomsg = new ClientInternalMsg("INTERNAL_INIT_CONNECTION_MSG ", INTERNAL_INIT_CONNECTION_MSG );;
				infomsg->setConnectAddress("infoServer");
				infomsg->setInterfaceID(0);
				infomsg->setConnectToAp(false);
				infomsg->setConnectToFileServer(false);
				scheduleAt(simTime(), infomsg);

        	}
        	else
        	{
        		//error("ERROR in notifications");
        		std::cerr << "ERROR in notifications. Do nothing." << endl;
        	}

        }

    }

}
/**
 * Determines whether to do a handover
 */

bool DDEClient::doHandover()
{
	bool changeAP = false;

	// Get information about current access and the best other access



	// Pure Q-learning
	if(getpureQLearning() == true)
	{
		std::cerr << "Pure Q-learning is used" << endl;
		double ownSS = 0;
		double ownLoss = assocAP.packetLoss;
		int ownUsers = assocAP.numUsers;
		double bestSS = 0;
		double bestLoss = bestOtherAp.packetLoss;
		double bestUsers = bestOtherAp.numUsers;

		changeAP = feedbackPureAgent(ownSS, ownLoss, ownUsers, bestSS, bestLoss, bestUsers);

	}
	// Fuzzy based Q-learning
	else if(getLearning() == true)
	{
		int assocAPStatus = getFuzzyState(assocAP.nsClass);
		int bestAPStatus = getFuzzyState(bestOtherAp.nsClass);
		int assocAPUsers = assocAP.numUsers;
		int bestAPUsers = bestOtherAp.numUsers;
		std::cerr << "Fuzzy based Q-learning is used" << endl;

		changeAP = feedbackAgent(assocAPStatus, assocAPUsers, bestAPStatus, bestAPUsers);
	}

	// Deterministic decision
	else if(getpureQLearning() == false && getpureQLearning() == false && getFuzzy() == true && getLoadBalancing() == false)
	{
		std::cout << "Deterministic decision is used" << endl;
		//if(assocAP.nsClass < 2.0 && bestOtherAp.nsClass > 3.0 && bestOtherAp.classifyValue > 3.0)  && getSessionLock() == false
		if(assocAP.nsClass < 2.0 && bestOtherAp.nsClass > 2.0 && getDecisionLock() == false)
		{
			std::cout << "assocAP PoA Grade: " << assocAP.nsClass << endl;
			std::cout << "best PoA status: " << bestOtherAp.classifyValue << endl;
			std::cout << "best PoA grade: " << bestOtherAp.nsClass << endl;
			std::cerr << "Changing to best other AP." << endl;
			changeAP = true;
		}
		else
		{
			std::cout << "assocAP PoA Grade: " << assocAP.nsClass << endl;
			std::cout << "best PoA status: " << bestOtherAp.classifyValue << endl;
			std::cout << "best PoA grade: " << bestOtherAp.nsClass << endl;
			std::cout << "NOT Changing to best other AP." << endl;
		}
	}

	if(changeAP == true)
	{
		// Set handover start
		setHandoverStart(simTime());

		//timestep for starting the change c_str()
		setChangeStart(simTime());

		setNewAPssid(bestOtherAp.ID);
		setNewAPMac(bestOtherAp.macAddress);

		addHandover(false);

		//Suspend AP
		suspendAP(getAssocApSSID());
	}

	return changeAP;

}

//Gets the signal strength from the radio model

void DDEClient::getSignalStrength(float &signalStrength, float &rssi)
{

	module = getParentModule()->getSubmodule("wlan",0)->getSubmodule("radio");
	radio = check_and_cast<AbstractRadioExtended*> (module);


	radio->getSignalStrength(signalStrength, rssi);

//	std::cout << "getSignalStrength Signal strength: " << signalStrength << endl;
//	std::cout << "getSignalStrength rssi: " << rssi << endl;

    if(signalStrength < 0)
    	signalStrength = 0;


	return;
}

//Calculates an estimated signal strength based on distance
void DDEClient::getEstimatedSignalStrength(const char* ssid, float &signalStrength, float &rssi)
{

//	float signalStrength = 0.0;
//	float rssi = 0.0;

	// Get my position
	cModule* hostMod = getParentModule()->getSubmodule("wlan",0)->getSubmodule("radio");
	AbstractRadioExtended* hostRadio = check_and_cast<AbstractRadioExtended*> (hostMod);
    const Coord& myPos = hostRadio->getPosition();

    // Get AP position
	cModule* apMod = getParentModule()->getParentModule()->getSubmodule(ssid)->getSubmodule("wlan",0)->getSubmodule("radio");
	AbstractRadioExtended* apRadio = check_and_cast<AbstractRadioExtended*> (apMod);
    const Coord& apPos = apRadio->getPosition();

    //Calculate the distance
    double distance = myPos.distance(apPos);

    hostRadio->getEstimatedSignalStrength(distance, signalStrength, rssi);

    //std::cout << "SSID: " << ssid << ",signal strength: " << signalStrength << ", distance: " << distance << endl;

    if(signalStrength < 0)
    	signalStrength = 0;

    return;
}

//Used for debugging
void DDEClient::getAPSignalStrengths()
{
	float ss = 0.0;
	float sum = 0.0;
	float average = 0.0;
	const char* apSSID;
	float rssi = 0.0;


	for(int k = 0; k < 9; k++)
	{
		ss = 0.0;
		sum = 0.0;
		average = 0.0;

		if(k == 0)
		{
			apSSID = "ap1";
		}
		else if(k == 1)
		{
			apSSID = "ap2";
		}
		else if(k == 2)
		{
			apSSID = "ap3";
		}
		else if(k == 3)
		{
			apSSID = "ap4";
		}
		else if(k == 4)
		{
			apSSID = "ap5";
		}
		else if(k == 5)
		{
			apSSID = "ap6";
		}
		else if(k == 6)
		{
			apSSID = "ap7";
		}
		else if(k == 7)
		{
			apSSID = "ap8";
		}
		else if(k == 8)
		{
			apSSID = "ap9";
		}

		for(int i = 0; i < 100 ; i++)
		{
			getEstimatedSignalStrength(apSSID, ss, rssi);
			sum += (ss * 100);
		}
		average = sum/100;
		// Get my position
		cModule* hostMod = getParentModule()->getSubmodule("wlan",0)->getSubmodule("radio");
		AbstractRadioExtended* hostRadio = check_and_cast<AbstractRadioExtended*> (hostMod);
		const Coord& myPos = hostRadio->getPosition();

		// Get AP position
		cModule* apMod = getParentModule()->getParentModule()->getSubmodule(apSSID)->getSubmodule("wlan",0)->getSubmodule("radio");
		AbstractRadioExtended* apRadio = check_and_cast<AbstractRadioExtended*> (apMod);
		const Coord& apPos = apRadio->getPosition();

		//Calculate the distance
		double distance = myPos.distance(apPos);

		std::cout << "SSID: " << apSSID << ", Signal strength average: " << average << ", distance: " << distance  << ", position: " << apPos << endl;
	}

}

void DDEClient::getMNclassification(int interfaceID, float &signalStrength, float &mnClassification, float &rssi)
{
	int mn = 1;
	float ss = 0.0;
	float ec = 0.0;
	float bl = 0.0;
	float classifyValue = 0.0;

//	arg1 = signal strength 0..100
//	arg2 = interface energy consumption  0..100
//	arg3 = battery level 0..100

	//getSignalStrength(ss, rssi);

	ec = 100;
	bl = 100;


	ss = 0.0;

	float averageSS = 0.0;
	float sumSS = 0.0;
	float sumRSSI = 0.0;
	float averageRSSI = 0.0;

	for(int i = 0; i < 100 ; i++)
	{
		getEstimatedSignalStrength(getAssocApSSID(), ss, rssi);
		sumSS += ss;
		sumRSSI += rssi;
		ss = 0.0;
		rssi = 0.0;
	}
	averageSS = sumSS/100;
	averageRSSI = sumRSSI/100;

	ss = averageSS;
	rssi = averageRSSI;


	ss = ss * 100;

//	std::cout << "Signal strength function: " << ss << endl;
//	std::cout << "rssi function: " << rssi << endl;

	//TODO:nsfis
	classifyValue = nsfis(mn,ss,ec,bl);
	//classifyValue = 4.0;

	signalStrength = ss;
	mnClassification = classifyValue;

	setSignalStrengthValue(ss);
	setRSSIvalue(rssi);


	return;

}

void DDEClient::getNoiseLevel(float &sensitivityValue, float &noiseLevelDBM)
{

	module = getParentModule()->getSubmodule("wlan",0)->getSubmodule("radio");
	radio = check_and_cast<AbstractRadioExtended*> (module);


	radio->getNoiseLevel(sensitivityValue, noiseLevelDBM);

	return;

}

void DDEClient::setSignalStrengthValue(double ss)
{
	ss_var = ss;
}

double DDEClient::getSignalStrengthValue()
{
	return ss_var;
}

void DDEClient::setRSSIvalue(double rssi)
{
	rssi_var = rssi;
}

double DDEClient::getRSSIvalue()
{
	return rssi_var;
}


void DDEClient::sendScanRequest(const char* ssid, const MACAddress& oldMAC, const MACAddress& newMAC)
{
	module = getParentModule()->getSubmodule("wlan",0)->getSubmodule("agent");
	agent = check_and_cast<Ieee80211AgentSTA*> (module);

	std::cout << " send scan: " << ssid << endl;
	std::cout << " new mac: " << newMAC << endl;
	std::cout << " old mac: " << oldMAC << endl;

	//TODO: choose either way

	if(handoverScan == true)
	{

		agent->setDefaultSSID(ssid);
		agent->doScanRequest(oldMAC);
	}
	else
	{
		agent->doAuthenticate(oldMAC, newMAC);
	}


}

const char* DDEClient::getAssocApSSID()
{
	module = getParentModule()->getSubmodule("wlan",0)->getSubmodule("mgmt");
	mgmt = check_and_cast<Ieee80211MgmtSTA*> (module);

	return mgmt->getAssocApSSID();
}

int DDEClient::getFuzzyState(float fuzzyValue)
{

	int state;
	if(fuzzyValue < 2)
	{
		state = BAD;
	}
	else if((2 <= fuzzyValue) && (fuzzyValue < 3))
	{
		state = REGULAR;
	}
	else if((3 <= fuzzyValue) && (fuzzyValue < 4))
	{
		state = GOOD;
	}
	else if((4 <= fuzzyValue) && (fuzzyValue <= 5))
	{
		state = EXCELLENT;
	}
	else
	{
		state = GOOD;
		//error("invalid fuzzy value!");
	}

	return state;
}

const char* DDEClient::getFuzzyStateLiteral(int state)
{
	const char* string;
	if(state == BAD)
	{
		string = "BAD";
	}
	else if(state == REGULAR)
	{
		string = "REGULAR";
	}
	else if(state == GOOD)
	{
		string = "GOOD";
	}
	else if(state == EXCELLENT)
	{
		string = "EXCELLENT";
	}

	return string;
}

//Gives feedback on the change to the RL agent
//void DDEClient::feedbackOnChange(int state)
//{
//	simtime_t End = getChangeEnd();
//	simtime_t Start = getChangeStart();
//	double delay = 0.0;
//	int users = getAssocAPUsers() - 1; // -1 to omit the client itself
//	double bandwidth = getTotalBandwidth();
//
//	cModule* agentMod = getParentModule()->getSubmodule("rlAgent");
//	RLagent* agent = check_and_cast<RLagent*> (agentMod);
//
//	delay = (End - Start).dbl();
//
//	agent->feedbackOnChange(state, bandwidth, users, delay);
//
//}

bool DDEClient::feedbackAgent(int ownState, int ownUsers, int bestState, int bestUsers)
{
	bool changeAP;
	double reward;
	bool video;

	if(getApplicationType() == FTP_APP)
	{
		reward = getTotalBandwidth();
		video = false;
	}
	else if(getApplicationType() == VIDEO_APP)
	{
		if(pauseEvent == false)
		{
			reward = (double)getPlayBufferSize();
		}
		// Video is paused, negative reward
		// reward = reward/100
		else
		{
			reward = -1000;
		}

		video = true;
	}
	//int users = getAssocAPUsers() - 1; // -1 to omit the client itself
	cModule* agentMod = getParentModule()->getSubmodule("rlAgent");
	RLagent* agent = check_and_cast<RLagent*> (agentMod);

	// Load balancing is in use, check if there is lock on the decision
	if(getLoadBalancing() == true)
	{

		changeAP = agent->giveFeedback(ownState, ownUsers, bestState, bestUsers, reward, video, getDecisionLock()); //getSessionLock()
		if(getDecisionLock() == true)
		{
			std::cerr << "Decision lock is active." << endl;
			std::cerr << "ChangeAP: " << changeAP << endl;

		}
	}
	// Else: no decision lock is used
	else
	{
		changeAP = agent->giveFeedback(ownState, ownUsers, bestState, bestUsers, reward, video, false); //getSessionLock()
	}


	return changeAP;

}

bool DDEClient::feedbackPureAgent(double ownSS, double ownLoss, int ownUsers, double bestSS, double bestLoss, int bestUsers)
{
	bool changeAP;
	double bandwidth = getTotalBandwidth();
	//int currentUsers = getAssocAPUsers() - 1; // -1 to omit the client itself
	cModule* agentMod = getParentModule()->getSubmodule("rlAgent");
	RLagent* agent = check_and_cast<RLagent*> (agentMod);

	if(getApplicationType() == FTP_APP)
	{
		changeAP = agent->givePureFeedback(ownSS, ownLoss, ownUsers, bestSS, bestLoss, bestUsers, bandwidth, false);
	}
	else if(getApplicationType() == VIDEO_APP)
	{
		double bufferSize = (double)getPlayBufferSize();
		changeAP = agent->givePureFeedback(ownSS, ownLoss, ownUsers, bestSS, bestLoss, bestUsers, bufferSize, true);
	}


	return changeAP;

}

double DDEClient::computeDownloadThruput()
{
	double downloadThruput = 0.0;
	cModule *moduleDL;

	moduleDL = getParentModule()->getSubmodule("wlan",0)->getSubmodule("DLthruputMeter");
	ThruputMeter *DL = check_and_cast<ThruputMeter*> (moduleDL);
	downloadThruput = DL->getThruput();
	//downloadThruput = DL->getTotalThruput();

	return downloadThruput;

}

// ----------- VIDEO METHODS ---------------
/*
 ** This function creates a timer for enabling PLAY event
 */
void DDEClient::createPlayTimer()
{

		cMessage *playTimer = new cMessage("playContent", MK_PLAY_CONTENT);
		playerControl.push_back(playTimer);
		EV<<"Timer "<<playTimer->getName()<<" of type "<<playTimer->getKind()<<" created at time "<<simTime()<<endl;
		EV<<"Scheduling the Play_Timer msg to trigger at"<<simTime()+playRate<<endl;
		scheduleAt(simTime()+playRate, playTimer);

//		// OM add: generate a cMessage for starting the agent
//		cMessage *startAgent = new cMessage("startAgent", MK_START_AGENT_MSG);
//		EV <<"Informing the FS4VP module that client is creating a playTimer. " << endl;
//		cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
//		cliAppContext -> setPortId(port);
//		cliAppContext->setUserClass(userClass); // Set user class
//		startAgent->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
//		sendDirect(startAgent, fs4vp, "directInput"); //sending info to the FS4VP module

		return;
}

/*
 ** This function creates a timer for enabling PAUSE event
 */
void DDEClient::createPauseTimer()
{
	EV<<"Creating timer for the PAUSE event"<<endl;
	cMessage *pauseTimer = new cMessage("pauseContent",MK_PAUSE_CONTENT); //creating a PAUSE_CONTENT_TIMER
	playerControl.push_back(pauseTimer); // Pushing the timer message into the PlayerControl vector
	EV<<"Timer "<<pauseTimer->getName()<<" of type "<<pauseTimer->getKind()<<" created at time "<<simTime()<<endl;
	EV<<"Scheduling the Pause_Timer msg to trigger at"<<simTime()+pauseRate<<endl;
	scheduleAt(simTime()+pauseRate, pauseTimer); //initiating the timer for the PAUSE event
	//return;
}

/*
 ** This function models the PAUSE event of a video player.
 ** The function will PAUSE till the play buffer fills up
 ** to the minimum number of content chunks
 */
void DDEClient::pauseContent()
{
	EV <<"Routine for PAUSE content"<<endl;

	//PAUSE event condition check
	//if(playBuffer.length() < minAdvBufferSize)
	if(playBuffer.length() < minAdvBufferSize && (bytesRcvd < getFileSize()))
	{
		EV<<"Play Buffer still below minAdvBufferSize of: "<<minAdvBufferSize<<endl;
		//re-scheduling the pauseTimer
//		for(int x=0; x < playerControl.size(); x++)
//			{
//				EV<<"Iter # "<< x <<": PlayControl Vector has timer message:" <<playerControl[x]->getKind()<<endl;
//				if(playerControl[x]->getKind()== MK_PAUSE_CONTENT)
//			  			scheduleAt(simTime()+pauseRate, playerControl[x]);
//			}
	}
	else
	{

		if(bytesRcvd < getFileSize())
			ASSERT(playBuffer.length() >= minAdvBufferSize);

		//ASSERT(playBuffer.length() >= minAdvBufferSize);

		//Buffer has reached the minAdvBufferSize threshold
		//1- cancel the timer for the PAUSE event
		EV<<"Canceling the PAUSE event !!"<<endl;// <<playerControl[x]->getKind()<<endl;
		pauseEvent = false; //seeting the pause event to false
		playBreakVec.record(1.0); //statistics

		double starvationDuration = 0.0;
		starvationDuration = SIMTIME_DBL(simTime() - starvationStart);
		starvationDurationVector.insert(starvationDurationVector.end(), starvationDuration);

		stats->addStarvationDuration(starvationDuration, getFuzzy(), getUserClass());

//		for(int x=0; x < playerControl.size(); x++)
//			{
//				if(playerControl[x]->getKind()== MK_PAUSE_CONTENT)
//			  			cancelEvent(playerControl[x]);
//			}
		//2- re-schedule the timer for the PLAY event
		EV<<"Immediately initiating the PLAY event !!"<<endl; //:" <<playerControl[x]->getKind()<<endl;
		createPlayTimer();
//		for(int x=0; x < playerControl.size(); x++)
//					{
//						if(playerControl[x]->getKind()== MK_PLAY_CONTENT)
//				  			scheduleAt(simTime(), playerControl[x]); //Immidiately start the PLAY event
//					}
	}
	return;
}

void DDEClient::determinePlayBufferState(int _playBufferSize)
{
	EV<<"Routine for determining PalyBuffer State!"<<endl;
	EV<<"Current Size of Play Buffer is: "<<_playBufferSize<<endl;

	//std::cout <<"Current Size of Play Buffer is: "<<_playBufferSize<<endl;

	if(_playBufferSize < threshYO && _playBufferSize !=0)
	{
		playBufferState = ORANGE;
		//currentBufferState = playBufferState; //set the currentBufferState to teh latest state
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);

//		std::cout << getParentModule()->getName() << " " << simTime() << endl;
//		std::cout << "PLay buffer state is ORANGE." << endl;
	}
	//OM: added >= and <=
	else if(_playBufferSize >= threshYO && _playBufferSize <= threshGY)
	{
		playBufferState = YELLOW;
		//currentBufferState = playBufferState; //set the currentBufferState to teh latest state
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);
//		std::cout << getParentModule()->getName() << " " << simTime() << endl;
//		std::cout << "PLay buffer state is YELLOW." << endl;
	}

	else if (_playBufferSize > threshGY)
	{
		playBufferState = GREEN;
		//currentBufferState = playBufferState; //set the currentBufferState to teh latest state
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);
//		std::cout << getParentModule()->getName() << " " << simTime() << endl;
//		std::cout << "PLay buffer state is GREEN." << endl;
	}

	else if (_playBufferSize < minAdvBufferSize || _playBufferSize == 0) //we could use the minAdvBufferSize as an criticial alarm that the system is about to get starved
	{
		playBufferState = RED; //i.e., buffer starvation
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);

//		std::cout << getParentModule()->getName() << " " << simTime() << endl;
//		std::cout << "PLay buffer state is RED." << endl;
	}

	return;
}

void DDEClient::verifyBufferStateTransition(int _playBufferState)
{
	EV<<"Verifying Buffer State Transition and trigger generation !"<<endl;

	if(_playBufferState == currentBufferState) //check if there has been a state change (or state transition)
		EV<<"No change in State of PlayBuffer"<<endl;
	else
	{


		EV<<"State Transition from PREV_STATE:"<<currentBufferState<<" to CURR_STATE:"<<playBufferState <<endl;
		//========= Generating StateTransition trigger towards the FS4VP ===================================
		//OM: Send only to fsvp if all content has not been delivered yet.
//		if(contentDelivered == false)
//		{
//			cMessage *bufferStatus = new cMessage("playBufferStatus", MK_BUFFER_STATUS_MSG);
//			cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
//			cliAppContext -> setPortId(port);
//			cliAppContext -> setPlayBufferSize(msgsRcvd_var);
//			cliAppContext -> setPlayBufferState(_playBufferState);
//			bufferStatus->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
//			//bufferStatus-> addPar("bytesRxed").setLongValue(segmentsRcvd);
//			EV<<"Sending PlayBuffer State Transition Trigger Notification !!"<<endl;
//			sendDirect(bufferStatus, fs4vp, "directInput"); //sending buffer status to the FS4VP module
//		}
		currentBufferState = playBufferState; //set the currentBufferState to teh latest state
	}
	return;
}

/*
 ** This function models the playing of a progressively video downloaded content
 */
void DDEClient::playContent()

{
	EV<<"Start PLAY at time "<<simTime()<<" and creating the timer\n"<<endl;
	if(!playBuffer.isEmpty())// && (playBuffer.length() > minAdvBufferSize))
	{
    	EV <<"Playing the Content - Popping video packet from the playBuffer)";
//	    	playBufferSizeVec.record(playBuffer.getLength());


    	cPacket *pkt = PK(playBuffer.pop()); //returns the pointer to the first element in the queue
    	EV<<"About to delete packet with pointer value"<< pkt <<" id:"<<pkt->getId()<<"timestamp: "<<pkt->getTimestamp()<<"pointerId: "<<&pkt<<endl;


    	if(pkt->getByteLength() != 1452)
    		error("Packet not 1452 bytes when playing content!");

    	delete pkt;


    	playBufferSize--;
   	 	playBufferSizeVec.record(playBufferSize);

   	 	bufferSizeVector.insert(bufferSizeVector.end(), playBufferSize);
   	 	determinePlayBufferState(playBufferSize); //determine the state of the playbuffer (RED, YELLOW or GREEN)



   	 	return;
	}
	else if(playBuffer.isEmpty() && (bytesRcvd < getFileSize()))//if(!playBuffer.isEmpty() && (playBuffer.length() <= minAdvBufferSize))
	{

		EV <<"Buffer Starvation - PAUSE !!"<<endl;
		// OM: Count the number of buffer starvations
		numberOfBufferStarvations++;
		//std::cout << "Buffer Starvation: " << getParentModule()->getName() << " " << simTime() << endl;
		/*
		 ** 1- Canceling the Play_Content Timer,
		 */
//		for(int x=0; x < playerControl.size(); x++)
//	    	{
//	    		EV<<"Iter # "<< x <<": PlayControl Vector has timer message:" <<playerControl[x]->getKind()<<endl;
//	    		if(playerControl[x]->getKind()== MK_PLAY_CONTENT)
//	    		{
//	    			EV<<"Canceling PLAY event timer "<< playerControl[x]->getName() <<" of type: "<<playerControl[x]->getKind()<<" scheduled at "<<playerControl[x]->getArrivalTime()<<endl;
//	    			if(playerControl[x]->isScheduled())
//	    				EV << "It was scheduled"<<endl;
//	    			if(playerControl[x]->isScheduled())
//	    				if(cancelEvent(playerControl[x]) == playerControl[x])
//	    					EV << "It was really canceled"<<endl;
//	    			//DEBUG:
//	    			ASSERT(playerControl[x]!=0); //to make sure that it is not a NULL pointer
//
//	    			if(playerControl[x]->isScheduled())
//	    				EV<<"PLAY timer is still scheduled at "<<playerControl[x]->getArrivalTime()<<" and is not cancelled"<<endl;
//	    			else
//	    				EV<<"PLAY timer is canceled successfully and is scheduled at "<<playerControl[x]->getArrivalTime()<<endl;
//	    			break;
//	    		}
//	    		EV<<"DEBUG: For-loop break exit"<<endl;
//	    	}
	    	/*
	    	 ** 2 - Schedule the timer for the PAUSE event
	    	 */

            pauseEvent = true;
            playBreakVec.record(0.0); //statistics

            starvationStart = simTime(); // Start time of the starvation

            createPauseTimer();
	}
	else //all the content has been delivered
	{
		EV<<"Content delivered - canceling and deleting PLAY/PAUSE timers!!"<<endl;
		playFinished = true;
		recordPlayTime();
		recordDownloadTime();

		//std::cout << "Client: " << getParentModule()->getName() << ", content delivered: " << simTime() << endl;


		//cancelAndDelete()
//		if(contentDelivered == false)
//		{
//			// OM add: generate a cMessage for informing the agent that the content has been delivered
//			cMessage *finishAgent = new cMessage("finishAgent", MK_FINISH_AGENT_MSG);
//			EV <<"Informing the FS4VP module that client's content has been delivered. " << endl;
//			cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
//			cliAppContext -> setPortId(port);
//			finishAgent->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
//			sendDirect(finishAgent, fs4vp, "directInput"); //sending info to the FS4VP module
//
//			contentDelivered = true;
//		}
	}
}

void DDEClient::updatePlayBuffer(GenericAppMsg* appmsg)
{
    playBuffer.insert(appmsg); //inserting the arriving content chunk in the playbuffer
    playBufferSize++; //incrementing the size stat
    segmentsRcvd++;
    segmentsReceivedVec.record(segmentsRcvd);
    bytesRcvd += PK(appmsg)->getByteLength();
    playBufferSizeByteVec.record(bytesRcvd);
    determinePlayBufferState(playBufferSize);

    setRemainingFileSize(PK(appmsg)->getByteLength());


    //as soon as the play buffer gets filled with initial number of advance packets, the app should start playing it
	if(segmentsRcvd == initAdvBufferSize)
	{
		EV<<"### CREATING Play Timer ###"<<endl;
		createPlayTimer(); //create the self-msg (timer) that will periodically trigger the app to pop out packets from teh playbuffer
		//std::cout << "Client: " << getParentModule()->getName() << " Creating play timer: " << simTime() << endl;
		playStarted = true;
		playTimeStart = simTime();
	}

}

bool DDEClient::getPlayStarted()
{
	return playStarted;
}

//----------- VIDEO METHODS END-------------------

double DDEClient::getDownloadThruput()
{
	return dlThruput_var;

}

void DDEClient::setDownloadThruput(double thruput)
{
	dlThruput_var = thruput;

}

double DDEClient::computeUploadThruput()
{
	double uploadThruput = 0.0;
	cModule *moduleUL;

	moduleUL = getParentModule()->getSubmodule("wlan",0)->getSubmodule("ULthruputMeter");
	ThruputMeter *UL = check_and_cast<ThruputMeter*> (moduleUL);
	uploadThruput = UL->getThruput();
	//downloadThruput = DL->getTotalThruput();

	return uploadThruput;

}

double DDEClient::getUploadThruput()
{
	return ulThruput_var;

}

void DDEClient::setUploadThruput(double thruput)
{
	ulThruput_var = thruput;

}

double DDEClient::getTotalBandwidth()
{
	return totalBW_var;

}

void DDEClient::setTotalBandwidth(double value)
{
	totalBW_var = value;
	TotalBW_vec.insert(TotalBW_vec.end(), value);
	if(debuggedNode == true)
	{
		totalBW_vector.record(value);
	}


}

double DDEClient::getAverageTotalBandwidth()
{
	double sum = 0.0;
	double avgTotalBandwidth = 0.0;

	for(int i = 0; i < (int) TotalBW_vec.size(); i++)
	{
		sum = sum + TotalBW_vec[i];
	}

	if(sum != 0)
	{
		avgTotalBandwidth = sum/(TotalBW_vec.size());
	}
	else
	{
		avgTotalBandwidth = 0;
	}

	return avgTotalBandwidth;
}

double DDEClient::getAverageDelay()
{
	double sum = 0;
	double avgDelay = 0.0;

	for(int i = 0; i < (int) requestDelayVector.size(); i++)
	{
		sum = sum + requestDelayVector[i];
		//std::cout << "avgTotalBW  : " << TotalBW_vec[i] << endl;
	}

	if(sum != 0)
	{
		avgDelay = sum/(requestDelayVector.size());
	}
	else
	{
		avgDelay = 0;
	}

	return avgDelay;
}

void DDEClient::addAPinfo(std::string ID, float classifyValue, int numUsers, float load, float packetLoss, IPvXAddress apAddr, bool cachedContent, float ns, float mn, const MACAddress& macAddress, float rssi)
{

	assocAP.ID = ID;
	assocAP.classifyValue = classifyValue;
	assocAP.apAddress = apAddr;
	assocAP.numUsers = numUsers;
	assocAP.load = load;
	assocAP.packetLoss = packetLoss;
	assocAP.cachedContent = cachedContent;
	assocAP.nsClass = ns;
	assocAP.mnClass = mn;
	assocAP.macAddress = macAddress;
	assocAP.rssi = rssi;
}

void DDEClient::addBestOtherApinfo(std::string ID, float classifyValue, int numUsers, float load, float packetLoss,
		IPvXAddress apAddr, bool cachedContent, float ns, const MACAddress& macAddress)
{

	bestOtherAp.ID = ID;
	bestOtherAp.classifyValue = classifyValue;
	bestOtherAp.apAddress = apAddr;
	bestOtherAp.numUsers = numUsers;
	bestOtherAp.load = load;
	bestOtherAp.packetLoss = packetLoss;
	bestOtherAp.cachedContent = cachedContent;
	bestOtherAp.nsClass = ns;
	bestOtherAp.mnClass = -1;
	bestOtherAp.macAddress = macAddress;
	bestOtherAp.rssi = -1;
}


void DDEClient::addAPListInfo(std::string ID, IPvXAddress apAddr)
{
	apInfo *ap = lookupAP(ID);
	if(!ap)
	{
		apInfo *ap;
		apList.push_back(apInfo());
		ap = &apList.back();
		ap->ID = ID;
		ap->apAddress = apAddr;
	}
	else
	{
		ap->ID = ID;
		ap->apAddress = apAddr;
	}
}

//Looks up AP from the client list
DDEClient::apInfo *DDEClient::lookupAP(std::string ID)
{
    for (APList::iterator it=apList.begin(); it!=apList.end(); ++it)
        if (it->ID == ID)
            return &(*it);
    return NULL;
}

void DDEClient::suspendAP(std::string ID)
{
	apInfo *ap = lookupAP(ID);
	ap->suspended = true;

	//Schedule a timer
	ClientInternalMsg* suspendTimeout = new ClientInternalMsg("suspendTimeoutServer", INTERNAL_SUSPEND_TIMEOUT);
	suspendTimeout->setSsid(ID.c_str());
	scheduleAt(simTime() + (simtime_t)getSuspendTimeout(), suspendTimeout);
}

bool DDEClient::isAPSuspended(std::string ID)
{
	apInfo *ap = lookupAP(ID);
	if(ap)
		return ap->suspended;
	else
		return false;
}

void DDEClient::unSuspendAP(std::string ID)
{
	apInfo *ap = lookupAP(ID);
	ap->suspended = false;
}

float DDEClient::getAssocAPrssi()
{
	return assocAP.rssi;
}
float DDEClient::getAssocAPclassification()
{
	return assocAP.classifyValue;
}

float DDEClient::getAssocAPNSclassification()
{
	return assocAP.nsClass;
}

float DDEClient::getAssocAPMNclassification()
{
	return assocAP.mnClass;
}

int DDEClient::getAssocAPUsers()
{
	return assocAP.numUsers;
}

double DDEClient::getAssocAPLoad()
{
	return assocAP.load;
}

double DDEClient::getAssocAPLoss()
{
	return assocAP.packetLoss;
}


MACAddress DDEClient::getAssocAPMacAddress()
{
	return assocAP.macAddress;
}

void DDEClient::recordDownloadTime()
{
	double downloadTime = (simTime()-downloadStart).dbl();
	setDownloadTime(downloadTime);

	stats->addDownloadTime(downloadTime, getFuzzy(), getApplicationType());
}

double DDEClient::getDownloadTime()
{
	return downloadTime_var;
}

void DDEClient::setDownloadTime(double time)
{
	downloadTime_var = time;
}

void DDEClient::recordPlayTime()
{
	double playTime = (simTime()-playTimeStart).dbl();
	setPlayTime(playTime);

	//stats->addDownloadTime(downloadTime, getFuzzy());
}

double DDEClient::getPlayTime()
{
	return playTime_var;
}

void DDEClient::setPlayTime(double time)
{
	playTime_var = time;
}

void DDEClient::addHandover(bool infoServerRecommendation)
{
	numberOfHandovers++;
	stats->addHandover(infoServerRecommendation);
}

std::string DDEClient::getNewAPssid()
{
	return newSSID;
}

void DDEClient::setNewAPssid(std::string ssid)
{
	newSSID = ssid;
}

MACAddress DDEClient::getNewAPMac()
{
	return newMAC;
}

void DDEClient::setNewAPMac(MACAddress MAC)
{
	newMAC = MAC;
}

void DDEClient::setSessionLock(bool lock)
{
	sessionLock_var = lock;
}

bool DDEClient::getSessionLock()
{
	return sessionLock_var;
}

void DDEClient::setDecisionLock(bool lock)
{
	decisionLock_var = lock;
}

bool DDEClient::getDecisionLock()
{
	return decisionLock_var;
}

double DDEClient::getBandwidthLimit()
{
	return bandwidthLimit_var;
}

// VIDEO

double DDEClient::getTxRate()
{
	return txRate;
}

int DDEClient::getChunkSize()
{
	return chunkSize;
}

int DDEClient::getRemainingFileSize()
{
	return remainingFileSize;
}

void DDEClient::setRemainingFileSize(int value)
{
	remainingFileSize -= value;
}


long DDEClient::getPlayBufferSize()
{
	return playBufferSize;
}

int DDEClient::getInitAdvBufferSize()
{
	return initAdvBufferSize;
}

/**
 * Gets the amount of buffered playTime
 *
 */

double DDEClient::getBufferedPlayTime()
{
	double playTime = 0.0;

	playTime = playBufferSize * playRate;

	return playTime;

}

int DDEClient::getThreshYO()
{
	return threshYO;
}

simtime_t DDEClient::getHandoverStart()
{
	return handoverStart;
}

void DDEClient::setHandoverStart(simtime_t time)
{
	handoverStart = time;
}

int DDEClient::getUserClass()
{
	return userClass;
}

float DDEClient::getNSclassification()
{
	return assocAP.nsClass;
}

bool DDEClient::closeTcpConnections()
{
	return closeTcpConnections_var;
}

const MACAddress& DDEClient::getMacAddress()
{

    IInterfaceTable* ift = InterfaceTableAccess().get();
    const MACAddress& address = ift->getInterfaceByName("wlan0")->getMacAddress();

    return address;
}


//This function should cover all the statistics
void DDEClient::finish()
{

	if(backgroundTraffic_var == false)
	{
		// Buffer size
		double sum = 0.0;
		double avgbufferSize = 0.0;

		for(int i = 0; i < (int) bufferSizeVector.size(); i++)
		{
			sum = sum + bufferSizeVector[i];
		}

		if(sum != 0)
		{
			avgbufferSize = sum/(bufferSizeVector.size());
		}
		else
		{
			avgbufferSize = 0;
		}
		bufferSizeVector.clear();

		//Handover delay
		double sumHO = 0.0;
		double avgHOtime = 0.0;

		for(int i = 0; i < (int) handoverTime.size(); i++)
		{
			sumHO = sumHO + handoverTime[i];
		}

		if(sumHO != 0)
		{
			avgHOtime = sumHO/(handoverTime.size());
		}
		else
		{
			avgHOtime = 0;
		}
		handoverTime.clear();

		// Average buffer starvation duration
		double sumDuration = 0.0;
		double avgDuration = 0.0;

		for(int i = 0; i < (int) starvationDurationVector.size(); i++)
		{
			sumDuration = sumDuration + starvationDurationVector[i];
		}

		if(sumDuration != 0)
		{
			avgDuration = sumDuration/(starvationDurationVector.size());
		}
		else
		{
			avgDuration = 0;
		}

		starvationDurationVector.clear();


		double avgTotalBW = getAverageTotalBandwidth();

		double avgDelay = getAverageDelay();


		// Starvation frequency, i.e. starvation inter-arrival time
		// video duration (s) / number of buffer starvations + 1
		double starvationFrequency = 0.0;
		starvationFrequency = (double)playTime_var / ((double)numberOfBufferStarvations + 1);
		stats->addStarvationFrequency(starvationFrequency, getFuzzy());

//		if(numberOfBufferStarvations != 0)
//		{
//			starvationFrequency = (double)playTime_var / (double)numberOfBufferStarvations;
//			stats->addStarvationFrequency(starvationFrequency, getFuzzy());
//		}



		std::cout << getFullPath() << ": avgTotalBWvalue: " << avgTotalBW << endl;
		std::cout << getFullPath() << ": numberOfHandovers " << numberOfHandovers << endl;
		std::cout << getFullPath() << ": Download time: " << downloadTime_var << endl;
		std::cout << getFullPath() << ": Buffer starvations: " << numberOfBufferStarvations << endl;
		std::cout << getFullPath() << ": avgHOtime: " << avgHOtime << endl;
		std::cout << getFullPath() << ": playTime: " << playTime_var << endl;

		stats->addTotalBW(avgTotalBW);
		stats->addAvgDelay(avgDelay);
		stats->addBufferStarvations(numberOfBufferStarvations);

		if(getFuzzy() == true)
		{
			stats->addLearningTotalBW(avgTotalBW);
			stats->addLearningAvgDelay(avgDelay);
			stats->addLearningBufferStarvations(numberOfBufferStarvations);
		}
		else
		{
			stats->addNonLearningTotalBW(avgTotalBW);
			stats->addNonLearningAvgDelay(avgDelay);
			stats->addNonLearningBufferStarvations(numberOfBufferStarvations);
		}
		// User class based statistics

		// Gold
		if(getUserClass() == 3)
		{
			stats->addGoldStats(numberOfBufferStarvations, starvationFrequency, numberOfHandovers);

			recordScalar("GoldBufferStarvations", numberOfBufferStarvations);
			recordScalar("GoldHandovers", numberOfHandovers);
			if(numberOfBufferStarvations != 0)
			{
				recordScalar("GoldStarvationDuration", avgDuration);
			}

		}
		// Silver
		else if(getUserClass() == 2)
		{
			stats->addSilverStats(numberOfBufferStarvations,  starvationFrequency, numberOfHandovers);

			recordScalar("SilverBufferStarvations", numberOfBufferStarvations);
			recordScalar("SilverHandovers", numberOfHandovers);
			if(numberOfBufferStarvations != 0)
			{
				recordScalar("SilverStarvationDuration", avgDuration);
			}
		}
		// Bronze
		else if(getUserClass() == 1)
		{
			stats->addBronzeStats(numberOfBufferStarvations, starvationFrequency, numberOfHandovers);

			recordScalar("BronzeBufferStarvations", numberOfBufferStarvations);
			recordScalar("BronzeHandovers", numberOfHandovers);
			if(numberOfBufferStarvations != 0)
			{
				recordScalar("BronzeStarvationDuration", avgDuration);
			}
		}



		std::cout << getFullPath() << ": starvationFrequency: " << starvationFrequency << endl;
		std::cout << getFullPath() << ": avgStarvationDuration: " << avgDuration << endl;

		BEV << getFullPath() << ": sent " << bytesSent_var << " bytes in " << msgsSent_var << " packets\n";
		BEV << getFullPath() << ": received " << bytesRcvd_var << " bytes in " << msgsRcvd_var << " packets\n";
		BEV << getFullPath() << ": numberOfHandovers " << numberOfHandovers << endl;
		BEV << getFullPath() << ": Sim time: " << simTime() << endl;
		BEV << getFullPath() << ": avgTotalBW: " << avgTotalBW << endl;
		BEV << getFullPath() << ": Download time: " << downloadTime_var << endl;

		recordScalar("packets sent", msgsSent_var);
		recordScalar("packets rcvd", msgsRcvd_var);
		recordScalar("bytes sent", bytesSent_var);
		recordScalar("bytes rcvd", bytesRcvd_var);
		recordScalar("learning", learning_var);
		recordScalar("fuzzy", fuzzy_var);
		recordScalar("avgHandoverTime", avgHOtime);
		//recordScalar("avgStarvationDuration", avgDuration);
		//recordScalar("avgStarvationFrequency", starvationFrequency);

		int Xposition = getParentModule()->getSubmodule("mobility")->par("x");
		int Yposition = getParentModule()->getSubmodule("mobility")->par("y");

		recordScalar("X coordinate", Xposition);
		recordScalar("Y coordinate", Yposition);


//	    if(applicationType == FTP_APP)
//	    {
//
//	    }
//	    else if(applicationType == VIDEO_APP)
//	    {
//
//	    }

		// If the host is learning or a fuzzy host, write a different name to the scalar
		if(fuzzy_var == true)
		{
			if(applicationType == FTP_APP)
			{
				recordScalar("DownloadTimeLearning", downloadTime_var);
			}
			recordScalar("numberOfHandoversLearning", numberOfHandovers);
			recordScalar("avgTotalBWLearning", avgTotalBW);
			recordScalar("avgDelayLearning", avgDelay);

			if(applicationType == VIDEO_APP)
			{
				recordScalar("BufferStarvationEventsLearning", numberOfBufferStarvations);
				recordScalar("avgPlayBufferSizeLearning", avgbufferSize);
				recordScalar("playTimeLearning", playTime_var);
				recordScalar("StarvationFrequencyLearning", starvationFrequency);
			}

			if(numberOfBufferStarvations != 0)
			{

				recordScalar("StarvationDurationLearning", avgDuration);

			}

		}
		else
		{
			if(applicationType == FTP_APP)
			{
				recordScalar("Download time", downloadTime_var);
			}

			//recordScalar("numberOfHandovers", numberOfHandovers);
			recordScalar("avg_TotalBW", avgTotalBW);
			recordScalar("avg_Delay", avgDelay);

			if(applicationType == VIDEO_APP)
			{
				recordScalar("BufferStarvation_Events", numberOfBufferStarvations);
				recordScalar("avg_PlayBufferSize", avgbufferSize);
				recordScalar("play_Time", playTime_var);
				recordScalar("Starvation_Frequency", starvationFrequency);
			}

			if(numberOfBufferStarvations != 0)
			{
				recordScalar("Starvation_Duration", avgDuration);

			}

		}
	}
}

//Looks up server from the server list
//DDEClient::serverInfo *DDEClient::lookupServer(int index)
//{
//    for (ServerList::iterator it=srvList.begin(); it!=srvList.end(); ++it)
//        if (it->ID == index)
//            return &(*it);
//    return NULL;
//}


//Looks up interface from the interface list
//DDEClient::iFaceInfo *DDEClient::lookupInterface(int index)
//{
//    for (iFaceList::iterator it=ifList.begin(); it!=ifList.end(); ++it)
//        if (it->ID == index)
//            return &(*it);
//    return NULL;
//}

//void DDEClient::setIfacePrevSent(int index, long prevSent)
//{
//	iFaceInfo *iFace = lookupInterface(index);
//	iFace->prevSent = prevSent;
//}
//
//void DDEClient::setIfacePrevRcvd(int index, long prevRcvd)
//{
//	iFaceInfo *iFace = lookupInterface(index);
//	iFace->prevRcvd = prevRcvd;
//}


/***** GET/SET methods *****/


long DDEClient::getMsgsRcvd()
{
	return msgsRcvd_var;
}

void DDEClient::setMsgsRcvd(long msgsRcvd)
{
	msgsRcvd_var = msgsRcvd;
}

void DDEClient::incrementMsgsRcvd()
{
	msgsRcvd_var++;
}

long DDEClient::getMsgsSent()
{
	return msgsSent_var;
}

void DDEClient::setMsgsSent(long msgsSent)
{
	msgsSent_var = msgsSent;
}

void DDEClient::incrementMsgsSent()
{
	msgsSent_var++;
}


long DDEClient::getBytesRcvd()
{
	return bytesRcvd_var;
}

void DDEClient::setBytesRcvd(long bytesRcvd)
{
	bytesRcvd_var = bytesRcvd;
}

void DDEClient::incrementBytesRcvd(long byteLength)
{
	bytesRcvd_var += byteLength;
}

long DDEClient::getBytesSent()
{
	return bytesSent_var;
}

void DDEClient::setBytesSent(long bytesSent)
{
	bytesSent_var = bytesSent;
}

void DDEClient::incrementBytesSent(long byteLength)
{
	bytesSent_var += byteLength;
}

long DDEClient::getBytesDownloaded()
{
	return bytesDownloaded_var;
}

void DDEClient::setBytesDownloaded(long bytesDownloaded)
{
	bytesDownloaded_var = bytesDownloaded;
}

void DDEClient::incrementBytesDownloaded(long bytesDownloaded)
{
	bytesDownloaded_var += bytesDownloaded;
}

/**
 * Get the session timeout value (in seconds).
 */
simtime_t DDEClient::getSessionTimeout() const
{
	return sessionTimeout_var;
}

/**
 * Set the session timeout value (in seconds).
 */
void DDEClient::setSessionTimeout(simtime_t sessionTimeout)
{
	sessionTimeout_var = sessionTimeout;
}

simtime_t DDEClient::getSuspendTimeout() const
{
	return suspendTimeout_var;
}

/**
 * Set the session timeout value (in seconds).
 */
void DDEClient::setSuspendTimeout(simtime_t suspendTimeout)
{
	suspendTimeout_var = suspendTimeout;
}

int DDEClient::getRequestLength()
{
	return requestLength_var;
}

void DDEClient::setRequestLength(int requestLength)
{
	requestLength_var = requestLength;
}

int DDEClient::getReplyLength()
{
	return replyLength_var;
}
void DDEClient::setReplyLength(int replyLength)
{
	replyLength_var = replyLength;
}

int DDEClient::getFileSize()
{
	return fileSize_var;
}
void DDEClient::setFileSize(int fileSize)
{
	fileSize_var = fileSize;
}

double DDEClient::getClassifyInterval()
{
	return classifyInterval_var;
}

void DDEClient::setClassifyInterval(double classifyInterval)
{
	classifyInterval_var = classifyInterval;
}

double DDEClient::getIdleInterval()
{
	return idleInterval_var;
}

void DDEClient::setIdleInterval(double idleInterval)
{
	idleInterval_var = idleInterval;
}

double DDEClient::getDecisionThreshold()
{
	return decisionThreshold_var;
}

void DDEClient::setDecisionThreshold(double decisionThreshold)
{
	decisionThreshold_var = decisionThreshold;
}

int DDEClient::getNumRequests()
{
	return numRequests_var;
}
void DDEClient::setNumRequests(int numRequests)
{
	numRequests_var = numRequests;
}

int DDEClient::getNumRequestsSent()
{
	return numRequestsSent_var;
}
void DDEClient::setNumRequestsSent(int numRequestsSent)
{
	numRequestsSent_var = numRequestsSent;
}
void DDEClient::incrementNumRequestsSent()
{
	numRequestsSent_var++;
}

bool DDEClient::getDownloadComplete()
{
	return downloadComplete_var;
}

void DDEClient::setDownloadComplete(bool complete)
{
	downloadComplete_var = complete;
}

TCPServerThreadBase* DDEClient::getFileServerThread()
{
	return fileServerThread;
}
void DDEClient::setFileServerThread(TCPServerThreadBase* thread)
{
	fileServerThread = thread;
}

TCPServerThreadBase* DDEClient::getInfoServerThread()
{
	return infoServerThread;
}
void DDEClient::setInfoServerThread(TCPServerThreadBase* thread)
{
	infoServerThread = thread;
}

TCPServerThreadBase* DDEClient::getAPThread()
{
	return apThread;
}
void DDEClient::setAPThread(TCPServerThreadBase* thread)
{
	apThread = thread;
}

simtime_t DDEClient::getChangeStart()
{
	return changeStart;
}

void DDEClient::setChangeStart(simtime_t start)
{
	changeStart = start;
}

simtime_t DDEClient::getChangeEnd()
{
	return changeEnd;
}

void DDEClient::setChangeEnd(simtime_t end)
{
	changeEnd = end;
}

void DDEClient::setRequestDelay(double value)
{
	requestDelay_var = value;

	requestDelayVector.insert(requestDelayVector.end(), value);
}

bool DDEClient::getFuzzy()
{
	return fuzzy_var;
}

bool DDEClient::getLearning()
{
	return learning_var;
}

bool DDEClient::getpureQLearning()
{
	return pureQLearning_var;
}

bool DDEClient::getLoadBalancing()
{
	return loadBalancing_var;
}

int DDEClient::getApplicationType()
{
	return applicationType;
}

bool DDEClient::getBackgroundTraffic()
{
	return backgroundTraffic_var;
}

/**
 * Constructor.
 */

DDEClientServerHandler::DDEClientServerHandler()
{
//	evtTout		= new cMessage("timeoutServer", EVT_TOUT);
//	delThreadMsg = new ClientInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);
	fileServer_var = false;
	firstClassifySent = false;
	classificationOK = false;
	requestSent = 0;
	timestep = 0;

}

/**
 * Destructor.
 */

DDEClientServerHandler::~DDEClientServerHandler()
{
	// cancel any pending messages
	//hostModule()->cancelAndDelete(evtTout);
	//hostModule()->cancelAndDelete(delThreadMsg);
	//hostModule()->cancelAndDelete(internalMsg);
	evtTout = NULL;
	//delThreadMsg = NULL;
	internalMsg = NULL;
	firstClassifySent = false;
	classificationOK = false;
	requestSent = 0;
	fileServer_var = false;
	cachedContent_var = false;
	timestep = 0;

}

void DDEClientServerHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
	//timestep = 0;
}

void DDEClientServerHandler::sendMessage(cMessage *msg)
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

void DDEClientServerHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [DDE Client ServerHandler]: connection with server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

	if(getFileServer() == false)
	{
		cMessage* lockTimeout = new cMessage("timeoutServer", INTERNAL_LOCK_TIMEOUT);
		hostModule()->setSessionLock(true);
		scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), lockTimeout);


		hostModule()->addAPListInfo(hostModule()->getAssocApSSID(), getSocket()->getRemoteAddress());

		// Send an initial  classify msg, the AP handles the rest

		if(hostModule()->getBackgroundTraffic() == false && hostModule()->getFuzzy() == true)
		{
			sendClassify();
		}


		sendRequest();

	}
	else
	{
		sendRequest();
	}

}


void DDEClientServerHandler::dataArrived(cMessage* msg, bool urgent)
{
	hostModule()->incrementMsgsRcvd();
	if(hostModule()->getDownloadComplete() == false)
	{
		// AP Classification is received
		if(strcmp (msg->getName(), "AP_CLASSIFICATION") == 0 && hostModule()->getFuzzy() == true && this->getSocket()->getState() == TCPSocket::CONNECTED)
		{
			// General handling
			ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);

			if (!appmsg)
				hostModule()->error("Message (%s)%s is not a ddeMsg -- "
						  "probably wrong client app, or wrong setting of TCP's "
						  "sendQueueClass/receiveQueueClass parameters "
						  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
						  msg->getClassName(), msg->getName());


			if(hostModule()->getDownloadComplete() == true)
			{
				BEV << "Download completed, deleting message." << endl;
				delete appmsg;

			}
			else if(strcmp (hostModule()->getAssocApSSID(), "") != 0)
			{

				// Extract the classification information from the message

				bool cachedContent = appmsg->getCachedContent();
				setCachedContent(cachedContent);

				float cache = 0.0;
				float signalStrength = 0.0;
				float MNclassification = 0.0;
				float rssi = 0.0;

				hostModule()->getMNclassification(getInterfaceID(), signalStrength, MNclassification, rssi);
				float APclassification = appmsg->getApClassifyValue();


				if(cachedContent == true)
					cache = 1.0;


				//float seedsPerLeechers = appmsg->getSeedsPerLeechers();

		//		NS option   (not ready)
		//		=========
		//		arg1 = AP index 0..1
		//		arg2 = MN index 0..1
		//		arg3 = cached content true...false
		//		index = result of classification 0..1
				float NSclassification = 0.0;


				// Different classification based on the application

				if(hostModule()->getApplicationType() == FTP_APP)
				{
					//TODO:nsfis
					NSclassification = nsfis(2,APclassification,MNclassification,cache);
					//NSclassification = 4.0;
				}

				else if(hostModule()->getApplicationType() == VIDEO_APP)
				{
					//double playTime = 0.0;
					double bufferSize = 0.0;
					float NScache = 0.0;

					//playTime = hostModule()->getBufferedPlayTime();
					bufferSize = (double) hostModule()->getPlayBufferSize() / (double) hostModule()->getInitAdvBufferSize();

					std::cout << "\n" << endl;
					std::cout << "bufferSize: " << bufferSize << endl;
					std::cout << "playStarted: " << hostModule()->getPlayStarted() << endl;
					std::cout << "sessionLock: " << hostModule()->getSessionLock() << endl;



					// If the play has not yet started or the client has just done a handover
//					if(hostModule()->getSessionLock() == true || hostModule()->getPlayStarted() == false)
//						bufferSize = 1;

					//TODO:nsfis
					NSclassification = nsfis(3,APclassification,MNclassification,bufferSize*100);
					//NSclassification = 4.0;


					//TODO:nsfis
					NScache = nsfis(2,APclassification,MNclassification,cache);
					//NScache = 4.0;

					std::cout << "NS classification bufferSize: " << NSclassification << endl;
					std::cout << "NS classification cache: " << NScache << endl;

				}

				//When theres traffic, classification msg is received after too much time --> interfering traffic needs to be properly set

				//		float NSclassification = nsfis(2,1,1,100);

				double portion = (hostModule()->getTotalBandwidth())/(appmsg->getConsumedBW());

				BEV << "MN classification: " << MNclassification << endl;
				BEV  << "AP classification: " << APclassification << endl;
				BEV  << "Network classification: " << NSclassification << endl;
				std::cout  << "Network classification: " << NSclassification << " , simTime: " << simTime() << endl;
				std::cout << "MN classification: " << MNclassification << endl;
				std::cout  << "Client: " << hostModule()->getParentModule() << ", AP classification: " << APclassification << " , simTime: " << simTime() << endl;
				std::cout << "AP packet loss: " << appmsg->getPacketLoss() << ", load: " << appmsg->getLoad() << ", users: " << appmsg->getNumUsers() << endl;

	//			std::cout << "Current download thruput: " << hostModule()->getDownloadThruput() << endl;
	//			std::cout << "Current upload thruput: " << hostModule()->getUploadThruput() << endl;
	//			std::cout << "Current total BW: " << hostModule()->getTotalBandwidth() << endl;
	//			std::cout << "AP consumed BW: " << appmsg->getConsumedBW() << endl;
				std::cout << "Client's portion of the BW: " << portion << endl;

				float apClass = 0.0;

				//TODO:nsfis
				apClass = nsfis(0,appmsg->getNumUsers(),portion,appmsg->getPacketLoss());
				//apClass = 4.0;

				std::cout << "AP classification with portion: " << apClass << endl;

				std::cout << "Percentage of download completed: " << ((double)hostModule()->getBytesDownloaded() / (double)hostModule()->getFileSize()) << endl;

				std::cout << "NS classification before: " << hostModule()->getAssocAPNSclassification() << endl;

				hostModule()->addAPinfo(hostModule()->getAssocApSSID(), APclassification, appmsg->getNumUsers(), appmsg->getLoad(), appmsg->getPacketLoss(), getSocket()->getRemoteAddress(), appmsg->getCachedContent(), NSclassification, MNclassification, appmsg->getMacAddress(), rssi);

				std::cout << "NS classification after: " << hostModule()->getAssocAPNSclassification() << endl;

				bool changeAP = false;
//				if(hostModule()->getSessionLock() == true)
//					NSclassification = 3.0;


				// Load balancing
				if(hostModule()->getLoadBalancing() == true && hostModule()->getLearning() == false)
				{
					//TODO: send information only if it has changed

					BEV << "Load Balancing algorithm is in use. Contacting the information server." << endl;
					std::cout << "Load Balancing algorithm is in use. Contacting the information server." << endl;
					// initiate classify operation to InfoServer
					ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_MN_CLASSIFY_MSG", INTERNAL_MN_CLASSIFY_MSG);
					scheduleAt(simTime(), msg);

				}
				// Fuzzy, learning, pure Q-learning
				else if(hostModule()->getFuzzy() == true)
				{

					// increment the timestep
					timestep++;
					// timestep = 2 --> interval of 5 seconds
					if(timestep == 2)
					{

						std::cout << "Timestep for Q-L complete, 5 s has expired." << endl;
						std::cout << "Timestep: " << timestep << endl;


						if(hostModule()->getLoadBalancing() == true)
						{

							BEV << "Load Balancing algorithm is in use. Contacting the information server." << endl;
							std::cout << "Load Balancing algorithm is in use. Contacting the information server." << endl;
							// initiate classify operation to InfoServer
							ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_MN_CLASSIFY_MSG", INTERNAL_MN_CLASSIFY_MSG);
							scheduleAt(simTime(), msg);
						}

						if(hostModule()->getPlayStarted() == true || hostModule()->getApplicationType() == FTP_APP)
						{

							std::cout<< "Play has started." << endl;
							// Contact information server if the fuzzyState is BAD or buffer is low
							if(NSclassification > 2.0)
							{
								std::cout<< "Current AP status is good enough." << endl;
								// Do nothing but inform the RL agent
								// Best AP -> current AP
								if(hostModule()->getLearning() == true)
								{
									int assocAPStatus = hostModule()->getFuzzyState(NSclassification);
									int bestAPStatus = hostModule()->getFuzzyState(NSclassification);
									int assocAPUsers = appmsg->getNumUsers();
									int bestAPUsers = appmsg->getNumUsers();
									changeAP = hostModule()->feedbackAgent(assocAPStatus, assocAPUsers, bestAPStatus, bestAPUsers);

									std::cerr << "changeAP: " << changeAP << endl;

								}

							}
							// Else: contact the infoServer
							else
							{

								std::cout << "Current AP status is BAD. Contacting the information Server." << endl;
								// initiate classify operation to InfoServer
								ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_INFO_CLASSIFY_MSG", INTERNAL_INFO_CLASSIFY_MSG);
								scheduleAt(simTime(), msg);

							}
						}
						else
						{
							std::cout<< "Play has not started." << endl;
						}

						// set the timestep = 0.
						// After 2.5 seconds, the timestep will be 1 and after another 2.5 seconds again 2, which will invoke Q-learning
						timestep = 0;
					}
					else
					{
						std::cout << "Timestep not complete for Q-L yet, only 2.5 s has expired." << endl;
						std::cout << "Timestep: " << timestep << endl;
					}


				}
				// Default setting
				else
				{
					// Do nothing
				}

				delete appmsg;

			}

		}
		// FTP
		else if(strcmp (msg->getName(), "request") == 0)
		{
			ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);

			if (!appmsg)
				hostModule()->error("Message (%s)%s is not a ddeMsg -- "
						  "probably wrong client app, or wrong setting of TCP's "
						  "sendQueueClass/receiveQueueClass parameters "
						  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
						  msg->getClassName(), msg->getName());


			BEV << "Received a packet of size: " << appmsg->getByteLength() << " from the Server." << endl;
			double delay = getDelay();
			hostModule()->setRequestDelay(delay);
			double thruput = hostModule()->getDownloadThruput();
			BEV << "Delay for the request: " << delay <<  ", download thruput: " << thruput << endl;
			hostModule()->incrementBytesDownloaded(appmsg->getByteLength());


			//std::cout << "Delay for the request: " << delay <<  ", download thruput: " << thruput << endl;
			//std::cout << "Received a packet of size: " << appmsg->getByteLength() << " from the Server. SimTime: " << simTime() << endl;
			//std::cout << "Received bytes: " << hostModule()->getBytesRcvd() << endl;
			//std::cout << "File size: " << hostModule()->getFileSize() << endl;
			delete appmsg;

			//if(hostModule()->getNumRequestsSent() < hostModule()->getNumRequests())
			if(hostModule()->getBytesDownloaded() < hostModule()->getFileSize())
			{

				BEV << "Bytes downloaded: " << hostModule()->getBytesDownloaded() << " sending another request." << endl;
				if(this->getSocket()->getState() == TCPSocket::CONNECTED)
				{
					sendRequest();
				}
				else
				{
					BEV << "Not connected, not sending another request." << endl;
				}

			}
			else if(hostModule()->getBytesDownloaded() >= hostModule()->getFileSize())
			{
				std::cout << "File download Complete! SimTime:  " << simTime() << endl;
				BEV << "File download Complete! SimTime:  " << simTime() << endl;
				//hostModule()->setBytesDownloaded(0);
				//hostModule()->setNumRequestsSent(0);
				hostModule()->setDownloadComplete(true);
				hostModule()->recordDownloadTime();
				//TODO: this can cause errors
				// Close AP connection
				close();
				// Close infoServer connection
				ClientInternalMsg *closemsg = new ClientInternalMsg("INTERNAL_INFO_CLOSE_MSG", INTERNAL_INFO_CLOSE_MSG);
				scheduleAt(simTime(), closemsg);
			}
			else
			{
				// do nothing
			}

		}
		// VIDEO
		else if(strcmp (msg->getName(), "Video packet") == 0)
		{
			GenericAppMsg *appmsg = dynamic_cast<GenericAppMsg *>(msg);

			if (!appmsg)
				hostModule()->error("Message (%s)%s is not a GenericAppMsg -- "
						  "probably wrong client app, or wrong setting of TCP's "
						  "sendQueueClass/receiveQueueClass parameters "
						  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
						  msg->getClassName(), msg->getName());


			BEV << "Received a packet of size: " << appmsg->getByteLength() << " from the Server." << endl;
			double delay = getDelay();
			hostModule()->setRequestDelay(delay);
			double thruput = hostModule()->getDownloadThruput();
			BEV << "Delay for the request: " << delay <<  ", download thruput: " << thruput << endl;
			hostModule()->incrementBytesDownloaded(appmsg->getByteLength());


			// Update the play buffer
	        hostModule()->updatePlayBuffer(appmsg);


			//if(hostModule()->getNumRequestsSent() < hostModule()->getNumRequests())
			if(hostModule()->getBytesDownloaded() < hostModule()->getFileSize())
			{

				BEV << "Bytes downloaded: " << hostModule()->getBytesDownloaded() << endl;

			}

			if(hostModule()->getBytesDownloaded() >= hostModule()->getFileSize())
			{
				std::cout << "Video download Complete! SimTime:  " << simTime() << "," << hostModule()->getParentModule() << endl;
				BEV << "Video download Complete! SimTime:  " << simTime() << endl;
				hostModule()->setDownloadComplete(true);

				// Close AP connection
				if(getSocket()->getState() == TCPSocket::CONNECTED)
					close();

				// Close infoServer connection
				ClientInternalMsg *closemsg = new ClientInternalMsg("INTERNAL_INFO_CLOSE_MSG", INTERNAL_INFO_CLOSE_MSG);
				scheduleAt(simTime(), closemsg);

			}

		}
		else
		{
			BEV << "unneeded msg, deleting message." << endl;
			//std::cout << "Download already completed, deleting message." << endl;
			delete msg;
		}

	}


}

/**
 * Handles a session timeout event.
 *
 */
void DDEClientServerHandler::timerExpired(cMessage* msg)
{
	switch(msg->getKind())
	{
		// timer expired while waiting
		case EVT_TOUT:
		{
			// logging
			BEV << "session lock with server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] expired\n";

			//Release session lock
			//hostModule()->setSessionLock(false);
			// perform close()
			//close();
			delete msg;
			break;
		}
		case INTERNAL_CLASSIFY_MSG:
		{
			if(hostModule()->getDownloadComplete() == false)
			{

				//std::cout << "Generating a classify operation to AP." << endl;
				if(getSocket()->getState() == TCPSocket::CONNECTED)
				{
					BEV << "Generating a classify operation to AP." << endl;
					sendClassify();
				}
			}
			delete msg;
			break;
		}

//		default:
//		{
//			delete msg;
//		}
	}

}

void DDEClientServerHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void DDEClientServerHandler::closed()
{

	BEV << "Connection closed." << endl;
	cMessage* delThreadMsg;		// delete thread
	delThreadMsg = new ClientInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);
	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void DDEClientServerHandler::close()
{

	// cancel the session timeout event and call default close()
//	cancelEvent(evtTout);
	//hostModule()->cancelAndDelete(evtTout);
	//hostModule()->cancelEvent(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}

void DDEClientServerHandler::sendRequest()
{
	BEV <<"Sending client's request to the server."<< endl;


	if(hostModule()->getApplicationType() == FTP_APP)
	{
		ddeMsg *msg = new ddeMsg("request");
		msg->setByteLength(hostModule()->getRequestLength());
		msg->setExpectedReplyLength(hostModule()->getReplyLength());
		msg->setServerClose(false);
		msg->setUserClass(hostModule()->getUserClass());

		sendMessage(msg);
	}
	else if(hostModule()->getApplicationType() == VIDEO_APP)
	{
		//TODO: codec rate!
		AppContext *msg = new AppContext("videoRequest");
		msg->setByteLength(hostModule()->getRequestLength());
		msg->setChunkSize(hostModule()->getChunkSize());
		msg->setTxDuration(hostModule()->getTxRate());
		msg->setFileSize(hostModule()->getReplyLength());
		msg->setRemainingFileSize(hostModule()->getRemainingFileSize());
		msg->setCodecRate(1.0);
		msg->setUserClass(hostModule()->getUserClass());
		sendMessage(msg);
	}
	else
	{
		hostModule()->error("Wrong application type!");
	}

	setRequestSentTime(simTime());

	hostModule()->incrementNumRequestsSent();
}

void DDEClientServerHandler::sendClassify()
{
	ddeMsg *msg = new ddeMsg("CLASSIFY_AP");
	msg->setByteLength(hostModule()->getRequestLength());
	msg->setClassifyOperation(0);
	sendMessage(msg);
}

void DDEClientServerHandler::callClose()
{
	if(getSocket()->getState() == TCPSocket::CONNECTED)
		close();
}

void DDEClientServerHandler::unRegister()
{
	ddeMsg *msg = new ddeMsg("unregister");
	msg->setByteLength(hostModule()->getRequestLength());
	sendMessage(msg);
}


/**** GET/SET methods ***/

void DDEClientServerHandler::setInterfaceID(int interfaceID)
{
	interfaceID_var = interfaceID;
}

int DDEClientServerHandler::getInterfaceID()
{
	return interfaceID_var;
}

void DDEClientServerHandler::setRequestSentTime(simtime_t time)
{
	requestSent = time;
}


simtime_t DDEClientServerHandler::getRequestSentTime()
{
	return requestSent;
}

double DDEClientServerHandler::getDelay()
{
	double delay = 0.0;

	delay = (simTime()-requestSent).dbl();

	return delay;
}

void DDEClientServerHandler::setFileServer(bool fileServer)
{
	fileServer_var = fileServer;
}

bool DDEClientServerHandler::getFileServer()
{
	return fileServer_var;
}

void DDEClientServerHandler::setCachedContent(bool cachedContent)
{
	cachedContent_var = cachedContent;
}

bool DDEClientServerHandler::getCachedContent()
{
	return cachedContent_var;
}

/**
 * Get the host module (i.e., the instance of DDEClient which spawns the thread).
 */
DDEClient* DDEClientServerHandler::hostModule()
{
	// get the host module and check its type
	DDEClient* hostMod = dynamic_cast<DDEClient*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}



/**
 * Constructor.
 */

DDEClientInfoHandler::DDEClientInfoHandler()
{
	//evtTout		= new cMessage("timeoutServer", EVT_TOUT);
	//delThreadMsg = new ClientInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

}

/**
 * Destructor.
 */

DDEClientInfoHandler::~DDEClientInfoHandler()
{
	// cancel any pending messages
//	hostModule()->cancelAndDelete(evtTout);
//	delThreadMsg = NULL;
//	hostModule()->cancelAndDelete(delThreadMsg);
//	hostModule()->cancelAndDelete(internalMsg);
//	evtTout = NULL;

	internalMsg = NULL;

}

void DDEClientInfoHandler::init( TCPSrvHostApp* hostmodule, TCPSocket* socket)
{
	TCPServerThreadBase::init(hostmodule, socket);
}

void DDEClientInfoHandler::sendMessage(cMessage *msg)
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

void DDEClientInfoHandler::established()
{
	// start the session timer
	//scheduleAt(simTime() + (simtime_t)hostModule()->getSessionTimeout(), evtTout);

	// logging
	BEV << " [DDE Client ServerHandler]: connection with infoServer[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "] established\n";
	//std::cout << "Info server connection established: " << simTime() << endl;
	//BEV << "starting session timer[" << hostModule()->getSessionTimeout() << " secs] for server[address=" << getSocket()->getRemoteAddress() << ", port=" << getSocket()->getRemotePort() << "]\n";

	//Send MN classification to infoServer
//	if(hostModule()->getLoadBalancing() == false)
//	{
//		sendClassificationMN();
//	}

	// Start the process of retrieving information about the best AP
	if(hostModule()->getFuzzy() == true && hostModule()->getLoadBalancing() == false)
	{
		//sendClassificationMN();
		//sendClassifyAP();
	}


}

/**
 * Handles the reception of a message
 *
 *
 */

void DDEClientInfoHandler::dataArrived(cMessage* msg, bool urgent)
{
	ddeMsg *appmsg = dynamic_cast<ddeMsg *>(msg);

	if (!appmsg)
		hostModule()->error("Message (%s)%s is not a ddeMsg -- "
				  "probably wrong client app, or wrong setting of TCP's "
				  "sendQueueClass/receiveQueueClass parameters "
				  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
				  msg->getClassName(), msg->getName());

	hostModule()->incrementMsgsRcvd();
	hostModule()->incrementBytesRcvd(appmsg->getByteLength());

	// Read AP list and form the best available other AP
	if(strcmp (appmsg->getName(), "INFO_CLASSIFICATION") == 0 && this->getSocket()->getState() == TCPSocket::CONNECTED)
	{
		//Read the list from a message
		::APList test = appmsg->getApList();

        //Based on AP list, select AP with the best classification value
        ::APList::iterator bestIT = test.begin();

        int tempIndex = 0;
        char tempString[80];
        const char *tempSSID;
        float NSclassification = 0.0;
        float candidateValue = -1;
        MACAddress assocApMac;
        MACAddress newAPMac;

        std::cout << hostModule()->getParentModule() << ", received infoServer classification: " << simTime() << endl;

        //Selects the best AP out of the list according to its network classifyValue
        for (::APList::iterator it=test.begin(); it!=test.end(); ++it)
        {
        	tempIndex = it->ID;
        	sprintf(tempString, "ap%d", tempIndex);
        	tempSSID = tempString;

        	//ss = computeEstimatedSignalStrength(tempSSID);

        	//std::cout << tempSSID << ", packet loss: " << it->packetLoss  << ", load: " << it->load << " cachedContent: " << it->cachedContent << endl;

        	NSclassification = getNetworkClassification(it->classifyValue, tempSSID, it->cachedContent);

        	//if((NSclassification > candidateValue) && (!strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0) && (hostModule()->isAPSuspended(tempSSID) == false))
        	if((NSclassification > candidateValue) && (!strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0))
        	{
        		candidateValue = NSclassification;
        		bestIT = it;
        	}
        	else if(strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0)
        	{
        		assocApMac = it->macAddress;
        	}

        }
    	tempIndex = bestIT->ID;
    	sprintf(tempString, "ap%d", tempIndex);
    	tempSSID = tempString;


        // Add info about the best available other AP
        hostModule()->addBestOtherApinfo(tempSSID, bestIT->classifyValue, bestIT->numUsers, bestIT->load, bestIT->packetLoss,
        		bestIT->apAddress, bestIT->cachedContent, candidateValue, bestIT->macAddress);

        bool changeAP;
        changeAP = hostModule()->doHandover();

		// Methods for doing a handover
		if(changeAP == true)
		{

			// Close connections
			if(hostModule()->closeTcpConnections() == true)
			{

				ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_CLOSE_CONNECTION_MSG", INTERNAL_CLOSE_CONNECTION_MSG);
				scheduleAt(simTime(), msg);

				BEV <<"Closing connection to infoServer." << endl;
				close();
			}
			// Do not close connections, but unregister from the servers
			else
			{

				// Unregister from the AP
				ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_UNREGISTER_MSG", INTERNAL_UNREGISTER_MSG);
				scheduleAt(simTime(), msg);

				// Unregister from the infoServer
				unRegister();

				// Schedules association without closing TCP connections
				hostModule()->scheduleAssociation();
			}
		}

//        // Schedule another message
//		ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_INFO_CLASSIFY_MSG", INTERNAL_INFO_CLASSIFY_MSG);
//		scheduleAt(simTime()+ 1.0, msg);

        delete appmsg;
	}

//	// OLD code: makes the decision on handover
//	if(strcmp (appmsg->getName(), "INFO_CLASSIFICATION") == 0 && this->getSocket()->getState() == TCPSocket::CONNECTED)
//	{
//
//        //Read the list from a message
//        ::APList test = appmsg->getApList();
//        //float seedsPerLeechers = appmsg->getSeedsPerLeechers();
//
//
////        ::APList test;
////        for (int i = 0; i < 5; i++)
////        {
////        	apInfo *ap;
////        	test.push_back(apInfo());
////        	ap = &test.back();
////        	ap->ID = (i+1);
////        	ap->classifyValue = 0.1*i;
////        	ap->apAddress = IPvXAddress("0.0.0.0");
////        }
//
////		for (::APList::iterator it=test.begin(); it!=test.end(); ++it)
////		{
////			BEV << "test list ID: " << it->ID << endl;
////			BEV << "test list classifyValue: " << it->classifyValue << endl;
////			BEV << "test list apAddress: " << it->apAddress << endl;
////			BEV << "test list numUsers: " << it->numUsers << endl;
////			BEV << "test list load: " << it->load << endl;
////			BEV << "test list loss: " << it->packetLoss << endl;
////
////		}
//
//        std::cout << hostModule()->getParentModule() << ", received infoServer classification: " << simTime() << endl;
//
//        //Based on AP list, select AP with the best classification value
//
//        ::APList::iterator bestIT = test.begin();
//
//        int tempIndex = 0;
//        char tempString[80];
//        const char *tempSSID;
//
//		// Own parameters
//		double ownSS = 0.0;
//		double ownLoss = 0.0;
//		int ownUsers = 0;
//		// Best parameters
//		double bestSS = 0.0;
//		double bestLoss = 0.0;
//		int bestUsers = 0;
//
//        float NSclassification = 0.0;
//        float candidateValue = -1;
//        double ss = 0.0;
//
//        MACAddress assocApMac;
//        MACAddress newAPMac;
//
//        bool changeAP = false;
//
//        if(hostModule()->getpureQLearning() == true)
//        {
//            //Selects the best AP out of the list according to its signal strength value
//            for (::APList::iterator it=test.begin(); it!=test.end(); ++it)
//            {
//            	tempIndex = it->ID;
//            	sprintf(tempString, "ap%d", tempIndex);
//            	tempSSID = tempString;
//
//            	ss = computeEstimatedSignalStrength(tempSSID);
//
//            	//std::cout << tempSSID << ", packet loss: " << it->packetLoss  << ", load: " << it->load << " cachedContent: " << it->cachedContent << endl;
//
//            	//NSclassification = getNetworkClassification(it->classifyValue, tempSSID, it->cachedContent);
//
//            	//if((ss > candidateValue) && (!strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0) && (hostModule()->isAPSuspended(tempSSID) == false))
//            	if((ss > candidateValue) && (!strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0))
//            	{
//            		candidateValue = ss;
//            		bestIT = it;
//            	}
//            	else if(strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0)
//            	{
//            		assocApMac = it->macAddress;
//            	}
//
//            }
//
//        }
//        else
//        {
//            //Selects the best AP out of the list according to its network classifyValue
//            for (::APList::iterator it=test.begin(); it!=test.end(); ++it)
//            {
//            	tempIndex = it->ID;
//            	sprintf(tempString, "ap%d", tempIndex);
//            	tempSSID = tempString;
//
//            	//ss = computeEstimatedSignalStrength(tempSSID);
//
//            	//std::cout << tempSSID << ", packet loss: " << it->packetLoss  << ", load: " << it->load << " cachedContent: " << it->cachedContent << endl;
//
//            	NSclassification = getNetworkClassification(it->classifyValue, tempSSID, it->cachedContent);
//
//            	//if((NSclassification > candidateValue) && (!strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0) && (hostModule()->isAPSuspended(tempSSID) == false))
//            	if((NSclassification > candidateValue) && (!strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0))
//            	{
//            		candidateValue = NSclassification;
//            		bestIT = it;
//            	}
//            	else if(strcmp(hostModule()->getAssocApSSID(), tempSSID) == 0)
//            	{
//            		assocApMac = it->macAddress;
//            	}
//
//            }
//
//        }
//
//
//
//
//        int AP_index = bestIT->ID;
//		char string[80];
//		sprintf(string, "ap%d", AP_index);
//		std::string testString = string;
//		newAPMac = bestIT->macAddress;
//
//		// Set Own parameters
//		ownSS = hostModule()->getSignalStrengthValue();
//		ownLoss = hostModule()->getAssocAPLoss();
//		ownUsers = hostModule()->getAssocAPUsers() - 1;
//
//		// Set Best parameters
//		bestSS = ss;
//		bestLoss = bestIT->packetLoss;
//		bestUsers = bestIT->numUsers;
//
//
//		BEV << "Best AP: " << AP_index << ", AP classifyValue: " << bestIT->classifyValue << endl;
//		BEV << "NSclassification: " << candidateValue << endl;
//		BEV << "Network state is: " << hostModule()->getFuzzyState(candidateValue) << endl;
//
//		std::cout << "Best AP: " << AP_index << ", AP classifyValue: " << bestIT->classifyValue << endl;
//		std::cout << "NSclassification: " << candidateValue << endl;
//		std::cout  << "Network state is: " << hostModule()->getFuzzyState(candidateValue) << endl;
//		std::cout << "MAC address: " << bestIT->macAddress << endl;
//
//		BEV << "Current AP: " << hostModule()->getAssocApSSID() << ", AP classifyValue: " << hostModule()->getAssocAPclassification() << endl;
//		BEV << "NSclassification: " << hostModule()->getAssocAPNSclassification() << endl;
//		BEV << "Network state is: " << hostModule()->getFuzzyState(hostModule()->getAssocAPNSclassification()) << endl;
//
//		std::cout << "Current AP: " << hostModule()->getAssocApSSID() << ", AP classifyValue: " << hostModule()->getAssocAPclassification() << endl;
//		std::cout << "NSclassification: " << hostModule()->getAssocAPNSclassification() << endl;
//		std::cout << "Network state is: " << hostModule()->getFuzzyState(hostModule()->getAssocAPNSclassification()) << endl;
//		std::cout << "Current thruput: " << hostModule()->getDownloadThruput() << endl;
//
//		//int state = hostModule()->getFuzzyState(candidateClassification);
//		//std::cout << "Network state is: " << hostModule()->getFuzzyStateLiteral(state) << endl;
//
//
////		cModule* agentMod = hostModule()->getParentModule()->getSubmodule("rlAgent");
////		RLagent* agent = check_and_cast<RLagent*> (agentMod);
//
//
//		if(hostModule()->getLearning() == true)
//		{
//			if(hostModule()->getpureQLearning() == true)
//			{
//
//				changeAP = hostModule()->feedbackPureAgent(ownSS, ownLoss, ownUsers, bestSS, bestLoss, bestUsers);
//
//			}
//			else
//			{
//				changeAP = hostModule()->feedbackAgent(hostModule()->getFuzzyState(candidateValue));
//			}
//
//		}
//		// If learning is not activated, a change should always come if new state != BAD
//		else
//		{
//			//if(hostModule()->getFuzzyState(candidateValue) == hostModule()->BAD)
//			if (candidateValue > 3.0 && bestIT->classifyValue > 3.0)
//				changeAP = true;
//			else
//				changeAP = false;
//		}
//
//		//double action = 0.0;
//
//		//action = agent->getAction(state, bestIT->numUsers, hostModule()->getTotalBandwidth());
//
//		//std::cout << "Agent action: " << action << endl;
//
//		if(changeAP == true)
//		{
//			std::cout << "Changing AP." << endl;
//			ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_CLOSE_CONNECTION_MSG", INTERNAL_CLOSE_CONNECTION_MSG);
//			scheduleAt(simTime(), msg);
//
//			BEV <<"Closing connection to infoServer." << endl;
//	        close();
//
//	        // Set handover start
//	        hostModule()->setHandoverStart(simTime());
//
//	        //timestep for starting the change c_str()
//	        hostModule()->setChangeStart(simTime());
//
//	        hostModule()->setNewAPssid(testString);
//	        hostModule()->setNewAPMac(newAPMac);
//
//	        hostModule()->addHandover();
//
//	        //Suspend AP
//	        hostModule()->suspendAP(hostModule()->getAssocApSSID());
//
//	        //TODO:
//	       // hostModule()->scheduleAssociation();
//
//
//		}
//		else
//		{
//			std::cout << "NOT Changing AP." << endl;
//	        //timestep for starting the change
//	        hostModule()->setChangeStart(-1);
//
//			// Continue classification operation with AP
//			//ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_AP_CLASSIFY_MSG", INTERNAL_AP_CLASSIFY_MSG);
//			//scheduleAt(simTime()+(hostModule()->getClassifyInterval()), msg);
//
//		}
//
//		delete appmsg;
//
//	}

	else if(strcmp (appmsg->getName(), "CLASSIFY_MN") == 0)
	{
		if(this->getSocket()->getState() == TCPSocket::CONNECTED)
		{
			sendClassificationMN();
		}
		delete appmsg;
	}
	else if(strcmp (appmsg->getName(), "HANDOVER_RECOMMENDATION") == 0 && this->getSocket()->getState() == TCPSocket::CONNECTED)
	{
		MACAddress apMAC = appmsg->getHO_to_AP_MAC();
		string apSSID = appmsg->getHO_to_AP_SSID();
		std::cout << hostModule()->getFullPath() << ": Received a handover recommendation event from the infoServer to AP: " << apSSID << " , " << apMAC  << ", simTime: " << simTime() << endl;
		BEV << "Received a handover recommendation event from the infoServer to AP: " << apSSID << " , " << apMAC << endl;

		// Actions for doing a handover


        // Set handover start
        hostModule()->setHandoverStart(simTime());

        //timestep for starting the change c_str()
        hostModule()->setChangeStart(simTime());

        hostModule()->setNewAPssid(apSSID);
        hostModule()->setNewAPMac(apMAC);

        hostModule()->addHandover(true);

        //Suspend AP
        hostModule()->suspendAP(hostModule()->getAssocApSSID());


//		hostModule()->setDecisionLock(true);
//		// Schedule timeout for the decision lock
//		cMessage* decisionLockTimeout = new cMessage("decisionLockTimeout", INTERNAL_DECISION_TIMEOUT);
//		scheduleAt(simTime() + (simtime_t)hostModule()->getDecisionThreshold(), decisionLockTimeout);

        // Decision lock
        if(hostModule()->getLearning() == true)
        {
			hostModule()->setDecisionLock(true);
			// Schedule timeout for the decision lock
			cMessage* decisionLockTimeout = new cMessage("decisionLockTimeout", INTERNAL_DECISION_TIMEOUT);
			scheduleAt(simTime() + (simtime_t)hostModule()->getDecisionThreshold(), decisionLockTimeout);
        }



		// Close connections
        if(hostModule()->closeTcpConnections() == true)
        {

			ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_CLOSE_CONNECTION_MSG", INTERNAL_CLOSE_CONNECTION_MSG);
			scheduleAt(simTime(), msg);

			BEV <<"Closing connection to infoServer." << endl;
			close();
        }
        // Do not close connections, but unregister from the servers
        else
        {

			// Unregister from the infoServer and the AP
			ClientInternalMsg *msg = new ClientInternalMsg("INTERNAL_UNREGISTER_MSG", INTERNAL_UNREGISTER_MSG);
			scheduleAt(simTime(), msg);

			// Unregister from the infoServer
			unRegister();

			// Schedules association without closing TCP connections
			hostModule()->scheduleAssociation();
        }


		delete appmsg;
	}
	else
	{
		//std::cout << "TCP state: " << this->getSocket()->getState() << endl;
		//hostModule()->error("Received a msg while not connected: ", this->getSocket()->getState());
		delete appmsg;
	}

}

float DDEClientInfoHandler::computeEstimatedSignalStrength(const char* ssid)
{
	float ss = 0.0;
	float rssi = 0.0;

	float average = 0.0;
	float sum = 0.0;

	for(int i = 0; i < 100 ; i++)
	{
		hostModule()->getEstimatedSignalStrength(ssid, ss, rssi);
		sum += (ss * 100);
		ss = 0.0;
		rssi = 0.0;
	}
	average = sum/100;

	return average;

}

//Gets network classification
float DDEClientInfoHandler::getNetworkClassification(float apClassification, const char* ssid, bool cachedContent)
{

	float cache = 0.0;
	if(cachedContent == true)
		cache = 1.0;

	 //Get estimated AP signal strength
	float ss = 0.0;

	//Get average signal strength from 10 samples
	//an average is needed when the shadowing deviation is non-zero.

	float average = 0.0;
	float sum = 0.0;
	float rssi = 0.0;
	for(int i = 0; i < 100 ; i++)
	{
		hostModule()->getEstimatedSignalStrength(ssid, ss, rssi);
		sum += ss * 100;
		rssi = 0.0;
		ss = 0.0;
	}
	average = sum/100;

	//std::cout << "Signal strength average: " << average << endl;

	//ss = (hostModule()->getEstimatedSignalStrength(ssid)) * 100;

	float ec = 100; //energy consumption
	float bl = 100; //battery level
	int mn = 1;

	float MNclassification = 0.0;
	float NSclassification = 0.0;

	//TODO:nsfis
	MNclassification = nsfis(mn,average,ec,bl);
	//MNclassification = 4.0;

	BEV << "MNclassification: " << MNclassification << endl;
	//std::cout << "MNclassification: " << MNclassification << endl;
	//std::cout << "apClassification: " << apClassification << endl;

	//TODO:nsfis
	NSclassification = nsfis(2, apClassification, MNclassification, cache);
	//NSclassification = 4.0;

	//std::cout << "NSclassification: " << NSclassification << endl;

	return NSclassification;
}

/**
 * Handles a session timeout event.
 *
 */
void DDEClientInfoHandler::timerExpired(cMessage* msg)
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
	}

}

void DDEClientInfoHandler::peerClosed()
{

	//If this thread hasn't initated this CLOSE, respond by CLOSING
	if (getSocket()->getState()!=TCPSocket::CLOSED) {
		close();
		BEV << "Client closed, sending a close msg." << endl;
	}


}

void DDEClientInfoHandler::closed()
{

	BEV << "Connection closed." << endl;

	cMessage* delThreadMsg;		// delete thread
	delThreadMsg = new ClientInternalMsg("INTERNAL_REMOVE_THREAD_MSG", INTERNAL_REMOVE_THREAD_MSG);

	scheduleAt(simTime(), delThreadMsg);

}

/**
 * Closes an active connection
 */
void DDEClientInfoHandler::close()
{

	// cancel the session timeout event and call default close()
//	cancelEvent(evtTout);
	//hostModule()->cancelAndDelete(evtTout);
	//hostModule()->cancelEvent(evtTout);
	getSocket()->close();

	///TCP_TIMEOUT_2MSL = 240s
   	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);

}

void DDEClientInfoHandler::sendClassifyAP()
{
	if(this->getSocket()->getState() == TCPSocket::CONNECTED)
	{
		ddeMsg *msg = new ddeMsg("CLASSIFY_AP");
		msg->setByteLength(hostModule()->getRequestLength());
		msg->setClassifyOperation(0);
		sendMessage(msg);
	}
}

void DDEClientInfoHandler::sendClassificationMN()
{
	float ss = 0.0;
	float MNclassification = 0.0;
	float rssi = 0.0;
	MNclassification = hostModule()->getAssocAPMNclassification();
	ss = hostModule()->getSignalStrengthValue();
	rssi = hostModule()->getRSSIvalue();

	const char* hostID = hostModule()->getParentModule()->getFullName();
	const char* assocAP = hostModule()->getAssocApSSID();

	float NSclassification = 0.0;
	NSclassification = hostModule()->getNSclassification();
	float sensitivityValue = 0.0;
	float NoiseLevelDBM = 0.0;
	hostModule()->getNoiseLevel(sensitivityValue, NoiseLevelDBM);

//	std::cout << "client sending." << endl;
//	std::cout << "signal strength: " << ss << endl;
//	std::cout << "rssiDBM: " << rssi << endl;
//	std::cout << "noiseLevelDBM: " << NoiseLevelDBM << endl;


	ddeMsg *msg = new ddeMsg("MN_CLASSIFICATION");
	msg->setByteLength(hostModule()->getRequestLength());
	msg->setMnClassifyValue(MNclassification);
	msg->setHostID(hostID);
	msg->setAssocAP(assocAP);
	msg->setSignalStrength(ss);
	msg->setUserClass(hostModule()->getUserClass());
	msg->setNsClassifyValue(NSclassification);
	msg->setMacAddress(hostModule()->getMacAddress());
	msg->setBW_requirement(hostModule()->getBandwidthLimit());
	msg->setRssiDBM(rssi);
	msg->setSensitivity(sensitivityValue);
	msg->setNoiseLevelDBM(NoiseLevelDBM);



	// Get my position
	cModule* hostMod = hostModule()->getParentModule()->getSubmodule("wlan",0)->getSubmodule("radio");
	AbstractRadioExtended* hostRadio = check_and_cast<AbstractRadioExtended*> (hostMod);
    const Coord& myPos = hostRadio->getPosition();

    msg->setClient_x(myPos.x);
    msg->setClient_y(myPos.y);


	sendMessage(msg);
}

void DDEClientInfoHandler::unRegister()
{
	ddeMsg *msg = new ddeMsg("unregister");
	msg->setByteLength(hostModule()->getRequestLength());
	sendMessage(msg);
}

void DDEClientInfoHandler::callClose()
{
	if(getSocket()->getState() == TCPSocket::CONNECTED)
		close();
}



/**** GET/SET methods ***/

void DDEClientInfoHandler::setInterfaceID(int interfaceID)
{
	interfaceID_var = interfaceID;
}

int DDEClientInfoHandler::getInterfaceID()
{
	return interfaceID_var;
}


/**
 * Get the host module (i.e., the instance of DDEClient which spawns the thread).
 */
DDEClient* DDEClientInfoHandler::hostModule()
{
	// get the host module and check its type
	DDEClient* hostMod = dynamic_cast<DDEClient*>(TCPServerThreadBase::getHostModule());

	// return the correct module
	return hostMod;
}




