//
// Copyright (C) 2004 Andras Varga
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
// along with this program; if not, see <http://www.gnu.org/licenses/>
//


cplusplus {{
#include "INETDefs.h"
#include "GenericAppMsg_m.h"
#include "IPvXAddress.h"
}}


class GenericAppMsg;
class noncobject IPvXAddress;
//
// Message class for generic request-reply style applications.
// The client sends a GenericAppMsg which contains the number of
// bytes the client wants the server to send back as reply.
//
// This way intelligence (behaviour specific to the modelled application,
// e.g. HTTP, SMB, database protocol) needs only to be present in
// the client, and the server model can be kept simple and dumb.
//
// @see TCPGenericSrvApp, TCPGenericCliAppBase (C++ only)
//

packet SuperComMsg extends GenericAppMsg

{
    long inputData = -1;
    long outputData = -1;
    int cores = -1;
    int nodes = -1;
    int memory = -1;
    int load = -1;
    double runTime = -1;
    int serverNumber = -1;
    int type = -1; //message type
    double networkLoad = 0.0; //how many packets/s this job creates, MiM	
    double loadedTime = 1.0;
    int priority = 1;
    int sleepMode = -1; // type of sleep, 0 = standby, 1 = hybernated
    simtime_t timeOfStart = -1;
    IPvXAddress clientAddress = IPvXAddress("0.0.0.0");
    IPvXAddress usoAddress = IPvXAddress("0.0.0.0");
    IPvXAddress rmsAddress = IPvXAddress("0.0.0.0");
    int jobID = -1;
    int submitTime = -1;
    bool energyAware = false;
    
}

//Internal message, can be used for contacting a server and holding information about a job
message SuperComInternalMsg extends cMessage
{
    int serverIndex = -1;
    double runTime = -1;
    int connId = -1;
    IPvXAddress clientAddress = IPvXAddress("0.0.0.0");
    IPvXAddress usoAddress = IPvXAddress("0.0.0.0");
    IPvXAddress rmsAddress = IPvXAddress("0.0.0.0");
    int needMemory = -1;
    int nodes = -1;
    int cores = -1;
    int load = -1;
    simtime_t subTime = -1;
    long inputData = -1;
    long outputData = -1;
    double networkLoad = 0.0; //how many packets/s this job creates, MiM 
    double loadedTime = 1.0;
    int priority = 1;
    simtime_t timeOfStart = -1;
    int sleepMode = -1; // type of sleep, 1 = standby, 2 = hybernated
    int jobID = -1;
    int submitTime = -1;
    bool energyAware = false;
}



