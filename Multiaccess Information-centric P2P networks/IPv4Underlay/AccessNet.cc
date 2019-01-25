//
// Copyright (C) 2006 Institut fuer Telematik, Universitaet Karlsruhe (TH)
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
 * @file AccessNet.cc
 * @author Markus Mauch
 */

#include <vector>
#include <iostream>

#include <omnetpp.h>

#include <RoutingTable.h>
#include <InterfaceTable.h>
#include <IPAddressResolver.h>
#include <IPv4InterfaceData.h>
#include <NotificationBoard.h>
#include <PPP.h>
#include <ChannelControl.h> //MiM
#include "AccessNet.h"

Define_Module(AccessNet);

std::ostream& operator<<(std::ostream& os, NodeInfo& n)
{
    os << IPAddress(n.IPAddress);
    return os;
}

void AccessNet::initialize(int stage)
{
    if(stage != MIN_STAGE_UNDERLAY + 1)
        return;

    router.module = parentModule();
    router.interfaceTable = IPAddressResolver().interfaceTableOf(parentModule());
    router.routingTable = IPAddressResolver().routingTableOf(parentModule());
    router.IPAddress = IPAddressResolver().addressOf(parentModule()).get4().getInt();

    // statistics
    lifetimeVector.setName("Terminal Lifetime");

    WATCH_VECTOR(overlayTerminal);

    lastIP = 0;

    updateDisplayString();
}

void AccessNet::handleMessage(cMessage* msg)
{
    error("this module doesn't handle messages, it runs only in initialize()");
}

