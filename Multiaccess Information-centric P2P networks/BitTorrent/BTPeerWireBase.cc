//
// Copyright 2009 Konstantinos V. Katsaros
//                              ntinos@aueb.gr
//                              http://mm.aueb.gr/~katsaros
//

// This file is part of BitTorrent Implementation for OMNeT++.

//    BitTorrent Implementation for OMNeT++ is free software: you can redistribute
//	  it and/or modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the License,
//	  or (at your option) any later version.

//    BitTorrent Implementation for OMNeT++ is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with BitTorrent Implementation for OMNeT++.
//    If not, see <http://www.gnu.org/licenses/>.


#include "BTPeerWireClientHandlerBase.h"
#include "BTPeerWireBase.h"
#include <TCP.h>
#include <algorithm>
#include <limits>
#include "NotifierConsts.h"


#define BEV	EV << "[" << this->parentModule()->fullName() << "]: "

Define_Module(BTPeerWireBase);

BTPeerWireBase::BTPeerWireBase()
{
  
}

BTPeerWireBase::~BTPeerWireBase() {
	// cancel timer events
	stopChokingAlorithms();

	delete tcp;
	delete localBitfield_var;

}

void BTPeerWireBase::finish()
{
  connectFirstTracker = false;
  ExitFlag = false;
  //finishedDownloads = 0;
}

void BTPeerWireBase::initialize()
{
	// parent initialize
	TCPSrvHostApp::initialize();

	//TODO://To be removed, used for debugging
	debuggedNode = ((string)par("debuggedNode")).c_str();

	// member init
	file_size_var 				= (double)par("file_size")*1024;
	piece_size_var 				= (double)par("piece_size");
	block_size_var 				= (double)par("block_size");
	DHT_port_var 				= (size_t)par("DHT_port");
	pstr_var 				= (const char*)par("pstr");
    	pstrlen_var 				= (int)par("pstrlen");
	keep_alive_var 				= (int)par("keep_alive");
	have_supression_var 			= (bool)par("have_supression");
	chocking_interval_var 			= (int)par("chocking_interval");
	downloaders_var 			= (int)par("downloaders");
	optUnchockedPeers_var 			= (int)par("optUnchockedPeers");
	optUnchocking_interval_var 		= (int)par("optUnchocking_interval");
	snubbing_interval_var 			= (int)par("snubbing_interval");
	rarest_list_size_var			= (int)par("rarest_list_size");
	minNumConnections_var			= (int)par("minNumConnections");
	maxNumConnections_var			= (int)par("maxNumConnections");
	timeToSeed_var				= (int)par("timeToSeed");
	request_queue_depth_var			= (int)par("request_queue_depth");
	super_seed_mode_var			= (bool)par("super_seed_mode");
	maxNumEmptyTrackerResponses_var 	= (int)par("maxNumEmptyTrackerResponses");
	newlyConnectedOptUnchokeProb_var	= (double)par("newlyConnectedOptUnchokeProb");
	enableEndGameMode_var			= (bool)par("enableEndGameMode");
	downloadRateSamplingDuration_var= (int)par("downloadRateSamplingDuration");
	subtrackerNum 				= (int)par("subtrackerNum");
	seederNum 				= (int)par("seederNum");
	recordVectors				= (bool)par("recordVectors");

	setProcDelay((int)par("procDelay")/1000000);

	const char* ch1 = "BitTorrent: Received blocks rate of ";
	const char* ch2 = "BitTorrent: Sent blocks rate of ";
	const char* ch4 = "BitTorrent: Downloadrate rate of ";
	const char* ch5 = "BitTorrent: Uploadrate rate of ";
	const char* ch3 = this->parentModule()->fullName();
	char str1[200];
	char str2[200];
	char str3[200];

	char str4[200];
	char str5[200];
	char str6[200];

	strcpy(str1, ch1);
	strcpy(str2, ch2);
	strcpy(str3, ch3);

	strcpy(str4, ch4);
	strcpy(str5, ch5);
	strcpy(str6, ch3);

	strcat(str1, str3);
	strcat(str2, str3);

	strcat(str4, str6);
	strcat(str5, str6);

	recBlocksRate_vec.setName(str1);
	sentBlocksRate_vec.setName(str2);

	downloadRate_vec.setName(str4);
	uploadRate_vec.setName(str5);
	

	tcp = new TCP();

	btStatistics = (cSimpleModule*)simulation.moduleByPath(par("statisticsModulePath"));

	if (btStatistics == NULL)
		opp_error("Wrong statisticsModulePath configuration");

	// records the time this peer was created
	if (strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12) != 0) {
	    BTStatisticsStartupTimeMsg* msgST= new BTStatisticsStartupTimeMsg("BT_STATS_STIME", BT_STATS_STIME);
	    msgST->setStartupTime(simTime());
	    sendDirect(msgST, 0, btStatistics, btStatistics->findGate("direct_in"));
	}

	setAnnounceInterval(0);
	setCurrentNumConnections(0);
	setPendingNumConnections(0);
	setDownloadDuration(simTime());
	setCurrentNumEmptyTrackerResponses(0);
	setBlocksFromSeeder(0);
	trackerResponse_var = NULL;
	setAverageDownloadRate(0);
	setAverageUploadRate(0);

	connectFirstTracker = false;
	ExitFlag = false;
	//finishedDownloads = 0;
	setNumPieces((int)(file_size_var/piece_size_var));
	setNumBlocks((int)(piece_size_var/block_size_var));
	peerState.setMinDownloaderRate(numeric_limits<float>::max());
	pieceFreqState.initializePieceFrequencies(numPieces());

	//if (!strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12)) {
	if (strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12) == 0) {
		setSuperSeedMode(superSeedMode());
		setState(SEEDER);
		downloaders_var = (int)par("seederDownloaders");
		optUnchockedPeers_var = (int)par("seederOptUnchockedPeers");
		initializeLocalBitfield(true);
		setTimeToSeed((int)numeric_limits<double>::max());
	} else {
		setSuperSeedMode(false);
		setState(NORMAL);
		initializeLocalBitfield(false);
// 		nb = NotificationBoardAccess().get();
// 		if (nb==NULL) {
// 		  error("Notification board not found.");
// 		}
		// for a wireless interface subscribe the association event to start the peerWire protocol
// 		nb->subscribe(this, NF_L2_ASSOCIATED);
	}

	

	// WATCHES
	WATCH(piece_size_var);
	WATCH(block_size_var);
	WATCH(numPieces_var);
	WATCH(numBlocks_var);
	WATCH(DHT_port_var);
	WATCH(pstr_var);
    	WATCH(pstrlen_var);
	WATCH(keep_alive_var);
	WATCH(have_supression_var);
	WATCH(chocking_interval_var);
	WATCH(downloaders_var);
	WATCH(optUnchockedPeers_var);
	WATCH(optUnchocking_interval_var);
	WATCH(snubbing_interval_var);
	WATCH(rarest_list_size_var);
	WATCH(minNumConnections_var);
	WATCH(maxNumConnections_var);
	WATCH(timeToSeed_var);
	WATCH(request_queue_depth_var);
	WATCH(super_seed_mode_var);
	WATCH(maxNumEmptyTrackerResponses_var);
	WATCH(newlyConnectedOptUnchokeProb_var);
	WATCH(currentNumConnections_var);
	WATCH(pendingNumConnections_var);
	WATCH(downloadDuration_var);
	WATCH(currentNumEmptyTrackerResponses_var);
	WATCH(state_var);

	evtChokeAlg = new cMessage(toString(INTERNAL_CHOKE_TIMER), INTERNAL_CHOKE_TIMER);
	evtOptUnChoke = new cMessage(toString(INTERNAL_OPT_UNCHOKE_TIMER), INTERNAL_OPT_UNCHOKE_TIMER);
	evtTrackerComm = new cMessage(toString(INTERNAL_TRACKER_COM_MSG), INTERNAL_TRACKER_COM_MSG);
	evtAggRate = new cMessage(toString(INTERNAL_MEASURE_AGGREGATE_RATE_TIMER), INTERNAL_MEASURE_AGGREGATE_RATE_TIMER);
	

	//Immidiately schedule the communication with the tracker. We will then learn by the tracker about the interval between requests.

	if (strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12) == 0) {

	      scheduleAt(simTime() + uniform(0, 5), evtTrackerComm);

	} else {

	      scheduleAt(simTime() + 5, evtTrackerComm);
	      scheduleAt(simTime(), evtAggRate);
	}


}

// void BTPeerWireBase::receiveChangeNotification(int category, cPolymorphic *details)
// {
// 
//     if (strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12) != 0) {
// 
// 	  Enter_Method_Silent();
// 	  printNotificationBanner(category, details);
// 
// 	  if (category == NF_L2_ASSOCIATED)
// 	  {
// 
// // 	      EV << "myEntry: " << (cPolymorphic *)myEntry << endl;
// // 	      EV << "details: " << details << endl;
// // 	      if ((cPolymorphic *)myEntry != details) {
// // 		      EV << "Notification is not to this agent, not starting scanning\n";
// // 		      return;
// // 	      }
// 	      
// 	      if(!(evtTrackerComm->isScheduled())) {
// 		    scheduleAt(simTime(), evtTrackerComm);
		   //setDownloadDuration(simTime());
// 	      }
// 	  }
//     }
// }
/**
 * This method is called in order to close the server socket listening for incoming
 * connection requests. This way the number of allowed TCP connections is capped.
 */
void BTPeerWireBase::stopListening()
{
	int sockstate = serverSocket.state();
	if (sockstate==TCPSocket::LISTENING)
	{
		serverSocket.close();
		serverSocket.renewSocket();
	}
}

/**
 * This method is called in order to prepare the server socket for incoming connection requests,
 * in case it was previously closed to cap the number of allowed TCP connections.
 */
void BTPeerWireBase::startListening()
{
	int sockstate = serverSocket.state();

	if ((sockstate==TCPSocket::NOT_BOUND) && (state()!=EXITING))
	{
		const char *address = par("address");
		int port = par("port");

		serverSocket.setOutputGate(gate("tcpOut"));
		serverSocket.bind(address[0] ? IPvXAddress(address) : IPvXAddress(), port);
		serverSocket.listen();
	}
}

/**
 * Main peer wire protocol functionality. This method manages connections/messages/timers etc.
 * The major deviation from the standard TCPSrvHost (INET) is that it also supports active
 * establishment of connections (and the corresponding threads).
 *
 *
 */
