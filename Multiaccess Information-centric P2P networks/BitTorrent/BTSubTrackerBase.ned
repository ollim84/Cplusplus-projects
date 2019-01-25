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
//


// Implements a basic <i>BitTorrent Tracker</i>.<br>
// Serves as a generic request-reply HTTP handler (for BitTorrent tracker protocol), over TCP.<br>
// Compatible with both IPv4 and IPv6.<br>
//
// The model communicates with the <i>client</i> (i.e., instances type of <tt>BitTorrentTrackerClientBase</tt>
// or subclass) in sessions. During a session, the client <i>opens</i> a single TCP connection to 
// the server, <i>sends</i> its announce (request), <i>receives</i> the tracker\'s reply (response) and 
// <i>closes</i> the connection.
//
// The model interacts via:
// - <tt>BitTorrentTrackerMsgAnnounce</tt> messages that receives from the clients via TCP
// - <tt>BitTorrentTrackerMsgResponse</tt> messages that sends back to the clients via TCP
// 
// Extending <tt>BitTorrentTrackerBase</tt>:
// -# each tracker application should be, or subclassed from, <tt>BitTorrentTrackerBase</tt>
// -# each thread (i.e., the connection handling module) should be, or subclassed from, 
// <tt>BitTorrentTrackerClientHandlerBase</tt>
// -# to <i>add</i>, <i>change</i> or <i>extend</i> tracker\'s current behavior the following 
// functions have to reimplemented:
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>processAnnounce()</i>, 
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>sendResponse()</i>,
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>fillPeersInResponse()</i>,
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>findAndSetResponseSize()</i>,
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>established()</i>,
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>dataArrived()</i>,
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>timerExpired()</i>,
// <tt>BitTorrentTrackerClientHandlerBase</tt>::<i>peerClosed()</i>
// -# <tt>BitTorrentTrackerMsgAnnounce</tt> or <tt>BitTorrentTrackerMsgResponse</tt> should be 
// redefined/extended to add new fields in the exchanged messages
// 
// Limitations:
// -# support for one .torrent only
// -# support for HTTP only - no HTTPS
// -# since there is no official standard for any part of the protocol the implementation is based on 
//  <a href=\"http://wiki.theory.org/BitTorrentSpecification\">this</a> specification
//
// @see BitTorrentTrackerClientBase, BitTorrentTrackerMsgAnnounce, BitTorrentTrackerMsgResponse
//
simple BTSubTrackerBase
	parameters:
		address: string, 		// local address - may be symbolic, IPv4 or IPv6

		port: numeric const,		// listening port

		infoHash: string,		// the SHA-1 (20 bytes) hash value of the info section
 						// (.torrent file)

		trackerId: string,		// the tracker id

		alwaysSendTrackerId: bool,	// flag - indicates whether to send the tracker id in 
						// every response or only once (i.e., after receiving 
						// a valid announce with event=started)

		compactSupport: bool,		// flag - support for compact replies or not

		maxPeersInReply: numeric const,	// max number of peers in a reply

		announceInterval: numeric const,// re-announce interval (seconds)

		cleanupInterval: numeric const,	// cleanup interval (seconds) - removes the inactive 
						// peers (this should probably be three or four times
						// the re-announce interval)

		sessionTimeout: numeric const,	// session timeout (seconds)

		serverThreadClass : string,	// class name of "thread" objects to launch on incoming 
						// connections

		maxClean_exit : numeric const;	// the number of subsequent cleanup intervals in which no peer has announced
						// its presence
	gates:
		in: tcpIn;
		out: tcpOut;
endsimple

