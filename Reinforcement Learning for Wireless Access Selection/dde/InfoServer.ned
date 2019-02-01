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
// // Copyright 2013 Olli Mämmelä (VTT)

package inet.applications.dde;
import inet.applications.tcpapp.TCPApp;

simple InfoServer like TCPApp
{
    parameters:
        // Server parameters
        string address = default(""); // local address; may be left empty ("")
        int port = default(1000);     // port number to listen on
        int numAPs = default(1); // Number of APs in the system
        volatile int requestLength @unit(B) = default(200B); // length of a request
        double startTime @unit("s") = default(0s);
        double loadBalancing_interval @unit("s") = default(1s);
        double congestionThreshold;
        double replyDelay @unit("s") = default(0s);
        double sessionTimeout @unit("s") = default(600s);
        double sleepTimeout @unit("s") = default(600s);       
        string serverThreadClass;	// class name of "thread" objects to launch on incoming connections
        bool loadBalancing = default(false);
               
        @display("i=block/app");
    gates:
        input tcpIn @labels(TCPCommand/up);
        output tcpOut @labels(TCPCommand/down);
}
