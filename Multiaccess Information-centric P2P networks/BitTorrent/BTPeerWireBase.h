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


#ifndef __BTPEERWIREBASE_H_
#define __BTPEERWIREBASE_H_

#include <omnetpp.h>
#include "../TCPApp/TCPSrvHostApp.h"
#include "BTPeerWireMsg_m.h"
#include "BTUtils.h"
#include "BitField.h"
#include "BTTrackerClientBase.h"

#include "BTTrackerMsg_m.h"
#include "BTStatistics.h"
#include "NotificationBoard.h"

#include <TCP.h>

using namespace std;

// Messages/timers sent/scheduled by the PeerWireBase module itself.
// They all must be less than _PEER_WIRE_BASE_MSG_FLAG
#define INTERNAL_TRACKER_COM_MSG				62000
#define INTERNAL_INIT_CONNECTION_MSG			62001
#define INTERNAL_ACCEPT_CONNECTION_TIMER		62002
#define INTERNAL_REFUSE_CONNECTION_TIMER		62003
#define INTERNAL_CHOKE_TIMER					62004
#define INTERNAL_OPT_UNCHOKE_TIMER				62005
#define INTERNAL_EXIT_MSG						62006
#define INTERNAL_EXIT_SAFE_MSG					62007
#define INTERNAL_MEASURE_AGGREGATE_RATE_TIMER					62008
#define _PEER_WIRE_BASE_MSG_FLAG				62999

// Messages/timers used for the PeerWireBase-Threads communication
#define INTERNAL_REMOVE_THREAD_MSG				63000
#define INTERNAL_UPDATE_THREAD_MSG				63001
#define INTERNAL_SUPER_SEED_HAVE_MSG			63002
#define INTERNAL_SUPER_SEED_COMPLETE_MSG		63003
#define INTERNAL_NEXT_REQUEST_MSG				63004
#define INTERNAL_UPDATE_INTERESTS_MSG			63005
#define INTERNAL_MEASURE_DOWNLOAD_RATE_TIMER	63006
#define INTERNAL_MEASURE_UPLOAD_RATE_TIMER		63007
#define INTERNAL_RECORD_DATA_PROVIDER_TIMER		63008

#define KEEP_ALIVE_TIMER			64000
#define IS_ALIVE_TIMER				64001
#define DEL_THREAD_TIMER			64002
#define BITFIELD_TIMER				64003
#define HAVE_TIMER					64004
#define INTERESTED_TIMER			64005
#define NOT_INTERESTED_TIMER		64006
#define REQUEST_TIMER				64007
#define PIECE_TIMER					64008
#define CANCEL_TIMER				64009
#define CHOKE_TIMER					64010
#define UNCHOKE_TIMER				64011
#define CLOSE_CONNECTION_TIMER		64012
#define ANTI_SNUB_TIMER				64013
#define SUPER_SEED_HAVE_TIMER		64014

#define HANDSHAKE_MSG				65000
#define KEEP_ALIVE_MSG				65001
#define CHOKE_MSG					65002
#define UNCHOKE_MSG					65003
#define INTERESTED_MSG				65004
#define NOT_INTERESTED_MSG			65005
#define HAVE_MSG					65006
#define BITFIELD_MSG				65007
#define REQUEST_MSG					65008
#define PIECE_MSG					65010
#define CANCEL_MSG					65011
#define PORT_MSG					65012

#define HANDSHAKE_MSG_SIZE			(49+peerWireBase->pstrlen())
#define KEEP_ALIVE_MSG_SIZE			4
#define CHOKE_MSG_SIZE				5
#define UNCHOKE_MSG_SIZE			5
#define INTERESTED_MSG_SIZE			5
#define NOT_INTERESTED_MSG_SIZE		5
#define HAVE_MSG_SIZE				7
#define BITFIELD_MSG_SIZE			(peerWireBase->numPieces()+5)
#define PIECE_HEADER_MSG_SIZE		9
#define REQUEST_MSG_SIZE			9
#define CANCEL_MSG_SIZE				9

// client states
#define NORMAL		0 // during downloading
#define ENDGAME		1 // during end-game mode
#define COMPLETED 	2 // finished downloading
#define SEEDING		3 // seeding after download
#define SEEDER		4 // initial seeder
#define EXITING		5 // exiting the application



/**
 * BitTorrent protocol
 *
 * Server side of peer-wire protocol: handles all request/response messages during
 * the client <-> peer conversation.
 */

