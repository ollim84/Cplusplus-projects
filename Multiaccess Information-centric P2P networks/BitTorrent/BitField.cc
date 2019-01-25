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


#include "BitField.h"

BitField::BitField(int pieces, int blocks, bool seeder)
{
	local_var = true;
	numPieces_var = pieces;
	numBlocks_var = blocks;
	numRemainingPieces_var = numPieces_var;
	numRemainingBlocks_var = numPieces_var*numBlocks_var;
	numRequestedBlocks_var = 0;
	numRequestedPieces_var = 0;
	numNonRequestedNonAvailableBlocks_var = numPieces_var*numBlocks_var;

	bitfield = new bool[numPieces_var];
	requested = new bool*[numPieces_var];
	blockfield = new bool*[numPieces_var];
	currentBlock_var = new int[numPieces_var];
	nextBlock_var = new int[numPieces_var];
	currentlyRequestedPieceBlocks_var = new int[numPieces_var];

	for (int i=0; i<numPieces_var; i++)
	{
		blockfield[i] = new bool[numBlocks_var];
		requested[i] = new bool[numBlocks_var];
	}

	for (int i=0;i<numPieces_var;i++)
	{
		bitfield[i] = 0;
		currentBlock_var[i] = -1;
 		nextBlock_var[i] = 0;
		currentlyRequestedPieceBlocks_var[i] = 0;

		for (int j=0;j<numBlocks_var;j++)
		{
			blockfield[i][j] = 0;
			requested[i][j] = 0;
		}
	}

	setLastDownloadTime(0);

	if (seeder)
		fillBlocks();
}

//NOTE:This is the case of a remote BitField so we do not have accurate information about blocks.
BitField::BitField(BTBitfieldMsg* msg, int numblocks)
{
	local_var = false;
	numPieces_var = (int) msg->bitfieldArraySize();
	numBlocks_var = numblocks;
	numRemainingPieces_var = numPieces_var;
	numRemainingBlocks_var = numPieces_var*numBlocks_var;
	numNonRequestedNonAvailableBlocks_var = numPieces_var*numBlocks_var;
	currentBlock_var = new int[numPieces_var];
	nextBlock_var  = new int[numPieces_var];
	currentlyRequestedPieceBlocks_var = new int[numPieces_var];

	//Initialize local copy of remote bitfield (so to delete the message)
	bitfield =  new bool[numPieces_var];
	blockfield = new bool*[numPieces_var];

	for (int i=0;i<numPieces_var;i++)
	{
		bitfield[i] = msg->bitfield(i);
		blockfield[i] = new bool[numBlocks_var];
		currentBlock_var[i] = -1;
		nextBlock_var[i] = 0;
		currentlyRequestedPieceBlocks_var[i] = 0;

		if (bitfield[i])
		{
			numRemainingPieces_var--;
			numRemainingBlocks_var-= numBlocks_var;
			for (int j=0;j<numBlocks_var;j++)
				blockfield[i][j]=1;
		}
		else
			for (int j=0;j<numBlocks_var;j++)
				blockfield[i][j]=0;
	}

	setLastDownloadTime(0);
}

void BitField::fillBlocks()
{
	for (int i=0;i<numPieces_var;i++)
		update(i);
}

BitField::~BitField()
{
	delete [] bitfield;
	//delete [] blockfield;
	//delete [] requested;
	delete [] currentBlock_var;
	delete [] nextBlock_var;
	//Fix 22.3.2010
	delete [] currentlyRequestedPieceBlocks_var;

	for (int i = 0; i < numPieces_var; i++)
	{
	      delete blockfield[i];
	      delete requested[i];
	}

	delete [] blockfield;
	delete [] requested;
}

void BitField::update(int bitfieldIndex)
{
	numRemainingPieces_var--;
	setNumRemainingBlocks(max(numRemainingBlocks()-numBlocks(),0));
	bitfield[bitfieldIndex] = true;
	currentBlock_var[bitfieldIndex] = numBlocks()-1;
	nextBlock_var[bitfieldIndex] = -1;

	for (int i=0;i<numPieces();i++)
		for (int j=0;j<numBlocks();j++)
			blockfield[i][j]=1;
}