int AccessNet::addOverlayNode(cModule* node, bool migrate)
{
    Enter_Method("addOverlayNode()");

    TerminalInfo terminal;
    terminal.module = node;
    terminal.interfaceTable = IPAddressResolver().interfaceTableOf(node);
    terminal.remoteInterfaceTable = router.interfaceTable;
    terminal.routingTable = IPAddressResolver().routingTableOf(node);
    terminal.PPPInterface = node->submodule("ppp", 0);
    terminal.createdAt = simTime();

    // find unassigned ip address:
    //   Start at last given address, check if next address is valid and free.
    bool ip_test = false;
    for (uint32 ipOffset = lastIP + 1; ipOffset != lastIP; ipOffset++) {
        if ( ipOffset == 0x10000) {
            // Netmask = 255.255.0.0, so roll over if offset = 2**16
            ipOffset = 0;
            continue;
        }

        uint ip = router.IPAddress + ipOffset;

        // Check if IP is valid:
        //   Reject x.y.z.0 or x.y.z.255 or x.y.255.z
        if ( ((ip & 0xff) == 0) || ((ip & 0xff) == 0xff)
	     || ((ip & 0xff00) == 0xff00) ) {
            continue;
        }

        // Check if IP is free
        ip_test = true;
        for (uint i = 0; i < overlayTerminal.size(); i++) {
            if (overlayTerminal[i].IPAddress == ip) {
                ip_test = false;
                break;
            }
        }

        // found valid IP
        if (ip_test) {
            terminal.IPAddress = ip;
            lastIP = ipOffset;
            break;
        }
    }
    if (!ip_test)
        opp_error ("Error creating node: No available IP in access net!");

    // update ip display string
    if (ev.isGUI()) {
        const char* ip_disp = const_cast<char*>
	    (IPAddress(terminal.IPAddress).str().c_str());
        terminal.module->displayString().insertTag("t", 0);
        terminal.module->displayString().setTagArg("t", 0, ip_disp);
        terminal.module->displayString().setTagArg("t", 1, "l");
    }


    //
    // Create new remote ppp interface module for this terminal
    //

    // create ppp interface module

    int k = 1;
    while ( router.module->findSubmodule("ppp", k) != -1 )
        k++;

    cModuleType* pppInterfaceModuleType = findModuleType("PPPInterface");
    terminal.remotePPPInterface = pppInterfaceModuleType->
	create("ppp", router.module, 0, k);


    // set up gate sizes
    terminal.remotePPPInterface->setGateSize("physIn", 1);
    terminal.remotePPPInterface->setGateSize("physOut", 1);
    terminal.remotePPPInterface->setGateSize("netwIn", 1);
    terminal.remotePPPInterface->setGateSize("netwOut", 1);


    //
    // Connect all gates
    //

    // connect terminal to access router and vice versa
    cGate* routerInGate = firstUnusedGate(router.module, "in");
    cGate* routerOutGate = firstUnusedGate(router.module, "out");

    //cChannelType* channelType = findChannelType( channelTypeStr.c_str() );
    cChannelType* channelType = findChannelType( "fiberline" );

    if (!channelType) opp_error("Could not find Channel Type. Most likely "
		    "parameter channelTypes does not match the channels defined "
		    "in channels.ned");

    terminal.module->gate("out", 0)->connectTo(routerInGate,
				channelType->create(channelTypeStr.c_str()));
    routerOutGate->connectTo(terminal.module->gate("in", 0),
			     channelType->create(channelTypeStr.c_str()));

    // connect ppp interface module to router module and vice versa
    routerInGate->connectTo(terminal.remotePPPInterface->gate("physIn", 0));
    terminal.remotePPPInterface->gate("physOut", 0)->connectTo(routerOutGate);

    // connect ppp interface module to network layer module and vice versa
    cModule* netwModule = router.module->submodule("networkLayer");

    cGate* netwInGate = firstUnusedGate(netwModule, "ifIn");
    cGate* netwOutGate = firstUnusedGate(netwModule, "ifOut");

    netwOutGate->connectTo(terminal.remotePPPInterface->gate("netwIn", 0));
    terminal.remotePPPInterface->gate("netwOut", 0)->connectTo(netwInGate);

    // connect network layer module to ip and arp modules
    cModule* ipModule = router.module->submodule("networkLayer")->
	submodule("ip");

#ifndef _MAX_SPEED
    cModule* arpModule = router.module->submodule("networkLayer")->submodule("arp"); //comment out for speed-hack
#endif

#ifndef _MAX_SPEED
    cGate* arpOut = firstUnusedGate(arpModule, "nicOut"); //comment out for speed-hack
#endif
    cGate* ipIn = firstUnusedGate(ipModule, "queueIn");
#ifndef _MAX_SPEED
    cGate* ipOut = firstUnusedGate(ipModule, "queueOut"); //comment out for speed-hack

    arpOut->connectTo(netwOutGate);	//comment out for speed-hack
#endif

    netwInGate->connectTo(ipIn);
    

    //
    // Start ppp interface modules
    //

    terminal.remotePPPInterface->setDisplayString("i=block/ifcard");
    terminal.remotePPPInterface->buildInside();
    terminal.remotePPPInterface->scheduleStart(simulation.simTime());
    terminal.remotePPPInterface->callInitialize();



    if ( !migrate) {
	// we are already in stage 4 and need to call initialize
	// for all previous stages manually
	for (int i=0; i < MAX_STAGE_UNDERLAY + 1; i++) {
	    terminal.module->callInitialize(i);	    
	}
    }
//     }
    
   
    
    terminal.remoteInterfaceEntry = router.interfaceTable->interfaceAt(
	router.interfaceTable->numInterfaces() - 1);
    terminal.interfaceEntry = terminal.interfaceTable->interfaceByName("ppp0");


    //
    // Fill in interface table.
    //

    // router
    IPv4InterfaceData* interfaceData = new IPv4InterfaceData;
    interfaceData->setInetAddress(router.IPAddress);
    interfaceData->setNetmask(IPAddress::ALLONES_ADDRESS);
    terminal.remoteInterfaceEntry->setIPv4Data(interfaceData);

    // terminal
    terminal.interfaceEntry->ipv4()->setInetAddress(
	IPAddress(terminal.IPAddress));
    terminal.interfaceEntry->ipv4()->setNetmask(IPAddress::ALLONES_ADDRESS);

    //
    // Fill in routing table.
    //

    // router
    RoutingEntry* re = new RoutingEntry();
    re->host = IPAddress(terminal.IPAddress);
    re->netmask = IPAddress(IPAddress::ALLONES_ADDRESS);
    re->interfaceName = terminal.remoteInterfaceEntry->name();
    re->interfacePtr = terminal.remoteInterfaceEntry;
    re->type = RoutingEntry::DIRECT;
    re->source = RoutingEntry::MANUAL;
    router.routingTable->addRoutingEntry(re);
    terminal.remoteRoutingEntry = re;

    // terminal
    RoutingEntry* te = new RoutingEntry();
    te->host = IPAddress::UNSPECIFIED_ADDRESS;
    te->netmask = IPAddress::UNSPECIFIED_ADDRESS;
    te->gateway = router.IPAddress;
    te->interfaceName = terminal.interfaceEntry->name();
    te->interfacePtr = terminal.interfaceEntry;
    te->type = RoutingEntry::REMOTE;
    te->source = RoutingEntry::MANUAL;
    terminal.routingTable->addRoutingEntry(te);
    terminal.routingEntry = te;


    // append module to overlay terminal vector
    overlayTerminal.push_back(terminal);
    int ID = terminal.module->id();

    updateDisplayString();

    return ID;
}