class INET_API BTPeerWireBase : public TCPSrvHostApp //, public INotifiable
{
	protected:
		/* new members in BTPeerWireBase */
		double file_size_var;				// the size of the file, in KB
		double piece_size_var;				// the size of a piece, in KB
		double block_size_var;				// the size of a block, in KB
		int numPieces_var;					// the number of pieces comprising the file
		int numBlocks_var;					// the number of blocks comprising piece file
		size_t DHT_port_var;				// for DHT trackers (optional,not supported)
		const char* pstr_var;				// string identifier of the protocol, default value = "BitTorrent protocol"
		int pstrlen_var;					// length of pstr, used to avoid the calculation
		int keep_alive_var;					// keep-alive message timer, in sec (default = 120 sec)
		bool have_supression_var;			// if HAVE message suppresion is supported (not supported)
		int chocking_interval_var;			// time between choke/unchoke, in sec (default = 10 sec)
		int downloaders_var;				// number of concurrent downloaders allowed (default = 4)
		int optUnchockedPeers_var;			// number of optimisticaly unchocked peers (default = 1)
		int optUnchocking_interval_var;		// time between optimistic unchocking events, in sec (default = 30 sec)
		int snubbing_interval_var;			// time interval until a peer is condifered to snub the client, in sec (default = 120 sec)
		int rarest_list_size_var;			// the number of rarest pieces among which we shall randomize selection
		int minNumConnections_var;			// minimum number of maintained connections (default = 30)
		int maxNumConnections_var;			// maximum number of maintained connections (default = 55)
		int request_queue_depth_var;		// number of allowed pending requests (default = 5)
		bool super_seed_mode_var;			// whether this node is in super seeding mode
		int timeToSeed_var;					//the time (in seconds) that this client will act as a seeder
		int maxNumEmptyTrackerResponses_var;	// maximum number of subsequent empty peer sets received from the tracker before exiting
		double newlyConnectedOptUnchokeProb_var;// the probability by which a newly connected peer is selected for optimistic unchoke
		int currentNumEmptyTrackerResponses_var;
		int currentNumConnections_var;			// current number of connections. Equal to peerState size but maintained for faster access.updateBitField
		int pendingNumConnections_var;			// number of scheduled but not established connections
		simtime_t downloadDuration_var;			// total time required for this client to download the complete file
		int downloadRateSamplingDuration_var;	// the size of the interval during which the download rate for a peer is sampled
		double announceInterval_var;			// time interval between consequtive announce messages
		bool enableEndGameMode_var; 			//whether to enable end-game mode of the peer-wire protocol
		bool connectFirstTracker;
		bool ExitFlag;
		int subtrackerNum;
		int seederNum;
		bool recordVectors;
		float averageDownloadRate_var;
		float averageUploadRate_var;

		TCP* tcp;
		int procDelay_var;					//processing delay between subsequent requests (in microsec)

		cMessage* evtChokeAlg;		//Timer for running the choke algorithm
		cMessage* evtOptUnChoke;	//Timer for changing the optimistacally unchoked peer
		cMessage* evtTrackerComm;	//Timer to schedule communication with the Tracker
 		cMessage* evtAggRate;		//Timer for measuring aggregate rate

		//statistic vectors for download and upload rates
		cOutVector recBlocksRate_vec;
		cOutVector sentBlocksRate_vec;
		cOutVector downloadRate_vec;
		cOutVector uploadRate_vec;

		NotificationBoard *nb;

		//State related vars
		BitField* localBitfield_var;
		PieceFreqState pieceFreqState;
		PeerState peerState;								//Containes the state for all current connections
		BTTrackerMsgResponse* trackerResponse_var;  //Keeps the peer dictionary.
		int state_var;										//The state of this client:
		RequestEntryVector endGameRequests;
		BlockItemVector superSeedPending;

		cSimpleModule* btStatistics;
		set<string> dataProviderPeerIDs;
		double blocksFromSeeder_var;
		vector<float> downloadRateStats;
		vector<float> uploadRateStats;
		vector<float> aggregateDownloadRateSamples;
		vector<float> aggregateUploadRateSamples;
	public:
		// constructor
		BTPeerWireBase();
		// destructor
		virtual ~BTPeerWireBase();

		const char* debuggedNode; //TODO:To be removed, for debugginh only!
		//int finishedDownloads;	