bool BitField::update(int bitfieldIndex, int blockfieldIndex,bool expected)
{
	if (!local())
		opp_error("Operation not allowed for non-local bitfield");

	currentBlock_var[bitfieldIndex] = max(blockfieldIndex,currentBlock_var[bitfieldIndex]);

	if (expected)
		decreaseCurrentlyRequestedPieceBlocks(bitfieldIndex);

	blockfield[bitfieldIndex][blockfieldIndex] = true;

	setNumRemainingBlocks(max(numRemainingBlocks()-1,0));
	setNumRequestedBlocks(max(numRequestedBlocks()-1,0));

	nextBlock_var[bitfieldIndex] = findNextBlock(bitfieldIndex);

	//Assuming sequencial block requests
	if ((nextBlock_var[bitfieldIndex] == -1) && (currentlyRequestedPieceBlocks_var[bitfieldIndex] ==0))
	{
		bitfield[bitfieldIndex] = true;
		setNumRemainingPieces(max(numRemainingPieces()-1,0));
		return true;
	}
	else
		return false;
}

//FIX 22.3.2010

void BitField::update(BTBitfieldMsg* msg)
{
	for (int i = 0; i < numPieces_var; i++)
	{
	    bitfield[i] = msg->bitfield(i);
	    currentBlock_var[i] = -1;
	    nextBlock_var[i] = 0;
	    currentlyRequestedPieceBlocks_var[i] = 0;
	
	    if (bitfield[i])
	    {

		numRemainingPieces_var--;
		numRemainingBlocks_var-= numBlocks_var;
		for (int j = 0; j < numBlocks_var; j++)
		    blockfield[i][j] = 1;
	    }
	    else
		for (int j = 0; j < numBlocks_var; j++)
		    blockfield[i][j] = 1;
	}

}


void BitField::putInMessage(BTBitfieldMsg* msg)
{
	for (int i=0;i<numPieces();i++)
		msg->setBitfield(i,bitfield[i]);
}

bool BitField::isPieceAvailable(int index)
{
	return bitfield[index];
}

bool BitField::isBlockAvailable(int bitfieldIndex, int blockfieldIndex)
{
	return blockfield[bitfieldIndex][blockfieldIndex];
}

bool BitField::isPieceRequested(int bitfieldIndex)
{
	if (!local())
		opp_error("Operation not allowed for non-local bitfield");

	return (currentlyRequestedPieceBlocks_var[bitfieldIndex] > 0) ? true : false;
}

int BitField::setBlockRequested(int pieceIndex,int blockIndex, bool value)
{
	if (!local())
		opp_error("Operation not allowed for non-local bitfield");


	requested[pieceIndex][blockIndex] = value;
	nextBlock_var[pieceIndex] = findNextBlock(pieceIndex);

	if (value)
	{
		currentlyRequestedPieceBlocks_var[pieceIndex]++;
		setNumRequestedBlocks(numRequestedBlocks() +1);
		setNumNonRequestedNonAvailableBlocks(max(numNonRequestedNonAvailableBlocks() - 1,0));

		if (currentBlock(pieceIndex)==-1)
			setNumRequestedPieces(numRequestedPieces() + 1);
	}
	else
	{
		decreaseCurrentlyRequestedPieceBlocks(pieceIndex);
		setNumRequestedBlocks(numRequestedBlocks() -1);
		setNumNonRequestedNonAvailableBlocks(max(numNonRequestedNonAvailableBlocks() + 1,0));

		if (currentBlock(pieceIndex)==-1)
			setNumRequestedPieces(numRequestedPieces() - 1);
	}

	return (numRemainingBlocks() - numRequestedBlocks());
}

int BitField::findNextBlock(int pieceIndex)
{
	int i=0;
	while (((requested[pieceIndex][i]) || isBlockAvailable(pieceIndex,i) ) && (i <= (numBlocks_var-1)))
		i++;

	if (i > (numBlocks_var-1))
		return -1;
	else
		return i;
}


bool BitField::isBlockRequested(int pieceIndex, int blockIndex)
{
	if (!local())
		opp_error("Operation not allowed for non-local bitfield");

	return requested[pieceIndex][blockIndex];
}

