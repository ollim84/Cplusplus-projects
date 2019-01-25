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

/**
 * @file IPv4UnderlayConfigurator.cc
 * @author Markus Mauch, Stephan Krause, Bernhard Heep
 */

#include "IPv4UnderlayConfigurator.h"

#include <BootstrapOracle.h>
#include <TransportAddress.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <AccessNet.h>
#include <RoutingTable.h>
#include <InterfaceTable.h>
#include <IPAddressResolver.h>
#include <IPv4InterfaceData.h>
#include <cstring>
#include <AccessNet.h>
#include <IPv4Info.h>
#include <NotificationBoard.h>

Define_Module(IPv4UnderlayConfigurator);

void IPv4UnderlayConfigurator::initializeUnderlay(int stage) {
    //backbone configuration
    if (stage == MIN_STAGE_UNDERLAY) {
        // Find all router module types.
        cTopology topo("topo");
        const char* typeNames[7];
        typeNames[0] = "Router";
        typeNames[1] = "OverlayRouter";
        typeNames[2] = "AccessRouter";
        typeNames[3] = "OverlayAccessRouter";
        typeNames[4] = "TunOutRouter";
	typeNames[5] = "WirelessAccessRouter";
        typeNames[6] = NULL;
        topo.extractByModuleType(typeNames);
	lastIP = 0;
        // Assign IP addresses to all router modules.
        std::vector<uint32> nodeAddresses;
        nodeAddresses.resize(topo.nodes());

        // IP addresses for backbone
        // Take start IP from config file
        // FIXME: Make Netmask for Routers configurable!
        uint32 lowIPBoundary = IPAddress(par("startIP").stringValue()).getInt();

        // uint32 lowIPBoundary = uint32((1 << 24) + 1);
        int numIPNodes = 0;
	int k;
	int nodeCtr;

        for (int i = 0; i < topo.nodes(); i++) {
            ++numIPNodes;
	    cTopology::Node* destNode = topo.node(i);
    
//            uint32 addr = lowIPBoundary + uint32(numIPNodes << 16);
//             nodeAddresses[i] = addr;

	    //Get the first neighbour of the node.
	    cTopology::Node *neighbor = destNode->out(0)->remoteNode();

	     // find neighbour's index in cTopology ==> k
            for (k=0; k<topo.nodes(); k++){
                if (topo.node(k)==neighbor){
                    break;
		}
	    }

	    //If the node is a backbone router, assign a x.0.0.0 address for it
	    if (strcmp(destNode->module()->name(), "backboneRouter") == 0) {
		  uint32 addr = uint32(intrand(223)) << 24;  // x.0.0.0 addresses
		  nodeAddresses[i] = addr;

		  // update ip display string
		  if (ev.isGUI()) {
		      const char* ip_disp =
			      const_cast<char*> (IPAddress(addr).str().c_str());
		      topo.node(i)->module()->displayString().insertTag("t", 0);
		      topo.node(i)->module()->displayString().setTagArg("t", 0,
			      ip_disp);
		      topo.node(i)->module()->displayString().setTagArg("t", 1, "l");
		      topo.node(i)->module()->displayString().setTagArg("t", 2, "red");
		  }

		  // find interface table and assign address to all (non-loopback) interfaces
		  InterfaceTable* ift = IPAddressResolver().interfaceTableOf(
			  topo.node(i)->module());

		  for (int k = 0; k < ift->numInterfaces(); k++) {
		      InterfaceEntry* ie = ift->interfaceAt(k);
		      if (!ie->isLoopback()) {
			  ie->ipv4()->setInetAddress(IPAddress(addr));
			  // full address must match for local delivery
			  ie->ipv4()->setNetmask(IPAddress::ALLONES_ADDRESS);
		      }
		  }
	    } 
	    //If the node is a gateway router, assign an address for it according to the backbone router it is connected to
	    else if (strcmp(destNode->module()->name(), "gatewayRouter") == 0) {

		    nodeCtr = intrand(255);

		    uint32 addr = nodeAddresses[k] + (uint32(nodeCtr) << 16);  // x.x.0.0 addresses
		    nodeAddresses[i] = addr;
		    if (ev.isGUI()) {
			const char* ip_disp =
				const_cast<char*> (IPAddress(addr).str().c_str());
			topo.node(i)->module()->displayString().insertTag("t", 0);
			topo.node(i)->module()->displayString().setTagArg("t", 0,
				ip_disp);
			topo.node(i)->module()->displayString().setTagArg("t", 1, "l");
			topo.node(i)->module()->displayString().setTagArg("t", 2, "red");
		    }

		    // find interface table and assign address to all (non-loopback) interfaces
		    InterfaceTable* ift = IPAddressResolver().interfaceTableOf(
			    topo.node(i)->module());

		    for (int k = 0; k < ift->numInterfaces(); k++) {
			InterfaceEntry* ie = ift->interfaceAt(k);
			if (!ie->isLoopback()) {
			    ie->ipv4()->setInetAddress(IPAddress(addr));
			    // full address must match for local delivery
			    ie->ipv4()->setNetmask(IPAddress::ALLONES_ADDRESS);
			}
		    }

	    }
	    //Else: node is an access router, assign an address for it according to the gateway router it is connected to
	    else {

		  nodeCtr = intrand(255);
		  uint32 addr = nodeAddresses[k] + (uint32(nodeCtr) << 8);  // x.x.x.0 addresses
		  nodeAddresses[i] = addr;
		  if (ev.isGUI()) {
		      const char* ip_disp =
			      const_cast<char*> (IPAddress(addr).str().c_str());
		      topo.node(i)->module()->displayString().insertTag("t", 0);
		      topo.node(i)->module()->displayString().setTagArg("t", 0,
			      ip_disp);
		      topo.node(i)->module()->displayString().setTagArg("t", 1, "l");
		      topo.node(i)->module()->displayString().setTagArg("t", 2, "red");
		  }

		  // find interface table and assign address to all (non-loopback) interfaces
		  InterfaceTable* ift = IPAddressResolver().interfaceTableOf(
			  topo.node(i)->module());

		  for (int k = 0; k < ift->numInterfaces(); k++) {
		      InterfaceEntry* ie = ift->interfaceAt(k);
		      if (!ie->isLoopback()) {
			  ie->ipv4()->setInetAddress(IPAddress(addr));
			  // full address must match for local delivery
			  ie->ipv4()->setNetmask(IPAddress::ALLONES_ADDRESS);
		      }
		  }
		  

 	    }

        }

        // Fill in routing tables.
        for (int i = 0; i < topo.nodes(); i++) {
            cTopology::Node* destNode = topo.node(i);
            uint32 destAddr = nodeAddresses[i];

            // calculate shortest paths from everywhere towards destNode
            topo.unweightedSingleShortestPathsTo(destNode);

            // add overlayAccessRouters and overlayBackboneRouters
            // to the BootstrapOracle
            if ((strcmp(destNode->module()->name(), "overlayBackboneRouter")
                    == 0) || (strcmp(destNode->module()->name(),
                    "overlayAccessRouter") == 0)) {
                //add node to bootstrap oracle
                PeerInfo* info = new PeerInfo(0, destNode->module()->id());
                bootstrapOracle->addPeer(IPvXAddress(nodeAddresses[i]), info);
            }

            // If destNode is the outRouter, add a default route
            // to outside network via the TunOutDevice and a route to the
            // Gateway
            if (strcmp(destNode->module()->name(), "outRouter") == 0) {
                RoutingEntry* defRoute = new RoutingEntry();
                defRoute->host = IPAddress::UNSPECIFIED_ADDRESS;
                defRoute->netmask = IPAddress::UNSPECIFIED_ADDRESS;
                defRoute->gateway = IPAddress(par("gatewayIP").stringValue());
                defRoute->interfaceName = "tunDev";
                defRoute->interfacePtr = IPAddressResolver().interfaceTableOf(
                        destNode->module())->interfaceByName("tunDev");
                defRoute->type = RoutingEntry::REMOTE;
                defRoute->source = RoutingEntry::MANUAL;
                IPAddressResolver().routingTableOf(destNode->module())->addRoutingEntry(
                        defRoute);

                RoutingEntry* gwRoute = new RoutingEntry();
                gwRoute->host = IPAddress(par("gatewayIP").stringValue());
                gwRoute->netmask = IPAddress(255, 255, 255, 255);
                gwRoute->interfaceName = "tunDev";
                gwRoute->interfacePtr = IPAddressResolver().interfaceTableOf(
                        destNode->module())->interfaceByName("tunDev");
                gwRoute->type = RoutingEntry::DIRECT;
                gwRoute->source = RoutingEntry::MANUAL;
                IPAddressResolver().routingTableOf(destNode->module())->addRoutingEntry(
                        gwRoute);
            }

            // add route (with host=destNode) to every routing table in the network
            for (int j = 0; j < topo.nodes(); j++) {
                // continue if same node
                if (i == j)
                    continue;	
                // cancel simulation if node is not conencted with destination
                cTopology::Node* atNode = topo.node(j);

                if (atNode->paths() == 0) {
                    error((std::string(atNode->module()->name())
                            + ": Network is not entirely connected."
                                "Please increase your value for the "
                                "connectivity parameter").c_str());
                }

                //
                // Add routes at the atNode.
                //

                // find atNode's interface and routing table
                InterfaceTable* ift = IPAddressResolver().interfaceTableOf(
                        atNode->module());
                RoutingTable* rt = IPAddressResolver().routingTableOf(
                        atNode->module());

                // find atNode's interface entry for the next hop node
                int outputGateId = atNode->path(0)->localGate()->id();
                InterfaceEntry *ie = ift->interfaceByNodeOutputGateId(
                        outputGateId);

		
                // find the next hop node on the path towards destNode
                cModule* next_hop = atNode->path(0)->remoteNode()->module();

                IPAddress next_hop_ip =
                        IPAddressResolver().addressOf(next_hop).get4();

                // Requirement 1: Each router has exactly one routing entry
                // (netmask 255.255.0.0) to each other router
                RoutingEntry* re = new RoutingEntry();

                re->host = IPAddress(destAddr);
                re->interfaceName = ie->name();
                re->interfacePtr = ie;
                re->source = RoutingEntry::MANUAL;
                re->netmask = IPAddress(255, 255, 255, 0);
                re->gateway = IPAddress(next_hop_ip);
                re->type = RoutingEntry::REMOTE;

                rt->addRoutingEntry(re);

                // Requirement 2: Each router has a point-to-point routing
                // entry (netmask 255.255.255.255) for each immediate neighbour
                if (atNode->distanceToTarget() == 1) {
                    RoutingEntry* re2 = new RoutingEntry();

                    re2->host = IPAddress(destAddr);
                    re2->interfaceName = ie->name();
                    re2->interfacePtr = ie;
                    re2->source = RoutingEntry::MANUAL;
                    re2->netmask = IPAddress(255, 255, 255, 255);
                    re2->type = RoutingEntry::DIRECT;

                    rt->addRoutingEntry(re2);
                }

                // If destNode is the outRouter, add a default route
                // to the next hop in the direction of the outRouter
                if (strcmp(destNode->module()->name(), "outRouter") == 0) {
                    RoutingEntry* defRoute = new RoutingEntry();
                    defRoute->host = IPAddress::UNSPECIFIED_ADDRESS;
                    defRoute->netmask = IPAddress::UNSPECIFIED_ADDRESS;
                    defRoute->gateway = IPAddress(next_hop_ip);
                    defRoute->interfaceName = ie->name();
                    defRoute->interfacePtr = ie;
                    defRoute->type = RoutingEntry::REMOTE;
                    defRoute->source = RoutingEntry::MANUAL;

                    rt->addRoutingEntry(defRoute);
                }
            }
        }
    }
    //accessnet configuration
    else if (stage == MAX_STAGE_UNDERLAY) {
        // fetch some parameters
        accessRouterNum = parentModule()->par("accessRouterNum");
	wirelessAccessRouterNum = parentModule()->par("wirelessAccessRouterNum");
        overlayAccessRouterNum = parentModule()->par("overlayAccessRouterNum");

        // count the overlay clients
        overlayTerminalCount = 0;

        numCreated = 0;
        numKilled = 0;
	subtrackerIndex = 0;
	seederIndex = 0;
	//trackerNum = 2;
	//TransportAddress* trackerAddress[trackerNum];
	subtrackerNum = par("subtrackerNum");
	seederNum = par("seederNum");

        // add access node modules to access node vector
        // and assing the channel tpye to be used by the access node
        cModule* node;
        AccessNet* nodeAccess;

        for (int i = 0; i < accessRouterNum; i++) {
            node = parentModule()->submodule("accessRouter", i);
            accessNode.push_back(node);
            nodeAccess = check_and_cast<AccessNet*> (node->submodule(
                    "accessNet"));
            nodeAccess->selectChannel(channelTypes[intuniform(0,
                    channelTypes.size() - 1)]);
        }

	//MiM for WirelessAccessRouters:
	for (int i = 0; i < wirelessAccessRouterNum; i++) {
            node = parentModule()->submodule("wirelessAccessRouter", i);
            accessNode.push_back(node);
            nodeAccess = check_and_cast<AccessNet*> (node->submodule(
                    "accessNet"));
            nodeAccess->selectChannel(channelTypes[intuniform(0,
                    channelTypes.size() - 1)]); //TODO: should we add a new wireless channel?
        }

        for (int i = 0; i < overlayAccessRouterNum; i++) {
            node = parentModule()->submodule("overlayAccessRouter", i);
            accessNode.push_back(node);
            nodeAccess = check_and_cast<AccessNet*> (node->submodule(
                    "accessNet"));
            nodeAccess->selectChannel(channelTypes[intuniform(0,
                    channelTypes.size() - 1)]);
        }

        // debug stuff
        WATCH_PTRVECTOR(accessNode);

        /**
         *  Create and connect the tracker and the initial seeder to a randomly chosen access router.
         *  Add this code at the end of the following method:
         *
         *  void IPv4UnderlayConfigurator::initializeUnderlay(int stage)
         *
         *  This will also store the address of the Tracker as explained in STEP 1.
         */
        std::vector<std::string> terminalTypes;
        std::vector<std::string>::iterator it;
        std::vector<std::string>::iterator end;

        terminalTypes = cStringTokenizer(par("terminalTypes"), " ").asVector();

        it = terminalTypes.begin();
        end = terminalTypes.end();

        int i = 0;
        // discover if BTHost is a terminal type, and if it is, create a seed and a tracker
        for (; it != end; it++) {
            if ((!std::strcmp(it->c_str(), "BTHost")) || (!std::strcmp(it->c_str(), "WirelessBTHost"))) {
                //trackerAddress = createBTNode("Tracker", false);
// 		if (subtrackerNum > 1) {
		    for (subtrackerIndex = 0; subtrackerIndex < subtrackerNum; subtrackerIndex++) {
			trackerAddress[subtrackerIndex] = createBTNode("SubTracker", false);
		    }
 		    //trackerIndex++;
  		    trackerAddress[subtrackerIndex+1] = createBTNode("Tracker", false);
// 		} else {
 		   // trackerAddress[0] = createBTNode("Tracker", false);
// 		}

		//trackerAddress[0] = createBTNode("Tracker", false);
		//trackerAddress[1] = createBTNode("Tracker", false);
		for (seederIndex = 0; seederIndex < seederNum; seederIndex++) {
		    createBTNode("BTHostSeeder", false);
		}
                
		//createBTNode("BTHostSeeder", false);
                break;
            }
        }
    }
}

