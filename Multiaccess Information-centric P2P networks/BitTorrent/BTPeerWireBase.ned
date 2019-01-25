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


//
// Client for the Peer-Wire (P2P) BitTorrent communication.
// Serves as a generic request-reply handler, for BitTorrent, over TCP.
//
// The model communicates with other BitTorrent clients (i.e. BitTorrentPeerWireBase modules).
// This communication is based on the TCPSrvHostApp model. However, this model is extended 
// to act symmetrically i.e. the BitTorrentPeerWireBase can actively spawn a BitTorrentPeerWireClientHandlerBase
// thread that will initiate a connection with a remote peer. After connection establishement, each 
// thread separately handles the exchange of the protocol's messages with the remote peer. The 
// neccessary coordination among the spawned threads (e.g. rarest piece selection, choking algorithm, etc.)
// is provided by this module.
//
// @see TCPSrvHostApp, TCPGenericSrvThread, BitTorrentPeerWireClientHandlerBase, <Y>
//

simple BTPeerWireBase
    parameters:

	debuggedNode: string,				//To be removed, used for debugging

	address: string,

	port: numeric,
	file_size: numeric const,			// the size of the file, in KB

	piece_size: numeric const,			// the size of a piece, in KB

	block_size: numeric const,			// the size of a block, in KB

	DHT_port: numeric const,			// for DHT trackers (optional,not supported)

	pstr: string,					// string identifier of the protocol, 
							//default value = "BitTorrent protocol"

        pstrlen: numeric const,				// length of pstr, used to avoid the calculation

	keep_alive: numeric const,			// keep-alive message timer, in sec (default = 120 sec)

	have_supression: bool,				// if HAVE message suppresion is supported

	chocking_interval: numeric const,		// time between choke/unchoke, in sec (default = 10 sec)

	seederDownloaders: numeric const,			// number of concurrent downloaders allowed from a seeder	

	downloaders: numeric const,			// number of concurrent downloaders allowed (default = 4)	

	optUnchockedPeers: numeric const,		// number of optimisticaly unchocked peers (default = 1)

	seederOptUnchockedPeers: numeric const,		// number of optimisticaly unchocked peers by the seeder

	optUnchocking_interval: numeric const,		// time between optimistic unchocking events, in sec 
												// (default = 30 sec)

	snubbing_interval: numeric const,		// time interval until a peer is condifered to snub the client, in sec
											// (default = 60 sec)

	rarest_list_size: numeric const,		// the number of rarest pieces among which we shall 
											// randomize selection

	minNumConnections: numeric const,		// minimum number of maintained connections (default = 30)

	maxNumConnections: numeric const,		// maximum number of maintained connections (default = 55)

	serverThreadClass: string,

	timeToSeed: numeric const,				// the time a peer remains in the system as a seeder

	request_queue_depth: numeric const,		// number of allowed pending requests (default = 5)

	super_seed_mode: bool,					// whether this node is in super seeding mode

	maxNumEmptyTrackerResponses: numeric const, 	// maximum number of subsequent empty peer sets received 
													// from the tracker before exiting

	newlyConnectedOptUnchokeProb: numeric const,	// probability by which a newly connected peer is selected 
													// for optimistic unchoke, compared to a random choice (default = 0.75)

	enableEndGameMode: bool,						//whether to enable end-game mode of the peer-wire protocol

	statisticsModulePath: string,					//location of BtTorrentStatics module under simulation

	downloadRateSamplingDuration: numeric const,	//duration of dowload rate sampling (default = 20 sec)

	subtrackerNum: numeric const,

	seederNum: numeric const,
  
	recordVectors: bool, // record vectors for momentary download rate / upload rate

	procDelay: numeric;								//processing delay (in micro-seconds)
    gates:
        in: tcpIn, btorrentIn;
        out: tcpOut, btorrentOut;
endsimple
