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


#include "BTUtils.h"
#include "BTPeerWireClientHandlerBase.h"
#include <algorithm>
#include <limits>

PieceFreqEntry::PieceFreqEntry() {}

PieceFreqEntry::PieceFreqEntry(const PieceFreqEntry& cp)
{
	index_var = cp.index_var;
	frequency_var = cp.frequency_var;
}

PieceFreqEntry::PieceFreqEntry(int i, int f)
{
	index_var = i;
	frequency_var = f;
}

PieceFreqEntry::~PieceFreqEntry()
{

}

int PieceFreqEntry::index()
{
	return index_var;
}

void PieceFreqEntry::setIndex(int i)
{
	index_var = i;
}

int PieceFreqEntry::frequency()
{
	return frequency_var;
}

void PieceFreqEntry::setFrequency(int f)
{
	frequency_var = f;
}

bool PieceFreqEntry::operator<(const PieceFreqEntry &item) const
{
	if ((frequency_var < item.frequency_var) || (frequency_var == item.frequency_var && index_var < item.index_var))
		return true;

	return false;
}

void PieceFreqEntry::operator=(const PieceFreqEntry &item)
{
	index_var = item.index_var;
	frequency_var = item.frequency_var;

}

void PieceFreqEntry::operator++(int)
{
	frequency_var++;
}

void PieceFreqEntry::operator+=(int i)
{
	frequency_var = frequency_var + i;
}

//===============================================================================================//

void PieceFreqState::initializePieceFrequencies(int numOfPieces)
{
	for (int i=0;i<numOfPieces;i++)
	{
		pieceFreqVector.push_back(PieceFreqEntry::PieceFreqEntry(i,0));
	}

}

void PieceFreqState::printPieceFrequencies()
{
	vector<PieceFreqEntry>::iterator it = pieceFreqVector.begin();

	for (it=pieceFreqVector.begin();it<pieceFreqVector.end();it++)
	{
		ev<<"\t\tIndex = "<<(*it).index()<<" Frequency = "<<(*it).frequency()<<endl;
	}
}

void PieceFreqState::increasePieceFrequency(int index)
{
	PieceFreqEntry entry = pieceFreqVector.at(index);
	entry++;
	pieceFreqVector.at(index) = entry;
}

PieceFreqEntryVector PieceFreqState::getVector()
{
	return pieceFreqVector;
}

void PieceFreqState::updatePieceFrequencies(BTBitfieldMsg* msg)
{
	int bitfieldSize = (int) msg->bitfieldArraySize();

	//Initialize local copy of remote bitfield (so to delete the message)
	for (int i=0;i<bitfieldSize;i++)
		if (msg->bitfield(i))
			increasePieceFrequency(i);
}

void PieceFreqState::clear()
{
	pieceFreqVector.clear();
}


//===============================================================================================//
PeerEntry::PeerEntry()
{
	//this->peerInfo = NULL;
	this->thread = NULL;
	connTime_var = 0;

}
PeerEntry::PeerEntry(const PeerEntry& pe)
{
	this->peerInfo = pe.peerInfo;
	this->thread = pe.thread;
	this->connTime_var = pe.connTime_var;
}

PeerEntry::PeerEntry(PEER peer, TCPServerThreadBase* thread, double connTime)
{
	this->peerInfo = peer;
	this->thread = thread;
	this->connTime_var = connTime;
}

PeerEntry::~PeerEntry()
{
}

string PeerEntry::getPeerID()
{
	return peerInfo.peerId.c_str();
}

IPvXAddress PeerEntry::getPeerIP()
{
	return peerInfo.ipAddress;
}

unsigned int PeerEntry::getPeerPort()
{
	return peerInfo.peerPort;
}

void PeerEntry::setPeerID(opp_string id)
{
	this->peerInfo.peerId = id;
}

PEER PeerEntry::getPeer()
{
	return peerInfo;
}

void PeerEntry::setPeer(PEER peer)
{
	this->peerInfo = peer;
}


TCPServerThreadBase* PeerEntry::getPeerThread()
{
	return thread;
}

bool PeerEntry::isValid()
{
	return (thread == NULL ? true:false);
}

double PeerEntry::connTime()
{
	return connTime_var;
}

void PeerEntry::setConnTime(double connTime)
{
	connTime_var = connTime;
}

//===============================================================================================//