void BTPeerWireBase::handleMessage(cMessage *msg)
{
   	if (msg->isSelfMessage())
    	{
		if (msg->kind()<_PEER_WIRE_BASE_MSG_FLAG)
			handleSelfMessage(msg);
		else
			handleThreadMessage(msg);
    	}// !=selfMessage()
    	else
    	{
		//TODO: Make this a method call ...
		if (msg->arrivedOn("btorrentIn"))
		{
			if (state() < EXITING)
			{
				//This is the only msg possible to receive from the tracker client. All checks have already been
				//performed by that module.
				BTTrackerMsgResponse* trackerResponse_msg = check_and_cast<BTTrackerMsgResponse*>(msg);

				//Keep this response for reference (until the next one)...
				setTrackerResponse(trackerResponse_msg);

				setAnnounceInterval(trackerResponse()->announceInterval());

				BEV<<"received a Tracker Response containing "<< trackerResponse()->peersArraySize()<<" peers."<<endl;
 				

// 				if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_1")) {
// 				    cerr<<"\n*********received a Tracker Response containing "<< trackerResponse()->peersArraySize()<<" peers. SimTime: "<< simTime()<<"********\n" <<endl;
// 				    printConnections();
// 				    //localBitfield()-> printRemainingPieces();
// 				    //localBitfield()-> printRemainingBlocks();
// 				    //peerState.print();
// 				    
// 				}

				//Based on the peer dictionary, establish connections to remote peers.
				//Furthermore, initiate the (optimistic un-)choking procedures.
				if (trackerResponse()->peersArraySize() > 0)
				{
					//setCurrentNumEmptyTrackerResponses(0);
					//scheduleConnections(trackerResponse());

					//Check if we have already scheduled the execution of the algorithms i.e. this is not the first
					//tracker response received
					
// 					if (!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder") && seederNum > 1) {
// 					    if (trackerResponse()->peersArraySize() == 1) {
// 						  BEV<<"NOT Scheduling choking algorithm:  "<< simTime()<<endl;
// 						  setCurrentNumEmptyTrackerResponses(currentNumEmptyTrackerResponses()+1);
// 
// 						  if ((currentNumEmptyTrackerResponses() >= maxNumEmptyTrackerResponses()) && (peerState.size()==0) && (state()!=SEEDING))
// 						  {
// 							  BEV<<"reached maximum allowed number of empty subsequent tracker responses ( ="<<maxNumEmptyTrackerResponses() <<")."<<endl;
// 							  setState(EXITING);
// 							  stopChokingAlorithms();
// 							  /*if ((!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder"))) {
// 							      cancelAndDelete(evtTrackerComm2);
// 							      evtTrackerComm2 = NULL;
// 							      ExitFlag = true;
// 							  }*/					
// 
// 							  //Setting download duration to zero: will be interprented as a failure
// 							  //in BTStatistics
// 							  setDownloadDuration(0);
// 							  scheduleAt(simTime(),new cMessage(toString(INTERNAL_EXIT_MSG), INTERNAL_EXIT_MSG));
// 						  }
// 					    }
// 					    else {
// 						  setCurrentNumEmptyTrackerResponses(0);
// 						  scheduleConnections(trackerResponse());
// 						  if (!((cMessage*)evtChokeAlg)->isScheduled()) {
// 						    scheduleAt(simTime(), evtChokeAlg);
// 						    BEV<<"Scheduling choking algorithm:  "<< simTime()<<endl;
// 						  }
// 						  if (!((cMessage*)evtOptUnChoke)->isScheduled()) {
// 							  scheduleAt(simTime(), evtOptUnChoke);
// 						  }						  
// 					    }
// 					}
// 
// 					else {

					    setCurrentNumEmptyTrackerResponses(0);
					    scheduleConnections(trackerResponse());

					    if (!((cMessage*)evtChokeAlg)->isScheduled()) {
						    scheduleAt(simTime(), evtChokeAlg);
						    BEV<<"Scheduling choking algorithm:  "<< simTime()<<endl;
					    }
					    if (!((cMessage*)evtOptUnChoke)->isScheduled()) {
						    scheduleAt(simTime(), evtOptUnChoke);
					    }
// 					}
				}
				else
				{
					setCurrentNumEmptyTrackerResponses(currentNumEmptyTrackerResponses()+1);

					//Exiting due to empty response will be scheduled only if this is not a Seeding (not seeder) node.
					//Else, exit would be scheduled twice!
					if ((currentNumEmptyTrackerResponses() >= maxNumEmptyTrackerResponses()) && (peerState.size()==0)
						&& (state()!=SEEDING))
					{
						cerr<<"reached maximum allowed number of empty subsequent tracker responses ( ="<<maxNumEmptyTrackerResponses() <<")."<<endl;
						BEV<<"reached maximum allowed number of empty subsequent tracker responses ( ="<<maxNumEmptyTrackerResponses() <<")."<<endl;
						setState(EXITING);
						stopChokingAlorithms();				

						//Setting download duration to zero: will be interprented as a failure
						//in BTStatistics
						setDownloadDuration(0);
						scheduleAt(simTime(),new cMessage(toString(INTERNAL_EXIT_MSG), INTERNAL_EXIT_MSG));
					}
				}

				//Scheduling next contact with the tracker.
				if(!(evtTrackerComm->isScheduled())) {
				      scheduleAt(simTime()+announceInterval(), evtTrackerComm);
				}


			}

			delete msg;
		}
		else
		{
			TCPSocket *socket = socketMap.findSocketFor(msg);

			if (!socket)
			{
				//This is not a self-message. If it is not a TCP_I_ESTABLISHED message then there should
				//exist a socket for it.
				if (msg->kind()==TCP_I_ESTABLISHED)
				{

					// new connection -- create new socket object and server process
					socket = new TCPSocket(msg);
					socket->setOutputGate(gate("tcpOut"));

					const char *serverThreadClass = (const char*)par("serverThreadClass");
					TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

					socket->setCallbackObject(proc);
					//proc->init(this, socket);

					BTPeerWireClientHandlerBase* myProc = (BTPeerWireClientHandlerBase*) proc;
					//fix 22.3.2010
					myProc->init(this, socket);
					myProc->setActiveConnection(false);

					//NOTE: Since we dont have the peer's name we just make an entry with the IP address and port.
					// We will update this entry with the peerId once we have received it via the Handshake msg.
					ostringstream o;
					o << socket->remoteAddress().str()<< "_"<<socket->remotePort();
					PEER peer;

					opp_string* tmp = new opp_string(o.str().c_str());
					peer.peerId = *tmp;
					myProc->setRemotePeerID(o.str().c_str());

					peer.ipAddress = socket->remoteAddress();
					peer.peerPort = socket->remotePort();
					peerState.addPeer(peer,proc,simTime());
					socketMap.addSocket(socket);
					increasePendingNumConnections();

					int index  = peerState.findPeer(peer.ipAddress);
					bool rejectConn = false;

					if (index>=0)
						rejectConn = connectionAlreadyEstablished(index);

					if (!rejectConn)
					{
						BEV<<"established ('passive') connection to "<<socket->remoteAddress()<<":"<<socket->remotePort()<<" , adding peerInfo..."<<endl;

						updateDisplay();
						socket->processMessage(msg);
					}
					else
					{
						//The peer-state will be taken care of later when the connection will have closed
						//i.e. the thread itself will indicate that it must be removed from the state.
						socket->close();

						//By closing the socket here the current number of connections is decreased
						//however "established" was never called so we have to increase it again.
						increaseCurrentNumConnections();
						delete msg;
					}
				}
				else
				{
					delete msg;
					//error("%s:%d at %s() Peer-wire protocol error, invalid message type (msg->kind() = %d)\n", __FILE__, __LINE__, __func__,msg->kind());
				}
			}
			else
			{
				int kind = msg->kind();

				bool rejectConn = false;

				if (kind==TCP_I_ESTABLISHED)
				{
					BEV<<"a connection initiated by this peer is now established."<<endl;

					int connIndex  = peerState.findPeer(socket->remoteAddress());
					if (connIndex>=0)
					{
						rejectConn = connectionAlreadyEstablished(connIndex);
						peerState.getPeerEntryRef(connIndex)->setConnTime(simTime());
					}
				}

				if (!rejectConn)
				{
					socket->processMessage(msg);
				}
				else
				{
					socket->close();

					//By closing the socket here the current number of connections is decreased
					//however "established" was never called so we have to increase it again.
					increaseCurrentNumConnections();
					delete msg;
				}
			}
		}
	}
}

/**
 * Handles a message sent by a thread to inform this Peer Wire protocol base module about an event or
 * an action that must take place.
 */

void BTPeerWireBase::handleThreadMessage(cMessage* msg)
{
	TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->contextPointer();
	BTPeerWireClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

	switch (msg->kind())
	{
		case INTERNAL_REMOVE_THREAD_MSG:
		{
			//This socket may be removed only if it has closed or failed (due to a timeout e.g. the remote peer exited)
			if ((thread->socket()->state()!= TCPSocket::CLOSED) && (thread->socket()->state()!= TCPSocket::SOCKERROR))
			{
				scheduleAt(simTime()+100,msg);
			}
			else
			{
// 				if (strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12)!=0) {
// 				      handler->calculateThreadUploadRate();
// 				      //handler->calculateThreadDownloadRate();
// 				}
				removeThread(thread);
				peerState.removePeer((handler)->getRemotePeerID());
				delete msg;
			}
			break;
		}
		case INTERNAL_UPDATE_THREAD_MSG:
		{
			BTInternalMsg* upmsg = (BTInternalMsg*)msg;

			PEER peer = upmsg->peer();

			int index = peerState.findPeer((string)upmsg->text());

			if (index>=0)
				peerState.updatePeerID(index, peer.peerId.c_str());
			else
				error("%s:%d at %s() Inconsistent peer state, received a handshake msg from an unknown connection. \n", __FILE__, __LINE__, __func__);

			delete upmsg;
			break;
		}

		case INTERNAL_SUPER_SEED_HAVE_MSG:
		{
			BEV<<"in Super-Seed mode: sending a Have message for a rare piece"<<endl;
			scheduleSuperSeedHaveMsg(thread);
			delete msg;
			break;
		}
		case INTERNAL_SUPER_SEED_COMPLETE_MSG:
		{
			BTInternalMsg* intMsg = check_and_cast<BTInternalMsg*>(msg);
			BlockItem* item = new BlockItem(intMsg->pieceIndex(), numBlocks()-1,intMsg->text());
			superSeedPending.push_back(item);
			delete msg;
			break;
		}
		case INTERNAL_UPDATE_INTERESTS_MSG:
		{
			updateInterests();
			delete msg;
			break;
		}
		case BITFIELD_MSG:
		{
			BEV<<"received a Bitfield message, updating piece frequencies."<<endl;
			pieceFreqState.updatePieceFrequencies(check_and_cast<BTBitfieldMsg*>(msg));

			if (state() == SEEDER)
			{
				checkandScheduleHaveMsgs(check_and_cast<BTBitfieldMsg*>(msg), handler->getRemotePeerID().c_str());
			}
			else
			{
				//NOTE: Super-seeding is only recommended for initial seeding servers and NOT for general use.
				//In effect, this seeding server will never be requiered to send a (NOT_)INTERESTED msg.
				//Check whether we are interested in this peer.
				//In any case, send the appropriate msg and set the AM_INTERESTED to true/false
				int interest = amInterested(check_and_cast<BTBitfieldMsg*>(msg));
				if (interest > 0)
				{
					if (interest == numPieces())
						handler->setSeeder(true);
					if (!handler->amInterested())
						thread->timerExpired(new cMessage(toString(INTERESTED_TIMER),INTERESTED_TIMER));
				}
				else
				{
					if (handler->amInterested()) 
						thread->timerExpired(new cMessage(toString(NOT_INTERESTED_TIMER),NOT_INTERESTED_TIMER));
					
				}
			}
			delete msg;
			break;
		}
		case HAVE_MSG:
		{
			int index = check_and_cast<BTHaveMsg*>(msg)->index();
			//BEV<<"received a Have message, updating piece's #"<< index <<" frequency."<<endl;
			pieceFreqState.increasePieceFrequency(index);
			//Check whether we were not interested in this peer and this piece makes us interested.
			if ((!handler->amInterested()) && (amInterested(handler->getRemoteBitfield())>0))
			{
				thread->timerExpired(new cMessage(toString(INTERESTED_TIMER),INTERESTED_TIMER));
			}

			delete msg;
			break;
		}
		case UNCHOKE_MSG:
		{
			//BEV<<"received an Unchoke message, from peer "<<handler->getRemotePeerID()<<"."<<endl;
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_0")) {
// 			    cerr<<"\nreceived Unchoke message, from peer "<<handler->getRemotePeerID()<<". simTime: \n"<< simTime() <<endl;	
// 			}
			//BEV<<"received an Unchoke message, from peer "<<handler->getRemotePeerID()<<"."<<endl;
			//This message is simply ignored if this client does not want any data
		}
		case INTERNAL_NEXT_REQUEST_MSG:
		{
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_27")) {
// 			    cerr<<"scheduling next request, peerwirebase, simTime: "<< simTime()<<endl;
// 			    cerr<<"state: "<< state()<<endl;
// 			}

			if (state() < COMPLETED) 
			{
				/// v.1.1 Fix
				if(state() != ENDGAME)
				      makeNextPeerMove(thread);
				else
				      scheduleEndGameRequests();
			}

			delete msg;
			break;
		}
		case INTERESTED_MSG:
		{
			//This module will receive this message only if the remote peer is unchoked.
			//In this case, and if this peer has a better upload rate than a downloader
			// the latter must be replaced (by this one).
			if (handler->getDownloadRate() > peerState.minDownloaderRate())
				chokeWorstDownloader();

			delete msg;
			break;
		}
		case INTERNAL_RECORD_DATA_PROVIDER_TIMER:
		{
			dataProviderPeerIDs.insert(handler->getRemotePeerID());
			
			delete msg;
			break;
		}
		default:
		{
			//If there is a thread responsible for the message, let it handle it.
			if (thread!=NULL)
				thread->timerExpired(msg);
			else
				error("%s:%d at %s() Inconsistent thread state, could not find thread to handle this message. \n", __FILE__, __LINE__, __func__);

			break;
		}
	}

}