TransportAddress* IPv4UnderlayConfigurator::createNode(NodeType type,
        bool initialize) {
    Enter_Method_Silent();
    // derive overlay node from ned
    cModuleType* moduleType = findModuleType(type.terminalType.c_str());
    
    // append index to module name
    char nodeName[80];
    sprintf(nodeName, "overlayTerminal_%i", numCreated);

    cModule* node = moduleType->create(nodeName, parentModule());
    
    overlayTerminalCount++;
    numCreated++;

    if (!strcmp(type.terminalType.c_str(), "OverlayHost")) {
        node->par("overlayType").setStringValue(type.overlayType.c_str());
        node->par("tier1Type").setStringValue(type.tier1Type.c_str());
        node->par("tier2Type").setStringValue(type.tier2Type.c_str());
        node->par("tier3Type").setStringValue(type.tier3Type.c_str());
    } else {
        node->setDisplayString("i=device/wifilaptop_l");
    }

     int numRadios = 0;
     int numPPP = 1;
    // int routerNum = 0;
     // If the host is a wireless BTHost set ppp gatesizes to zero. (OM)
     if (!strcmp(type.terminalType.c_str(), "WirelessBTHost")) {
	numRadios = node->par("numRadios");
	node->setGateSize("in", 0);
	node->setGateSize("out", 0);
	node->setGateSize("radioIn", numRadios);
	//routerNum = node->par("routerNum");

     } else {
	numPPP = node->par("numPPP");
	node->setGateSize("in", numPPP);
	node->setGateSize("out", numPPP);
     }

    std::string displayString;

    if ((type.typeID > 1) && (type.typeID <= (NUM_COLORS + 1))) {
        ((displayString += "i=device/wifilaptop_l,") += colorNames[type.typeID
                - 2]) += ",40;i2=block/circle_s";
    } else {
        displayString = "i=device/wifilaptop_l;i2=block/circle_s";
    }

    node->setDisplayString(displayString.c_str());

    node->buildInside();
    node->scheduleStart(simulation.simTime());

    
    if (!strcmp(type.terminalType.c_str(), "WirelessBTHost")) {

	  //Convert string accessNodeNumber to a vector
// 	  std::vector<int> accessNodeNumbers; // holds result
// 
// 	  const char *accessNodeNumber = node->par("accessNodeNumbers");
// 	  cStringTokenizer tokenizer(accessNodeNumber);
// 	  while (tokenizer.hasMoreTokens()) {
// 		accessNodeNumbers.push_back(atoi(tokenizer.nextToken()));
// 	  }

	  int routerPair;
	  int routerNumber[2];
	  int router1 = 0;
	  int router2 = 0;
	  double routerX[2];
	  double routerY[2];
	  cModule* routerNode;

	  ///If: multihomed
	  if(numRadios > 1) {

		  /// Get router pair from the 64 available alternatives
	 	 routerPair = intuniform(1, 64);
		 //routerPair = 19;   

		  /// Get two routers corresponding to that pair
		  getAccessRouters(routerPair, router1, router2);

		  ///Convert the values to int
		  routerNumber[0] = (int)router1;
		  routerNumber[1] = (int)router2;

	 ///Else: singlehomed
	 } else {
		  ///Just get a random wireless access router
		  routerNumber[0] = intuniform(0, wirelessAccessRouterNum - 1);
		  //routerNumber[0] = 29;

	 }

	  /// Go through all the radio interfaces of the node.
	  /// This is used for setting the link layer parameters channelNumber, channelsToScan, and default_ssid
	  for(int i = 0; i < numRadios ; i++ ) {
		  
		///Select a random wireless access router for the interface. Ignore the access router which has the main tracker.
		//routerNumber[i] = intuniform(1, accessNode.size() - 1);
		//number[i] = accessNodeNumbers[i];
		
		///Get the mobility parameters of the router
		routerNode = parentModule()->submodule("wirelessAccessRouter", routerNumber[i]);
		routerX[i] = routerNode->submodule("mobility")->par("x");
		routerY[i] = routerNode->submodule("mobility")->par("y");


		///Get the Channel number and default ssid of the router and set the same values for the node	
		node->submodule("wlan",i)->submodule("agent")->par("default_ssid") = routerNode->submodule("wlan",0)->submodule("mgmt")->par("ssid");
		node->submodule("wlan",i)->submodule("radio")->par("channelNumber") = routerNode->submodule("wlan",0)->submodule("radio")->par("channelNumber");

		///Set the channelsToScan parameter for the node. This is obtained from the channelNumber of the router
		std::string ChannelString;
		char ChannelChar[80];
		int ChannelNumber = routerNode->submodule("wlan",0)->submodule("radio")->par("channelNumber");
		sprintf(ChannelChar, "%d", ChannelNumber);
		node->submodule("wlan",i)->submodule("agent")->par("channelsToScan").setStringValue(ChannelChar);
		
	  }
	  
	  /// If: multihomed
	  if (numRadios > 1) {

	      /// Set the position of the node between the two routers it is connected to.
	      if(routerX[1] > routerX[0]) {
		  node->submodule("mobility")->par("x") = intuniform(routerX[0], routerX[1]);
	      } else {
		  node->submodule("mobility")->par("x") = intuniform(routerX[1], routerX[0]);
	      }

	      if(routerY[1] > routerY[0]) {
		  node->submodule("mobility")->par("y") = intuniform(routerY[0], routerY[1]);
	      } else {
		  node->submodule("mobility")->par("y") = intuniform(routerY[1], routerY[0]);
	      }
	  }
	  ///Else: the position of a singlehomed node is within a 60m radius of the router
	  else {
	      node->submodule("mobility")->par("x") = intuniform((routerX[0] - 60), (routerX[0] + 60));
	      node->submodule("mobility")->par("y") = intuniform((routerY[0] - 60), (routerY[0] + 60));

	  }

	  /// Add the node to the access net that was previously randomly chosen.
	  for(int i = 0; i < numRadios ; i++ ) {

		/// Create IPv4Info
		IPv4Info* info = new IPv4Info(type.typeID, node->submodule("wlan", i)->id());

		/// routerNumber[i] + 1, because main tracker is located in routerNumber[0]
		AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[routerNumber[i]+1] ->submodule("accessNet"));
		info->setAccessNetModule(accessNet);

		///add node to a randomly chosen access net
		info->setNodeID(accessNet->addWirelessOverlayNode(node, i));
		
		 ///add node to bootstrap oracle
		if (i == 0) {
		      bootstrapOracle->addPeer(IPAddressResolver().addressOf(node, "wlan0"), info);
		}
		else if (i == 1) {
		      bootstrapOracle->addPeer(IPAddressResolver().addressOf(node, "wlan1"), info);
		}
	  }


    } else {

	  int routerPair;
	  int routerNumber[2];
	  int router1 = 0;
	  int router2 = 0;
	  cModule* routerNode;

	  ///If: multihomed
	  if(numPPP > 1) {

		  /// Get router pair from the 64 available alternatives
	 	 routerPair = intuniform(1, 64);
		 //routerPair = 1;   

		  /// Get two routers corresponding to that pair
		  getAccessRouters(routerPair, router1, router2);

		  ///Convert the values to int
		  routerNumber[0] = (int)router1;
		  routerNumber[1] = (int)router2;

	 ///Else: singlehomed
	 } else {
		  ///Just get a random access router
// 		  if(overlayTerminalCount <= 20){
// 			routerNumber[0] = 12;
// 		  }
// 		  else if (overlayTerminalCount > 20 && overlayTerminalCount <= 40) {
// 			routerNumber[0] = 2;
// 		  }
// 		  else {
// 			routerNumber[0] = 22;
// 		  }

		  routerNumber[0] = intuniform(0, accessRouterNum - 2);


	 }

	  /// Add the node to the access net that was previously randomly chosen.
	  for(int i = 0; i < numPPP ; i++ ) {

		/// Create IPv4Info
		IPv4Info* info = new IPv4Info(type.typeID, node->submodule("ppp", i)->id());

		AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[routerNumber[i]] ->submodule("accessNet"));
		info->setAccessNetModule(accessNet);

		///add node to a randomly chosen access net
		info->setNodeID(accessNet->addOverlayTerminalNode(node, i));
		//info->setNodeID(accessNet->addOverlayNode(node));

		  ///add node to bootstrap oracle
		if (i == 0) {
		      bootstrapOracle->addPeer(IPAddressResolver().addressOf(node, "ppp0"), info);
		}
		else if (i == 1) {
		      bootstrapOracle->addPeer(IPAddressResolver().addressOf(node, "ppp1"), info);
		}
	  }

    }

  

