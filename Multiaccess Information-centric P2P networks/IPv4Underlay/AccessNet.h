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
 * @file AccessNet.h
 * @author Markus Mauch
 */

#ifndef __ACCESSNET_H__
#define __ACCESSNET_H__

#include <omnetpp.h>

#include <InitStages.h>

class InterfaceTable;
class InterfaceEntry;
class RoutingTable;
class RoutingEntry;

/**
 * Information about a node (usually a router)
 */
class NodeInfo
{
public:
    uint32 IPAddress; //!< the IP Address @todo -> IPvXAddress
    uint32 IPAddress2; //!< the IP Address @todo -> IPvXAddress
    cModule* module; //!< pointer to node module (not this module)
    InterfaceTable* interfaceTable; //!< pointer to interface table of this node
    RoutingTable* routingTable; //!< pointer to routing table of this node
    simtime_t createdAt; //!< creation timestamp

    /**
     * Stream out
     *
     * @param os the output stream
     * @param n the node info
     * @return the stream
     */
    friend std::ostream& operator<<(std::ostream& os, NodeInfo& n);
};

/**
 * Information about a terminal
 */
class TerminalInfo : public NodeInfo
{
public:
    cModule* PPPInterface; //!< pointer to PPP module
    cModule* PPPInterface2; //!< pointer to PPP module
    cModule* remotePPPInterface; //!< pointer to remote PPP module
    cModule* remotePPPInterface2; //!< pointer to remote PPP module
    InterfaceEntry* interfaceEntry; //!< pointer to interface entry
    InterfaceEntry* interfaceEntry2; //!< pointer to interface entry
    InterfaceEntry* remoteInterfaceEntry; //!< pointer to remote interface entry
    InterfaceEntry* remoteInterfaceEntry2; //!< pointer to remote interface entry
    InterfaceTable* remoteInterfaceTable; //!< pointer to remote interface table
    InterfaceTable* remoteInterfaceTable2; //!< pointer to remote interface table
    RoutingEntry* remoteRoutingEntry; //!< pointer to remote routing table
    RoutingEntry* remoteRoutingEntry2; //!< pointer to remote routing table
    RoutingEntry* routingEntry; //!< pointer to routing entry
    RoutingEntry* routingEntry2; //!< pointer to routing entry
    RoutingEntry* routingEntry3; //!< pointer to routing entry
    RoutingEntry* routingEntry4; //!< pointer to routing entry
};

/**
 * Configuration module for access networks
 */
class AccessNet : public cSimpleModule
{
public:

    /**
     * Returns number of nodes at this access router
     *
     * @return number of nodes
     */
    virtual int size()
    {
        return overlayTerminal.size();
    }

    /**
     * Getter for router module
     *
     * @return pointer to router module
     */
    virtual cModule* getAccessNode()
    {
        return router.module;
    }

    /**
     * Gathers some information about the terminal and appends it to
     * the overlay terminal vector
     *
     * Gathers some information about the terminal and appends it to
     * the overlay terminal vector.
     * (called by IPv4UnderlayConfigurator in stage MAX_STAGE_UNDERLAY)
     */
    virtual int addOverlayNode(cModule* overlayNode, bool migrate = false);

    virtual int addWirelessOverlayNode(cModule* overlayNode, int interfaceIndex, bool migrate = false);

    virtual int addOverlayTerminalNode(cModule* overlayNode, int interfaceIndex, bool migrate = false);

    /**
     * returns a random ID
     */
    int getRandomNodeId();

    /**
     * Removes a node from the access net
     */
    virtual cModule* removeOverlayNode(int ID);

    /**
     * searches overlayTerminal[] for a given node
     *
     * @param ID position of the node in overlayTerminal
     * @return the nodeId if found, -1 else
     */
    virtual cModule* getOverlayNode(int ID);

    /**
     * set access type
     *
     * @param type access type
     */
    void selectChannel(const std::string& type)
    {
        channelTypeStr = type;
    }

protected:

    NodeInfo router; //!< this access router
    NodeInfo router2; //!< this access router
    std::vector<TerminalInfo> overlayTerminal; //!< the terminals at this access router

    /**
     * OMNeT number of init stages
     *
     * @return neede number of init stages
     */
    virtual int numInitStages() const
    {
        return MAX_STAGE_UNDERLAY + 1;
    }

    /**
     * Gather some information about the router node.
     */
    virtual void initialize(int stage);

    /**
     * OMNeT handleMessage method
     *
     * @param msg the message to handle
     */
    virtual void handleMessage(cMessage* msg);

    /**
     * Displays the current number of terminals connected to this access net
     */
    virtual void updateDisplayString();

    uint32_t lastIP; //!< last assigned IP address

    std::string channelTypeStr; //!< the different possible channel types
    // statistics
    cOutVector lifetimeVector; //!< vector of node lifetimes
};

/**
 * Returns a module's fist unconnected gate
 *
 * @param owner gate owner module
 * @param name name of the gate vector
 */
cGate* firstUnusedGate(cModule* owner, const char* name);



#endif
