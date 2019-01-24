// Copyright 2012 Zarrar Yousaf (NEC)

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

//
// A packet for carrying the context informatin about the TCP cleint application instance
//
packet CliAppContext{
    int appIndex; //app index in case it is a vector
    int appId; //application id
    int portId; //the port Id over which the client is receiving TCP segments
    long playBufferSize;//convey the size of the play buffer (in bits)
    int playBufferState; //convery the state of the playBuffer whether RED, ORANGE, YELLOW or GREEN
    int userClass; // Class of the user: Gold, Silver, Bronze
 }