bool PeerEntry::operator<(const PeerEntry &item) const
{
	BTPeerWireClientHandlerBase* handler = (BTPeerWireClientHandlerBase*)thread;
	BTPeerWireClientHandlerBase* itemHandler = (BTPeerWireClientHandlerBase*)item.thread;

	if (handler->getDownloadRate()<itemHandler->getDownloadRate())
		return true;
	else
		return false;
}

//===============================================================================================//

void PeerState::addPeer(PEER peer, TCPServerThreadBase* thread)
{
	addPeer(peer,thread,0);
}

void PeerState::addPeer(PEER peer, TCPServerThreadBase* thread, double connTime)
{
	PeerEntry entry(peer,thread,connTime);
	peerVector.push_back(entry);
}

int PeerState::findPeer(string id)
{
	for (unsigned int i=0; i<peerVector.size(); i++)
			if (!strcmp(peerVector[i].getPeerID().c_str(),id.c_str())) return i;

	return -1;
}

int PeerState::findPeer(IPvXAddress IP)
{

	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		if (peerVector[i].getPeerIP().equals(IP))
		{
			return i;
		}
	}

	return -1;
}

int PeerState::findPeer(IPvXAddress IP, unsigned int port)
{
	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		if ((peerVector[i].getPeerIP().equals(IP)) && (peerVector[i].getPeerPort() == port))
			return i;
	}

	return -1;
}

void PeerState::updatePeerID(int index,opp_string id)
{
	peerVector[index].setPeerID(id);
}

void PeerState::removePeer(string id)
{
	int index = findPeer(id);

	if ( index>-1 )
	{
		//Fix 22.3.2010
		//delete peerVector.at(index).getPeerThread();
		peerVector.erase(peerVector.begin()+index);
	}
	else
		opp_error("BTUtils: Peer state management problem. Peer not found!");
}

void PeerState::sortPeers()
{
	std::sort(peerVector.rbegin(), peerVector.rend());
}

void PeerState::print()
{
	for (unsigned int i=0; i<peerVector.size(); i++)
	{
		TCPServerThreadBase* thread = peerVector[i].getPeerThread();
		BTPeerWireClientHandlerBase* threadBT = (BTPeerWireClientHandlerBase*)thread;
		cerr<<"\tPeer entry #"<<(i+1)<<" "<<peerVector[i].getPeerID()<<", state =  "<<threadBT->state()<<", Choked: " <<threadBT->peerChoking()<<endl;
		threadBT->getRequests().print();
	}
}


PeerEntry PeerState::getPeerEntry(unsigned int i)
{
	if ((i<peerVector.size())||(i>=0))
		return peerVector[i];
	else
		return PeerEntry();
}

PeerEntry* PeerState::getPeerEntryRef(unsigned int i)
{
	if ((i<peerVector.size())||(i>=0))
		return &peerVector[i];
	else
		return NULL;
}

PeerEntry PeerState::getPeerEntry(const char* id)
{
	for (unsigned int i=0; i<peerVector.size(); i++)
			if (!strcmp(peerVector[i].getPeerID().c_str(),id)) return peerVector[i];

	return PeerEntry();
}

float PeerState::minDownloaderRate()
{
	return minDownloaderRate_var;
}

void PeerState::setMinDownloaderRate(float rate)
{
	minDownloaderRate_var = rate;
}

int  PeerState::size()
{
	return	peerVector.size();
}

void PeerState::clear()
{
	peerVector.clear();
}

//===============================================================================================//
RequestEntry::RequestEntry()
{
	index_var = -1;
	begin_var = -1;
	length_var =-1;
	timestamp_var = -1;
	peerID_var = NULL;
}

RequestEntry::RequestEntry(int i, int b, int l, simtime_t t,const char* id)
{
	index_var = i;
	begin_var = b;
	length_var =l;
	timestamp_var = t;
	peerID_var = id;
}

RequestEntry::RequestEntry(const RequestEntry& re)
{
	index_var = re.index_var;
	begin_var = re.begin_var;
	length_var =re.length_var;
	timestamp_var = re.timestamp_var;
	peerID_var = re.peerID_var;
}

RequestEntry::~RequestEntry()
{
}

 int RequestEntry::index()
{
	return index_var;
}

void RequestEntry::setIndex(int i)
{
	index_var = i;
}

int RequestEntry::begin()
{
	return begin_var;
}

void RequestEntry::setBegin(int b)
{
	begin_var = b;
}

int RequestEntry::length()
{
	return length_var;
}

void RequestEntry::setLength(int l)
{
	length_var = l;
}

simtime_t RequestEntry::timestamp()
{
	return timestamp_var;
}

void RequestEntry::setTimestamp(simtime_t timestamp)
{
	timestamp_var = timestamp;
}