void BTPeerWireBase::updateBitField(int pieceIndex, int blockIndex, bool expected, const char* blockProvider)
{
// 	if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_27")) {
// 	    cerr<<"received a Piece message for piece #"<< pieceIndex <<" , block #"<< blockIndex<<endl;
// 	}

	localBitfield()->setLastDownloadTime(simTime());

	bool doUpdateBitfield = true;
	if (state() == ENDGAME)
	{
		scheduleEndGameCancel(pieceIndex,blockIndex,blockProvider);
		if (localBitfield()->isBlockAvailable(pieceIndex,blockIndex))
		{
			doUpdateBitfield = false;

		}
	}

	//Update the local bitfield and check whether this piece has completed
	//so as to possibly send Have messages and check if we finished downloading.
	//Do not do so however if this is a block already received (due to the endgame mode)
	if (doUpdateBitfield)
	{
		if (localBitfield()->update(pieceIndex,blockIndex, expected))
		{

			bool notCanceledEndGameReq = false;

			//Now check whether we have completed the downloading process
			if ((localBitfield()->finishedDownloading()) && (state() < COMPLETED))
			{
				setState(COMPLETED);
				setDownloadDuration(simTime()-downloadDuration());


				BEV<<"\ndownload of file completed! Downloaded = "<< fileSize()/1024 <<" MBs in "<< downloadDuration() << " seconds." <<endl;

				cerr<<"\n*********** "<<parentModule()->fullName()<< ": Download of file completed! Downloaded = "<< fileSize()/1024 <<" MBs in "<< downloadDuration() << " seconds. SimTime = "<< simTime()<<" ***********\n"<<endl;
				//cerr<<"\n***********Peers finished downloading = "<< finishedDownloads << "***********\n"<<endl;
				if (timeToSeed()>0)
				{
					setState(SEEDING);
					scheduleAt(simTime()+timeToSeed(),new cMessage(toString(INTERNAL_EXIT_MSG), INTERNAL_EXIT_MSG));
				}
				else
				{
					setState(EXITING);
					stopChokingAlorithms();
					scheduleAt(simTime(),new cMessage(toString(INTERNAL_EXIT_MSG), INTERNAL_EXIT_MSG));
					return;
				}

			}
			else if ((localBitfield()->finishedDownloading()) && (state() >= COMPLETED))
			{
				BEV<<"received a block item more than once, could not cancel end-game mode request."<<endl;
				notCanceledEndGameReq = true;
			}

			//Do not inform peers if exiting or if they have been previously informed
			if ((state() != EXITING) && (!notCanceledEndGameReq))
			{
				informPeers(pieceIndex);
			}
		}
	}
}

/**
 * Handles a message scheduled by this Peer Wire protocol base module. It practically handles timers
 * scheduled by this module for itself.
 */

void BTPeerWireBase::handleSelfMessage(cMessage* msg)
{
	switch (msg->kind())
	{
		
		case INTERNAL_TRACKER_COM_MSG:
		{
			BEV << "instructing communication with the tracker..."<<endl;
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_57")) {
// 			      cerr << "\n************instructing communication with the tracker... simTime: "<< simTime()<< "******\n" <<endl;
// 			      //cerr << "numremaining pieces: " << localBitfield()->numRemainingPieces() << endl;
// 			      //localBitfield()->printRemainingPieces();
// 				    
// 			}


			switch (state())
			{
				case NORMAL:
				case ENDGAME:
				case SEEDER:
				{

					send(new cMessage(toString(EVT_CONN),EVT_CONN),"btorrentOut");
					break;
				}
				case COMPLETED:
				case SEEDING:
				{
					send(new cMessage(toString(EVT_COMP),EVT_COMP),"btorrentOut");
					break;
				}
				case EXITING:
				{
					send(new cMessage(toString(EVT_STOP),EVT_STOP),"btorrentOut");
					break;
				}
				default:
					error("%s:%d at %s() Invalid client state (STATE = %d). \n", __FILE__, __LINE__, __func__,state());

			}

			//NOTE: Take a look at not in handleMessage() from gate "btorrentIn"...
			//if ((state()<EXITING) && (announceInterval()>0))
			//	scheduleAt(simTime()+announceInterval(), evtTrackerComm);

			break;
		}
		
		case INTERNAL_CHOKE_TIMER:
		{
			BEV<<"running chocking algorithm."<<endl;
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_20")) {
// 			      cerr << "numremaining pieces: " << localBitfield()->numRemainingPieces() << endl;
// 			      localBitfield()->printRemainingPieces();
// 				    
// 			}

			ChokingAlgorithm();
			scheduleAt(simTime() + chockingInterval(), evtChokeAlg);

			break;
		}
		case INTERNAL_OPT_UNCHOKE_TIMER:
		{
			BEV << "optimistic unchoking..."<<endl;
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_20")) {
// 			      cerr << "numremaining pieces: " << localBitfield()->numRemainingPieces() << endl;
// 			      localBitfield()->printRemainingPieces();
// 				    
// 			}

			OptimisticUnChokingAlgorithm();
			scheduleAt(simTime()+ optUnchockingInterval(), evtOptUnChoke );

			break;
		}
		case INTERNAL_INIT_CONNECTION_MSG:
		{
			if (state()< EXITING)
			{
				//Get the information for the peer that we are going to connect and send a handshake
				BTInternalMsg* intMsg = (BTInternalMsg*)msg;
				PEER peer = intMsg->peer();

				BEV <<"initializing a thread for the connection with peer: " << peer.peerId <<"."<<endl;
				BEV <<"Peer IP address: " << peer.ipAddress <<"."<<endl;

				// new connection -- create new socket object and server process
				TCPSocket *newsocket = new TCPSocket();

				unsigned short port = tcp->getEphemeralPort();
				int peerWirePort = par("port");

				//It could be the case that the peerWire port is freed so that we do not accept connections.
				while ((port==peerWirePort))
					port = tcp->getEphemeralPort();

				newsocket->bind(port);

				const char *serverThreadClass = (const char*)par("serverThreadClass");

				TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

				newsocket->setCallbackObject(proc);
				//proc->init(this, newsocket);

				BTPeerWireClientHandlerBase* myProc = (BTPeerWireClientHandlerBase*) proc;
				//fix 22.3.2010
				myProc->init(this, newsocket);
				myProc->setActiveConnection(true);
				myProc->setRemotePeerID(peer.peerId.c_str());
				peerState.addPeer(peer,proc);

				newsocket->setOutputGate(gate("tcpOut"));

				newsocket->connect(peer.ipAddress, peer.peerPort);
				socketMap.addSocket(newsocket);

				increasePendingNumConnections();
				updateDisplay();
			}

			delete msg;
			break;
		}
		case INTERNAL_EXIT_MSG:
		{
			if (state()==SEEDING)
			{
				setState(EXITING);
				stopChokingAlorithms();
			}

			BEV<<"exiting application ..."<<endl;
			BEV<<"closing all peer wire connections..."<<endl;

			
			cerr<<"exiting application ..."<<endl;
			cerr<<"closing all peer wire connections...: " << simTime() <<endl;
			cerr<<"\n*********** "<<parentModule()->fullName()<< "***********\n" <<endl;
			TCPServerThreadBase *thread;
			PeerEntryVector peerVector = peerState.getVector();
			BTPeerWireClientHandlerBase* handler;


			for (unsigned int i=0; i<peerVector.size(); i++)
			{
				PeerEntry entry= peerVector[i];
				thread = check_and_cast<TCPServerThreadBase *>(entry.getPeerThread());
				handler = (BTPeerWireClientHandlerBase*)thread;
				if (!thread)
					error("%s:%d at %s() Inconsistent thread state, could not find peer thread. \n", __FILE__, __LINE__, __func__);

				thread->timerExpired(new cMessage(toString(CLOSE_CONNECTION_TIMER),CLOSE_CONNECTION_TIMER));
				//downloadRateStats.insert(downloadRateStats.end(), handler->getDownloadRate());
			}
			
			//calculateAggregateDownloadRate();
			stopListening();
			delete msg;

			scheduleAt(simTime()+2000, new cMessage(toString(INTERNAL_EXIT_SAFE_MSG),INTERNAL_EXIT_SAFE_MSG));

 			//scheduleAt(simTime()+2000, new cMessage(toString(INTERNAL_EXIT_SAFE_MSG),INTERNAL_EXIT_SAFE_MSG));
			//scheduleAt(simTime(), new cMessage(toString(INTERNAL_EXIT_SAFE_MSG),INTERNAL_EXIT_SAFE_MSG));
			break;

		}
		case INTERNAL_EXIT_SAFE_MSG:
		{
			delete msg;
// 			cerr<<"***** INTERNAL EXIT SAFE MESSAGE *****: " << simTime() <<endl;
// 			cerr<<"***** "<<parentModule()->fullName()<<" TRYING TO EXIT SAFELY *****"<<endl;
// 			cerr<<"***** "<<peerState.size()<<" PEER STATE SIZE *****"<<endl;
// 			cerr << "numremaining pieces: " << localBitfield()->numRemainingPieces() << endl;
// 			printConnections();
			//Wait until all connections have been closed properly
// 			if ((peerState.size() > 0) || (evtTrackerComm->isScheduled()))
// 			{
// 				//FIXME: No time available ... remove this hardcoded value (working nice though)
// 				//cerr<<"***** RESCHEDULE EXIT MESSAGE *****: " << simTime() <<endl;
// 				scheduleAt(simTime()+1000, new cMessage(toString(INTERNAL_EXIT_SAFE_MSG),INTERNAL_EXIT_SAFE_MSG));
// 
// 			}
// 			else
// 			{
				BEV<<"***** EXITING SAFELY *****"<<endl;
				cerr<<"\t\t\t\t\t***** "<<parentModule()->fullName()<<" EXITING SAFELY *****"<<endl;
				
				if (strncmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder", 12)!=0) 
				{
					BEV<<"recording collected statistics..."<<endl;
					calculateAverageDownloadRate();
					calculateAverageUploadRate();
					writeStats();
				}

				//Clean up ...
				cancelAndDelete(evtTrackerComm);
				deleteTrackerResponse();
				superSeedPending.clear();
				endGameRequests.clear();
				peerState.clear();
				dataProviderPeerIDs.clear();
				pieceFreqState.clear();
				downloadRateStats.clear();
				aggregateDownloadRateSamples.clear();
				aggregateUploadRateSamples.clear();
				uploadRateStats.clear();

				BEV<<"Exit."<<endl;
//  			}
			//delete msg;
			break;
		}
		case INTERNAL_MEASURE_AGGREGATE_RATE_TIMER:
		{
			//delete msg;
			TCPServerThreadBase *thread;
			BTPeerWireClientHandlerBase* handler;
			PeerEntryVector peerVector = peerState.getVector();
			//printf("\n****MEASURE AGGREGATE RATE******\n");
			if (peerVector.size() > 0) {
			      for (unsigned int i=0; i<peerVector.size(); i++)
			      {
				      PeerEntry entry= peerVector[i];
				      thread = check_and_cast<TCPServerThreadBase *>(entry.getPeerThread());
				      handler = (BTPeerWireClientHandlerBase*)thread;
				      if (!thread)
					      error("%s:%d at %s() Inconsistent thread state, could not find peer thread. \n", __FILE__, __LINE__, __func__);
				      downloadRateStats.insert(downloadRateStats.end(), handler->receivedBlocks());
				      handler->setReceivedBlocks(0);
				      //printf("\n****Aggregate Sampled Download rate = %f, simtime = %f, Peer: %s\n******", handler->getDownloadRate(), simTime(), handler->getRemotePeerID().c_str());
				      uploadRateStats.insert(uploadRateStats.end(), handler->sentBlocks());
				      //printf("\n sent blocks: %f\n", handler->sentBlocks());
				      handler->setSentBlocks(0);

			      }
			}

			calculateAggregateDownloadRate();
			calculateAggregateUploadRate();
			scheduleAt(simTime()+ getDownloadRateSamplingDuration() , evtAggRate);
			break;

		}

		default:
			error("%s:%d at %s() Unknown self-message type (msg->kind() = %d). \n", __FILE__, __LINE__, __func__,msg->kind());
	}
}

/**
 * Here we try to avoid the situation in which a peer thread is unchoked but it cannot ask for more
 * blocks because they have been marked as requested BUT the peer thread that marked them as such
 * has been choked. The former thread must be informed so that it can ask for data.
 *
 */

void BTPeerWireBase::updateInterests()
{
	TCPServerThreadBase *thread;
	BTPeerWireClientHandlerBase* handler;
	PeerEntryVector peerVector = peerState.getVector();

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		PeerEntry entry= peerVector[i];
		thread = check_and_cast<TCPServerThreadBase *>(entry.getPeerThread());
		handler = (BTPeerWireClientHandlerBase*)thread;

		///FIX 22.3.2010
		if ( handler->state() >= HANDSHAKE_COMPLETE ) {

		      if ((!handler->allowedToRequest()) && (!handler->peerChoking()) && (amInterested(handler->getRemoteBitfield())))
		      {
			      handler->setAllowedToRequest(true);

			      if (state() != ENDGAME) {

				      makeNextPeerMove(thread);
			      } 
			      else {
				      scheduleEndGameRequests();

			      }
		      }

		}
	}
}

