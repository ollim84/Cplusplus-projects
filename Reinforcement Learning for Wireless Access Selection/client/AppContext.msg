// Copyright 2011 Zarrar Yousaf (NEC)

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

cplusplus {{
#include "INETDefs.h"
#include "GenericAppMsg_m.h"
}}


class GenericAppMsg;

packet AppContext extends GenericAppMsg
{
//    int appIndex; //app index in case it is a vector
//    int appId; //application id
//    int portId; //the port id on which teh server is listening
    int fileSize; //total fileSize 
    int remainingFileSize; //remaining bytes of the file
//    bool cxStatus = false; // status of connection: 0 when CLOSED and 1 when ESTABLISHED. By default it is false
//    int playBufferSize; //convey the size of the play buffer at the corresponding UE. 
    double txDuration; // the amount of time the app is allowed to tx content
    int chunkSize; //the chunk size that the app is allowed to transmit
    double codecRate; //the codec rate or profile
    int userClass; // userClass of the client
    //TODO: add initial play started?
//    bool initialPlayStarted; // OM 11.02.2013: tells the agent if the client has reached initial buffer size
//    int userClass; // OM 12.02.2013: specifies the user class
//    bool playFinished; // OM: tells the agent that the client has finished
}
