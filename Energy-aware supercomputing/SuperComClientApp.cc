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
// along with this program; if not, see <http://www.gnu.org/licenses/>.
//


#include "SuperComClientApp.h"
#include "GenericAppMsg_m.h"
#include "SuperComMsg_m.h"
#include "TCPCommand_m.h"
#include "StatisticsCollector.h"
#include <iostream>
#include <fstream>
#include <string>

#define BEV	EV << "[" << this->getParentModule()->getFullName() << "]: "

//TODO: these could be defined differently, use different numbers?
#define MSGKIND_CONNECT		10
#define MSGKIND_REQUEST     11
#define MSGKIND_SEND        12
#define MSGKIND_INPUT       13
#define MSGKIND_RESPONSE    14
#define MSGKIND_FINISH      15

using namespace std;


Define_Module(SuperComClientApp);

SuperComClientApp::SuperComClientApp()
{
    connectMsg = NULL;
}

SuperComClientApp::~SuperComClientApp()
{
    cancelAndDelete(connectMsg);
}

void SuperComClientApp::initialize()
{

	// parent initialize()
	TCPGenericCliAppBase::initialize();

	//Client also needs to listen to the socket in order to accept a connection from the RMS
	socket.listen();

	//Generate values for memory, load, loadedTime and inputData
	//writeData();
	//initializeJobs();

	jobQueue.setName("jobQueue");

    // Create a self-message for contacting the rms
    connectMsg = new cMessage("connect", MSGKIND_CONNECT);

    numRequestsToSend = 0; //Redundant parameter
    earlySend = false;  //Redundant parameter
    requestSent = false;
    jobRequests = par("jobRequests");
    sentRequests = 0;
    federated = par("federated");
    contactRMS = false;

    subTime = 0;
    compTime = 0;

    createJobQueue();

    // Statistics
	//modp = simulation.getModuleByPath("datacenter.stats");
	modp = simulation.getModuleByPath("HPCNetwork.stats");
	stats = check_and_cast<StatisticsCollector *>(modp);

	if (stats == NULL)
			opp_error("Wrong statisticsModulePath configuration");

    WATCH(numRequestsToSend);
    WATCH(earlySend);
    simtime_t startTime = par("startTime");


    // Schedule sending of message at startTime
    scheduleAt(startTime, connectMsg);
    //std::cerr << "Client start time " << startTime << endl;
}

void SuperComClientApp::createJobQueue()
{
	 //std::cerr << "Creating job queue.. " << jobRequests << endl;

	 int tempNodes = 0;
	 int tempCores = 0;
	 long tempInputData = 0;
     int tempMemory = 0;
     int tempRunTime = 0;
     int tempLoad = 0;
     double tempLoadedTime = 0.0;
     long requestLength;
     long replyLength;
     bool serverClose = false;
     int maxNodes = 0;
     int tempEnergyAware;
     bool energyAware;
     maxNodes = par("nodes");


	 for(int k = 0; k < jobRequests; k++)
	 {
		 tempNodes = intuniform(1,maxNodes);
		 tempCores = par("cores");
		 tempMemory = par("memory");
		 tempRunTime = par("runTime");
		 tempLoad = par("load");
		 tempLoadedTime = par("loadedTime");
		 tempInputData = par("inputData");
		 tempEnergyAware = par("energyAware");
		 if(tempEnergyAware == 0)
		 {
			 energyAware = false;

		 }
		 else if(tempEnergyAware == 1)
		 {
			 energyAware = true;

		 }
		 else
		 {
			 error("energyAware parameter has to be between 0 and 1");
		 }

	     //The number of cores can be either 1, 2 or 4
	     if(tempCores == 3)
	     {
	    	 tempCores = 4;
	     }
		 requestLength = par("requestLength");
		 replyLength = par("replyLength");

	     if(tempLoadedTime < 0.05 || tempLoadedTime > 1)
	     {
	    	 error("The loaded time parameter must be between 0.05 and 1.");
	     }

	     if (requestLength<1) requestLength=1;
	     if (replyLength<1) replyLength=1;

	     // Based on TCPGenericCliAppBase
	     int numBytes = requestLength;
	     int expectedReplyBytes = replyLength;

	     //std::cerr << "Job number: " << k << " Nodes: " << tempNodes << " cores: " << tempCores  << " runTime: " << tempRunTime << endl;

		 // Create a HPC application request message
		 SuperComMsg *rmsg = new SuperComMsg("request");
		 rmsg->setType(MSGKIND_REQUEST);

		 //TODO: size of the request message?
		 rmsg->setByteLength(numBytes);
		 rmsg->setExpectedReplyLength(expectedReplyBytes);
		 rmsg->setServerClose(serverClose);

		 // HPC app parameters
		 rmsg->setInputData(tempInputData);
		 rmsg->setCores(tempCores);
		 rmsg->setNodes(tempNodes);
		 rmsg->setMemory(tempMemory);
		 rmsg->setRunTime(tempRunTime);
		 rmsg->setLoad(tempLoad);
		 rmsg->setNetworkLoad(networkLoad);
		 rmsg->setLoadedTime(tempLoadedTime);
		 rmsg->setEnergyAware(energyAware);
		 jobQueue.insert(rmsg);


	 }

}

