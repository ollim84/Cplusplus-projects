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


#ifndef __BTPEERWIREHANDLERBASE_H_
#define __BTPEERWIREHANDLERBASE_H_

#include "BTPeerWireBase.h"
#include "BTTrackerClientBase.h"
#include <TCPConnection.h>

enum PeerWireStates{
EARLY_ABORTING, //When we were instructed to close the connection before its final establishment
ACTIVE_ABORTING,	//When we are closing the connection
PASSIVE_ABORTING,	//When the peer has closed the connection
INITIAL,			//When the thread has just initialized
CONNECTED,			//When the connection has been established, app layer acceptance pending ,,,
ACTIVE_HANDSHAKE,	//When initating the handshake procedure
PASSIVE_HANDSHAKE,	//When the remote peer initiates the handshake procedure (not actually required, just for
					//state transition clearness).
SENT_HANDSHAKE,		//Have ONLY sent a handshake msg
RECEIVED_HANDSHAKE,	//Have ONLY received a handshake msg
HANDSHAKE_COMPLETE,	//Have completed the handshake procedure
//BITFIELD_PENDING,	//Have not sent the bitfield because it is empty
BITFIELD_COMPLETE,	//Have sent the bitfield
ANTI_SNUBBING		//In anti-snubbing state i.e. not uploading unless as an optimistic unchoke
};

/**
 * Handles communication with a remote peer.
 */
class INET_API BTPeerWireClientHandlerBase : public TCPServerThreadBase
{
	protected:
		/* new members in BitTorrentPeerWireClientHandlerBase */
		cMessage* evtKeepAlive;	// prevent connection timeout (i.e. send KEEP-ALIVE)
		cMessage* evtIsAlive;	// anticipate connection timeout
		cMessage* evtDelThread;	// thread timeout
		cMessage* evtAntiSnub;
		cMessage* evtHandshake;	//Used to delay a Handshake msg in case the remote peer closes (refuses) the connection.
		cMessage* evtMeasureDownloadRate; //Used to gather average of download rate measurements
		cMessage* evtMeasureUploadRate; //Used to gather average of upload rate measurements
		BTInternalMsg* delThreadMsg;
		int Keep_Alive_Duration;
		//cModule* parentModule;
		BTPeerWireBase*  peerWireBase;
		BTTrackerClientBase* trackerClient;
		string remotePeerID;
		BitField* remoteBitfield;
		bool keepAliveExpired;	//whether a Keep-Alive time-out has expired
								//Required for the cases where a Keep-Alive arrives
								//after expiration. It gets ignored.
		bool amChoking_var;		// Initial value = true
		bool amInterested_var;	// Initial value  = false
		bool peerChoking_var;	// Initial value  = true
		bool peerInterested_var;// Initial value  = false
		bool HAVE_SENT_KEEP_ALIVE;

		int state_var;

		double receivedBlocks_var;
		double sentBlocks_var;

		bool activeConnection_var;
		//Piece exchange info
		int pieceSize;
		int blockSize;

		simtime_t connectTimeShift_var;
		simtime_t lastDownloadTime_var;
		float downloadRate;
		vector<float> downloadRateSamples;

		simtime_t lastUploadTime_var;
		float uploadRate;
		vector<float> uploadRateSamples;


		//Stats and variables for download and upload rate statistics
		vector<float> downloadRateStats;
		vector<float> uploadRateStats;


		bool inEndGame_var;					//true when this thread participates in end game mode
		bool seeder_var;
		bool optimisticallyUnchoked;

		void closeConnection();
		void removeCurrentThread();

		simtime_t lastChokeUnchoke_var; //The time of the last choking/unchoking decision for the (remote) peer

		bool allowedToRequest_var;		//False when this client is interested but no non-requested blocks appear.

		//Timer related methods
		void renewAliveTimer(cMessage*);
		void renewAntiSnubTimer();

		//Request related
		RequestState requests;			//Requests sent by this client
		RequestState chokedRequests;	//Requests that were sent before a choke msg was received
		RequestState incomingRequests;	//Requests received by this client.
		RequestState chokedIncomingRequests;	//Requests that were received before a choke msg was sent

		void cancelBlockRequest(BTRequestCancelMsg*);
		void cancelIncomingBlockRequests();

		//Message related methods
		cMessage* createBTPeerWireMessage(const char*, int);
		cMessage* createBTPeerWireMessage(const char*, int,int, int, int);
		void sendMessage(cMessage*);	// Encapsulates and sends the message, renews Keep-Alive timer
		void cancelAndDelete(cMessage*);

		//Help methods
		void printState();
	public:
		// constructor
		BTPeerWireClientHandlerBase();
		// destructor
		virtual ~BTPeerWireClientHandlerBase();
		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);
		void sendBlockRequests(int,int);
		void sendEndGameBlockRequests(int,int);
		//void initialize();
		void setRemotePeerID(string);
		string getRemotePeerID();
		BitField* getRemoteBitfield(){return remoteBitfield;};

		bool amChoking();
		void setAmChoking(bool);
		bool amInterested();
		void setAmInterested(bool);
		bool peerChoking();
		void setPeerChoking(bool);
		bool peerInterested();
		void setPeerInterested(bool);

		float getDownloadRate();
		void setDownloadRate(float);
		float getUploadRate();
		void setUploadRate(float);
		//bool isDownloader(){ return downloader;};
		//void setDownloader(bool);
		bool seeder();
		void setSeeder(bool);
		simtime_t connectTimeShift();
		void setConnectTimeShift(simtime_t);
		bool isOptimisticallyUnchoked(){return optimisticallyUnchoked;};
		void setOptimisticallyUnchoked(bool);
		void clearPendingRequests();
		void clearPendingIncomingRequests();
		RequestState getRequests();
		int getNumPendingRequests();
		void increaseRequestQueueSize(int);

		int state();
		void setState(int);
		double receivedBlocks();
		void setReceivedBlocks(double receivedBlocks_var);
		void incrementReceivedBlocks();

		double sentBlocks();
		void setSentBlocks(double sentBlocks_var);
		void incrementSentBlocks();


		simtime_t lastChokeUnchoke();
		void  setLastChokeUnchoke(simtime_t);

		bool allowedToRequest();
		void setAllowedToRequest(bool);

		bool inEndGame();
		void setInEndGame(bool);

		bool activeConnection();
		void setActiveConnection(bool);
		const char* socketState();


	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void closed();
		virtual void failure(int);

		virtual void initiatePeerWireProtocol(cMessage*);

};

#endif

