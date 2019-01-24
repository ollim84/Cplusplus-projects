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
import inet.applications.tcpapp.TCPApp;

simple SuperComClientApp like TCPApp
{
    parameters:
        string address = default(""); // may be left empty ("")
        
        //Parameters for a request
        //NOTE: Volatile = value can change at runtime
        volatile int inputData @unit("B") = default(200B);
        volatile int cores = default(1);
        volatile int nodes = default(1);
        volatile int memory @unit("MB") = default(10MB);
        volatile double runTime @unit("s");
        volatile int load = default(20); // Job load
		volatile double networkLoad = default(0.0); // network load created by this job between servers
		volatile double loadedTime = default(1.0); //Percentage of the walltime the job has read/write operations on the HDD
		volatile int priority = default(1); //0 = interactive (= user priority), 1 = no user priority, RMS calculates priority
        volatile int energyAware = default(0); //for the federated case, if user wants energy aware cluster scheduling
        
        bool federated = default(false);
        int port = default(-1); // port number to listen on
        int jobRequests = default(10); // the number requests a node sends during a simulation run
        string connectAddress = default("");  // server address (may be symbolic)
        int connectPort = default(1000); // port number to connect to
        double startTime @unit("s") = default(1s); // time first session begins
        volatile int numRequestsPerSession = default(1);  // number of requests sent per session
        volatile int requestLength @unit(B) = default(200B); // length of a request
        volatile int replyLength @unit("B") = default(1MB); // length of a reply
        volatile double thinkTime @unit("s"); // time gap between requests
        volatile double idleInterval @unit(s); // time gap between sessions
        volatile double reconnectInterval @unit("s") = default(30s);  // if connection breaks, waits this much before trying to reconnect
        @display("i=block/app");
    gates:
        input tcpIn @labels(TCPCommand/up);
        output tcpOut @labels(TCPCommand/down);
}