//     AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[intuniform(0,
//             accessNode.size() - 1)] ->submodule("accessNet"));
// 
//     info->setAccessNetModule(accessNet);
//     //add node to a randomly chosen access net
//     info->setNodeID(accessNet->addOverlayNode(node));
// 
//     //add node to bootstrap oracle
//     bootstrapOracle->addPeer(IPAddressResolver().addressOf(node), info);    

    // if the node was not created during startup we have to
    // finish the initialization process manually

    if (!initialize) {
        for (int i = MAX_STAGE_UNDERLAY + 1; i < NUM_STAGES_ALL; i++) {
            node->callInitialize(i);
        }
    }


    churnGenerator[type.typeID - 1]->terminalCount++;
    TransportAddress *address = new TransportAddress(
            IPAddressResolver().addressOf(node));
    // Set the address and port at which the tracker listens
    // and the address and port for this node's tcp client.
    if ((!strcmp(type.terminalType.c_str(), "BTHost")) || (!strcmp(type.terminalType.c_str(), "WirelessBTHost"))) {


// 	    IPvXAddress trackerAddr = trackerAddress[0]->getAddress();
// 	    std::string addrString;
// 
// 	    if (!trackerAddr.isIPv6()) {
// 		addrString = trackerAddr.get4().str();
// 	    }
// 	    else {
// 		addrString = trackerAddr.get6().str();
// 	    }
// 	    
// 	    node->submodule("trackerClient")->par("connectAddress") = addrString.c_str();
// 	    node->submodule("trackerClient")->par("connectPort") = trackerAddress[0]->getPort();
// 
// 	    IPvXAddress localAddr = address->getAddress();
// 
// 	    if (!localAddr.isIPv6())
// 		addrString = localAddr.get4().str();
// 	    else
// 		addrString = localAddr.get6().str();
// 
// 	    node->submodule("trackerClient")->par("address") = addrString.c_str();
// 	    node->submodule("trackerClient")->par("port") = address->getPort();
      

    }
    // update display
    setDisplayString();

    return address;
}