/**
 * Help method used to print information about this clients peer state i.e. connections with other peers.
 *
 */

void BTPeerWireBase::printConnections()
{
	TCPServerThreadBase *thread;

	PeerEntryVector peerVector = peerState.getVector();

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		PeerEntry entry= peerVector[i];
		thread = check_and_cast<TCPServerThreadBase *>(entry.getPeerThread());
		if (!thread)
			error("%s:%d at %s() Inconsistent thread state, could not find peer thread. \n", __FILE__, __LINE__, __func__);

		BTPeerWireClientHandlerBase* handler  =  (BTPeerWireClientHandlerBase*)thread;
		cerr<<"\t\t\t\t"<<parentModule()->fullName()<<" connected to  "<<handler->getRemotePeerID()<<", State = "<<handler->state()<<", SocketState = "<< handler->socketState()<<", Time: "<< entry.connTime()<<" Active: "<< handler->activeConnection()<<endl;
		//BEV<<"\t\t\t\t"<<parentModule()->fullName()<<" connected to  "<<handler->getRemotePeerID()<<", State = "<<handler->state()<<", SocketState = "<< handler->socketState()<<", Time: "<< entry.connTime()<<" Active: "<< handler->activeConnection()<<endl;
	}
}

void BTPeerWireBase::stopChokingAlorithms()
{
	cancelAndDelete(evtChokeAlg);
	cancelAndDelete(evtOptUnChoke);
	cancelAndDelete(evtAggRate);
    // load the variables with NULL so that subsequent calls to this function
    // does not result in an error, since cancelAndDelete does nothing with
    // NULL pointers
    evtChokeAlg = NULL;
    evtOptUnChoke = NULL;
    evtAggRate = NULL;
}