void SuperComClientApp::initializeJobs()
{
		ifstream input("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/workload.txt");
	     if(!input)
	     {
	    	error("Cannot open file");

	     }

	     int numb;
	     std::string line;
	     std::string obj = " ";
	     int n = 0;
	     std::string substring;
	     std::string prevstring;
	     std::string nextstring;

	     int jobNumber = 0;
	     int subTime = 0;
	     int wallTime = 0;
	     int procs = 0;
	     int usedMemory = 0;

	     while (getline(input, line))
	     {
	    	 n = 0;
	    	 jobNumber++;
			 std::string ::size_type pos = 0;
			 subTime = 0;
			 wallTime = 0;
			 procs = 0;
			 usedMemory = 0;
			 //std::string ::size_type space = 0;
			 //space = line.find_last_not_of( obj, pos );

			 //EV << "JobNumber: " << jobNumber << endl;
			 //EV << "Last space: " << space << endl;


			 while( (pos = line.find_first_not_of( obj, pos ))!= std::string::npos )
			 {
				substring = line.substr(pos);
				if(pos!= 0)
					prevstring = line.substr(pos-1, 1);

				//EV << "Position 1: " << pos << endl;
				numb = atoi(substring.c_str());
				//EV << numb << endl;

				if(prevstring.compare(" ") == 0)
				{
					n++;

					switch(n)
					{
						case 2:
						{
							//EV << "Submission time: " << numb << endl;
							subTime = numb;
							break;
						}
						case 4:
						{
							//EV << "Wall time: " << numb << endl;
							wallTime = numb;
							break;
						}
						case 5:
						{
							//EV << "Number of allocated processors: " << numb << endl;
							procs = numb;
							break;
						}
						case 7:
						{
							//EV << "Used memory: " << numb << endl;
							usedMemory = numb;
							break;
						}
					}

				}

				substring.clear();
				++pos;
			 }
		     int memory = 0;
		     int load = 0;
		     double loadedTime = 0;
		     long inputData = 0;

		     memory = getMemoryFromFile(jobNumber);
	         load = getLoadFromFile(jobNumber);
	         loadedTime = getLoadedTimeFromFile(jobNumber);
	         inputData = getInputDataFromFile(jobNumber);

			 // Create a HPC application request message
			 SuperComMsg *rmsg = new SuperComMsg("request");
			 rmsg->setType(MSGKIND_REQUEST);

			 // HPC app parameters
			 rmsg->setInputData(inputData);
			 rmsg->setCores(procs);
			 rmsg->setNodes(procs);
			 rmsg->setMemory(memory);
			 rmsg->setRunTime(wallTime);
			 rmsg->setLoad(load);
			 rmsg->setNetworkLoad(networkLoad);
			 rmsg->setLoadedTime(loadedTime);
			 rmsg->setPriority(priority);
			 rmsg->setJobID(jobNumber);
			 rmsg->setSubmitTime(subTime);
			 rmsg->setClientAddress(socket.getLocalAddress());
			 jobQueue.insert(rmsg);

	     }

	     input.close();
}