int AccessNet::addWirelessOverlayNode(cModule* node, int interfaceIndex, bool migrate)
{
    Enter_Method("addWirelessOverlayNode()");

    TerminalInfo terminal;
    terminal.module = node;
    terminal.interfaceTable = IPAddressResolver().interfaceTableOf(node);
    terminal.remoteInterfaceTable = router.interfaceTable;
    terminal.routingTable = IPAddressResolver().routingTableOf(node);;
    terminal.PPPInterface = node->submodule("wlan", interfaceIndex);//MiM
    terminal.createdAt = simTime();

    // find unassigned ip address:
    //   Start at last given address, check if next address is valid and free.
    bool ip_test = false;
    for (uint32 ipOffset = lastIP + 1; ipOffset != lastIP; ipOffset++) {
        if ( ipOffset == 0x10000) {
            // Netmask = 255.255.0.0, so roll over if offset = 2**16
            ipOffset = 0;
            continue;
        }

        uint ip = router.IPAddress + ipOffset;

        // Check if IP is valid:
        //   Reject x.y.z.0 or x.y.z.255 or x.y.255.z
        if ( ((ip & 0xff) == 0) || ((ip & 0xff) == 0xff)
	     || ((ip & 0xff00) == 0xff00) ) {
            continue;
        }

        // Check if IP is free
        ip_test = true;
        for (uint i = 0; i < overlayTerminal.size(); i++) {
            if (overlayTerminal[i].IPAddress == ip) {
                ip_test = false;
                break;
            }
        }

        // found valid IP
        if (ip_test) {
            terminal.IPAddress = ip;
            lastIP = ipOffset;
            break;
        }
    }
    if (!ip_test)
        opp_error ("Error creating node: No available IP in access net!");

    // update ip display string
    if (ev.isGUI()) {
        const char* ip_disp = const_cast<char*>
	    (IPAddress(terminal.IPAddress).str().c_str());
        terminal.module->displayString().insertTag("t", 0);
        terminal.module->displayString().setTagArg("t", 0, ip_disp);
        terminal.module->displayString().setTagArg("t", 1, "l");
    }


    //
    // Create new remote ppp interface module for this terminal
    //

    // create ppp interface module

//     int k = 1;
//     while ( router.module->findSubmodule("ppp", k) != -1 )
//         k++;
// 
//     cModuleType* pppInterfaceModuleType = findModuleType("PPPInterface");
//     terminal.remotePPPInterface = pppInterfaceModuleType->
// 	create("ppp", router.module, 0, k);
// 
// 
//     // set up gate sizes
//     terminal.remotePPPInterface->setGateSize("physIn", 1);
//     terminal.remotePPPInterface->setGateSize("physOut", 1);
//     terminal.remotePPPInterface->setGateSize("netwIn", 1);
//     terminal.remotePPPInterface->setGateSize("netwOut", 1);
// 
// 
//     //
//     // Connect all gates
//     //
// 
//TODO: THIS SHOULD PERHAPS BE DONE WITH WLAN ALSO !?? (MiM)
//     // connect terminal to access router and vice versa
//     cGate* routerInGate = firstUnusedGate(router.module, "in");
//     cGate* routerOutGate = firstUnusedGate(router.module, "out");
// 
//     cChannelType* channelType = findChannelType( channelTypeStr.c_str() );
//     if (!channelType) opp_error("Could not find Channel Type. Most likely "
// 		    "parameter channelTypes does not match the channels defined "
// 		    "in channels.ned");
// 
//     terminal.module->gate("out", 0)->connectTo(routerInGate,
// 				channelType->create(channelTypeStr.c_str()));
//     routerOutGate->connectTo(terminal.module->gate("in", 0),
// 			     channelType->create(channelTypeStr.c_str()));
// 
//     // connect ppp interface module to router module and vice versa
//     routerInGate->connectTo(terminal.remotePPPInterface->gate("physIn", 0));
//     terminal.remotePPPInterface->gate("physOut", 0)->connectTo(routerOutGate);
// 
//     // connect ppp interface module to network layer module and vice versa
//     cModule* netwModule = router.module->submodule("networkLayer");
// 
//     cGate* netwInGate = firstUnusedGate(netwModule, "ifIn");
//     cGate* netwOutGate = firstUnusedGate(netwModule, "ifOut");
// 
//     netwOutGate->connectTo(terminal.remotePPPInterface->gate("netwIn", 0));
//     terminal.remotePPPInterface->gate("netwOut", 0)->connectTo(netwInGate);
// 

//TODO: THIS SHOULD BE PERHAPS BE DONE WITH WLAN ALSO !?? (MiM)

//     // connect network layer module to ip and arp modules
//     cModule* ipModule = router.module->submodule("networkLayer")->
// 	submodule("ip");
// 
// #ifndef _MAX_SPEED
//     cModule* arpModule = router.module->submodule("networkLayer")->submodule("arp"); //comment out for speed-hack
// #endif
// 
// #ifndef _MAX_SPEED
//     cGate* arpOut = firstUnusedGate(arpModule, "nicOut"); //comment out for speed-hack
// #endif
//     cGate* ipIn = firstUnusedGate(ipModule, "queueIn");
// #ifndef _MAX_SPEED
//     cGate* ipOut = firstUnusedGate(ipModule, "queueOut"); //comment out for speed-hack
// 
//     arpOut->connectTo(netwOutGate);	//comment out for speed-hack
// #endif
// 
//     netwInGate->connectTo(ipIn);
//     
// 
//     //
//     // Start ppp interface modules
//     //
// 
//     terminal.remotePPPInterface->setDisplayString("i=block/ifcard");
//     terminal.remotePPPInterface->buildInside();
//     terminal.remotePPPInterface->scheduleStart(simulation.simTime());
//     terminal.remotePPPInterface->callInitialize();

    if(interfaceIndex == 0) {

	  if ( !migrate) {
	      // we are already in stage 4 and need to call initialize
	      // for all previous stages manually
	      for (int i=0; i < MAX_STAGE_UNDERLAY + 1; i++) {
		  terminal.module->callInitialize(i);	    
	      }
	  }
	
    } 
    
    terminal.remoteInterfaceEntry = router.interfaceTable->interfaceByName("wlan");//OM

//     terminal.interfaceEntry = terminal.interfaceTable->interfaceAt(terminal.interfaceTable->numInterfaces() - 1);

    if(interfaceIndex == 0) {
	  terminal.interfaceEntry = terminal.interfaceTable->interfaceByName("wlan0");//OM
    } else {
	  terminal.interfaceEntry = terminal.interfaceTable->interfaceByName("wlan1");//OM
    }

    if(terminal.interfaceEntry == NULL) {
	  error("error: did not find interface wlan\n");
    }


    //
    // Fill in interface table.
    //

    // router
    IPv4InterfaceData* interfaceData = new IPv4InterfaceData;
    interfaceData->setInetAddress(router.IPAddress);
    interfaceData->setNetmask(IPAddress::ALLONES_ADDRESS);
    terminal.remoteInterfaceEntry->setIPv4Data(interfaceData);

    // terminal
    terminal.interfaceEntry->ipv4()->setInetAddress(
	IPAddress(terminal.IPAddress));
    terminal.interfaceEntry->ipv4()->setNetmask(IPAddress::ALLONES_ADDRESS);

    //
    // Fill in routing table.
    //

    // router
    RoutingEntry* re = new RoutingEntry();
    re->host = IPAddress(terminal.IPAddress);
    re->netmask = IPAddress(IPAddress::ALLONES_ADDRESS);
    re->interfaceName = terminal.remoteInterfaceEntry->name();
    re->interfacePtr = terminal.remoteInterfaceEntry;
    re->type = RoutingEntry::DIRECT;
    re->source = RoutingEntry::MANUAL;
    router.routingTable->addRoutingEntry(re);
    terminal.remoteRoutingEntry = re;

    // terminal

    if (interfaceIndex == 0) {

	  RoutingEntry* te = new RoutingEntry();
	  te->host = IPAddress::UNSPECIFIED_ADDRESS;
	  te->netmask = IPAddress::UNSPECIFIED_ADDRESS;
	  te->gateway = router.IPAddress;
	  te->interfaceName = terminal.interfaceEntry->name();
	  te->interfacePtr = terminal.interfaceEntry;
	  te->type = RoutingEntry::REMOTE;
	  te->source = RoutingEntry::MANUAL;
	  terminal.routingTable->addRoutingEntry(te);
	  terminal.routingEntry = te;

    }
    else if (interfaceIndex == 1) {

	  IPAddress netmask = IPAddress("255.255.0.0");
	  uint32 netIPAddress = IPAddress(router.IPAddress).doAnd(netmask).getInt();

	  RoutingEntry* te = new RoutingEntry();
	  te->host = IPAddress(terminal.IPAddress);
	  te->netmask.set("255.255.255.0");
	  te->gateway = IPAddress::UNSPECIFIED_ADDRESS;
	  te->interfaceName = terminal.interfaceEntry->name();
	  te->interfacePtr = terminal.interfaceEntry;
	  te->type = RoutingEntry::DIRECT;
	  te->source = RoutingEntry::IFACENETMASK;
	  terminal.routingTable->addRoutingEntry(te);
	  terminal.routingEntry = te;

	  RoutingEntry* te2 = new RoutingEntry();
	  te2->host.set(netIPAddress);
	  te2->netmask.set("255.255.0.0");
	  te2->gateway = router.IPAddress;
	  te2->interfaceName = terminal.interfaceEntry->name();
	  te2->interfacePtr = terminal.interfaceEntry;
	  te2->type = RoutingEntry::REMOTE;
	  te2->source = RoutingEntry::MANUAL;
	  terminal.routingTable->addRoutingEntry(te2);
	  terminal.routingEntry2 = te2;

    }


    // append module to overlay terminal vector
    overlayTerminal.push_back(terminal);
    int ID = terminal.module->id();

    updateDisplayString();

    return ID;
}