const char* RequestEntry::peerID()
{
	return peerID_var;
}

void RequestEntry::setPeerID(const char* id)
{
	peerID_var = id;
}

RequestEntry RequestEntry::dup()
{
	return RequestEntry(*this);
}

//===============================================================================================//
void RequestState::addRequest(RequestEntry re)
{
	addRequest(re.index(),re.begin(),re.length(),re.timestamp(),re.peerID());
}

void RequestState::addRequest(int index, int begin,int length,simtime_t t,const char* id)
{
	RequestEntry entry(index,begin,length,t,id);
	requestVector.push_back(entry);

	if ((checkSize())&&(requestVector.size() > maxSize()))
		opp_error("Exceeded maximum request queue size (maxSize = %d)",maxSize());
}

void RequestState::insert(RequestEntry entry)
{
	requestVector.insert(requestVector.begin(),entry);

	if ((checkSize())&&(requestVector.size()>maxSize()))
		opp_error("Exceeded maximum request queue size (maxSize = %d)",maxSize());
}

int RequestState::findRequest(RequestEntry entry)
{
	return findRequest(entry.index(),entry.begin());
}

int RequestState::findRequest(int index, int begin)
{
	for (unsigned int i=0; i<requestVector.size(); i++)
			if ((requestVector[i].index() == index) &&  (requestVector[i].begin() == begin))
				return i;

	return -1;
}

void RequestState::removeRequest(RequestEntry entry, bool silently)
{
	removeRequest(entry.index(),entry.begin(), silently);
}

void RequestState::removeRequest(int index, int begin, bool silently)
{
	int i = findRequest(index, begin);

	if (i>-1)
	{
		removeRequest(i);
	}
	else
		if ( (!silently) && (size()>0) )
			opp_error("BTUtils: Request state management problem. Request not found!");
}

void RequestState::removeRequest(int i)
{
	requestVector.erase(requestVector.begin()+i);
}

void RequestState::print()
{
	for (unsigned int i=0; i<requestVector.size(); i++)
	{
		cerr<<"\t\tRequest entry #"<<(i+1)<<" ("<<requestVector[i].index()<<","<<requestVector[i].begin()<<", "<<requestVector[i].timestamp()<<")"<<endl;
		ev<<"\t\tRequest entry #"<<(i+1)<<" ("<<requestVector[i].index()<<", "<<requestVector[i].begin()<<", "<<requestVector[i].timestamp()<<")"<<endl;
	}
}


RequestEntry RequestState::getRequestEntry(unsigned int i)
{
	if ((i<requestVector.size())||(i>=0))
		return requestVector[i];
	else
	{
		return RequestEntry();
	}
}

RequestEntry RequestState::getFirstCome()
{
	if (requestVector.size()>0)
	{
		RequestEntry entry = requestVector.back();
		requestVector.pop_back();
		return entry;
	}
	else
	{
		return RequestEntry();
	}
}

int RequestState::getNumRequests()
{
	return requestVector.size();
}

unsigned int RequestState::maxSize()
{
	return maxSize_var;
}

void RequestState::setMaxSize(unsigned int i)
{
	maxSize_var = i;
}

bool  RequestState::canRequestMore()
{
	return requestVector.size() < maxSize();
}

int RequestState::maxBlockRequested(int pieceIndex)
{
	int maxBlockRequested = -1;

	for (unsigned int i=0; i<requestVector.size(); i++)
	{
		if ((requestVector[i].index() == pieceIndex) && (requestVector[i].begin()>maxBlockRequested))
			maxBlockRequested = requestVector[i].begin();
	}

	return maxBlockRequested;
}

int RequestState::minBlockRequested(int pieceIndex)
{
	int minBlockRequested = std::numeric_limits<int>::max();
	for (unsigned int i=0; i<requestVector.size(); i++)
	{
		if ((requestVector[i].index() == pieceIndex) && (requestVector[i].begin()<minBlockRequested))
			minBlockRequested = requestVector[i].begin();
	}

	if (minBlockRequested < std::numeric_limits<int>::max())
		return minBlockRequested;
	else
	return -1;
}

int  RequestState::size()
{
	return	requestVector.size();
}

void RequestState::clear()
{
	requestVector.clear();
}

RequestEntryVector RequestState::getVector()
{
	return requestVector;
}

bool RequestState::checkSize()
{
	return checkSize_var;
}

void RequestState::setCheckSize(bool checkSize)
{
	checkSize_var = checkSize;
}

//===============================================================================================//
