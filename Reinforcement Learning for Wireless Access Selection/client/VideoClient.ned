//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// Copyright 2013 Olli Mämmelä (VTT)

package inet.applications.ddeclient;
import inet.applications.tcpapp.TCPApp;

simple VideoClient like TCPApp
{
    parameters:
        // Server parameters
        string address = default(""); // local address; may be left empty ("")
        int port = default(1000);     // port number to listen on
        double replyDelay @unit("s") = default(0s);
        double sessionTimeout @unit("s") = default(600s);
        double suspendTimeout @unit("s") = default(240s);       
        string serverThreadClass;	// class name of "thread" objects to launch on incoming connections
        
        // Client parameters
        string connectAddress = default("");  // server address (may be symbolic)
        string connectAddressInterface1 = default("");
        string connectAddressInterface2 = default("");
        int connectPort = default(1000); // port number to connect to
        double startTime @unit("s") = default(1s); // time first session begins
        volatile int numRequestsPerSession = default(1);  // number of requests sent per session
        int requestLength @unit(B) = default(200B); // length of a request
        int replyLength @unit("B") = default(1MB); // length of a reply
        int fileSize @unit("B") = default(1MB); // size of a file
        double classifyInterval @unit("s"); // time gap between classify requests
        volatile double idleInterval @unit(s); // time gap between sessions
        volatile double reconnectInterval @unit("s") = default(30s);  // if connection breaks, waits this much before trying to reconnect
        int xPosition = default(0);
        int yPosition = default(0);
        
        bool learning; //whether reinforcement learning is used
        bool fuzzy; //whether fuzzy classification is used
        bool pureQLearning; // Whether pure Q learning is used
        bool debuggedNode = default(false); // whether node is used for vector collection
        double bandwidthLimit = default(0.5); // a limit which the bandwidth cannot go below
        
        // Video parameters
        int fps = default(30); // corresponds to the frames per second. Default pertains to 30fps
        double playRate = default(0.033); //time to start playing (i.e., popping and deleting) the packets in the PlayBuffer (ZY 25.10.2011)
        double pauseRate = default(0.025); // timer frequency for PAUSE t check  if the player can start playing the content again
        int initAdvBufferSize = default(150); //the number of frames that a playBuffer must have in advance before it starts to lay the content
        									  // the number of 150 frames correspond to 5 seconds of content @30fps
        int minAdvBufferSize = default(75); //the number of frames that a playbuffer must have in advance of the current track-slider position.
        								 // the number of 75 frames correspond to 2.5 seconds of content @30fps
		int threshYO = default(50);			//threshold value (in terms of number of chunks) for the buffer state between state Yello-Orange 
		int threshGY = default(60);        	//threshold value (in terms of number of chunks) for the buffer state between state Green-Yellow
		int userClass = default(0); // User class (gold, silver, bronze). OM 12.02.2013
        double txRate = default(0.025); //specifies the rate at which an app is allowed to tx content 
        int chunkSize @unit("B") = default(100B); //divides the bytestToSend into chunks, where the size of the chunk is specified by chunkSize
        
        @display("i=block/app");
    gates:
        input tcpIn @labels(TCPCommand/up);
        output tcpOut @labels(TCPCommand/down);
}