TransportAddress* IPv4UnderlayConfigurator::createBTNode(const char* type,
        bool initialize) {
    const char* device;
    char buf[80];

    cModuleType* moduleType = findModuleType(type);

    if (!strcmp(type, "Tracker"))
	sprintf(buf, type);
    else if (!strcmp(type, "SubTracker"))
	sprintf(buf, "SubTracker_%i", subtrackerIndex);
    else if (!strcmp(type, "BTHostSeeder"))
	sprintf(buf, "BTHostSeeder_%i", seederIndex);

    cModule* node = moduleType->create(buf, parentModule());

    //sprintf(buf, type);

    if ((!strcmp(type, "Tracker")) || (!strcmp(type, "SubTracker"))) {
        device = "i=device/server_l";
    }
    else if (!strcmp(type, "BTHostSeeder")) {
        device = "i=device/mainframe_l";
    }

    node->setGateSize("in", 1);
    node->setGateSize("out", 1);
    node->setDisplayString(device);
    node->buildInside();
    node->scheduleStart(simulation.simTime());

    // create meta information
    IPv4Info* info = new IPv4Info(0, node->id());

     if (!strcmp(type, "Tracker")) {

 	  //AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[intuniform(0, accessNode.size() - 1)] ->submodule("accessNet"));
	  AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[0] ->submodule("accessNet"));
	  info->setAccessNetModule(accessNet);
	  //add node to a randomly chosen access net
	  info->setNodeID(accessNet->addOverlayNode(node));

	  //append index to module name
	  node->setName(buf);

	  //add node to bootstrap oracle
	  bootstrapOracle->addPeer(IPAddressResolver().addressOf(node), info);

    }

    else if (!strcmp(type, "SubTracker")) {
	  //AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[intuniform(0, accessNode.size() - 1)] ->submodule("accessNet"));
	  AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[subtrackerIndex+1] ->submodule("accessNet"));
	  info->setAccessNetModule(accessNet);
	  ///add node to a randomly chosen access net
	  info->setNodeID(accessNet->addOverlayNode(node));

	  ///append index to module name
	  node->setName(buf);

	  ///add node to bootstrap oracle
	  bootstrapOracle->addPeer(IPAddressResolver().addressOf(node), info);


    }
    else {

	//AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[intuniform(0, accessNode.size() - 1)] ->submodule("accessNet"));

	AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[seederIndex+1] ->submodule("accessNet"));
	info->setAccessNetModule(accessNet);
	///add node to a randomly chosen access net
	info->setNodeID(accessNet->addOverlayNode(node));

	///append index to module name
	node->setName(buf);

	///add node to bootstrap oracle
	bootstrapOracle->addPeer(IPAddressResolver().addressOf(node), info);

    }

