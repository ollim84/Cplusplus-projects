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


import
    "NotificationBoard",
    "InterfaceTable",
    "RoutingTable",
    "UDP",
    "NetworkLayer",
    "PingApp",
    "PPPInterface",
    "TCPApp.ned",
    "TCP.ned",
    "PingApp",
    "PPPInterface",
    "EthernetInterface",
    "Ieee80211Nic", //OM
    "Ieee80211NicSTA", //OM
    "Ieee80211NicSTASimplified", //OM
    "NullMobility"; //OM


//
// A wireless BitTorrent host that participates in the overlay
//
// @author Konstantinos Katsaros (OM)
//
// Based on StandardHost module created by Markus Mauch, Bernhard Heep
//

module WirelessBTHost
    parameters:
	channelType: string,
	ASID: numeric const,	//Ntinos Katsaros: ID of the AS this host belongs to (negative)
        IPForward: bool, // true if ip packets should be forwarded
        trackerClientTCPAppType: string,
        peerWireTCPAppType: string,
        routingFile: string, // use predefined routing file if given
	numRadios: numeric const, //Number of WLAN interfaces (OM)
	//accessNodeNumbers: string,
	//routerNum: numeric const,
	mobilityType: string; //OM

    gates:
        in: in[]; // incoming gates from router
        in: ethIn[]; // placeholder for zero-size vector

        out: out[]; // outgoing gates to router
        out: ethOut[]; // placeholder for zero-size vector

	in: radioIn[]; //Gate for WLAN (OM)

    submodules:
        notificationBoard: NotificationBoard;
            display: "p=68,208;i=block/control";
        interfaceTable: InterfaceTable;
            display: "p=68,278;i=block/table";
        routingTable: RoutingTable;
            parameters:
                IPForward = IPForward, // true if ip packets should be forwarded
                routerId = "", // id of the router
                routingFile = routingFile; // use predefined routing file if given
		//routingFile = "host.rt";
            display: "p=68,348;i=block/table";
	trackerClient: trackerClientTCPAppType like TCPApp;
		display: "p=170,80;i=block/app";        
	peerWire: peerWireTCPAppType like TCPApp;
		display: "p=250,80;i=block/app";
	tcp: TCP;
            display: "p=200,160;i=block/wheelbarrow";
	pingApp: PingApp;
            display: "i=block/app;p=343,200";
        networkLayer: NetworkLayer;
            parameters:
                proxyARP = false; // sets proxy \ARP mode (replying to \ARP requests for the addresses for which a routing table entry exists)
            gatesizes:
                ifIn[sizeof(out)+sizeof(ethOut)+sizeof(radioIn)],
                ifOut[sizeof(out)+sizeof(ethOut)+sizeof(radioIn)]; //+1 for in/out wlan (OM)
            display: "p=200,240;i=block/fork;q=queue";
        ppp: PPPInterface[sizeof(out)];
            display: "p=300,350,row,90;q=txQueue;i=block/ifcard";
	    //display: "p=200,320,row,90;q=txQueue;i=block/ifcard";
        eth: EthernetInterface[sizeof(ethOut)];
            display: "p=240,350,row,90;q=txQueue;i=block/ifcard";
	wlan: Ieee80211NicSTA[sizeof(radioIn)];	//OM
            //display: "p=200,350;q=queue;i=block/ifcard";
	    display: "p=205,350,row,90;q=txQueue;i=block/ifcard";
        mobility: NullMobility;	//OM
            display: "p=68,148;i=block/cogwheel_s";

    connections nocheck:
	peerWire.btorrentOut --> trackerClient.btorrentIn;
	trackerClient.btorrentOut --> peerWire.btorrentIn;

        peerWire.tcpOut --> tcp.from_appl++;
	peerWire.tcpIn <-- tcp.to_appl++;

        trackerClient.tcpOut --> tcp.from_appl++;
        trackerClient.tcpIn <-- tcp.to_appl++;

	tcp.to_ip --> networkLayer.TCPIn;
        tcp.from_ip <-- networkLayer.TCPOut;

	networkLayer.pingOut --> pingApp.pingIn;
        networkLayer.pingIn <-- pingApp.pingOut;

        // connections to network outside
        for i=0..sizeof(out)-1 do
            in[i] --> ppp[i].physIn;
            out[i] <-- ppp[i].physOut;
            ppp[i].netwOut --> networkLayer.ifIn[i];
            ppp[i].netwIn <-- networkLayer.ifOut[i];
        endfor;

        for i=0..sizeof(ethOut)-1 do
            ethIn[i] --> eth[i].physIn;
            ethOut[i] <-- eth[i].physOut;
            eth[i].netwOut --> networkLayer.ifIn[sizeof(out)+i];
            eth[i].netwIn <-- networkLayer.ifOut[sizeof(out)+i];
        endfor;

	for i=0..sizeof(radioIn)-1 do	    
	    radioIn[i] --> wlan[i].radioIn; //OM
	    wlan[i].uppergateOut --> networkLayer.ifIn[sizeof(out)+sizeof(ethOut)+i];	//OM
	    wlan[i].uppergateIn <-- networkLayer.ifOut[sizeof(out)+sizeof(ethOut)+i];	//OM
	endfor;

endmodule