int AccessNet::addOverlayTerminalNode(cModule* node, int interfaceIndex, bool migrate)
{
    Enter_Method("addOverlayNode()");

    TerminalInfo terminal;
    terminal.module = node;
    terminal.interfaceTable = IPAddressResolver().interfaceTableOf(node);
    terminal.remoteInterfaceTable = router.interfaceTable;
    terminal.routingTable = IPAddressResolver().routingTableOf(node);
    terminal.PPPInterface = node->submodule("ppp", interfaceIndex);
    terminal.createdAt = simTime();

    // find unassigned ip address:
    //   Start at last given address, check if next address is valid and free.
    bool ip_test = false;
    for (uint32 ipOffset = lastIP + 1; ipOffset != lastIP; ipOffset++) {
        if ( ipOffset == 0x10000) {
            // Netmask = 255.255.0.0, so roll over if offset = 2**16
            ipOffset = 0;
            continue;
        }

        uint ip = router.IPAddress + ipOffset;

        // Check if IP is valid:
        //   Reject x.y.z.0 or x.y.z.255 or x.y.255.z
        if ( ((ip & 0xff) == 0) || ((ip & 0xff) == 0xff)
	     || ((ip & 0xff00) == 0xff00) ) {
            continue;
        }

        // Check if IP is free
        ip_test = true;
        for (uint i = 0; i < overlayTerminal.size(); i++) {
            if (overlayTerminal[i].IPAddress == ip) {
                ip_test = false;
                break;
            }
        }

        // found valid IP
        if (ip_test) {
            terminal.IPAddress = ip;
            lastIP = ipOffset;
            break;
        }
    }
    if (!ip_test)
        opp_error ("Error creating node: No available IP in access net!");

    // update ip display string
    if (ev.isGUI()) {
        const char* ip_disp = const_cast<char*>
	    (IPAddress(terminal.IPAddress).str().c_str());
        terminal.module->displayString().insertTag("t", 0);
        terminal.module->displayString().setTagArg("t", 0, ip_disp);
        terminal.module->displayString().setTagArg("t", 1, "l");
    }


    //
    // Create new remote ppp interface module for this terminal
    //

    // create ppp interface module

    int k = 1;
    while ( router.module->findSubmodule("ppp", k) != -1 )
        k++;

    cModuleType* pppInterfaceModuleType = findModuleType("PPPInterface");
    terminal.remotePPPInterface = pppInterfaceModuleType->
	create("ppp", router.module, 0, k);


    // set up gate sizes
    terminal.remotePPPInterface->setGateSize("physIn", 1);
    terminal.remotePPPInterface->setGateSize("physOut", 1);
    terminal.remotePPPInterface->setGateSize("netwIn", 1);
    terminal.remotePPPInterface->setGateSize("netwOut", 1);


    //
    // Connect all gates
    //

    // connect terminal to access router and vice versa
    cGate* routerInGate = firstUnusedGate(router.module, "in");
    cGate* routerOutGate = firstUnusedGate(router.module, "out");

    //cChannelType* channelType = findChannelType( channelTypeStr.c_str() );
    cChannelType* channelType = findChannelType( "ethernetline" );

    if (!channelType) opp_error("Could not find Channel Type. Most likely "
		    "parameter channelTypes does not match the channels defined "
		    "in channels.ned");

    terminal.module->gate("out", interfaceIndex)->connectTo(routerInGate,
				channelType->create(channelTypeStr.c_str()));
    routerOutGate->connectTo(terminal.module->gate("in", interfaceIndex),
			     channelType->create(channelTypeStr.c_str()));

    // connect ppp interface module to router module and vice versa
    routerInGate->connectTo(terminal.remotePPPInterface->gate("physIn", 0));
    terminal.remotePPPInterface->gate("physOut", 0)->connectTo(routerOutGate);

    // connect ppp interface module to network layer module and vice versa
    cModule* netwModule = router.module->submodule("networkLayer");

    cGate* netwInGate = firstUnusedGate(netwModule, "ifIn");
    cGate* netwOutGate = firstUnusedGate(netwModule, "ifOut");

    netwOutGate->connectTo(terminal.remotePPPInterface->gate("netwIn", 0));
    terminal.remotePPPInterface->gate("netwOut", 0)->connectTo(netwInGate);

    // connect network layer module to ip and arp modules
    cModule* ipModule = router.module->submodule("networkLayer")->
	submodule("ip");

#ifndef _MAX_SPEED
    cModule* arpModule = router.module->submodule("networkLayer")->submodule("arp"); //comment out for speed-hack
#endif

#ifndef _MAX_SPEED
    cGate* arpOut = firstUnusedGate(arpModule, "nicOut"); //comment out for speed-hack
#endif
    cGate* ipIn = firstUnusedGate(ipModule, "queueIn");
#ifndef _MAX_SPEED
    cGate* ipOut = firstUnusedGate(ipModule, "queueOut"); //comment out for speed-hack

    arpOut->connectTo(netwOutGate);	//comment out for speed-hack
#endif

    netwInGate->connectTo(ipIn);
    

    //
    // Start ppp interface modules
    //

    terminal.remotePPPInterface->setDisplayString("i=block/ifcard");
    terminal.remotePPPInterface->buildInside();
    terminal.remotePPPInterface->scheduleStart(simulation.simTime());
    terminal.remotePPPInterface->callInitialize();

    if (interfaceIndex == 0) {

	  if ( !migrate) {
	      // we are already in stage 4 and need to call initialize
	      // for all previous stages manually
	      for (int i=0; i < MAX_STAGE_UNDERLAY + 1; i++) {
		  terminal.module->callInitialize(i);	    
	      }
	  }
    } else {

	PPP* ppp = check_and_cast<PPP*> (terminal.PPPInterface ->submodule("ppp"));
	ppp->update(0);
	ppp->update(3);

    }
//     }
    
   
    
    terminal.remoteInterfaceEntry = router.interfaceTable->interfaceAt(
	router.interfaceTable->numInterfaces() - 1);

    if (interfaceIndex == 0) {
	terminal.interfaceEntry = terminal.interfaceTable->interfaceByName("ppp0");
    }
    else {
	terminal.interfaceEntry = terminal.interfaceTable->interfaceByName("ppp1");
    }


    //
    // Fill in interface table.
    //

    // router
    IPv4InterfaceData* interfaceData = new IPv4InterfaceData;
    interfaceData->setInetAddress(router.IPAddress);
    interfaceData->setNetmask(IPAddress::ALLONES_ADDRESS);
    terminal.remoteInterfaceEntry->setIPv4Data(interfaceData);

    // terminal
    terminal.interfaceEntry->ipv4()->setInetAddress(
	IPAddress(terminal.IPAddress));
    terminal.interfaceEntry->ipv4()->setNetmask(IPAddress::ALLONES_ADDRESS);

    //
    // Fill in routing table.
    //

    // router
    RoutingEntry* re = new RoutingEntry();
    re->host = IPAddress(terminal.IPAddress);
    re->netmask = IPAddress(IPAddress::ALLONES_ADDRESS);
    re->interfaceName = terminal.remoteInterfaceEntry->name();
    re->interfacePtr = terminal.remoteInterfaceEntry;
    re->type = RoutingEntry::DIRECT;
    re->source = RoutingEntry::MANUAL;
    router.routingTable->addRoutingEntry(re);
    terminal.remoteRoutingEntry = re;
    
    if (interfaceIndex == 0) {

	  RoutingEntry* te = new RoutingEntry();
	  te->host = IPAddress::UNSPECIFIED_ADDRESS;
	  te->netmask = IPAddress::UNSPECIFIED_ADDRESS;
	  te->gateway = router.IPAddress;
	  te->interfaceName = terminal.interfaceEntry->name();
	  te->interfacePtr = terminal.interfaceEntry;
	  te->type = RoutingEntry::REMOTE;
	  te->source = RoutingEntry::MANUAL;
	  terminal.routingTable->addRoutingEntry(te);
	  terminal.routingEntry = te;

    }
    else if (interfaceIndex == 1) {

	  IPAddress netmask = IPAddress("255.255.0.0");
	  uint32 netIPAddress = IPAddress(router.IPAddress).doAnd(netmask).getInt();

	  RoutingEntry* te = new RoutingEntry();
	  te->host = IPAddress(terminal.IPAddress);
	  te->netmask.set("255.255.255.0");
	  te->gateway = IPAddress::UNSPECIFIED_ADDRESS;
	  te->interfaceName = terminal.interfaceEntry->name();
	  te->interfacePtr = terminal.interfaceEntry;
	  te->type = RoutingEntry::DIRECT;
	  te->source = RoutingEntry::IFACENETMASK;
	  terminal.routingTable->addRoutingEntry(te);
	  terminal.routingEntry = te;

	  RoutingEntry* te2 = new RoutingEntry();
	  te2->host.set(netIPAddress);
	  te2->netmask.set("255.255.0.0");
	  te2->gateway = router.IPAddress;
	  te2->interfaceName = terminal.interfaceEntry->name();
	  te2->interfacePtr = terminal.interfaceEntry;
	  te2->type = RoutingEntry::REMOTE;
	  te2->source = RoutingEntry::MANUAL;
	  terminal.routingTable->addRoutingEntry(te2);
	  terminal.routingEntry2 = te2;

    }

    // terminal
//     RoutingEntry* te = new RoutingEntry();
//     te->host = IPAddress::UNSPECIFIED_ADDRESS;
//     te->netmask = IPAddress::UNSPECIFIED_ADDRESS;
//     te->gateway = router.IPAddress;
//     te->interfaceName = terminal.interfaceEntry->name();
//     te->interfacePtr = terminal.interfaceEntry;
//     te->type = RoutingEntry::REMOTE;
//     te->source = RoutingEntry::MANUAL;
//     terminal.routingTable->addRoutingEntry(te);
//     terminal.routingEntry = te;


    // append module to overlay terminal vector
    overlayTerminal.push_back(terminal);
    int ID = terminal.module->id();

    updateDisplayString();

    return ID;
}