		// GET/SET methods
		double fileSize();
		void setFileSize(double);
		double pieceSize();
		void setPieceSize(double);
		double blockSize();
		void setBlockSize(double);
		int DHTPort();
		void setDHTPort(int);
		const char* pstr();
		int pstrlen();
		int keepAlive();
		void setKeepAlive(int);
		bool haveSupression();
		void setHaveSupression(bool);
		int chockingInterval();
		void setChockingInterval(int);
		int downloaders();
		void setDownloaders(int);
		int optUnchockedPeers();
		void setOptUnchockedPeers(int);
		int optUnchockingInterval();
		void setOptUnchockingInterval(int);
		int snubbingInterval();
		void setSnubbingInterval(int);
		int rarestListSize();
		void setRarestListSize(int);
		int requestQueueDepth();
		void setRequestQueueDepth(int);
		int minNumConnections();
		void setMinNumConnections(int);
		int maxNumConnections();
		void setMaxNumConnections(int);
		bool superSeedMode();
		void setSuperSeedMode(bool);
		int maxNumEmptyTrackerResponses();
		void setMaxNumEmptyTrackerResponses(int);
		int currentNumEmptyTrackerResponses();
		void setCurrentNumEmptyTrackerResponses(int);
		int currentNumConnections();
		void setCurrentNumConnections(int);
		int pendingNumConnections();
		void setPendingNumConnections(int);
		BTTrackerMsgResponse* trackerResponse();
		void setTrackerResponse(BTTrackerMsgResponse*);
		void deleteTrackerResponse();
		bool haveTrackerResponse();
		int numPieces();
		void setNumPieces(int);
		int numBlocks();
		void setNumBlocks(int);
		BitField* localBitfield();
		void initializeLocalBitfield(bool);
		int timeToSeed();
		void setTimeToSeed(int);
		double newlyConnectedOptUnchokeProb();
		void setNewlyConnectedOptUnchokeProb(double);
		simtime_t downloadDuration();
		void setDownloadDuration(simtime_t);
		int announceInterval();
		void setAnnounceInterval(int);
		bool enableEndGameMode();
		void setEnableEndGameMode(bool);
		int getDownloadRateSamplingDuration();
		void setDownloadRateSamplingDuration(int);
		double blocksFromSeeder();
		void setBlocksFromSeeder(double);
		void increamentBlocksFromSeeder();
		void setProcDelay(float);
		float  procDelay();
		
		void increasePendingNumConnections();
		void decreasePendingNumConnections();
		void increaseCurrentNumConnections();
		void decreaseCurrentNumConnections();
		void checkConnections();
		
		//Functions for gathering average downloadRate statistics
		void incrementDownloadRateStats(float);
		float averageDownloadRate();
		void setAverageDownloadRate(float);
		void calculateAverageDownloadRate();

		//Functions for gathering average uploadRate statistics
		void incrementUploadRateStats(float);
		float averageUploadRate();
		void setAverageUploadRate(float);
		void calculateAverageUploadRate();
		void calculateAggregateDownloadRate();
		void calculateAggregateUploadRate();

		void updateBitField(int,int,bool, const char*);

		int state();
		void setState(int);

		/**
		 * Updates the bitfield to show whether a block has been requested.
		 */
		int updateBlockRequests(int, int, bool);

		const char* toString(int);

		void printConnections();

	protected:
		/* Redefined methods from BTPeerWireBase */
		virtual void initialize();

		virtual void finish();

		virtual void handleMessage(cMessage *msg);

		virtual void removeThread(TCPServerThreadBase *);

		void handleThreadMessage(cMessage* msg);
		void handleSelfMessage(cMessage* msg);

		void initializePieceFrequencies(int);
		void printPieceFrequencies();
		void increasePieceFrequency(int);
		void updatePieceFrequencies(BTBitfieldMsg*);

		/**
		 * Method that decides whether to accept an 'incoming' connection or not.
		 */
		bool connectionAlreadyEstablished(int);

		/**
		 * Determine if this client is interested in any piece of a peer.
		 */
		int amInterested(BTBitfieldMsg*);
		int amInterested(BitField*);



		/**
		 * Informs all connected peers about the reception of a piece.
		 */
		virtual void informPeers(int);

		/**
		 * Schedules a Have msg in super-seed mode
		 */
		virtual void scheduleSuperSeedHaveMsg(TCPServerThreadBase*);

		/**
		 * In super-seeding mode, it schedules a Have msg to a peer that previously downloaded a piece.
		 */
		virtual void checkandScheduleHaveMsgs(BTBitfieldMsg*, const char*);

		/**
		 * Makes the next move regarding a certain peer that has just provided a piece or has just unchoked this client.
		 */
		virtual void makeNextPeerMove(TCPServerThreadBase*);

		void updateInterests();

		/**
		 * Method that handles the received peer set and schedules (or not)
		 * one or more connection attempts.
		 */
		virtual void scheduleConnections(BTTrackerMsgResponse*);

		void stopListening();
		void startListening();
		void stopChokingAlorithms();
// 		void receiveChangeNotification(int, cPolymorphic*);

		virtual void ChokingAlgorithm();
		virtual void chokeWorstDownloader();
		virtual void OptimisticUnChokingAlgorithm();
		virtual int chooseOptUnchokePeer(int);

		/**
		 * Select the piece that we will request from this peer
		 * Currently implementing Rarer First policy.
		 */
		virtual BlockItem* selectPiece(BitField*, bool);


		/**
		 * Determined whether the client shall enter the end-game mode.
		 */
		virtual bool enterEndGameMode();

		/**
		 * Schedules the requests in the end-game mode.
		 */
		virtual void scheduleEndGameRequests();

		/**
		 * Schedules the Cancel messages in the end-game mode.
		 */
		virtual void scheduleEndGameCancel(int,int,const char*);

		/**
		 * Sends measurements to the BTStatistics Module
		 */
		void writeStats();

		//void printConnections();
};

#endif