void SuperComClientApp::writeData()
{
	std::cerr << "Writing data to file.. " << endl;

	 ofstream tfile("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/workload.txt");

	 int tempNodes = 0;
	 int tempCores = 0;
	 long tempInputData = 0;
     int tempMemory = 0;
     int tempRunTime = 0;
     int tempLoad = 0;
     double tempLoadedTime = 0.0;


	 for(int k = 0; k<5; k++)
	 {
		 tempNodes = intuniform(1, 32);
		 tempCores = par("cores");
		 tempMemory = par("memory");
		 tempRunTime = par("runTime");
		 tempLoad = par("load");
		 tempLoadedTime = par("loadedTime");
		 tempInputData = par("inputData");

	     //The number of cores can be either 1, 2 or 4
	     if(tempCores == 3)
	     {
	    	 tempCores = 4;
	     }

		 tfile <<  tempNodes << "|";
		 tfile <<  tempCores << "|";
		 tfile <<  tempMemory << "|";
		 tfile <<  tempRunTime << "|";
		 tfile <<  tempLoad << "|";
		 tfile <<  tempLoadedTime << "|";
		 tfile <<  tempInputData << "|";
		 tfile << "\n" << endl;
	 }
	 tfile.close();


//	ofstream outmemory("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/memory.txt", ios_base::out | ios_base::binary);
//	ofstream outload("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/load.txt", ios_base::out | ios_base::binary);
//	ofstream outloadtime("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/loadedtime.txt", ios_base::out | ios_base::binary);
//	ofstream outinput("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/inputdata.txt", ios_base::out | ios_base::binary);
//
//
//	if (!outmemory || !outload || !outloadtime || !outinput)
//	//if(!outmemory)
//	{
//	   error("Can't open output file.");
//	}
//
//	int i; // loop index
//	int numberOfJobs = 5; //Number of jobs in the workload
//
//	int temp_memory[numberOfJobs]; // list of output values
//	int temp_load[numberOfJobs]; // list of output values
//	double temp_loadtime[numberOfJobs]; // list of output values
//	long temp_inputdata[numberOfJobs]; // list of output values
//
//	for (i = 0; i < numberOfJobs; ++i)
//	{
//		temp_memory[i] = par("memory");
//		temp_load[i] = par("load");
//		temp_loadtime[i] = par("loadedTime");
//		temp_inputdata[i] = par("inputData");
//
//		outmemory << temp_memory[i] << endl;
//		outload << temp_load[i] << endl;
//		outloadtime <<  temp_loadtime[i] << endl;
//		outinput << temp_inputdata[i] << endl;
//	}
//
//
//	outmemory.close();
//	outload.close();
//	outloadtime.close();
//	outinput.close();

}

/**
 * Sends a request to the RMS after receiving a response
 *
 */

