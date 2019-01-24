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
#include "InfoServer.h"
#include "MACAddress.h"
#include <list>
struct apInfo {
	int ID;
	IPvXAddress apAddress;
	int numUsers;
	float load;
	float packetLoss;
	float classifyValue;
	bool cachedContent;
	MACAddress macAddress;
	apInfo() {
		ID = -1; classifyValue = -1, load = -1, packetLoss = -1;
	}
};
typedef std::list<apInfo> APList;
}}


class GenericAppMsg;
class noncobject IPvXAddress;
class noncobject APList;
class noncobject MACAddress;



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



//dde Msg, used for getting AP classification
packet ddeMsg extends GenericAppMsg
{
    int classifyOperation = -1; // AP = 0, MN = 1, NS = 2
    float apClassifyValue = -1;
    float nsClassifyValue = -1;
    float mnClassifyValue = -1;
    float seedsPerLeechers = -1;
    double BW_requirement = -1;
    int numUsers = -1;
    float load = -1;
    float packetLoss = -1;
    APList apList;
    bool cachedContent;
    string hostID;
    string assocAP;
    double consumedBW = -1;
    MACAddress macAddress;
    float signalStrength = -1;
    double client_x = -1;
    double client_y = -1;
    int userClass = -1;
    MACAddress HO_to_AP_MAC;
    string HO_to_AP_SSID;
    double maxBW = -1;
    double usedBW = -1;
    double avBW = -1;
    float rssiDBM = -1;
    float sensitivity = -1;
    float noiseLevelDBM = -1;
    int numGold = -1;
    int numSilver = -1;
    int numBronze = -1;
    bool backgroundTraffic;
    
}

message ddeInternalMsg extends GenericAppMsg
{
    int apIndex = -1;
    APList apList;
    IPvXAddress clientAddr = IPvXAddress("0.0.0.0");
    bool cachedContent;
    
}