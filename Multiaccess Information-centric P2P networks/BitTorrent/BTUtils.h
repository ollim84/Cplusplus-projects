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


#ifndef __BTUTILS_H_
#define __BTUTILS_H_

using namespace std;

#include <omnetpp.h>
#include "../TCPApp/TCPSrvHostApp.h"
#include "BTTrackerMsg_m.h"
#include "BTPeerWireMsg_m.h"

class INET_API PieceFreqEntry : public cObject
{
	protected:
		int index_var;		//The bitfield index of this piece
 		int frequency_var;	//The number of peers known to have this piece
	public:
		PieceFreqEntry();
		PieceFreqEntry(int, int);
		PieceFreqEntry(const PieceFreqEntry&);
		virtual ~PieceFreqEntry();
		int index();
		void setIndex(int);
		int frequency();
		void setFrequency(int);
		bool operator<(const PieceFreqEntry &item) const;
		void operator=(const PieceFreqEntry &item);
		void operator++(int);
		void operator+=(int);
};

typedef std::vector<PieceFreqEntry> 	PieceFreqEntryVector;

class INET_API PieceFreqState
{
	protected:
		PieceFreqEntryVector pieceFreqVector;
	public:
		void initializePieceFrequencies(int numOfPieces);
		void printPieceFrequencies();
		void increasePieceFrequency(int index);
		PieceFreqEntryVector getVector();
		void updatePieceFrequencies(BTBitfieldMsg*);
		void clear();
};

class INET_API PeerEntry : public cObject
{
	protected:
		PEER peerInfo;
		TCPServerThreadBase* thread;
		double connTime_var;
	public:
		PeerEntry();
		PeerEntry(PEER, TCPServerThreadBase*,double);
		PeerEntry(const PeerEntry&);
		virtual ~PeerEntry();
		string getPeerID();
		IPvXAddress getPeerIP();
		unsigned int getPeerPort();
		void setPeerID(opp_string);
		PEER getPeer();
		void setPeer(PEER peer);
		TCPServerThreadBase* getPeerThread();
		bool operator<(const PeerEntry&) const;
		bool isValid();
		double connTime();
		void setConnTime(double);
};

typedef std::vector<PeerEntry> 		PeerEntryVector;

//TODO: should consider using a HashTable instead of a vector, for faster access!
class INET_API PeerState
{
	protected:
		PeerEntryVector peerVector;
		float minDownloaderRate_var;
	public:
		void addPeer(PEER, TCPServerThreadBase*);
		void addPeer(PEER, TCPServerThreadBase*,double);
		int findPeer(string);
		int findPeer(IPvXAddress);
		int findPeer(IPvXAddress, unsigned int);
		void removePeer(string);
		void updatePeerID(int,opp_string);
		void sortPeers();
		PeerEntryVector getVector(){return peerVector;};
		void print();
		PeerEntry getPeerEntry(unsigned int);
		PeerEntry* getPeerEntryRef(unsigned int);
		PeerEntry getPeerEntry(const char*);
		float minDownloaderRate();
		void setMinDownloaderRate(float rate);
		int size();
		void clear();
};

class INET_API RequestEntry : public cObject
{
	protected:
		int index_var;
		int begin_var; //block index, not byte offset
		int length_var;
		simtime_t timestamp_var;
		const char* peerID_var;
	public:
		RequestEntry();
		RequestEntry(int,int, int,simtime_t,const char*);
		RequestEntry(const RequestEntry&);
		virtual ~RequestEntry();
		int index();
		void setIndex(int);
		int begin();
		void setBegin(int);
		int length();
		void setLength(int);
 		simtime_t timestamp();
 		void setTimestamp(simtime_t);
		const char* peerID();
		void setPeerID(const char*);
		RequestEntry dup();
};

typedef std::vector<RequestEntry> 	RequestEntryVector;

class INET_API RequestState
{
	protected:
		RequestEntryVector requestVector;
		unsigned int maxSize_var;
		bool checkSize_var;
	public:
		void addRequest(RequestEntry);
		void addRequest(int,int,int,simtime_t,const char*);
		void insert(RequestEntry);
		int findRequest(RequestEntry);
		int findRequest(int,int);
		void removeRequest(RequestEntry,bool silently = false);
		void removeRequest(int,int,bool);
		void removeRequest(int);
		void sortPeers();
		RequestEntryVector getVector();
		void print();
		RequestEntry getRequestEntry(unsigned int);
		RequestEntry getFirstCome();
		int getNumRequests();
		int maxBlockRequested(int);
		int minBlockRequested(int);
		unsigned int maxSize();
		void setMaxSize(unsigned int);
		bool canRequestMore();
		int size();
		bool checkSize();
		void setCheckSize(bool);
		void clear();
};

#endif