/**
 * Schedules the transmission of a Have Msg in super-seed mode i.e. a Have msg redarding a rare piece not
 * available at the remote peer.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::scheduleSuperSeedHaveMsg(TCPServerThreadBase* thread)
{
	BTPeerWireClientHandlerBase* handler = check_and_cast<BTPeerWireClientHandlerBase*>(thread);

	BlockItem* bi = selectPiece(handler->getRemoteBitfield(),true);

	if (bi->isValid())
	{
		BTHaveMsg* have = new BTHaveMsg("HAVE_TIMER",HAVE_TIMER);
		have->setByteLength(HAVE_MSG_SIZE);
		have->setIndex(bi->pieceIndex());
		have->setLength_prefix(HAVE_MSG_SIZE);
		have->setID(HAVE);

		thread->timerExpired(have);
	}
	else
		error("%s:%d at %s() A peer that has completed downloading, is connected to the initial seeder. \n", __FILE__, __LINE__, __func__);

	delete bi;
}

/**
 * Makes the next move regarding a certain peer that has just provided a piece or has just unchoked this client.
 * This may either mean requesting one more piece or denoting that this client is not interested in this remote
 * peer any more.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::makeNextPeerMove(TCPServerThreadBase* thread)
{

	
	BTPeerWireClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;

	BlockItem* bi = selectPiece(handler->getRemoteBitfield(),false);

// 	if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_6")) {
// 	      cerr << "makeNextPeerMove called. simTime: " << simTime() << endl;
// 	      cerr << "piece #" << bi->pieceIndex() << " block #"<< bi->blockIndex() << endl;
// 
// 	}

	if (bi->isValid())
	{
		if (!handler->peerChoking())
		{
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_6")) {
// 			      cerr << "valid, send block requests. simTime: " << simTime() << endl;
// 
// 			}
			handler->sendBlockRequests(bi->pieceIndex(),bi->blockIndex());
		}

	}
	else
	{
		// This is either the case in which the peer does not have anything interesting to offer
		// or the case in which have already requested the remainig pieces from another peer.
		if (amInterested(handler->getRemoteBitfield())==0)
		{
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_6")) {
// 				cerr << "makeNextPeerMove not interested. simTime: " << simTime() << endl;;
// // 				cerr << "numrequested blocks: " << localBitfield()->numRequestedBlocks() << endl;
// // 				cerr << "numremaining blocks: " << localBitfield()->numRemainingBlocks() << endl;
// 
// 			}
			thread->timerExpired(new cMessage(toString(NOT_INTERESTED_TIMER),NOT_INTERESTED_TIMER));
		}
		else
		{
// 			if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_6")) {
// 			      cerr << "set allowed to request false. simTime: " << simTime() << endl;
// 
// 			}
			handler->setAllowedToRequest(false);
		}
	}

// 	if (!strcmp(this->parentModule()->fullName(), "overlayTerminal_6")) {
// 	      cerr << "makenextpeermove simTime: " << simTime() << endl;
// 	      cerr << "numrequested blocks: " << localBitfield()->numRequestedBlocks() << endl;
// 	      cerr << "numremaining blocks: " << localBitfield()->numRemainingBlocks() << endl;
// 	      cerr << "numrequested pieces: " << localBitfield()->numRequestedPieces() << endl;
// 	      cerr << "numremaining pieces: " << localBitfield()->numRemainingPieces() << endl;
// 	      //localBitfield()->printRemainingPieces();	
// 	      peerState.print();
// 
// 	}
			

	delete bi;
}

/**
 * Choking algorithm as specified in the <a href=\"http://wiki.theory.org/BitTorrentSpecification\"> BitTorrent
 * Protocol Specification v1.0. </a>
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::ChokingAlgorithm()
{

	TCPServerThreadBase* thread;
	BTPeerWireClientHandlerBase *handler;

	PeerEntryVector peerVector = peerState.getVector();

	//First short the peerVector in decreasing download rate order.
	sort(peerVector.rbegin(), peerVector.rend());

	int numDownloaders = 0;

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		PeerEntry* peer= &peerVector[i];
		thread = (TCPServerThreadBase*)peer->getPeerThread();
		handler = (BTPeerWireClientHandlerBase*)thread;


		//Ensuring that the algorithm acts upon established and not pending connections
		//and that will not result in (un-)choking a seeder.
		//Also ensuring that the combination of (opt-un-)choke timers values does not result
		//in fibrilation.

		///v.1.1 fix: Changed CONNECTED to HANDSHAKE_COMPLETE
		if ((handler->state()<HANDSHAKE_COMPLETE) || (handler->seeder())
			|| ((handler->lastChokeUnchoke() + chockingInterval() > simTime()) && handler->lastChokeUnchoke()!=0))
		{
			continue;
		}

		if (handler->amChoking())
		{

			if (handler->peerInterested())
			{

				if (numDownloaders < downloaders())
				{

					if (handler->getDownloadRate()<= peerState.minDownloaderRate())
						peerState.setMinDownloaderRate(handler->getDownloadRate());


					thread->timerExpired(new cMessage(toString(UNCHOKE_TIMER),UNCHOKE_TIMER));
					numDownloaders++;
				}
				else
				{
					BEV<<"[Choking Algorithm]: reached maxNumDownloaders. Not adding "<<handler->getRemotePeerID()<<" to Downloaders. "<<endl;
				}
			}
			else
			{

				if (handler->getDownloadRate()> peerState.minDownloaderRate())
				{
					BEV<<"[Choking Algorithm]: not Interested in "<<handler->getRemotePeerID()<<", unchoking though due to rate."<<endl;
					thread->timerExpired(new cMessage(toString(UNCHOKE_TIMER),UNCHOKE_TIMER));
				}
			}
		}
		else
		{
			if ((numDownloaders==downloaders()) && (!handler->isOptimisticallyUnchoked()))
			{
				BEV<<"[Choking Algorithm]: "<<handler->getRemotePeerID()<<" not in the top "<<downloaders()<<" of data providers (download rate), Choking it ..."<<endl;
				thread->timerExpired(new cMessage(toString(CHOKE_TIMER),CHOKE_TIMER));
			}
			else
			{
				BEV<<"[Choking Algorithm]: "<<handler->getRemotePeerID()<<" already in the top "<<downloaders()<<" of data providers (download rate)."<<endl;
				numDownloaders++;
			}

		}

	}//for
}


/**
 * Optimistic Unchoking algorithm. The implementation deviates from the <a href=\"http://wiki.theory.org/BitTorrentSpecification\"> specification </a>  in the following aspect:
 * the peer that is selected to be optimistically unchoked, is interested and does not count for a downloader. The reasoning
 * behind this deviation is that if this peer is not interested it means that it already has some data. In effect, the
 * bootstrapping aspect of this algorithm is weakened i.e. optimistic unchoking is also used (apart from finding better
 * connections) to let newly connected peers download some data that can later "exchange".
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::OptimisticUnChokingAlgorithm()
{

	//BEV<<"Optimistic Unchoking algorithm running now."<<endl;

	PeerEntry peer;
	TCPServerThreadBase* thread;
	BTPeerWireClientHandlerBase *handler;
	PeerEntryVector peerVector = peerState.getVector();
	vector<int> candidatesVector;
	set<int> optUnchoked;
	set<int>::iterator it;

	//PeerEntryVector antiSnubbingCandidatesVector;

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		peer= peerVector[i];
		thread = (TCPServerThreadBase*)peer.getPeerThread();
		handler = (BTPeerWireClientHandlerBase*)thread;

		//Ensuring that the algorithm acts upon established connections
		//and that will not result in (un-)choking a seeder.
		//Also ensuring that the combination of (opt-un-)choke timers values does not result
		//in fibrilation.
		if ((handler->state()<HANDSHAKE_COMPLETE) || (handler->seeder())
			|| ((handler->lastChokeUnchoke() + chockingInterval() > simTime()) && handler->lastChokeUnchoke()!=0) )
			continue;

		if ((handler->amChoking()))
		{
			candidatesVector.push_back(i);

			//Marking as antisnubbed
			//if ((!handler->isOptimisticallyUnchoked())
				//antiSnubbingCandidatesVector.push_back(peerVector[i]);
		}
		else if (handler->isOptimisticallyUnchoked())
		{
			optUnchoked.insert(i);
		}
	}

	//Now choose one of the candidates
	if (candidatesVector.size()>0)
	{
		int numUnchoked = 0;

		while ((numUnchoked < optUnchockedPeers()) && (candidatesVector.size()>0))
		{
			int optUnchokePeerIndex = chooseOptUnchokePeer(candidatesVector.size());
			if (optUnchokePeerIndex>-1)
			{
				peer = peerVector[candidatesVector[optUnchokePeerIndex]];
				candidatesVector.erase(candidatesVector.begin() + optUnchokePeerIndex);
			}

			thread = (TCPServerThreadBase*)peer.getPeerThread();
			handler = (BTPeerWireClientHandlerBase*)thread;

			BEV<<"optimistically unchoking peer: "<< handler->getRemotePeerID()<<"."<<endl;
			handler->setOptimisticallyUnchoked(true);


			thread->timerExpired(new cMessage(toString(UNCHOKE_TIMER),UNCHOKE_TIMER));
			numUnchoked++;
		}

		//If we have not reached the target number of optimistically unchoked peers
		//we will keep some of the previously optimistically unchoked.
		while (numUnchoked < optUnchockedPeers())
		{
			it  = optUnchoked.begin();
			optUnchoked.erase(it);
			numUnchoked++;
		}

		//Choke previously optimistically unchoked peer
		//-----------------------------------------------
		for (it=optUnchoked.begin(); it!=optUnchoked.end(); it++)
		{
			peer= peerVector[*it];
			thread = (TCPServerThreadBase*)peer.getPeerThread();
			handler = (BTPeerWireClientHandlerBase*)thread;

			BEV<<"choking peer: "<<handler->getRemotePeerID()<<" ,that was previously optimistically unchoked."<<endl;
			handler->setOptimisticallyUnchoked(false);

			thread->timerExpired(new cMessage(toString(CHOKE_TIMER),CHOKE_TIMER));
		}
	}
}

/**
 * Given the set of the candidate peers (for optimistic unchocking), it chooses one, giving priority to newly
 * connected peers. The followed <a href=\"http://wiki.theory.org/BitTorrentSpecification\"> specification </a>
 * states that "<i>Newly connected peers are three times as likely to start as the current optimistic unchoke as
 * anywhere else in the rotation</i>". This is interprented as follows:
 *
 * The candidate peers have been inserted in the candidates vector in decreasing connection time (the time elapsed
 * since the establishement of their connection with this client) i.e. the last peer in the vector is the most recently
 * connected. As defined in the specification, we select a newly connected peer with a probability three times greater
 * than the probability of selecting any other peer.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */

int BTPeerWireBase::chooseOptUnchokePeer(int candidatesVectorSize)
{
	int index = -1;

	if (dblrand()<=newlyConnectedOptUnchokeProb())
	{
		index =  (candidatesVectorSize-1);
	}
	else
	{
		index =  intrand(candidatesVectorSize);
	}

	return index;

}

/**
 * Called when a peer with an upload rate better than (at least) one of the current downloaders gets interested. In this case,
 * the downloader with the worst upload rate gets choked.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::chokeWorstDownloader()
{
	TCPServerThreadBase* thread;
	BTPeerWireClientHandlerBase *handler;

	PeerEntryVector peerVector = peerState.getVector();

	//First short the peerVector in decreasing download rate order.
	sort(peerVector.begin(), peerVector.end());

	int numDownloaders = 0;

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		PeerEntry* peer= &peerVector[i];
		thread = (TCPServerThreadBase*)peer->getPeerThread();
		handler = (BTPeerWireClientHandlerBase*)thread;

		//Ensuring that the algorithm acts upon established and not pending connections
		if (handler->state()<CONNECTED)
		{
			continue;
		}

		if (!handler->amChoking())
		{
			numDownloaders++;

			if (numDownloaders == downloaders())
			{
				thread->timerExpired(new cMessage(toString(CHOKE_TIMER),CHOKE_TIMER));
				return;
			}
		}
	}
}


/**
 * Schedules the establishment of connections with other peers. Note that this is orthogonal to the chocking algorithm.
 * The latter is rather enforced on the connections established by this method.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::scheduleConnections(BTTrackerMsgResponse* msg)
{
	checkConnections();
	int remainingConns = maxNumConnections()-currentNumConnections()-pendingNumConnections()-1;

	if (remainingConns<=0)
	{
		BEV<<"refraining from active connection establishment, reached limit of allowed connections."<<endl;
		return;
	}

	int index = -1;

	for (unsigned int i=0;i<msg->peersArraySize();i++)
	{
		PEER peer = msg->peers(i);
		opp_string id = peer.peerId;

		index  = peerState.findPeer(peer.ipAddress);

		//Keeping one for passive ...
		if ( (index<0) && (remainingConns>1))
		{
/*			
			if (!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder") && subtrackerNum > 0) {

			  if(!strcmp(peer.peerId.c_str(), "BTHostSeeder")) {
			       BEV<< "received peer "<< peer.peerId.c_str()<< " in tracker's response. Not Attempting connection ..."<<endl;
			  }
			  else {
			     
			      BEV<< "received peer "<< peer.peerId.c_str()<< " in tracker's response. Attempting connection ..."<<endl;
			      BTInternalMsg* intMsg = new BTInternalMsg("INTERNAL_INIT_CONNECTION_MSG",INTERNAL_INIT_CONNECTION_MSG);
			      intMsg->setPeer(peer);
			      scheduleAt(simTime(),intMsg);
			      remainingConns--;
			  }
			}
			else {*/
			  BEV<< "received peer "<< peer.peerId.c_str()<< " in tracker's response. Attempting connection ..."<<endl;
			  BTInternalMsg* intMsg = new BTInternalMsg("INTERNAL_INIT_CONNECTION_MSG",INTERNAL_INIT_CONNECTION_MSG);
			  intMsg->setPeer(peer);
			  scheduleAt(simTime(),intMsg);
			  remainingConns--;
// 			}
		}
		else //TODO: Remove PART OF THIS, used only for testing!
		{
			if (index >= 0)
			{
				BEV<< "received peer "<< peer.peerId.c_str()<< " in tracker's response. Already connected to this peer ..."<<endl;

			}

			if (remainingConns<=1)
			{
				BEV<< "received "<< peer.peerId.c_str()<< " in tracker's response. Refraining from connecting, reached maximum of connections (maxNumConnections = "<< maxNumConnections()<<")."<<endl;
				checkConnections();
				return;
			}
		}
	}
}


/**
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
bool BTPeerWireBase::connectionAlreadyEstablished(int connIndex)
{
	PeerEntry pe = peerState.getPeerEntry(connIndex);
	BTPeerWireClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)pe.getPeerThread();

	if (handler->state() >= CONNECTED)
		return true;

	return false;
}


/**
 * Selects the piece that this client will request from the (remote) peer. It actually implements the Rarest First policy.
 * Note that when this client has not downloaded any piece, this method actually implements the Random First policy.
 * In case superSeedMode var is set true, it operates in reverse mode i.e. it will select a rare piece available in this
 * client and not available in the remote peer.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */

BlockItem* BTPeerWireBase::selectPiece(BitField* remoteBitfield, bool superSeedMode)
{
	PieceFreqEntry pieceEntry;

	//First find the indexes and the frequencies of the pieces the peer can offer
	// and store them in a vector.
	PieceFreqEntryVector peerPieces;
	PieceFreqEntryVector::iterator it;
	int index,freq=-1;
	int size =0;
	PieceFreqEntryVector pieceFrequencies = pieceFreqState.getVector();

	for (it=pieceFrequencies.begin();it<pieceFrequencies.end();it++)
	{
		index = (*it).index();
		freq = (*it).frequency();

		if (!superSeedMode)
		{

			if ((remoteBitfield->isPieceAvailable(index)) && (!localBitfield()->isPieceAvailable(index)) && (freq!=0) && (!localBitfield()->isPieceRequested(index)))
			{
				peerPieces.push_back(*it);
				size++;
			}
		}
		else
		{
			if ((!remoteBitfield->isPieceAvailable(index)) && (localBitfield()->isPieceAvailable(index)))
			{
				peerPieces.push_back(*it);
				size++;
			}
		}
	}

	if (size>0)
	{
		//Sort this vector in rarest first order...
		sort(peerPieces.begin(), peerPieces.end());

		//If the first condition is true we actually implement the Random First Policy
		if ((!localBitfield()->havePiece()) || (rarestListSize()>=size))
		{
			if (size>1)
				index = intrand(size);
			else
				index = 0;
		}
		else
		{
			index = intrand(rarestListSize());
		}

		//Also return the offset within the piece.
		int pieceIndex = peerPieces[index].index();

		//Requesting the next of the currently available block
		//So that we can resume previously aborted pieces
		//==//int blockIndex  = localBitfield()->currentBlock(pieceIndex)+1;
		int blockIndex  = localBitfield()->nextBlock(pieceIndex);
		peerPieces.clear();

		return new BlockItem(pieceIndex,blockIndex);
	}
	else
	{
		return new BlockItem(-1,-1);
	}
}


/**
 * Updates the state about the requests placed by this client. A block is marked as requested when a thread
 * sends a request for it to a peer. The whole piece this block belongs to is also marked as requested so that
 * we avoid situation in which multiple threads request blocks of the same piece. This would induce unecessary
 * synchronization overhead. However, if a request is cancelled  (e.g. the peer has choked this client) the
 * blocks/pieces are set back to not requested so that no thread is blocked from requesting a block.
 *
 */
int BTPeerWireBase::updateBlockRequests(int pieceIndex,int  blockIndex, bool value)
{

	int result=-1;
	//Check whether this request triggers the endgame mode
	if ((state() != ENDGAME))
	{
		//We do not set the block as requested because if we are in such mode, this
		//has already been done.
		result = localBitfield()->setBlockRequested(pieceIndex,blockIndex,value);

		if ((enterEndGameMode()) && (value))
		{
			BEV<<"entering End-Game Mode ..."<<endl;
			setState(ENDGAME);
			scheduleEndGameRequests();
		}
	}

	return result;

}


/**
 * Checks the local bitfield against a remote one and determines whether this client is interested
 * in a piece available at the remote peer.
 *
 */
int BTPeerWireBase::amInterested(BTBitfieldMsg* msg)
{
	int interestingPieces = 0;

	if (state()<COMPLETED)
		for (unsigned int i=0;i<msg->bitfieldArraySize();i++)
		{
			if ((msg->bitfield(i)) && (!localBitfield()->isPieceAvailable(i)))
				interestingPieces++;
		}

	return interestingPieces;
}

/**
 * Checks the local bitfield against a remote one and determines whether this client is interested
 * in a piece available at the remote peer.
 */
int BTPeerWireBase::amInterested(BitField* bitField)
{
	int interestingPieces = 0;

	for (int i=0;i<bitField->numPieces();i++)
	{
		if ((bitField->isPieceAvailable(i)) && (!localBitfield()->isPieceAvailable(i)))
			interestingPieces++;
	}

	return interestingPieces;
}

/**
 * Schedules a Have msg to inform connected peers about the reception of this piece.
 *
 */
void BTPeerWireBase::informPeers(int pieceIndex)
{
	TCPServerThreadBase *thread;
	BTPeerWireClientHandlerBase* handler;

	PeerEntryVector peerVector = peerState.getVector();
	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		PeerEntry* entry= &peerVector[i];
		thread = (TCPServerThreadBase *)entry->getPeerThread();
		handler = (BTPeerWireClientHandlerBase*)thread;
		if (!thread)
			error("%s:%d at %s() Inconsistent thread state, could not find peer thread. \n", __FILE__, __LINE__, __func__);


		if (handler->state()>=HANDSHAKE_COMPLETE)
		{
			BTHaveMsg* have = new BTHaveMsg("HAVE_TIMER",HAVE_TIMER);
			have->setByteLength(HAVE_MSG_SIZE);
			have->setIndex(pieceIndex);
			have->setLength_prefix(HAVE_MSG_SIZE);
			have->setID(HAVE);

			thread->timerExpired(have);
		}
	}
}

/**
 * Determins whether this client should enter the end-game phase.
 * According to the <a href=\"http://wiki.theory.org/BitTorrentSpecification\"> specification </a>
 * "<i> There is no documented thresholds, recommended percentages, or block counts that could be used as a
 * guide or Recommended Best Practice here.</i>. In this implementation, we follow the approach described in:
 * B. Cohen, â€œIncentives build robustness in bittorrent,â€� in Proc. First Workshop on Economics of Peer-to-Peer Systems,
 * Berkeley, USA, June 2003 and also adopted in <a href=\"http://hal.inria.fr/inria-00000156/en\"> the technical report
 * suggested by the <a href=\"http://wiki.theory.org/BitTorrentSpecification\"> specification </a>. In this approach,
 * the end game mode "<i>... starts once a peer has requested all blocks, i.e., blocks are either requested or already
 * received.</i>" . At the same time, since the specification indicates that "<i> ... it's a good idea to keep the
 * number of the pending block low (1 or 2 blocks)..."</i>, we make a compromise and set this threashold to the maximum
 * size of the request queue.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
bool BTPeerWireBase::enterEndGameMode()
{
	if ((!enableEndGameMode()) || (state() >= ENDGAME)) {
		return false;
	}
	else {
		///v.1.1 fix
		return (localBitfield()->numNonRequestedNonAvailableBlocks() == 0);

		//return (localBitfield()->numRequestedBlocks() == localBitfield()->numRemainingBlocks());
	}
}

/**
 * This method schedules the transmission of the requests in the end-game phase i.e. the phase
 * in which all remaining blocks are requested by all non-choking peers in the peer set.
 *
 * This is impplemented with a double pass over the peer set. In the first pass all pending requests
 * are gathered and in the second pass each request is send to all peers (except the one it has already been sent to).
 * This may seem inefficient but it is only executed at the end game mode. Otherwise,
 * we would have to maintain all requests in this module rather than in each thread. This would reduce
 * the simplicity and flexibility, the threads offer us and would result in a high base-thread communication overhead
 * during the normal mode of the game, which is the major part of it.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::scheduleEndGameRequests()
{

	PeerEntryVector peerVector = peerState.getVector();
	TCPServerThreadBase* thread;
	BTPeerWireClientHandlerBase* handler;
	PeerEntry* peer;

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		peer= &peerVector[i];
		thread = (TCPServerThreadBase*)peer->getPeerThread();
		handler = (BTPeerWireClientHandlerBase*)thread;

		for ( int j=0; j<handler->getRequests().size(); j++)
		{
			///v.1.1 fix
			RequestEntry re = handler->getRequests().getRequestEntry(j).dup();
			
			if (!localBitfield()->isBlockAvailable(re.index(),re.begin()))
				endGameRequests.push_back(re);

			//endGameRequests.push_back(handler->getRequests().getRequestEntry(j).dup());
		}
	}

	for (unsigned int j=0; j<endGameRequests.size(); j++)
	{
		for (unsigned int i=0; i<peerVector.size(); i++)
		{
			peer= &peerVector[i];
			thread = (TCPServerThreadBase*)peer->getPeerThread();
			handler = (BTPeerWireClientHandlerBase*)thread;

			//The pending blocks (plus the ones carently  in the queues) may be more
			//than the queueing policy allows!
			//handler->getRequests().setMaxSize(numBlocks()*numPieces());
			handler->increaseRequestQueueSize(numBlocks()*numPieces());

			if ((strcmp(handler->getRemotePeerID().c_str(),endGameRequests[j].peerID())!=0) && (handler->state()>=HANDSHAKE_COMPLETE) && (!handler->peerChoking()))
			{
				handler->setInEndGame(true);
				///v.1.1 fix
				handler->sendEndGameBlockRequests(endGameRequests[j].index(),endGameRequests[j].begin());
				//handler->sendBlockRequests(endGameRequests[j].index(),endGameRequests[j].begin());
			}
		}
	}
}

/**
 * Called when the client is in end-game mode and a block has arrived. It schedules a Cancel message for each
 * peer to which a request was previously sent for this block.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::scheduleEndGameCancel(int pieceIndex, int blockIndex, const char* blockProvider)
{
	PeerEntryVector peerVector = peerState.getVector();
	TCPServerThreadBase* thread;
	BTPeerWireClientHandlerBase* handler;
	PeerEntry* peer;
	RequestEntryVector someRequests;
	int pos = -1;

	for (unsigned int j=0; j<endGameRequests.size(); j++)
	{
		if (( endGameRequests[j].index()==pieceIndex) && ( endGameRequests[j].begin()==blockIndex))
		{
			pos = j;
			break;
		}
	}

	if (pos>=0)
	{
		for (unsigned int i=0; i<peerVector.size(); i++)
		{
			peer= &peerVector[i];
			thread = (TCPServerThreadBase*)peer->getPeerThread();
			handler = (BTPeerWireClientHandlerBase*)thread;

			if ((handler->inEndGame()) && (strcmp(handler->getRemotePeerID().c_str(),blockProvider)!=0))
			   //&& (strcmp(handler->getRemotePeerID().c_str(),endGameRequests[pos].peerID())!=0))
			{
				//TODO: Check whether we need a createMsg method in this module
				BTRequestCancelMsg* cancelMsg = new BTRequestCancelMsg(toString(CANCEL_TIMER),CANCEL_TIMER);
				cancelMsg->setIndex(endGameRequests[pos].index());
				cancelMsg->setBegin(endGameRequests[pos].begin()/** 1024*blockSize()*/);
				cancelMsg->setLength((int)blockSize());
				thread->timerExpired(cancelMsg);
			}
		}
		endGameRequests.erase(endGameRequests.begin()+pos);
	}
	//else
		//error("%s:%d at %s() Inconsistent end-game state. Cannot find request for received block (Piece #%d, Block #%d. )\n", __FILE__, __LINE__, __func__, pieceIndex, (blockIndex+1));

}