int BitField::currentBlock(int bitfieldIndex)
{
	return currentBlock_var[bitfieldIndex];
}

int BitField::nextBlock(int bitfieldIndex)
{
	return nextBlock_var[bitfieldIndex];
}

bool BitField::finishedDownloading()
{
	return (numRemainingBlocks() == 0) ? true : false;
}

bool BitField::havePiece()
{
	return numRemainingPieces() < numPieces();
}

int BitField::numPieces()
{
	return numPieces_var;
}

int BitField::numBlocks()
{
	return numBlocks_var;
}

int BitField::BitField::numRemainingPieces()
{
	return numRemainingPieces_var;
}

void BitField::setNumRemainingPieces(int numRemainingPieces)
{
	numRemainingPieces_var = numRemainingPieces;
}

int BitField::numRemainingBlocks()
{
	return numRemainingBlocks_var;
}

void BitField::setNumRemainingBlocks(int numRemainingBlocks)
{
	numRemainingBlocks_var = numRemainingBlocks;
}

int BitField::numRequestedPieces()
{
	return numRequestedPieces_var;
}

void BitField::setNumRequestedPieces(int numRequestedPieces)
{
	numRequestedPieces_var = numRequestedPieces;
}

int BitField::numRequestedBlocks()
{
	return numRequestedBlocks_var;
}

void BitField::setNumRequestedBlocks(int numRequestedBlocks)
{
	numRequestedBlocks_var = numRequestedBlocks;
}

int BitField::numNonRequestedNonAvailableBlocks()
{
	return numNonRequestedNonAvailableBlocks_var;
}

void BitField::setNumNonRequestedNonAvailableBlocks(int numNonRequestedNonAvailableBlocks)
{
	numNonRequestedNonAvailableBlocks_var = numNonRequestedNonAvailableBlocks;
}


void BitField::decreaseCurrentlyRequestedPieceBlocks(int bitfieldIndex)
{
	currentlyRequestedPieceBlocks_var[bitfieldIndex] = max(--currentlyRequestedPieceBlocks_var[bitfieldIndex],0);
}

bool BitField::local()
{
	return local_var;
}

void BitField::setLocal(bool local)
{
	local_var = local;
}

simtime_t BitField::lastDownloadTime()
{
	return lastDownloadTime_var;
}

void BitField::setLastDownloadTime(simtime_t lastDownloadTime)
{
	lastDownloadTime_var = lastDownloadTime;
}

void BitField::printRemainingPieces()
{

	cerr<<"Remaining pieces"<<endl;
	cerr<<"----------------"<<endl;
	for (int i=0;i<numPieces();i++)
	{
		if (!bitfield[i])
			cerr<<"Piece #"<<i<<endl;
	}
}

void BitField::printRemainingBlocks()
{

	cerr<<"Remaining blocks"<<endl;
	cerr<<"----------------"<<endl;
	for (int i=0;i<numPieces_var;i++)
	{
		for (int j=0;j<numBlocks_var;j++)
		{
			if (!blockfield[i][j])
				cerr<<"Block #"<<j<<" of piece "<<i<<endl;
		}
	}
}

//-------------------------------------------
BlockItem::BlockItem(int i,int j)
{
	pieceIndex_var = i;
	blockIndex_var = j;
	peerID_var = NULL;
}

BlockItem::BlockItem(int i,int j, const char* id)
{
	pieceIndex_var = i;
	blockIndex_var = j;
	peerID_var = id;
}

bool BlockItem::isValid()
{
	return ((pieceIndex()>=0)&&(blockIndex()>=-1));
}

int BlockItem::pieceIndex()
{
	return pieceIndex_var;
}

void BlockItem::setPieceIndex(int i)
{
	pieceIndex_var = i;
}

int BlockItem::blockIndex()
{
	return blockIndex_var;
}

void BlockItem::setBlockIndex(int i)
{
	blockIndex_var = i;
}

const char* BlockItem::peerID()
{
	return peerID_var;
}

void BlockItem::setPeerID(const char* id)
{
	peerID_var = id;
}

void BlockItem::operator=(const BlockItem &item)
{
	pieceIndex_var = item.pieceIndex_var;
	blockIndex_var = item.blockIndex_var;
	peerID_var     = item.peerID_var;

}

