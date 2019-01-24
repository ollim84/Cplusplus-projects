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

//
// TODO generated message class
//
packet ContextInformation {
    int appId; //application id
    int bytesToSend; //total number of bytes that the app must send during a specified TxWindow
    int chunkSize; //the chunk size (in bytes) that the app must send during a specified TxWindow. The total number of chunks = bytesToSend/chunkSize
    double txRate; //the time for which the application is allowed to transmit chunks
    double iatChunk; //inter-arrival time between chunks 

}