void SuperComClientApp::sendRequest()
{
	 BEV << "Submitting a job to the RMS.." << endl;

//     long requestLength = par("requestLength");
//     long replyLength = par("replyLength");
//     bool serverClose = false;
//
//     // Parameters for a request message
//     // NOTE: these parameters have to be in this function
//     // They cannot be in the initialize function --> not volatile then
//     inputData = par("inputData");
//     cores = par("cores");
//
//     //The number of cores can be either 1, 2 or 4
//     if(cores == 3)
//     {
//    	 cores = 4;
//     }
//     priority = par("priority");
//
//     //Interactive jobs require only 1-2 nodes
//     if(priority == 0)
//     {
//    	 nodes = intuniform(1,2);
//     }
//     else
//     {
//    	 int maxNodes = 0;
//    	 maxNodes = par("nodes");
//    	 nodes = intuniform(1,maxNodes);
//     }
//
//
//     memory = par("memory");
//     runTime = par("runTime");
//     load = par("load");
//     networkLoad = par("networkLoad");
//     loadedTime = par("loadedTime");
//
//
//     if(loadedTime < 0.05 || loadedTime > 1)
//     {
//    	 error("The loaded time parameter must be between 0.05 and 1.");
//     }
//
//     if (requestLength<1) requestLength=1;
//     if (replyLength<1) replyLength=1;
//
//     // Based on TCPGenericCliAppBase
//     int numBytes = requestLength;
//     int expectedReplyBytes = replyLength;
//
//
//     BEV << "sending " << numBytes << " bytes, expecting " << expectedReplyBytes << endl;
//
//     BEV << "inputData length: " << inputData << endl;
//
//
//	 // Create a HPC application request message
//     //GenericAppMsg *msg = new GenericAppMsg("data");
//	 SuperComMsg *msg = new SuperComMsg("request");
//	 msg->setType(MSGKIND_REQUEST);
//
//	 //TODO: size of the request message?
//	 msg->setByteLength(numBytes);
//	 msg->setExpectedReplyLength(expectedReplyBytes);
//	 msg->setServerClose(serverClose);
//
//	 // HPC app parameters
//	 // RMS makes a decision based on these
//	 msg->setInputData(inputData);
//	 msg->setCores(cores);
//	 msg->setNodes(nodes);
//	 msg->setMemory(memory);
//	 msg->setRunTime(runTime);
//	 msg->setLoad(load);
//	 msg->setNetworkLoad(networkLoad);
//	 msg->setLoadedTime(loadedTime);
//	 msg->setPriority(priority);

	 //Get the job message from queue
	 cMessage *msg = (cMessage *)jobQueue.pop();
	 SuperComMsg *jobMsg = dynamic_cast<SuperComMsg*>(msg);

	 int numBytes = jobMsg->getByteLength();
	 inputData = jobMsg->getInputData();

	 BEV << "sending " << numBytes << " bytes." << endl;

	 BEV << "inputData length: " << inputData << endl;

	 // Send the actual message
	 socket.send(jobMsg);

	 //Set the submission time
	 subTime = simTime();

	 // Send the actual message
	// socket.send(msg);

	 // Default handling
	 packetsSent++;
	 bytesSent+=numBytes;

	 stats->addRequestsSent();
	 sentRequests++;

}

int SuperComClientApp::getMemoryFromFile(int jobNumber)
{
	ifstream inmemory("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/memory.txt");
	int i;
	int memory = 0;
	int index = 0;

	if(!inmemory)
		error("File cannot be opened");

    while (inmemory >> i) {
    	index++;
    	if(index == jobNumber)
    	{
    		memory = i;
    		break;
    	}
    }
    inmemory.close();

    return memory;

}

int SuperComClientApp::getLoadFromFile(int jobNumber)
{
	//int memory = 0;
	ifstream inload("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/load.txt");
	int i;
	int load = 0;
	int index = 0;

	if(!inload)
		error("File cannot be opened");

    while (inload >> i) {
    	index++;
    	if(index == jobNumber)
    	{
    		load = i;
    		break;
    	}
    }
    inload.close();

    return load;

}

double SuperComClientApp::getLoadedTimeFromFile(int jobNumber)
{
	//int memory = 0;
	ifstream inloadedtime("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/loadedtime.txt");
	double i;
	double loadedtime = 0;
	int index = 0;

	if(!inloadedtime)
		error("File cannot be opened");


    while (inloadedtime >> i) {
    	index++;
    	if(index == jobNumber)
    	{
    		loadedtime = i;
    		break;
    	}
    }
    inloadedtime.close();

    return loadedtime;

}

