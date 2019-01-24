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

simple SuperComUSO like TCPApp
{
    parameters:
        string address = default(""); // local address; may be left empty ("")
        int port = default(1000);     // port number to listen on
        double replyDelay @unit("s") = default(0s);
        int numRMS = default(1);
        bool fit4green = default(false); // true = f4g scheduling, false = standard cluster scheduling
        //int memory @unit("MB") = default(80MB);
        //int numCores = default(1);
        //double sessionTimeout @unit("s") = default(600s);
        //double sleepTimeout @unit("s") = default(600s);
        //bool energyAware = default(false);
        //int scheduling = default(1); // Scheduling mode
        string serverThreadClass;	// class name of "thread" objects to launch on incoming connections
        @display("i=block/app");
    gates:
        input tcpIn @labels(TCPCommand/up);
        output tcpOut @labels(TCPCommand/down);
}