//     AccessNet* accessNet = check_and_cast<AccessNet*> (accessNode[intuniform(0,
// 		accessNode.size() - 1)] ->submodule("accessNet"));
//     info->setAccessNetModule(accessNet);
//     // add node to a randomly chosen access net
//     info->setNodeID(accessNet->addOverlayNode(node));
// 
//     // append index to module name
//     node->setName(buf);
// 
//     //add node to bootstrap oracle
//     bootstrapOracle->addPeer(IPAddressResolver().addressOf(node), info);

    // if the node was not created during startup we have to
    // finish the initialization process manually
    if (!initialize) {
        for (int i = MAX_STAGE_UNDERLAY + 1; i < NUM_STAGES_ALL; i++) {
            node->callInitialize(i);
        }
    }

    IPvXAddress addr = IPAddressResolver().addressOf(node);
    TransportAddress *address = new TransportAddress(addr);
    address->setPort(80);

    if ((!strcmp(type, "Tracker")) || (!strcmp(type, "SubTracker"))) {
        //Ntinos Katsaros: 03/07/2008
        //Set the address and port at which the tracker listens
        std::string addrString;
        if (!addr.isIPv6())
            addrString = addr.get4().str();
        else
            addrString = addr.get6().str();

        //Now that we have the assinged IP addess (and port) we can change the values of the
        //module parameters.
        node->submodule("tcpApp", 0)->par("address").setStringValue(
                addrString.c_str());
        node->submodule("tcpApp", 0)->par("port") = address->getPort();
    }

    // update display
    setDisplayString();

    return address;
}