int AccessNet::getRandomNodeId()
{
    Enter_Method("getRandomNodeId()");

    return overlayTerminal[intuniform(0, overlayTerminal.size() - 1)].module->id();
}

cModule* AccessNet::removeOverlayNode(int ID)
{
    Enter_Method("removeOverlayNode()");

    cModule* node = NULL;
    TerminalInfo terminal;
    int index;

    for(unsigned int i=0; i<overlayTerminal.size(); i++) {
        if(overlayTerminal[i].module->id() == ID) {
            terminal = overlayTerminal[i];
            node = terminal.module;
            index = i;
        }
    }

    if(node == NULL) return NULL;

    cModule* ppp = terminal.remotePPPInterface;

    // disconnect terminal
    node->gate("out")->disconnect();
    node->gate("in")->fromGate()->disconnect();

    // disconnect ip and arp modules
    ppp->gate("netwIn", 0)->sourceGate()->disconnect();
    ppp->gate("netwOut", 0)->toGate()->disconnect();

    // remove associated ppp interface module
    ppp->callFinish();
    ppp->deleteModule();

    // remove associated interface table entry
    router.interfaceTable->deleteInterface(terminal.remoteInterfaceEntry);

    // remove routing entries
    terminal.routingTable->deleteRoutingEntry(terminal.routingEntry);
    router.routingTable->deleteRoutingEntry(terminal.remoteRoutingEntry);

    // statistics
    lifetimeVector.record(simTime() - overlayTerminal[index].createdAt);

    // remove terminal from overlay terminal vector
    overlayTerminal.erase(overlayTerminal.begin() + index);

    updateDisplayString();

    return node;
}

cModule* AccessNet::getOverlayNode(int ID)
{
    Enter_Method("getOverlayNode()");

    cModule* node = NULL;

    for(unsigned int i=0; i<overlayTerminal.size(); i++) {
        if(overlayTerminal[i].module->id() == ID)
            node = overlayTerminal[i].module;
    }
    return node;
}

void AccessNet::updateDisplayString()
{
    if (ev.isGUI()) {
        char buf[80];
        if ( overlayTerminal.size() == 1 ) {
            sprintf(buf, "1 terminal connected");
        } else {
            sprintf(buf, "%i terminals connected", overlayTerminal.size());
        }
        displayString().setTagArg("t", 0, buf);
        displayString().setTagArg("t", 2, "blue");
    }
}

cGate* firstUnusedGate(cModule* owner, const char* name)
{
    int index;
    for (index = 0; index < owner->gate(name)->size(); index++) {
        if (!owner->gate(name, index)->isConnectedOutside()) {
            return owner->gate(name, index);
        }
    }

    owner->setGateSize(name, index + 2);
    return owner->gate(name, index + 1);
}

