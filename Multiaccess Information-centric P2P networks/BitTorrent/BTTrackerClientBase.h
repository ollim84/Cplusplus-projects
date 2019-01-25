//
// Copyright 2008, 2009 Vasileios P. Kemerlis (aka vpk)
//                              vpk@cs.columbia.edu
//                              http://www.cs.columbia.edu/~vpk
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


#ifndef __BTTRACKERCLIENTBASE_H_
#define __BTTRACKERCLIENTBASE_H_

#include <string>
#include <omnetpp.h>
#include <sstream>
#include <set>
#include <IPAddressResolver.h>
#include <InterfaceTable.h>
#include <InterfaceEntry.h>
#include "../../Network/IPv4/IPv4InterfaceData.h"
#include "../../Network/IPv6/IPv6InterfaceData.h"
#include "IPvXAddress.h"
//#include "../TCPApp/TCPGenericCliAppBase.h"
#include "../TCPApp/TCPSrvHostApp.h"
#include <TCP.h>
#include "BTTrackerMsg_m.h"
#include "BTUtils.h"
#include <TCPConnection.h>
#include "BTPeerWireMsg_m.h"


using namespace std;

// event constants
#define EVT_TOUT	1 // timeout
#define EVT_CONN	2 // connect
#define EVT_STOP	3 // stop
#define EVT_COMP	4 // complete

#define INTERNAL_REMOVE_THREAD_MSG				63000
#define INTERNAL_INIT_REMOVE_THREAD_MSG				63001
#define INTERNAL_INIT_TRACKER_CONNECTION_MSG			63002
#define INTERNAL_SUBTRACKER_COM_MSG				63003


// client states - normal and transient
#define ST_STARTED	0 // started state
#define ST_NORMAL	1 // normal state
#define ST_COMPLETED 	2 // completed state
#define ST_STOPPED	3 // stopped state
#define ST_TRANS_NORM	4 // transient state to normal
#define ST_TRANS_COMP	5 // transient state to completed
#define ST_TRANS_STOP	6 // transient state to stopped

#define ANNOUNCE_TIMER			64000

class BTTrackerClientCommunicationBase;

/**
 * BitTorrent protocol.
 * Implements a tracker client as described at http://wiki.theory.org/BitTorrentSpecification
 */
//class INET_API BTTrackerClientBase : public TCPGenericCliAppBase
class INET_API BTTrackerClientBase : public TCPSrvHostApp
{
	protected:
		/* New members in BitTorrentTrackerClientBase */
// 		cMessage* evtTrackerComm;	// initiate tracker communication
// 		cMessage* evtTout;		// session timeout
// 		cMessage* evtDelThread;		// delete thread
// 		cMessage* evtRemThread;		// remove thread
		//size_t state_var;		// the state of the client (started, stopped, completed, normal)
		//size_t transient_var;		// transient state (the client is in the middle of state change)
		string trackerId_var;		// the tracker id (returned from the tracker)
		IPvXAddress ipaddress_var;	// the client's IP address
		// start of members documented in .ned file
		size_t connectGiveUp_var;
		simtime_t sessionTimeout_var;
		simtime_t reconnectInterval_var;
		string infoHash_var;
		string peerId_var;
		size_t peerPort_var;
		bool compact_var;
		bool noPeerId_var;
		bool connectFirstTracker;
		size_t numWant_var;
		string key_var;
		int subtrackerNum;
		int subtrackerIndex;
		const char *trackerConnAddr;
		int trackerConnPort;
		TCP* tcp;
		TCPServerThreadBase* thread_var;
		BTTrackerClientCommunicationBase* handler_var;
		int interfaceId;
		IPvXAddress destAddr;
		IPvXAddress localAddr;
		bool netinf_var;
		// end of members documented in .ned file

		/* Reflection methods */		
 		//virtual void findAndSetIPAddress();		

	public:
		/* Constructor */
		BTTrackerClientBase();
		/* Destructor */
		virtual ~BTTrackerClientBase();
		/* set and get methods */
// 		size_t cstate() const;
// 		void setCstate(size_t);
// 		size_t ctrans() const;
// 		void setCtrans(size_t);
		IPvXAddress findAndSetIPAddress();
		const string& trackerId() const;
		void setTrackerId(const string&);
		size_t connectGiveUp() const;
		void setConnectGiveUp(size_t);
		simtime_t sessionTimeout() const;
		void setSessionTimeout(simtime_t);
		double reconnectInterval() const;
		void setReconnectInterval(double);
		const string& infoHash() const;
		void setInfoHash(const string&);
		const string& peerId() const;
		void setPeerId(const string&);
		size_t peerPort() const;
		void setPeerPort(size_t);
		bool compact() const;
		void setCompact(bool);
		bool noPeerId() const;
		void setNoPeerId(bool);
		size_t numWant() const;
		void setNumWant(size_t);
		const string& key() const;
		void setKey(const string&);
		void setThread(TCPServerThreadBase*);
		TCPServerThreadBase* getThread();
		void setHandler(BTTrackerClientCommunicationBase*);
		BTTrackerClientCommunicationBase* getHandler();
		bool netInfMode();
		void setNetInfMode(bool);
		
		
		//const char* toString(int);
		virtual const char* generatePeerID();
	protected:
		/* Redefined methods from TCPGenericCliAppBase */
		virtual void initialize();
		virtual void handleMessage(cMessage*);
		virtual void handleTimer(cMessage*);
		virtual void handleThreadMessage(cMessage*);
		virtual void removeThread(TCPServerThreadBase*);
		virtual void handleSelfMessage(cMessage*);
		virtual void initConnection(PEER);
// 		virtual void timerExpired(cMessage*);
// 		virtual void connect();
// 		virtual void connectSeeder(const char*);
// 		virtual void close();
// 		virtual void socketEstablished(int, void*);
// 		//virtual void socketDataArrived(int, void*, cMessage*, bool);
// 		virtual void socketDataArrived(cMessage*, TCPSocket*);
// 		virtual void socketPeerClosed(int, void*);
// 		virtual void socketFailure(int, void*, int);
};

/**
 * BitTorrent protocol.
 * The connection handler thread.
 */
class INET_API BTTrackerClientCommunicationBase : public TCPServerThreadBase
{
	protected:
		/* New members in BitTorrentTrackerClientCommunicationBase */
		cMessage* evtTout;	// session timeout
 		cMessage* initDelThreadMsg;		// init delete thread
 		cMessage* delThreadMsg;		// delete thread
	//	cMessage* evtTrackerComm;		// tracker communication
		size_t state_var;		// the state of the client (started, stopped, completed, normal)
		size_t transient_var;		// transient state (the client is in the middle of state change)
		//size_t connectGiveUp_var;
		/* Reflection methods */
		
		virtual void findAndSetAnnounceSize(cMessage*, string) const;
		virtual void announce();

	public:
		/* Constructor */
		BTTrackerClientCommunicationBase();
		/* Destructor */
		virtual ~BTTrackerClientCommunicationBase();
		/* set and get methods */		
		size_t cstate() const;
		void setCstate(size_t);
		size_t ctrans() const;
		void setCtrans(size_t);
		/* Redefined method from TCPServerThreadBase */
		BTTrackerClientBase* hostModule();

	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void failure(int);
		virtual void close();
};

#endif