//TODO: getRandomNode()
void IPv4UnderlayConfigurator::preKillNode(NodeType type,
        TransportAddress* addr) {
    Enter_Method_Silent();

    AccessNet* accessNetModule = NULL;
    int nodeID;
    IPv4Info * info;

    // If no address given, get random node
    if (addr == NULL) {
        addr = bootstrapOracle->getRandomAliveNode(type.typeID);

        if (addr == NULL) {
            // all nodes are already prekilled
            std::cout << "all nodes are already prekilled" << std::endl;
            return;
        }
    }

    // get node information
    info = dynamic_cast<IPv4Info*> (bootstrapOracle->getPeerInfo(*addr));

    if (info != NULL) {
        accessNetModule = info->getAccessNetModule();
        nodeID = info->getNodeID();
    } else {
        opp_error("IPv4UnderlayConfigurator: Trying to pre kill node "
            "with nonexistant TransportAddress!");
    }

    uint32_t effectiveType = info->getTypeID();

    // do not kill node that is already scheduled
    if (scheduledID.count(nodeID))
        return;

    cModule* node = accessNetModule->getOverlayNode(nodeID);
    bootstrapOracle->removePeer(IPAddressResolver().addressOf(node));

    //put node into the kill list and schedule a message for final removal of the node
    killList.push_front(IPAddressResolver().addressOf(node));
    scheduledID.insert(nodeID);

    overlayTerminalCount--;
    numKilled++;

    churnGenerator[effectiveType - 1]->terminalCount--;

    // update display
    setDisplayString();

    // inform the notification board about the removal
    NotificationBoard* nb = check_and_cast<NotificationBoard*> (
            node->submodule("notificationBoard"));
    nb->fireChangeNotification(NF_OVERLAY_NODE_LEAVE);

    double random = uniform(0, 1);

    if (random < gracefulLeaveProbability) {
        nb->fireChangeNotification(NF_OVERLAY_NODE_GRACEFUL_LEAVE);
    }

    cMessage* msg = new cMessage();
    scheduleAt(simulation.simTime() + gracefulLeaveDelay, msg);

}

