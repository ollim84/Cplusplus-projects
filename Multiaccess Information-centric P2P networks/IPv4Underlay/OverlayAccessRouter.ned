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
    "Tiers",
    "Overlay",
    "NeighborCache",
    "BootstrapList",
    "InterfaceTable",
    "RoutingTable",
    "UDP",
    "NetworkLayer",
    "PingApp",
    "PPPInterface",
    "AccessNet";

//
// Access router that participates in the overlay
//
// @author Markus Mauch, Bernhard Heep
//
module OverlayAccessRouter
    parameters:
        IPForward: bool, // true if ip packets should be forwarded
        routingFile: string, // use predefined routing file if given
        overlayType: string, // overlay protocol compound module to use
        tier1Type: string, // tier 1 application to use
        tier2Type: string, // tier 2 module to use
        tier3Type: string, // tier 3 module to use
        numTiers: numeric const; // number of tiers

    gates:
        in: in[]; // incoming gates from overlay
        in: ethIn[]; // placeholder for zero-size vector
        in: overlayNeighborArrowIn[]; // incoming gates for direct connections from overlay neigbors

        out: out[]; // outgoing gates to overlay
        out: ethOut[]; // placeholder for zero-size vector
        out: overlayNeighborArrowOut[]; // outgoing gates for direct connections to overlay neighbors

    submodules:
        notificationBoard: NotificationBoard;
            display: "p=76,192;i=block/control";
        interfaceTable: InterfaceTable;
            display: "p=76,262;i=block/table";
        routingTable: RoutingTable;
            parameters:
                IPForward = IPForward, // true if ip packets should be forwarded
                routerId = "", // id of the router
                routingFile = routingFile; // use predefined routing file if given
            display: "p=76,340;i=block/table";
        accessNet: AccessNet;
            display: "p=76,402;i=block/cogwheeli";
        tier3: tier3Type like Tier;
            display: "p=56,64;i=block/segm";
        tier2: tier2Type like Tier;
            display: "p=139,88;i=block/segm";
        tier1: tier1Type like Tier;
            display: "p=218,122;i=block/segm";
        overlay: overlayType like Overlay;
            display: "p=290,184;i=block/network2";
        udp: UDP;
            display: "p=290,262;i=block/transport";
        networkLayer: NetworkLayer;
            parameters:
                proxyARP = false;
            gatesizes:
                ifIn[sizeof(out)+sizeof(ethOut)],
                ifOut[sizeof(out)+sizeof(ethOut)];
            display: "p=290,340;i=block/fork;q=queue";
        ppp: PPPInterface[sizeof(out)];
            display: "p=290,414,row,90;q=txQueue;i=block/ifcard";
        neighborCache: NeighborCache;
            display: "p=167,264;i=block/table";
        bootstrapList: BootstrapList;
            display: "p=168,344;i=block/table";
    connections nocheck:
        tier1.to_lowerTier --> overlay.from_app if numTiers > 0;
        tier1.from_lowerTier <-- overlay.to_app if numTiers > 0;
        tier1.to_udp --> udp.from_app++ if numTiers > 0;
        udp.to_app++ --> tier1.from_udp if numTiers > 0;

        tier2.to_lowerTier --> tier1.from_upperTier if numTiers > 1;
        tier2.from_lowerTier <-- tier1.to_upperTier if numTiers > 1;
        tier2.to_udp --> udp.from_app++ if numTiers > 1;
        udp.to_app++ --> tier2.from_udp if numTiers > 1;

        tier3.to_lowerTier --> tier2.from_upperTier if numTiers > 2;
        tier3.from_lowerTier <-- tier2.to_upperTier if numTiers > 2;
        tier3.to_udp --> udp.from_app++ if numTiers > 2;
        udp.to_app++ --> tier3.from_udp if numTiers > 2;

        overlay.to_udp --> udp.from_app++;
        overlay.from_udp <-- udp.to_app++;
        
        bootstrapList.to_udp --> udp.from_app++;
        bootstrapList.from_udp <-- udp.to_app++;

        udp.to_ip --> networkLayer.UDPIn;
        udp.from_ip <-- networkLayer.UDPOut;

        // connections to network outside
        for i=0..sizeof(out)-1 do
            in[i] --> ppp[i].physIn;
            out[i] <-- ppp[i].physOut;
            ppp[i].netwOut --> networkLayer.ifIn[i];
            ppp[i].netwIn <-- networkLayer.ifOut[i];
        endfor;
    display: "b=361,464";
endmodule

