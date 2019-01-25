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


// Implements a basic <i>client</i> for a <i>BitTorrent Tracker</i>.<br>
// Serves as a generic request-reply HTTP handler (for BitTorrent tracker protocol), over TCP.<br>
// Compatible with both IPv4 and IPv6.<br>
//
// The model communicates with the <i>tracker</i> (i.e., instances type of <tt>BitTorrentTrackerBase</tt>
// or subclass) in sessions. During a session, the client <i>opens</i> a single TCP connection to 
// the server, <i>sends</i> its announce (request), <i>receives</i> the tracker\'s reply (response) and 
// <i>closes</i> the connection.
//
// The model interacts via:
// - <tt>BitTorrentTrackerMsgAnnounce</tt> messages that sends to the tracker via TCP
// - <tt>BitTorrentTrackerMsgResponse</tt> messages that receives from the tracker via TCP 
// and forwards to the application via <i>btorrentOut</i> gate
// - <tt>cMessage</tt> messages that receives from the application via <i>btorrentIn</i> gate
// 
// Extending <tt>BitTorrentTrackerClientBase</tt>:
// -# each client application should be, or subclassed from, <tt>BitTorrentTrackerClientBase</tt>
// -# to <i>add</i>, <i>change</i> or <i>extend</i> client\'s current behavior the following 
// functions have to reimplemented:
// <tt>BitTorrentTrackerClientBase</tt>::<i>announce()</i>, 
// <tt>BitTorrentTrackerClientBase</tt>::<i>findAndSetIPAddress()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>findAndSetAnnounceSize()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>initialize()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>handleMessage()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>connect()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>close()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>socketEstablished()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>socketDataArrived()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>socketPeerClosed()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>socketFailure()</i>,
// <tt>BitTorrentTrackerClientBase</tt>::<i>findAndSetResponseSize()</i>
// -# <tt>BitTorrentTrackerMsgAnnounce</tt> or <tt>BitTorrentTrackerMsgResponse</tt> should be 
// redefined/extended to add new fields in the exchanged messages
// 
// Limitations:
// -# support for one .torrent only
// -# support for HTTP only - no HTTPS
// -# since there is no official standard for any part of the protocol the implementation is based on 
//  <a href=\"http://wiki.theory.org/BitTorrentSpecification\">this</a> specification
//
// @see BitTorrentTrackerBase, BitTorrentTrackerMsgAnnounce, BitTorrentTrackerMsgResponse
//
simple BTTrackerClientBase
	parameters:
		address: string,			// local address - may be symbolic, IPV4 or IPv6

		port: numeric const,			//local port

		connectAddress: string,			// tracker address - may be symbolic, IPV4 or IPv6

		connectPort: numeric const,		// tracker port number

		//connectAddress2: string,			// tracker address - may be symbolic, IPV4 or IPv6

		//connectPort2: numeric const,		// tracker port number
	
		connectGiveUp: numeric const, 		// give up after such failures

		sessionTimeout: numeric const,		// session timeout (seconds)

		reconnectInterval: numeric const,	// reconnect interval (seconds)

		infoHash: string,			// the SHA-1 (20 bytes) hash of the info field from the 
							// metafile (.torrent) - urlencoded

		peerId: string, 			// unique ID (20 bytes) for the client

		peerPort: numeric const, 		// the actual port that the client uses for the 
							// TCP Peer-wire protocol
		
		compact: bool, 				// flag - indicates whether the client wants a compact reply 
							// or not
		
		noPeerId: bool,				// flag - indicates whether the tracker can omit the peed_id 
							// field in the peers dictionary
		subtrackerNum: numeric const,

		numWant: numeric const, 		// the number of peers the client would like to receive 
							// from the tracker
		
		key: string,				// additional identification string - used in IP address change

		serverThreadClass: string,

		netinf: bool,				// boolean for netinf

		trackerConnectAddress: string;


		//trackerConnectPort: string;

	gates:
		in: tcpIn, btorrentIn;
		out: tcpOut, btorrentOut;
endsimple
