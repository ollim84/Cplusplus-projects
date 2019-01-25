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


cplusplus {{
#include "../TCPApp/GenericAppMsg_m.h"
#include "BTTrackerMsg_m.h"
}}

class GenericAppMsg;
struct PEER;
//
// Message classes for BitTorrent protocol.
// Peer <-> Peer communication (request-reply)
//
// @see BTClientBase (C++ only)
//


enum PeerWireMsgIDs
{
   CHOKE = 0;
   UNCHOKE = 1;
   INTERESTED = 2;
   NOT_INTERESTED = 3;
   HAVE = 4;
   BITFIELD = 5;
   REQUEST = 6;
   PIECE = 7;
   CANCEL = 8;
};

message BTMsgHandshake extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	char pstrlen;		//single raw byte
	string pstr;
	bool reserved[8];
	string infoHash;
	string peerId;
	
};

message BTKeepAliveMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	int length_prefix;
};

// General peer-state message for peer-wire messages: 
// choke, unchoke, interested, not interested
message BTPeerStateMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	int length_prefix;
	unsigned short ID enum(PeerWireMsgIDs);
};

message BTHaveMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	int length_prefix;
	unsigned short ID enum(PeerWireMsgIDs);
	int index;
};

message BTBitfieldMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	int length_prefix;
	unsigned short ID enum(PeerWireMsgIDs);
	bool bitfield[];
};


message BTRequestCancelMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	int length_prefix;
	unsigned short ID enum(PeerWireMsgIDs);
	int index;
	int begin;
	int dataLength;
	bool endGame;
};

message BTPieceMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	int length_prefix;
	unsigned short ID enum(PeerWireMsgIDs);
	int index;
	int begin;
	string data;
};

message BTInternalMsg  extends cMessage
{
    properties:
        omitGetVerb = true;

    fields:
	string text;
	PEER peer;
	int pieceIndex;
	int blockIndex;
	bool chokedPiece;
};

message BTAcceptConnectionMsg  extends GenericAppMsg
{
    properties:
        omitGetVerb = true;

    fields:
	bool accept = true;
};