void IPv4UnderlayConfigurator::migrateNode(NodeType type,
        TransportAddress* addr) {
    Enter_Method_Silent();

    AccessNet* accessNetModule = NULL;
    int nodeID = -1;
    IPv4Info * info;

    // If no address given, get random node
    if (addr == NULL) {
        info = dynamic_cast<IPv4Info*> (bootstrapOracle->getRandomPeerInfo(
                type.typeID));
    } else {
        // get node information
        info = dynamic_cast<IPv4Info*> (bootstrapOracle->getPeerInfo(*addr));
    }

    if (info != NULL) {
        accessNetModule = info->getAccessNetModule();
        nodeID = info->getNodeID();
    } else {
        opp_error(
                "IPv4UnderlayConfigurator: Trying to pre kill node with nonexistant TransportAddress!");
    }

    // do not migrate node that is already scheduled
    if (scheduledID.count(nodeID))
        return;

    cModule* node = accessNetModule->removeOverlayNode(nodeID);//intuniform(0, accessNetModule->size() - 1));

    if (node == NULL)
        opp_error(
                "IPv4UnderlayConfigurator: Trying to remove node which is nonexistant in AccessNet!");

    //remove node from bootstrap oracle
    bootstrapOracle->killPeer(IPAddressResolver().addressOf(node));

    node->bubble("I am migrating!");

    // connect the node to another access net
    AccessNet* newAccessNetModule;

    do {
        newAccessNetModule = check_and_cast<AccessNet*> (accessNode[intuniform(
                0, accessNode.size() - 1)]->submodule("accessNet"));
    } while ((newAccessNetModule == accessNetModule)
            && (accessNode.size() != 1));

    // create meta information
    IPv4Info* newinfo = new IPv4Info(type.typeID, node->id());
    // add node to a randomly chosen access net
    newinfo->setAccessNetModule(newAccessNetModule);
    newinfo->setNodeID(newAccessNetModule->addOverlayNode(node, true));

    //add node to bootstrap oracle
    bootstrapOracle->addPeer(IPAddressResolver().addressOf(node), newinfo);

    // inform the notofication board about the migration
    NotificationBoard* nb = check_and_cast<NotificationBoard*> (
            node->submodule("notificationBoard"));
    nb->fireChangeNotification(NF_OVERLAY_TRANSPORTADDRESS_CHANGED);
}

void IPv4UnderlayConfigurator::handleTimerEvent(cMessage* msg) {
    Enter_Method_Silent();

    // get next scheduled node from the kill list
    IPvXAddress addr = killList.back();
    killList.pop_back();

    AccessNet* accessNetModule = NULL;
    int nodeID = -1;

    IPv4Info* info =
            dynamic_cast<IPv4Info*> (bootstrapOracle->getPeerInfo(addr));
    if (info != NULL) {
        accessNetModule = info->getAccessNetModule();
        nodeID = info->getNodeID();
    } else {
        opp_error(
                "IPv4UnderlayConfigurator: Trying to kill node with nonexistant TransportAddress!");
    }

    scheduledID.erase(nodeID);
    bootstrapOracle->killPeer(addr);

    cModule* node = accessNetModule->removeOverlayNode(nodeID);

    if (node == NULL)
        opp_error(
                "IPv4UnderlayConfigurator: Trying to remove node which is nonexistant in AccessNet!");

    node->callFinish();
    node->deleteModule();

    delete msg;
}

void IPv4UnderlayConfigurator::setDisplayString() {
    char buf[80];
    sprintf(buf,
            "%i overlay terminals\n%i access router\n%i overlay access router",
            overlayTerminalCount, accessRouterNum, overlayAccessRouterNum);
    displayString().setTagArg("t", 0, buf);
}

