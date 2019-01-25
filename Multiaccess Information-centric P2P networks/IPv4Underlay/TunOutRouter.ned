//
// Copyright (C) 2000 Institut fuer Telematik, Universitaet Karlsruhe
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//


import
    "Tiers",
    "NotificationBoard",
    "InterfaceTable",
    "RoutingTable",
    "PacketParser",
    "PPPInterface",
    "TunOutDevice",
    "NetworkLayer";

//
// \TunOut router.
//
module TunOutRouter
    parameters:
        parser: string, // name of the PacketParser to use
        routingFile: string; // use predefined routing file if given
    gates:
        in: in[]; // incoming gates to router
        out: out[]; // outgoing gates to network or host
    submodules:
        notificationBoard: NotificationBoard;
            display: "p=60,60;i=block/control";
        interfaceTable: InterfaceTable;
            display: "p=150,60;i=block/table";
        routingTable: RoutingTable;
            parameters:
                IPForward = true, // true if ip packets should be forwarded
                routerId = "auto", // id of the router
                routingFile = routingFile; // use predefined routing file if given
            display: "p=240,60;i=block/table";
        packetParser: parser like PacketParser;
            display: "p=59,126;i=block/wheelbarrow";
        networkLayer: NetworkLayer;
            gatesizes:
                ifIn[sizeof(out)+1],
                ifOut[sizeof(out)+1];
            display: "p=200,141;i=block/fork;q=queue";
        ppp: PPPInterface[sizeof(out)];
            display: "p=90,257,row,110;q=l2queue;i=block/ifcard";
        tunDev: TunOutDevice;
            display: "p=145,257,row,110;q=l2queue;i=block/ifcard";
    connections nocheck:
        // connections to network outside
        for i=0..sizeof(out)-1 do
            in[i] --> ppp[i].physIn;
            out[i] <-- ppp[i].physOut;
            ppp[i].netwOut --> networkLayer.ifIn[i];
            ppp[i].netwIn <-- networkLayer.ifOut[i];
        endfor;

        tunDev.netwOut --> networkLayer.ifIn[sizeof(out)];
        tunDev.netwIn <-- networkLayer.ifOut[sizeof(out)];
endmodule

