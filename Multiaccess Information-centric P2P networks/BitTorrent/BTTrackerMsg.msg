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


cplusplus {{
#include <IPvXAddress.h>
}};

struct IPvXAddress;

// announce event types
enum ANNOUNCE_TYPE
{
	A_STARTED 	= 1; // announce event=started
	A_STOPPED 	= 2; // announce event=stopped
	A_COMPLETED	= 3; // announce event=completed
	A_NORMAL	= 4; // normal announce (after started and before completed)
};

// response types 
enum REPLY_TYPE
{
	R_FAIL		= 1; // response that includes a failure
	R_VALID		= 2; // normal response
	R_WARN		= 3; // response that includes a warning
};

// peer struct - the response includes one PEER entry for each peer
struct PEER
{
	fields:
		string peerId; 		// the peer id
		unsigned int peerPort;	// the peer port
		IPvXAddress ipAddress;	// the peer IP address
};

// Implements a basic <i>announce</i> message.<br>
// This message is send from the <i>clients</i> (i.e., instances of <tt>BitTorrentTrackerClientBase</tt>)
// to the tracker (i.e., instance of <tt>BitTorrentTrackerBase</tt>).
//
// Limitations:
// -# ip parameter is not supported since it is almost useless in our scenarios
// -# the message does not include the downloaded, uploaded and left byte counters (the tracker does not
// need them)
// -# since there is no official standard for any part of the protocol the implementation is based on 
//  <a href=\"http://wiki.theory.org/BitTorrentSpecification\">this</a> specification
//
// @see BTTrackerBase, BTTrackerClientBase
//
message BTTrackerMsgAnnounce 
{
	properties:
		omitGetVerb = true;

	fields:
		string infoHash;			// the info hash
		string peerId;				// the peer id
		unsigned int peerPort;			// the peer port
		unsigned int event enum(ANNOUNCE_TYPE);	// event type
		bool compact;				// flag - request a compact response
		bool noPeerId;				// flag - no peer ids in the response
		unsigned int numWant;			// max number of the peers wanted
		string key;				// the key of the peer
		string trackerId;
// 		IPvXAddress ipAddress;	// the peer IP address
};

// Implements a basic <i>response</i> message.<br>
// This message is send from the <i>tracker</i> (i.e., instance of <tt>BTTrackerBase</tt>)
// to the clients (i.e., instances of <tt>BTTrackerClientBase</tt>).
//
// Limitations:
// -# since there is no official standard for any part of the protocol the implementation is based on 
//  <a href=\"http://wiki.theory.org/BitTorrentSpecification\">this</a> specification
//
// @see BTTrackerBase, BTTrackerClientBase
//
message BTTrackerMsgResponse
{
	properties:
		omitGetVerb = true;

	fields:
		string failure;			// failure reason
		string warning;			// warning reason
		unsigned int announceInterval;	// announce interval
		string trackerId;		// tracker id
		unsigned int complete;		// complete count
		unsigned int incomplete;	// incomplete count
		PEER peers[];			// peers
};