long SuperComClientApp::getInputDataFromFile(int jobNumber)
{
	ifstream inputDataFile("/home/wimax/F4G/inetmanet-inetmanet-00f64c2/src/applications/supercom/inputdata.txt");
	int i;
	long inputData = 0;
	int index = 0;

	if(!inputDataFile)
		error("File cannot be opened");

    while (inputDataFile >> i) {
    	index++;
    	if(index == jobNumber)
    	{
    		inputData = i;
    		break;
    	}
    }
    inputDataFile.close();

    return inputData;

}

/**
 * This function sends input data
 * to the RMS after receiving a response
 *
 */

void SuperComClientApp::sendInputData()
{

	 int numBytes = inputData;

	 SuperComMsg *inputMsg = new SuperComMsg("inputData");
	 inputMsg->setType(MSGKIND_INPUT);
	 inputMsg->setByteLength(numBytes);
	 inputMsg->setClientAddress(socket.getLocalAddress());

	 BEV << "Sending " << numBytes << " bytes of input data to the RMS." << endl;
	 //std::cerr << "Sending input data in client. Simtime: " << simTime() << endl;

	 BEV << "inputData length: " << inputData << endl;

	 // Send the actual message
	 socket.send(inputMsg);

	 // Default handling
	 packetsSent++;
	 bytesSent+=numBytes;

}

//void SuperComClientApp::sendMessage(cMessage* msg)
//{
//
//	cPacket* mmsg = dynamic_cast<cPacket*>(msg);
//	mmsg->setTimestamp();
//
//	cPacket* wrapper = new cPacket(msg->getName(), TCP_I_DATA);
//	wrapper->setName(msg->getName());
//	wrapper->setKind(TCP_I_DATA);
//	wrapper->encapsulate(mmsg);
//	socket.send(wrapper);
//}

/**
 * Called each time a new message is received.
 */
void SuperComClientApp::handleMessage(cMessage* msg)
{
	if(msg->isSelfMessage())
	{
		handleTimer(msg);
	}
	// If the RMS is the one initiating the connection
	else if (msg->getKind() == TCP_I_ESTABLISHED && requestSent == true)
	{
		socket = TCPSocket::TCPSocket(msg); // Create a new socket object for the connection
		socket.setOutputGate(gate("tcpOut"));
		socket.setCallbackObject(this);
		socket.processMessage(msg);
	}
	else
	{
		socket.processMessage(msg);
	}
}

// This function handles self messages
void SuperComClientApp::handleTimer(cMessage *msg)
{
    switch (msg->getKind())
    {
        case MSGKIND_CONNECT:
            BEV << "starting session\n";
            TCPGenericCliAppBase::connect(); // active OPEN

            // significance of earlySend: if true, data will be sent already
            // in the ACK of SYN, otherwise only in a separate packet (but still
            // immediately)
            if (earlySend)
                sendRequest();
            break;

        case MSGKIND_SEND: // Redundant..
           sendRequest();
           numRequestsToSend--;
           // no scheduleAt(): next request will be sent when reply to this one
           // arrives (see socketDataArrived())
           break;
    }
}

void SuperComClientApp::socketEstablished(int connId, void *ptr)
{

	TCPGenericCliAppBase::socketEstablished(connId, ptr);


	// logging
	BEV << "Connected to the RMS." << endl;

	//NOTE: If a request has been sent (requestSent == true), the client is being contacted by the RMS
	//and does not need to do anything
	if(requestSent == false)
	{
		// Start sending a request
		sendRequest();
	}

//	else
//	{
//		if(federated == true && contactRMS == true)
//		{
//			sendInputData();
//			contactRMS = false;
//		}
//	}


//    // determine number of requests in this session
//    numRequestsToSend = (long) par("numRequestsPerSession");
//    if (numRequestsToSend<1) numRequestsToSend=1;
//
//    // perform first request if not already done (next one will be sent when reply arrives)
//    if (!earlySend)
//        sendRequest();
//    numRequestsToSend--;

}