/**
 * In super-seeding mode, it schedules a Have msg to a peer that previously downloaded a piece, only
 * if this piece has been seen in an other peer's bitfield.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTPeerWireBase::checkandScheduleHaveMsgs(BTBitfieldMsg* msg, const char* peerID)
{
	for (unsigned int i=0; i<superSeedPending.size();i++)
	{
		BlockItem* bi = superSeedPending[i];

		if ((strcmp(peerID,bi->peerID())!=0) && (msg->bitfield(bi->pieceIndex())))
		{
			PeerEntry peer = peerState.getPeerEntry(bi->peerID());
			if (!peer.isValid())
			{
				BEV<<"super-seed mode: peer "<<bi->peerID()<<" waiting for another Have msg, is not connected any more."<<endl;
				return;
			}
			else
			{
				TCPServerThreadBase* thread = peer.getPeerThread();
				scheduleSuperSeedHaveMsg(thread);
				continue;
			}
		}
	}
}

// GET/SET Functions
//----------------------------------------------------
double BTPeerWireBase::fileSize()
{
	return file_size_var;
}

void BTPeerWireBase::setFileSize(double file_size)
{
	file_size_var = file_size;
}

double BTPeerWireBase::pieceSize()
{
	return piece_size_var;
}

void BTPeerWireBase::setPieceSize(double piece_size)
{
	piece_size_var = piece_size;
}

double BTPeerWireBase::blockSize()
{
	return block_size_var;
}

void BTPeerWireBase::setBlockSize(double block_size)
{
	block_size_var = block_size;
}

int BTPeerWireBase::DHTPort()
{
	return DHT_port_var;
}

void BTPeerWireBase::setDHTPort(int DHT_port)
{
	DHT_port_var = DHT_port;
}

const char* BTPeerWireBase::pstr()
{
	return pstr_var;
}
/*
* This should probably be static and defined at startup (ini file)
void BTPeerWireBase::setPstr(const char* pstr)
{
	pstr_var = pstr;
}
*/

int BTPeerWireBase::pstrlen()
{
	return pstrlen_var;
}

/*
* This should probably be static and defined at startup (ini file)
void BTPeerWireBase::setPstrlen(int pstrlen)
{
	pstrlen_var = pstrlen;
}
*/


int BTPeerWireBase::keepAlive()
{
	return keep_alive_var;
}

void BTPeerWireBase::setKeepAlive(int keep_alive)
{
	keep_alive_var = keep_alive;
}

bool BTPeerWireBase::haveSupression()
{
	return have_supression_var;
}

void BTPeerWireBase::setHaveSupression(bool have_supression)
{
	have_supression_var = have_supression;
}

int BTPeerWireBase::chockingInterval()
{
	return chocking_interval_var;
}

void BTPeerWireBase::setChockingInterval(int chocking_interval)
{
	chocking_interval_var = chocking_interval;
}

int BTPeerWireBase::downloaders()
{
	return downloaders_var;
}

void BTPeerWireBase::setDownloaders(int downloaders)
{
	downloaders_var = downloaders;
}

int BTPeerWireBase::optUnchockedPeers()
{
	return optUnchockedPeers_var;
}

void BTPeerWireBase::setOptUnchockedPeers(int optUnchockedPeers)
{
	optUnchockedPeers_var = optUnchockedPeers;
}

int BTPeerWireBase::optUnchockingInterval()
{
	return optUnchocking_interval_var;
}

void BTPeerWireBase::setOptUnchockingInterval(int optUnchocking_interval)
{
	optUnchocking_interval_var = optUnchocking_interval;
}

int BTPeerWireBase::snubbingInterval()
{
	return snubbing_interval_var;
}

void BTPeerWireBase::setSnubbingInterval(int snubbing_interval)
{
	snubbing_interval_var = snubbing_interval;
}

int BTPeerWireBase::rarestListSize()
{
	return rarest_list_size_var;
}

void BTPeerWireBase::setRarestListSize(int size)
{
	rarest_list_size_var = size;
}

int BTPeerWireBase::requestQueueDepth()
{
	return request_queue_depth_var;
}

void BTPeerWireBase::setRequestQueueDepth(int depth)
{
	request_queue_depth_var = depth;

}

int BTPeerWireBase::minNumConnections()
{
	return minNumConnections_var;
}

void BTPeerWireBase::setMinNumConnections(int minNumConns)
{
	minNumConnections_var = minNumConns;
}

int BTPeerWireBase::maxNumConnections()
{
	return maxNumConnections_var;
}

void BTPeerWireBase::setMaxNumConnections(int maxNumConns)
{
	maxNumConnections_var = maxNumConns;
}

bool BTPeerWireBase::superSeedMode()
{
	return super_seed_mode_var;
}

void BTPeerWireBase::setSuperSeedMode(bool value)
{
	super_seed_mode_var = value;
}

int BTPeerWireBase::maxNumEmptyTrackerResponses()
{
	return maxNumEmptyTrackerResponses_var;
}

void BTPeerWireBase::setMaxNumEmptyTrackerResponses(int num)
{
	maxNumEmptyTrackerResponses_var = num;
}

int BTPeerWireBase::currentNumEmptyTrackerResponses()
{
	return currentNumEmptyTrackerResponses_var;
}

void BTPeerWireBase::setCurrentNumEmptyTrackerResponses(int currentNumEmptyTrackerResponses)
{
	currentNumEmptyTrackerResponses_var = currentNumEmptyTrackerResponses;
}

int BTPeerWireBase::currentNumConnections()
{
	return currentNumConnections_var;
}

void BTPeerWireBase::setCurrentNumConnections(int currNumConns)
{
	currentNumConnections_var = currNumConns;

}

void BTPeerWireBase::increaseCurrentNumConnections()
{
	currentNumConnections_var++;
	decreasePendingNumConnections();
	checkConnections();
}

void BTPeerWireBase::decreaseCurrentNumConnections()
{
	currentNumConnections_var--;
	checkConnections();
}

int BTPeerWireBase::pendingNumConnections()
{
	return pendingNumConnections_var;
}

void BTPeerWireBase::checkConnections()
{
	if ((maxNumConnections()-currentNumConnections_var-pendingNumConnections() -1) <= 0)
	{
		stopListening();
	}

	if ((currentNumConnections_var+pendingNumConnections() ) <= minNumConnections() )
	{
		startListening();
	}

}
void BTPeerWireBase::setPendingNumConnections(int pendingNumConns)
{
	pendingNumConnections_var = pendingNumConns;
	checkConnections();
}

void BTPeerWireBase::decreasePendingNumConnections()
{
	pendingNumConnections_var--;
	checkConnections();
}


void BTPeerWireBase::increasePendingNumConnections()
{
	pendingNumConnections_var++;
	checkConnections();
}


BTTrackerMsgResponse* BTPeerWireBase::trackerResponse()
{
	return trackerResponse_var;
}

void BTPeerWireBase::setTrackerResponse(BTTrackerMsgResponse* msg)
{
	if (trackerResponse_var)
		delete trackerResponse_var;

	trackerResponse_var = (BTTrackerMsgResponse*)msg->dup();
}


bool BTPeerWireBase::haveTrackerResponse()
{
	if (trackerResponse_var)
		return true;
	else
		return false;

}


void BTPeerWireBase::deleteTrackerResponse()
{
	cancelAndDelete(trackerResponse_var);
}

simtime_t BTPeerWireBase::downloadDuration()
{
	return downloadDuration_var;
}
void BTPeerWireBase::setDownloadDuration(simtime_t time)
{
	downloadDuration_var = time;
}

int BTPeerWireBase::getDownloadRateSamplingDuration()
{
	return downloadRateSamplingDuration_var;
}

void BTPeerWireBase::setDownloadRateSamplingDuration(int downloadRateSamplingDuration)
{
	downloadRateSamplingDuration_var = downloadRateSamplingDuration;
}


double BTPeerWireBase::blocksFromSeeder()
{
	return blocksFromSeeder_var;
}

void BTPeerWireBase::setBlocksFromSeeder(double blocksFromSeeder)
{
	blocksFromSeeder_var = blocksFromSeeder;
}

void BTPeerWireBase::increamentBlocksFromSeeder()
{
	blocksFromSeeder_var++;
}

int BTPeerWireBase::numPieces()
{
	return numPieces_var;
}

void BTPeerWireBase::setNumPieces(int numPieces)
{
	numPieces_var = numPieces;
}

int BTPeerWireBase::numBlocks()
{
	return numBlocks_var;
}

void BTPeerWireBase::setNumBlocks(int numBlocks)
{
	numBlocks_var = numBlocks;
}

BitField* BTPeerWireBase::localBitfield()
{
	return localBitfield_var;
}

void BTPeerWireBase::initializeLocalBitfield(bool seeder)
{
	localBitfield_var =  new BitField(numPieces(),numBlocks(), seeder);
}


int BTPeerWireBase::timeToSeed()
{
	return timeToSeed_var;
}

void BTPeerWireBase::setTimeToSeed(int timeToSeed)
{
	timeToSeed_var = timeToSeed;
}


double BTPeerWireBase::newlyConnectedOptUnchokeProb()
{
	return newlyConnectedOptUnchokeProb_var;
}

void BTPeerWireBase::setNewlyConnectedOptUnchokeProb(double newlyConnectedOptUnchokeProb)
{
	newlyConnectedOptUnchokeProb_var = newlyConnectedOptUnchokeProb;
}

int BTPeerWireBase::announceInterval()
{
	return (int)announceInterval_var;
}

void BTPeerWireBase::setAnnounceInterval(int announceInterval)
{
	announceInterval_var = announceInterval;
}



void BTPeerWireBase::setProcDelay(float delay)
{
	procDelay_var = (int)delay;
}

float  BTPeerWireBase::procDelay()
{
	return procDelay_var;
}


int BTPeerWireBase::state()
{
	return state_var;
}

void BTPeerWireBase::setState(int state)
{
	state_var = state;
}

bool BTPeerWireBase::enableEndGameMode()
{
	return enableEndGameMode_var;
}

void BTPeerWireBase::setEnableEndGameMode(bool enableEndGameMode)
{
	enableEndGameMode_var = enableEndGameMode;
}

void BTPeerWireBase::incrementDownloadRateStats(float downloadRate)
{
	//blocksFromSeeder_var++;
	downloadRateStats.insert(downloadRateStats.end(), downloadRate);
	//downloadRate_vec.enable();
	//downloadRate_vec.record(downloadRate);
	//printf("\ndownload rate Stats = %f, simTime = %f \n", downloadRate, simTime());
}

void BTPeerWireBase::calculateAggregateDownloadRate()
{
// 	if (blocksFromSeeder() > 0) {
// 	      aggregateDownloadRateSamples.insert(aggregateDownloadRateSamples.end(), blocksFromSeeder() * (16393/1024));
// 	      printf("\n*********Aggregate download rate = %f, Size = %d, simTime = %f ************\n", blocksFromSeeder() * (16393/1024), aggregateDownloadRateSamples.size(), simTime());
// 	}
// 	else {
// 	      aggregateDownloadRateSamples.insert(aggregateDownloadRateSamples.end(), 0);
// 	      printf("\n*********Aggregate download rate = 0, Size = %d, simTime = %f ************\n", aggregateDownloadRateSamples.size(), simTime());
// 	}

	if (downloadRateStats.size()>0)
	{
			float sum = 0;
			double blocks = 0;
			float bytes = 16393.0;
			float div = 1024.0;
			for (int i=0; i<downloadRateStats.size();i++)
			{
				sum = sum + ((float)downloadRateStats[i] * (bytes/div));
				blocks = blocks + downloadRateStats[i];
				//printf("\n*********Sampled download rate = %f, simTime = %f ************\n", downloadRateStats[i], simTime());

			}
			if ( recordVectors == true ) {
			    recBlocksRate_vec.record(blocks);
			    downloadRate_vec.record(sum);
			}
			//setAverageDownloadRate(sum/downloadRateStats.size());
			//setAggregateDownloadRate(sum);
			aggregateDownloadRateSamples.insert(aggregateDownloadRateSamples.end(), sum);
			//printf("\n*********Aggregate download rate = %f, Size = %d, simTime = %f ************\n", sum, aggregateDownloadRateSamples.size(), simTime());
	}
	else {

	    if ( recordVectors == true ) {
		recBlocksRate_vec.record(0);
		downloadRate_vec.record(0);
	    }
	    aggregateDownloadRateSamples.insert(aggregateDownloadRateSamples.end(), (float)0.0);
	    //printf("\n*********Aggregate download rate = 0, Size = %d, simTime = %f ************\n", aggregateDownloadRateSamples.size(), simTime());

	}
	downloadRateStats.clear();

	
}

