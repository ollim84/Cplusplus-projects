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
 * @file IPv4Info.h
 * @author Helge Backhaus
 */

#ifndef __IPV4INFO_H__
#define __IPV4INFO_H__

#include <PeerInfo.h>

class AccessNet;

class IPv4Info : public PeerInfo
{
public:
    /**
     * constructor
     */
    IPv4Info(uint32_t type, int moduleId);

    /**
     * setter and getter
     */
    void setAccessNetModule(AccessNet* accessNetModule) { this->accessNetModule = accessNetModule; };
    void setNodeID(int nodeID) { this->nodeID = nodeID; };
    AccessNet* getAccessNetModule() { return accessNetModule; };
    int getNodeID() { return nodeID; };

protected:
    void dummy(); /**< dummy-function to make IPv4Info polymorphic */

    AccessNet* accessNetModule;
    int nodeID;
};

#endif
