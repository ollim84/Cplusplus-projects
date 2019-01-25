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


#ifndef __BITFIELD_H_
#define __BITFIELD_H_

#include <omnetpp.h>
//#include <iostream.h>
#include <iostream>
#include "BTPeerWireMsg_m.h"

using namespace std;

//NOTE: It may seem we keep redudant information but we also need fast access to this information
class INET_API BitField
{
	protected:
		int numPieces_var;		//Total number of pieces
		int numBlocks_var;		//Total number of blocks
		bool* bitfield;			//The bitfield
		bool** blockfield;		//Which block is available

		//The following three variables hold information only for a local bitfield
		bool** requested;		//Denotes if the client currently requests this block
		int* currentlyRequestedPieceBlocks_var;
		int numRequestedPieces_var;	//Number of requested Pieces
		int numRequestedBlocks_var;	//Number of requested but not received Blocks
		int numNonRequestedNonAvailableBlocks_var;	//Number of neither requested nor received Blocks


		//NOTE: We can just use the current blockfield index for each piece, assuming the client requests the blocks
		//sequencially. This is the role of the "currentBlock" integer below.
		//However, we choose to also implement this map in order not to restrict the client to this
		//sequencial block request order.

		int* currentBlock_var; //Latest available block, assuming sequencial order.
		int* nextBlock_var;   //Next Block to request, -1 if all available
		int numRemainingPieces_var;
		int numRemainingBlocks_var;
		bool local_var;
		simtime_t lastDownloadTime_var; //Time the last downloaded block was received

	public:
		BitField(int,int, bool);
		BitField(BTBitfieldMsg*,int);
		virtual ~BitField();
		void update(int);
		bool update(int,int,bool);
		//new function 22.3.2010
		void update(BTBitfieldMsg* msg);
		void fillBlocks();
		void putInMessage(BTBitfieldMsg*);
		bool isPieceAvailable(int);
		bool isBlockAvailable(int, int);
		bool isPieceRequested(int);
		bool isBlockRequested(int,int);
		int setBlockRequested(int,int,bool);
		int  currentBlock(int);
		bool finishedDownloading();
		int numPieces();
		int numBlocks();
		int numRemainingPieces();
		void setNumRemainingPieces(int);
		int numRemainingBlocks();
		void setNumRemainingBlocks(int);
		int numRequestedPieces();
		void setNumRequestedPieces(int);
		int numRequestedBlocks();
		void setNumRequestedBlocks(int);
		int numNonRequestedNonAvailableBlocks();
		void setNumNonRequestedNonAvailableBlocks(int);
		void decreaseCurrentlyRequestedPieceBlocks(int);
		bool local();
		void setLocal(bool);
		bool havePiece();
		simtime_t lastDownloadTime();
		void setLastDownloadTime(simtime_t);
		void printRemainingPieces();
		void printRemainingBlocks();
		int nextBlock(int);
		int findNextBlock(int);
};

class BlockItem
{
	protected:
		int pieceIndex_var;
		int blockIndex_var;
		const char* peerID_var;
	public:
		BlockItem(int,int);
		BlockItem(int,int,const char*);
		int pieceIndex();
		void setPieceIndex(int);
		int blockIndex();
		void setBlockIndex(int);
		bool isValid();
		const char* peerID();
		void setPeerID(const char*);
		void operator=(const BlockItem &item);
};

typedef std::vector<BlockItem*> 		BlockItemVector;

#endif