void BTPeerWireBase::calculateAverageDownloadRate()
{

	if (aggregateDownloadRateSamples.size()>0)
	{
			float sum = 0;
			for (int i=0; i<aggregateDownloadRateSamples.size();i++)
			{
				sum = sum + aggregateDownloadRateSamples[i];

			}
  
			setAverageDownloadRate(sum/aggregateDownloadRateSamples.size());
			//setAverageDownloadRate(sum/downloadDuration());
			//printf("\n*********Average download rate = %f, Size = %d ************\n", sum/aggregateDownloadRateSamples.size(), aggregateDownloadRateSamples.size());
	}
	else {

	    setAverageDownloadRate(0);

	}
    
	printf("\n*********Average download rate = %f, Size = %d ************\n", averageDownloadRate(), aggregateDownloadRateSamples.size());
}

float BTPeerWireBase::averageDownloadRate()
{
	return averageDownloadRate_var;
}

void BTPeerWireBase::setAverageDownloadRate(float averageDownloadRate)
{
	averageDownloadRate_var = averageDownloadRate;
}

void BTPeerWireBase::incrementUploadRateStats(float uploadRate)
{
	uploadRateStats.insert(uploadRateStats.end(), uploadRate);
	//uploadRate_vec.enable();
	//uploadRate_vec.record(uploadRate);
	//printf("\nupload rate Stats = %f, simTime = %f, size = %d\n", uploadRate, simTime(), uploadRateStats.size());
}

void BTPeerWireBase::calculateAggregateUploadRate()
{

	if (uploadRateStats.size()>0)
	{
		
			float sum = 0;
			double blocks = 0;
			float bytes = 16393.0;;
			float div = 1024.0;
			for (int i=0; i<uploadRateStats.size();i++)
			{
				//printf("\naggregate upload rate sample = %f \n", aggregateUploadRateSamples[i]);
				sum = sum + ((float)uploadRateStats[i] * (bytes/div));
				blocks = blocks + uploadRateStats[i];

			}

			//setAverageDownloadRate(sum/downloadRateStats.size());
			//setAggregateDownloadRate(sum);
			if ( recordVectors == true ) {
			    sentBlocksRate_vec.record(blocks);
			    uploadRate_vec.record(sum);
			}
			aggregateUploadRateSamples.insert(aggregateUploadRateSamples.end(), sum);
			//printf("\n*********Aggregate Upload rate = %f, Size = %d, simTime = %f ************\n", sum, aggregateUploadRateSamples.size(), simTime());
	}
	else {

	    //setAggregateDownloadRate(0);
	      if ( recordVectors == true ) {
		sentBlocksRate_vec.record(0);
		uploadRate_vec.record(0);
	      }
	    aggregateUploadRateSamples.insert(aggregateUploadRateSamples.end(), 0);
	  //  printf("\n*********Aggregate Upload rate = 0, Size = %d, simTime = %f ************\n", aggregateUploadRateSamples.size(), simTime());

	}
	uploadRateStats.clear();

	
}

void BTPeerWireBase::calculateAverageUploadRate()
{

	if (aggregateUploadRateSamples.size()>0)
	{
			float sum = 0;
			for (int i=0; i<aggregateUploadRateSamples.size();i++)
			{
				//printf("\naggregate upload rate sample = %f \n", aggregateUploadRateSamples[i]);
				sum = sum + aggregateUploadRateSamples[i];

			}

			//setAverageUploadRate(sum/uploadRateStats.size());
			setAverageUploadRate(sum/aggregateUploadRateSamples.size());
			//uploadRate_vec.record(sum);
			//printf("\nobserved download rate = %f KB/sec, simTime: %f \n", getDownloadRate(), simTime());
			//printf("\ndownload rate samples = %f \n", downloadRateSamples.size());
	}
	else {

	    setAverageUploadRate(0);

	}

	printf("\n*********Average upload rate = %f, Size = %d ************\n", averageUploadRate(), aggregateUploadRateSamples.size());
}

float BTPeerWireBase::averageUploadRate()
{
	return averageUploadRate_var;
}

void BTPeerWireBase::setAverageUploadRate(float averageUploadRate)
{
	averageUploadRate_var = averageUploadRate;
}


void BTPeerWireBase::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->socket());
    updateDisplay();
}

void BTPeerWireBase::writeStats()
{
	BTStatisticsDWLMsg* msgDWL = new BTStatisticsDWLMsg("BT_STATS_DWL",BT_STATS_DWL);
	msgDWL->setDownloadTime(downloadDuration());
	msgDWL->setRemainingBlocks(localBitfield()->numRemainingBlocks());
	sendDirect(msgDWL, 0, btStatistics, btStatistics->findGate("direct_in"));

	BTStatisticsNumProvidersMsg* msgPP = new BTStatisticsNumProvidersMsg("BT_STATS_PP",BT_STATS_PP);
	msgPP->setNumPeers(dataProviderPeerIDs.size());
	sendDirect(msgPP, 0, btStatistics, btStatistics->findGate("direct_in"));

	BTStatisticsNumSeederBlocksMsg* msgNSB = new BTStatisticsNumSeederBlocksMsg("BT_STATS_NSB",BT_STATS_NSB);
	msgNSB->setNumSeederBlocks(blocksFromSeeder());
	sendDirect(msgNSB, 0, btStatistics, btStatistics->findGate("direct_in"));

	BTStatisticsDownloadRateMsg* msgDR = new BTStatisticsDownloadRateMsg("BT_STATS_DRATE",BT_STATS_DRATE);
	msgDR->setDownloadRate(averageDownloadRate());
	sendDirect(msgDR, 0, btStatistics, btStatistics->findGate("direct_in"));

	BTStatisticsUploadRateMsg* msgUR = new BTStatisticsUploadRateMsg("BT_STATS_URATE",BT_STATS_URATE);
	msgUR->setUploadRate(averageUploadRate());
	sendDirect(msgUR, 0, btStatistics, btStatistics->findGate("direct_in"));
 
}

//----- Used for testing -----------------------------------------
const char* BTPeerWireBase::toString(int type)
{

	switch (type)
	{
		case INTERNAL_REFUSE_CONNECTION_TIMER :
			return "INTERNAL_REFUSE_CONNECTION_TIMER";
			break;
		case INTERNAL_INIT_CONNECTION_MSG :
			return "INTERNAL_INIT_CONNECTION_MSG";
			break;
		case INTERNAL_REMOVE_THREAD_MSG :
			return "INTERNAL_REMOVE_THREAD_MSG";
			break;
		case INTERNAL_CHOKE_TIMER :
			return "INTERNAL_CHOKE_TIMER";
			break;
		case INTERNAL_OPT_UNCHOKE_TIMER :
			return "INTERNAL_OPT_UNCHOKE_TIMER";
			break;
		case INTERNAL_MEASURE_DOWNLOAD_RATE_TIMER :
			return "INTERNAL_MEASURE_DOWNLOAD_RATE_TIMER";
			break;
		case INTERNAL_MEASURE_UPLOAD_RATE_TIMER :
			return "INTERNAL_MEASURE_UPLOAD_RATE_TIMER";
			break;
		case INTERNAL_TRACKER_COM_MSG :
			return "INTERNAL_TRACKER_COM_MSG";
			break;
		case INTERNAL_UPDATE_THREAD_MSG :
			return "INTERNAL_UPDATE_THREAD_MSG";
			break;
		case INTERNAL_EXIT_MSG :
			return "INTERNAL_EXIT_MSG";
			break;
		case INTERNAL_SUPER_SEED_HAVE_MSG :
			return "INTERNAL_SUPER_SEED_HAVE_MSG";
			break;
		case INTERNAL_SUPER_SEED_COMPLETE_MSG :
			return "INTERNAL_SUPER_SEED_COMPLETE_MSG";
			break;
		case INTERNAL_NEXT_REQUEST_MSG :
			return "INTERNAL_NEXT_REQUEST_MSG";
			break;
		case INTERNAL_UPDATE_INTERESTS_MSG :
			return "INTERNAL_UPDATE_INTERESTS_MSG";
			break;
		case HANDSHAKE_MSG :
			return "HANDSHAKE_MSG";
			break;
		case KEEP_ALIVE_MSG :
			return "KEEP_ALIVE_MSG";
			break;
		case CHOKE_MSG :
			return "CHOKE_MSG";
			break;
		case UNCHOKE_MSG :
			return "UNCHOKE_MSG";
			break;
		case INTERESTED_MSG :
			return "INTERESTED_MSG";
			break;
		case NOT_INTERESTED_MSG :
			return "NOT_INTERESTED_MSG";
			break;
		case HAVE_MSG :
			return "HAVE_MSG";
			break;
		case BITFIELD_MSG :
			return "BITFIELD_MSG";
			break;
		case REQUEST_MSG :
			return "REQUEST_MSG";
			break;
		case PIECE_MSG :
			return "PIECE_MSG";
			break;
		case CANCEL_MSG :
			return "CANCEL_MSG";
			break;
		case PORT_MSG :
			return "PORT_MSG";
			break;
		case IS_ALIVE_TIMER :
			return "IS_ALIVE_TIMER";
			break;
		case KEEP_ALIVE_TIMER :
			return "KEEP_ALIVE_TIMER";
			break;
		case DEL_THREAD_TIMER :
			return "DEL_THREAD_TIMER";
			break;
		case BITFIELD_TIMER :
			return "BITFIELD_TIMER";
			break;
		case REQUEST_TIMER :
			return "REQUEST_TIMER";
			break;
		case PIECE_TIMER :
			return "PIECE_TIMER";
			break;
		case UNCHOKE_TIMER :
			return "UNCHOKE_TIMER";
			break;
		case INTERESTED_TIMER :
			return "INTERESTED_TIMER";
			break;
		case ANTI_SNUB_TIMER :
			return "ANTI_SNUB_TIMER";
			break;
		case INTERNAL_MEASURE_AGGREGATE_RATE_TIMER :
			return "INTERNAL_MEASURE_AGGREGATE_RATE_TIMER";
			break;
		default:
			return NULL;
			break;

	}
}

