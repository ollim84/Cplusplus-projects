//
// Copyright (C) 2004 Andras Varga
// Copyright 2011 Zarrar Yousaf (NEC)
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


package inet.applications.tcpapp;

//
// Extends the TCPSinkApp wwith a queue. Accepts any number of incoming TCP connections, and queues the packets 
// and periodically deques them. Models the playout buffer of a browser.
// Compatible with both IPv4 and IPv6.
//
simple TCPSinkAppQ like TCPApp
{
    parameters:
        string address = default(""); // may be left empty ("")
        int port = default(1000); // port number to listen on
        int fps = default(30); // corresponds to the frames per second. Default pertains to 30fps
        double playRate = default(0.033); //time to start playing (i.e., popping and deleting) the packets in the PlayBuffer (ZY 25.10.2011)
        double pauseRate = default(0.025); // timer frequency for PAUSE t check  if the player can start playing the content again
        int initAdvBufferSize = default(150); //the number of frames that a playBuffer must have in advance before it starts to lay the content
        									  // the number of 150 frames correspond to 5 seconds of content @30fps
        int minAdvBufferSize = default(75); //the number of frames that a playbuffer must have in advance of the current track-slider position.
        								 // the number of 75 frames correspond to 2.5 seconds of content @30fps
		int threshYO;			//threshold value (in terms of number of chunks) for the buffer state between state Yello-Orange 
		int threshGY;        	//threshold value (in terms of number of chunks) for the buffer state between state Green-Yellow
		int userClass = default(0); // User class (gold, silver, bronze). OM 12.02.2013
		double codecRate; // codec rate of the client, needed for dynamic play rate
		bool dynamicPlayRate; // if dynamic play rate is calculated
		string contentServerAddress; // name of the content server to contact
		int bytesToReceive @unit("B") = default(1MB); //indicates the total amount of content data in bytes.
		bool scheduleRestart = default (false);
 							 
        @display("i=block/sink");
    gates:
        input tcpIn @labels(TCPCommand/up);
        output tcpOut @labels(TCPCommand/down);
}

