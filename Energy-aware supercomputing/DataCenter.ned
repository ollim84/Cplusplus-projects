//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

package inet.applications.supercom;

import inet.nodes.inet.StandardHost;
import inet.nodes.inet.Router;
//import inet.nodes.inet.channels;
import ned.DatarateChannel;
import inet.applications.supercom.StatisticsCollector;


//
// Data center consisting of several servers and rms.
//

module DataCenter
{
    parameters:
        // Number of servers in the data centre
        int numServers = default(1);
        // int memory @unit("KB") = default(80MB);
        // int numCores = default(1);
        @display("i=device/server_l");
    gates:
        inout dcg[];
    types:
        channel ethernetline extends DatarateChannel
        {
            delay = 0.1us;
            datarate = 10Mbps;
        }
    submodules:
        server[numServers]: StandardHost {
            parameters:
                @display("i=device/pc2");
        }
        //Resource management system
        rms: StandardHost {
            parameters:
                IPForward = true;
                //@display("i=device/pc2");
        }
        router: Router {
            parameters:
        }
        stats: StatisticsCollector {
            @display("p=48,49");
        }


    connections allowunconnected:
        // connect RMS ppp gates to DC inout gate
        //rms.pppg++ <--> dcg++;
        rms.ethg++ <--> dcg++;
        
        //rms.pppg++ <--> ethernetline <--> router.pppg++;
        rms.ethg++ <--> router.ethg++;
        for i=0..(numServers)-1 {
            //router.pppg++ <--> ethernetline <--> server[i].pppg++;
            router.ethg++ <--> server[i].ethg++;
        }
}