void IPv4UnderlayConfigurator::finishUnderlay() {
    // statistics
    recordScalar("Terminals added", numCreated);
    recordScalar("Terminals removed", numKilled);

    if (!isInInitPhase()) {
        struct timeval now, diff;
        gettimeofday(&now, NULL);
        timersub(&now, &initFinishedTime, &diff);
        printf("Simulation time: %li.%06li\n", diff.tv_sec, diff.tv_usec);
    }
}

void IPv4UnderlayConfigurator::getAccessRouters(int routerPair, int &router1, int &router2) {
	
	
    switch(routerPair) {

	case 1:
		router1 = 0;
		router2 = 2;
		break;
	case 2:
		router1 = 10;
		router2 = 12;
		break;
	case 3:
		router1 = 20;
		router2 = 22;
		break;
	case 4:
		router1 = 1;
		router2 = 3;
		break;
	case 5:
		router1 = 11;
		router2 = 13;
		break;
	case 6:
		router1 = 21;
		router2 = 23;
		break;
	case 7:
		router1 = 2;
		router2 = 10;
		break;
	case 8:
		router1 = 10;
		router2 = 22;
		break;
	case 9:
		router1 = 22;
		router2 = 1;
		break;
	case 10:
		router1 = 1;
		router2 = 13;
		break;
	case 11:
		router1 = 13;
		router2 = 21;
		break;
	case 12:
		router1 = 0;
		router2 = 12;
		break;
	case 13:
		router1 = 12;
		router2 = 20;
		break;
	case 14:
		router1 = 20;
		router2 = 3;
		break;
	case 15:
		router1 = 3;
		router2 = 11;
		break;
	case 16:
		router1 = 11;
		router2 = 23;
		break;
	case 17:
		router1 = 2;
		router2 = 4;
		break;
	case 18:
		router1 = 12;
		router2 = 14;
		break;
	case 19:
		router1 = 22;
		router2 = 24;
		break;
	case 20:
		router1 = 3;
		router2 = 5;
		break;
	case 21:
		router1 = 13;
		router2 = 15;
		break;
	case 22:
		router1 = 23;
		router2 = 25;
		break;
	case 23:
		router1 = 4;
		router2 = 12;
		break;
	case 24:
		router1 = 12;
		router2 = 24;
		break;
	case 25:
		router1 = 24;
		router2 = 3;
		break;
	case 26:
		router1 = 3;
		router2 = 15;
		break;
	case 27:
		router1 = 15;
		router2 = 23;
		break;
	case 28:
		router1 = 2;
		router2 = 14;
		break;
	case 29:
		router1 = 14;
		router2 = 22;
		break;
	case 30:
		router1 = 22;
		router2 = 5;
		break;
	case 31:
		router1 = 5;
		router2 = 13;
		break;
	case 32:
		router1 = 13;
		router2 = 25;
		break;
	case 33:
		router1 = 4;
		router2 = 6;
		break;
	case 34:
		router1 = 14;
		router2 = 16;
		break;
	case 35:
		router1 = 24;
		router2 = 26;
		break;
	case 36:
		router1 = 5;
		router2 = 7;
		break;
	case 37:
		router1 = 15;
		router2 = 17;
		break;
	case 38:
		router1 = 25;
		router2 = 27;
		break;
	case 39:
		router1 = 6;
		router2 = 14;
		break;
	case 40:
		router1 = 14;
		router2 = 26;
		break;
	case 41:
		router1 = 26;
		router2 = 5;
		break;
	case 42:
		router1 = 5;
		router2 = 17;
		break;
	case 43:
		router1 = 17;
		router2 = 25;
		break;
	case 44:
		router1 = 4;
		router2 = 16;
		break;
	case 45:
		router1 = 16;
		router2 = 24;
		break;
	case 46:
		router1 = 24;
		router2 = 7;
		break;
	case 47:
		router1 = 7;
		router2 = 15;
		break;
	case 48:
		router1 = 15;
		router2 = 27;
		break;
	case 49:
		router1 = 6;
		router2 = 8;
		break;
	case 50:
		router1 = 16;
		router2 = 18;
		break;
	case 51:
		router1 = 26;
		router2 = 28;
		break;
	case 52:
		router1 = 7;
		router2 = 9;
		break;
	case 53:
		router1 = 17;
		router2 = 19;
		break;
	case 54:
		router1 = 27;
		router2 = 29;
		break;
	case 55:
		router1 = 8;
		router2 = 16;
		break;
	case 56:
		router1 = 16;
		router2 = 28;
		break;
	case 57:
		router1 = 28;
		router2 = 7;
		break;
	case 58:
		router1 = 7;
		router2 = 19;
		break;
	case 59:
		router1 = 19;
		router2 = 27;
		break;
	case 60:
		router1 = 6;
		router2 = 18;
		break;
	case 61:
		router1 = 18;
		router2 = 26;
		break;
	case 62:
		router1 = 26;
		router2 = 9;
		break;
	case 63:
		router1 = 9;
		router2 = 17;
		break;
	case 64:
		router1 = 17;
		router2 = 29;
		break;
	default:
		break;


    }

    return;


}
