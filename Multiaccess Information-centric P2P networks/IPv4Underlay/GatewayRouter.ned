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
    "NotificationBoard",
    "InterfaceTable",   
    "RoutingTable",     
    "NetworkLayer",
    "PPPInterface",
    "EthernetInterface",
    "AccessNet";

//
// gateway router connects the access nets to the network backbone
//
module GatewayRouter
    parameters:
        routingFile: string;    // use predefined routing file if given
    gates:
        in: in[];    // incoming gates from overlay
        out: out[];    // outgoing gates to router
        in: ethIn[];    // placeholder for zero-size vector
        out: ethOut[];    // placeholder for zero-size vector
    submodules:
        notificationBoard: NotificationBoard;
            display: "p=60,60;i=block/control";
        interfaceTable: InterfaceTable;
            display: "p=150,60;i=block/table";
        routingTable: RoutingTable;
            parameters:
                IPForward = true,    // true if ip packets should be forwarded
                routerId = "auto",    // id of the router
                routingFile = routingFile;    // use predefined routing file if given
            display: "p=240,60;i=block/table";
        accessNet: AccessNet;
            display: "p=330,60;i=block/cogwheeli";
        networkLayer: NetworkLayer;
            gatesizes:
                ifIn[sizeof(out)+sizeof(ethOut)],
                ifOut[sizeof(out)+sizeof(ethOut)];
            display: "p=199,141;i=block/fork;q=queue";
        ppp: PPPInterface[sizeof(out)];
            display: "p=90,257,row,110;q=queue;i=block/ifcard";
        eth: EthernetInterface[sizeof(ethOut)];
            display: "p=286,268,row,110;q=queue;i=block/ifcard";
    connections nocheck:
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
endmodule