void SuperComClientApp::socketDataArrived(int connId, void *ptr, cPacket *msg, bool urgent)
{

	// Default handling
	packetsRcvd++;
	bytesRcvd += msg->getByteLength();

	SuperComMsg *rmsg = dynamic_cast<SuperComMsg*>(msg);

    switch(rmsg->getType())
    {
		case MSGKIND_RESPONSE:
		{
			BEV << "Received a response from the RMS." <<  endl;
			requestSent = true; // request has been sent


			if(federated == true)
			{

				//Close the connection with USO
				TCPGenericCliAppBase::close();

				IPvXAddress rmsAddr = rmsg->getRmsAddress();
				setRMSAddress(rmsAddr);
				contactRMS = true;

			}
			else
			{
				// Start sending input data
				sendInputData(); //TODO: Should we generate a self-message for this also?
			}


			delete rmsg;
			break;

		}
		case MSGKIND_FINISH:
		{
			BEV << "Job finished, closing connection." << endl;
			BEV << "Amount of output data received:  " << rmsg->getByteLength()<< endl;
			//std::cerr << "Output data received in client. Simtime: " << simTime() << endl;
			TCPGenericCliAppBase::close(); // Default handling
			requestSent = false; // set as false for next request
			stats->addJobsCompleted();

			//Get the job turnaround time
			compTime = simTime();
			double turnAroundTime = 0.0;
			turnAroundTime = SIMTIME_DBL(compTime - subTime);
			stats->addTurnAroundTime(turnAroundTime);

			BEV << "amount of job requests: " << jobRequests << endl;
			BEV << "sent requests: " << sentRequests << endl;

			// start another session after a delay
			if(sentRequests < jobRequests)
			{
				connectMsg->setKind(MSGKIND_CONNECT);
				scheduleAt(simTime()+(simtime_t)par("idleInterval"), connectMsg);
				BEV << "Starting another request at: " << simTime()+(simtime_t)par("idleInterval")  << endl;
			}
			else if(sentRequests == jobRequests)
			{
				BEV << "Finished sending " << jobRequests << " job requests."  << endl;
			}

			delete rmsg;
			break;
		}
		default:
		{
			//delete rmsg;
			break;
		}

    }

//    if (numRequestsToSend>0)
//    {
//        BEV << "reply arrived\n";
//        connectMsg->setKind(MSGKIND_SEND);
//        scheduleAt(simTime()+(simtime_t)par("thinkTime"), connectMsg);
//    }

}

void SuperComClientApp::socketClosed(int connId, void *ptr)
{
    TCPGenericCliAppBase::socketClosed(connId, ptr);




    // start another session after a delay
    //connectMsg->setKind(MSGKIND_CONNECT);
    //scheduleAt(simTime()+(simtime_t)par("idleInterval"), connectMsg);
}

void SuperComClientApp::socketPeerClosed(int connId, void *ptr)
{

	TCPGenericCliAppBase::socketPeerClosed(connId, ptr);
//    if(contactRMS == true)
//    {
//    	BEV << "Contacting the RMS for sending input data." << endl;
//		//Open another connection to the selected RMS
//		// we need a new connId if this is not the first connection
//		socket.renewSocket();
//
//		// connect
//		int connectPort = par("connectPort");
//
//		EV << "issuing OPEN command\n";
//		setStatusString("connecting");
//
//		socket.connect(getRMSAddress(), connectPort);
//		contactRMS = false;
//		//numSessions++;
//    }

}

void SuperComClientApp::socketFailure(int connId, void *ptr, int code)
{
    TCPGenericCliAppBase::socketFailure(connId, ptr, code);

    // reconnect after a delay
    connectMsg->setKind(MSGKIND_CONNECT);
    scheduleAt(simTime()+(simtime_t)par("reconnectInterval"), connectMsg);
}

void SuperComClientApp::setRMSAddress(IPvXAddress rmsAddr)
{
	rmsAddr_var = rmsAddr;
}

IPvXAddress SuperComClientApp::getRMSAddress()
{
	return rmsAddr_var;
}



