#include "TCPSinkAppQ.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include "TCPScheduledSessionApp.h"


#define MK_PLAY_CONTENT 10000 //ZY 25.10.2011
#define MK_PAUSE_CONTENT 10001 //ZY 25.10.2011
#define MK_BUFFER_STATUS_MSG 10002 //ZY 16.07.2012
#define MK_START_AGENT_MSG 10003 //OM 07.02.2013
#define MK_FINISH_AGENT_MSG 10004 //OM


Define_Module(TCPSinkAppQ);

void TCPSinkAppQ::initialize()
{
	//Signal creation
	totalBytesReceived = registerSignal("Total_Bytes_Received"); //registering a signal to be emitted and listened by the remote server


	pauseEvent = false;
	restart = false;

	const char *address = par("address");
    port = par("port");
    playRate = par("playRate");
    pauseRate = par("pauseRate");
    initAdvBufferSize = par("initAdvBufferSize");
    minAdvBufferSize = par("minAdvBufferSize");
    //specifying playBuffer status informations
    threshYO = par("threshYO");
    threshGY = par("threshGY");

    codecRate = par("codecRate"); //OM
    dynamicPlayRate = par("dynamicPlayRate"); //OM

    playBuffer.setName("Playout_Buffer"); //cQueue used as a playout buffer

    userClass = par("userClass"); // OM 12.02.2013
    contentDelivered = false; //OM
    numberOfBufferStarvations = 0; //OM
    bufferSizeVector.clear();
    packetSizeVector.clear();
    starvationDurationVector.clear();
    starvationStart = 0; //OM
    starvationFrequencyVector.clear();
    starvationFreqStart = 0; //OM
    playTimeStart = playTime = 0.0; //OM

    scheduleRestart = par("scheduleRestart");

    contentServerAddress = par("contentServerAddress"); // OM: name of the content server

    bytesToReceive = par("bytesToReceive"); //OM

    // Statistics OM
	modp = simulation.getModuleByPath("FairTCP.stats");
	stats = check_and_cast<StatisticsCollector *>(modp);

    WATCH(pauseEvent);

    bytesRcvd = 0;
    WATCH(bytesRcvd);

    playBufferSize = 0; //ZY 25.10.2011
    WATCH(playBufferSize); //ZY 25.10.2011

    segmentsRcvd = 0;
    WATCH(segmentsRcvd);
    WATCH(initAdvBufferSize);
    WATCH(minAdvBufferSize);
    WATCH(threshYO);
    WATCH(threshGY);
	WATCH_PTRVECTOR(playerControl);

	currentBufferState = 0; //initializing the current playBufferState to 0 (= RED i.e., starved buffer )
	WATCH(currentBufferState); //to visually keep track of the playBufferState.


    //Setting up of Statistics vectors
    segmentsReceivedVec.setName("Total_Segments_Rcvd");
    playBufferSizeVec.setName("Play_Buffer_Size");
    playBufferSizeByteVec.setName("Play_Buffer_Size_Bytes");
    playBreakVec.setName("Play_Break");
    packetSizeVec.setName("Packet size");
    playTimeRemainingVec.setName("PlayTime_Remaining");

    //get pointer to teh FS4VP module in order to send teh buffer stats (16.07.2012)
//        cModule *sysmod = getParentModule()->getParentModule();
//
//         	EV<<"The system module is :"<< sysmod->getFullName()<<endl;
//           EV<<"The Server "<< sysmod->findSubmodule("DF")<<" has Id: "<< sysmod->getSubmodule("DF")<<endl;
//           EV<<"The Server contains "<<sysmod->getSubmodule("DF")->getSubmodule("fs4VP")<<" with id: "<<sysmod->getSubmodule("DF")->findSubmodule("fs4VP")<<endl;


    fs4vp = getParentModule()->getParentModule()->getSubmodule(contentServerAddress)->getSubmodule("fs4VP"); //acquired pointer to the FS4Vp module in teh server for sendDirect() msgs.


    TCPSocket socket;
    socket.setOutputGate(gate("tcpOut"));
    socket.bind(address[0] ? IPvXAddress(address) : IPvXAddress(), port);
    socket.listen();
}

void TCPSinkAppQ::handleMessage(cMessage *msg)
{
    if (msg->getKind()==TCP_I_PEER_CLOSED)
    {
        // we close too
        msg->setKind(TCP_C_CLOSE);
        send(msg, "tcpOut");
    }
    else if (msg->getKind()==TCP_I_DATA || msg->getKind()==TCP_I_URGENT_DATA)
    {
        bytesRcvd += PK(msg)->getByteLength();

//    	if(PK(msg)->getByteLength() > 1452)
//    	{
//    		std::cout << getParentModule()->getName() << endl;
//    		std::cout << simTime() << endl;
//			std::cout << "packet size: " << PK(msg)->getByteLength() << endl;
//			double play_rate = (double)PK(msg)->getBitLength() / codecRate;
//			std::cout << "play_rate: " << play_rate << endl;
//    	}

        playBuffer.insert(msg); //inserting the arriving content chunk in the playbuffer
        playBufferSize++; //incrementing the size stat
        segmentsRcvd++;
        segmentsReceivedVec.record(segmentsRcvd);
        playBufferSizeByteVec.record(bytesRcvd);
        determinePlayBufferState(playBufferSize);

        //std::cerr << "Size of data: " << PK(msg)->getByteLength() << endl;

        //========= Experimenting with Signals START======================================
//        if (mayHaveListeners(totalBytesReceived))
//        {
//        	EV<<"Listeners are subscribed -- emitting signal"<<endl;
//        	//emit(totalBytesReceived, playBufferSize); //will emit the signal (intended listener is FS4VP module in the server)
//        }
//        else
//        	EV<<"No Listeners -- Hence signal not emitted!!"<<endl;
        //========= Experimenting with Signals END ======================================

        //as soon as the play buffer gets filled with initial number of advance packets, the app should start playing it
            if(segmentsRcvd == initAdvBufferSize)
            {
            	EV<<"### CREATING Play Timer ###"<<endl;
            	createPlayTimer(); //create the self-msg (timer) that will periodically trigger the app to pop out packets from teh playbuffer
            	std::cerr << "Client: " << getParentModule()->getName() << " Creating play timer: " << simTime() << ", segments rcvd: " << segmentsRcvd << endl;
            	starvationFreqStart = simTime();
            	playTimeStart = simTime();
            	contentDelivered = false;
            }

        //delete msg;

        if (ev.isGUI())
        {
            char buf[32];
            sprintf(buf, "rcvd: %ld bytes", bytesRcvd);
            getDisplayString().setTagArg("t",0,buf);
        }
    }
    else if(msg->isSelfMessage())
    {
    	if(msg->getKind()==MK_PLAY_CONTENT)
			{
				EV<<"Play Timer Message Received !"<<endl;
				playContent();
				if(!pauseEvent && contentDelivered == false) //only schedule the message if the PAUSE event is FALSE
					scheduleAt(simTime()+playRate, msg);

			 }
    	if(msg->getKind()==MK_PAUSE_CONTENT)
    	    {
    	       	EV<<"Pause Timer Message Received !"<<endl;
    	       	pauseContent();
    	       	if(pauseEvent) //only schedule the message if PAUSE event is TRUE
    	       		scheduleAt(simTime()+pauseRate, msg);
    	     }
    }

    else
    {

        // must be data or some kind of indication -- can be dropped
    	EV<<"Unknown Message, hence dropping without further processing";
        delete msg;
    }
}

void TCPSinkAppQ::sendStats()
{


	if(playTime == 0)
		playTime = simTime()- playTimeStart;

	double avgFreq = (double)numberOfBufferStarvations / playTime;
	// Average buffer starvation duration
	double sumDuration = 0.0;
	double avgDuration = 0.0;

	for(int i = 0; i < (int) starvationDurationVector.size(); i++)
	{
		sumDuration = sumDuration + starvationDurationVector[i];
	}

	if(sumDuration != 0)
	{
		avgDuration = sumDuration/(starvationDurationVector.size());
	}
	else
	{
		avgDuration = 0;
	}

	stats->recordClientStats(avgFreq, avgDuration, numberOfBufferStarvations, userClass);
}


/*
 ** This function creates a timer for enabling PLAY event
 */
void TCPSinkAppQ::createPlayTimer()
{

		cMessage *playTimer = new cMessage("playContent", MK_PLAY_CONTENT);
		playerControl.push_back(playTimer);
		EV<<"Timer "<<playTimer->getName()<<" of type "<<playTimer->getKind()<<" created at time "<<simTime()<<endl;
		EV<<"Scheduling the Play_Timer msg to trigger at"<<simTime()+playRate<<endl;
		scheduleAt(simTime()+playRate, playTimer);

		// OM add: generate a cMessage for starting the agent
		cMessage *startAgent = new cMessage("startAgent", MK_START_AGENT_MSG);
		EV <<"Informing the FS4VP module that client is creating a playTimer. " << endl;
		cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
		cliAppContext -> setPortId(port);
		cliAppContext->setUserClass(userClass); // Set user class
		startAgent->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
		sendDirect(startAgent, fs4vp, "directInput"); //sending info to the FS4VP module

		return;
}

/*
 ** This function creates a timer for enabling PAUSE event
 */
void TCPSinkAppQ::createPauseTimer()
{
	EV<<"Creating timer for the PAUSE event"<<endl;
	cMessage *pauseTimer = new cMessage("pauseContent",MK_PAUSE_CONTENT); //creating a PAUSE_CONTENT_TIMER
	playerControl.push_back(pauseTimer); // Pushing the timer message into the PlayerControl vector
	EV<<"Timer "<<pauseTimer->getName()<<" of type "<<pauseTimer->getKind()<<" created at time "<<simTime()<<endl;
	EV<<"Scheduling the Pause_Timer msg to trigger at"<<simTime()+pauseRate<<endl;
	scheduleAt(simTime()+pauseRate, pauseTimer); //initiating the timer for the PAUSE event
	//return;
}

/*
 ** This function models the PAUSE event of a video player.
 ** The function will PAUSE till the play buffer fills up
 ** to the minimum number of content chunks
 */
void TCPSinkAppQ::pauseContent()
{
	EV <<"Routine for PAUSE content"<<endl;

	//PAUSE event condition check
	if(playBuffer.length() < minAdvBufferSize && (bytesRcvd < bytesToReceive))
	{
		EV<<"Play Buffer still below minAdvBufferSize of: "<<minAdvBufferSize<<endl;
		//re-scheduling the pauseTimer
//		for(int x=0; x < playerControl.size(); x++)
//			{
//				EV<<"Iter # "<< x <<": PlayControl Vector has timer message:" <<playerControl[x]->getKind()<<endl;
//				if(playerControl[x]->getKind()== MK_PAUSE_CONTENT)
//			  			scheduleAt(simTime()+pauseRate, playerControl[x]);
//			}
	}
	else
	{
		if(bytesRcvd < bytesToReceive)
			ASSERT(playBuffer.length() >= minAdvBufferSize);

		//Buffer has reached the minAdvBufferSize threshold
		//1- cancel the timer for the PAUSE event
		EV<<"Canceling the PAUSE event !!"<<endl;// <<playerControl[x]->getKind()<<endl;
		pauseEvent = false; //seeting the pause event to false
		playBreakVec.record(1.0); //statistics
		double starvationDuration = 0.0;
		starvationDuration = SIMTIME_DBL(simTime() - starvationStart);
		starvationDurationVector.insert(starvationDurationVector.end(), starvationDuration);

		// Record duration to statistics module
		stats->recordStarvationDuration(starvationDuration, userClass);


//		for(int x=0; x < playerControl.size(); x++)
//			{
//				if(playerControl[x]->getKind()== MK_PAUSE_CONTENT)
//			  			cancelEvent(playerControl[x]);
//			}
		//2- re-schedule the timer for the PLAY event
		EV<<"Immediately initiating the PLAY event !!"<<endl; //:" <<playerControl[x]->getKind()<<endl;
		createPlayTimer();
//		for(int x=0; x < playerControl.size(); x++)
//					{
//						if(playerControl[x]->getKind()== MK_PLAY_CONTENT)
//				  			scheduleAt(simTime(), playerControl[x]); //Immidiately start the PLAY event
//					}
	}
	return;
}

void TCPSinkAppQ::determinePlayBufferState(int _playBufferSize)
{
	EV<<"Routine for determining PalyBuffer State!"<<endl;
	EV<<"Current Size of Play Buffer is: "<<_playBufferSize<<endl;

	//std::cerr <<"Current Size of Play Buffer is: "<<_playBufferSize<<endl;

	if(_playBufferSize < threshYO && _playBufferSize !=0)
	{
		playBufferState = ORANGE;
		//currentBufferState = playBufferState; //set the currentBufferState to teh latest state
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);

//		std::cerr << getParentModule()->getName() << " " << simTime() << endl;
//		std::cerr << "PLay buffer state is ORANGE." << endl;
	}
	//OM: added >= and <=
	else if(_playBufferSize >= threshYO && _playBufferSize <= threshGY)
	{
		playBufferState = YELLOW;
		//currentBufferState = playBufferState; //set the currentBufferState to teh latest state
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);
//		std::cerr << getParentModule()->getName() << " " << simTime() << endl;
//		std::cerr << "PLay buffer state is YELLOW." << endl;
	}

	else if (_playBufferSize > threshGY)
	{
		playBufferState = GREEN;
		//currentBufferState = playBufferState; //set the currentBufferState to teh latest state
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);
//		std::cerr << getParentModule()->getName() << " " << simTime() << endl;
//		std::cerr << "PLay buffer state is GREEN." << endl;
	}

	else if (_playBufferSize < minAdvBufferSize || _playBufferSize == 0) //we could use the minAdvBufferSize as an criticial alarm that the system is about to get starved
	{
		playBufferState = RED; //i.e., buffer starvation
		EV<<"Play Buffer State is: "<< playBufferState<<endl;
		verifyBufferStateTransition(playBufferState);

//		std::cerr << getParentModule()->getName() << " " << simTime() << endl;
//		std::cerr << "PLay buffer state is RED." << endl;
	}

	return;
}

void TCPSinkAppQ::verifyBufferStateTransition(int _playBufferState)
{
	EV<<"Verifying Buffer State Transition and trigger generation !"<<endl;

	if(_playBufferState == currentBufferState) //check if there has been a state change (or state transition)
		EV<<"No change in State of PlayBuffer"<<endl;
	else
	{


		EV<<"State Transition from PREV_STATE:"<<currentBufferState<<" to CURR_STATE:"<<playBufferState <<endl;
		//========= Generating StateTransition trigger towards the FS4VP ===================================
		//OM: Send only to fsvp if all content has not been delivered yet.
		if(contentDelivered == false)
		{
			cMessage *bufferStatus = new cMessage("playBufferStatus", MK_BUFFER_STATUS_MSG);
			cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
			cliAppContext -> setPortId(port);
			cliAppContext -> setPlayBufferSize(segmentsRcvd); //TODO: this should reflect the current size of the buffer
			cliAppContext -> setPlayBufferState(_playBufferState);
			bufferStatus->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
			//bufferStatus-> addPar("bytesRxed").setLongValue(segmentsRcvd);
			EV<<"Sending PlayBuffer State Transition Trigger Notification !!"<<endl;
			sendDirect(bufferStatus, fs4vp, "directInput"); //sending buffer status to the FS4VP module
		}
		else if(restart == true && scheduleRestart == true)
		{
			cMessage *bufferStatus = new cMessage("playBufferStatus", MK_BUFFER_STATUS_MSG);
			cliAppContext = new CliAppContext(); //creating an object for carrying the client application context
			cliAppContext -> setPortId(port);
			cliAppContext -> setPlayBufferSize(segmentsRcvd); //TODO: this should reflect the current size of the buffer
			cliAppContext -> setPlayBufferState(_playBufferState);
			bufferStatus->setControlInfo(cliAppContext); //setting the control info carrying teh context info of the client application
			//bufferStatus-> addPar("bytesRxed").setLongValue(segmentsRcvd);
			EV<<"Sending PlayBuffer State Transition Trigger Notification !!"<<endl;
			sendDirect(bufferStatus, fs4vp, "directInput"); //sending buffer status to the FS4VP module
		}
		currentBufferState = playBufferState; //set the currentBufferState to teh latest state
	}
	return;
}

/*
 ** This function models the playing of a progressively video downloaded content
 */
void TCPSinkAppQ::playContent()

{
	EV<<"Start PLAY at time "<<simTime()<<" and creating the timer\n"<<endl;;
	if(!playBuffer.isEmpty())// && (playBuffer.length() > minAdvBufferSize))
	{
    	EV <<"Playing the Content - Popping video packet from the playBuffer)";
//	    	playBufferSizeVec.record(playBuffer.getLength());
    	cPacket *pkt = PK(playBuffer.pop()); //returns the pointer to the first element in the queue

    	// Remaining playTime code start //
//    	double bits = 0.0;
//
//    	cQueue* tempBuffer = playBuffer.dup();
//    	int tempSize = tempBuffer->getLength();
//
//    	for(int i = 0; i < tempSize; i++)
//    	{
//    		cPacket *tempPkt = PK(tempBuffer->pop());
//    		bits += (double)tempPkt->getBitLength();
//    		delete tempPkt;
//    	}
//    	double remaining = bits / codecRate;
//    	playTimeRemainingVec.record(remaining);
//
//    	delete tempBuffer;
    	// Remaining playTime code end //



//    	if(pkt->getByteLength() < 1452)
//    	{
//    		std::cout << getParentModule()->getName() << endl;
//    		std::cout << simTime() << endl;
//			std::cout << "packet size: " << pkt->getByteLength() << endl;
//			double play_Rate = (double)pkt->getBitLength() / codecRate;
//			//playRate = (double)pkt->getBitLength() / codecRate;
//			std::cout << "play rate: " << play_Rate << endl;
//    	}
    	//std::cout << getParentModule()->getName() << endl;
    	//std::cout << simTime() << endl;
    	if(dynamicPlayRate == true)
    	{
    		playRate = (double)pkt->getBitLength() / codecRate;
    	}
    	//std::cout << "play rate: " << playRate << endl;
    	packetSizeVector.insert(packetSizeVector.end(), (double)pkt->getByteLength());
    	packetSizeVec.record((double)pkt->getByteLength());

    	EV<<"About to delete packet with pointer value"<< pkt <<" id:"<<pkt->getId()<<"timestamp: "<<pkt->getTimestamp()<<"pointerId: "<<&pkt<<endl;
    	delete pkt;
    	playBufferSize--;
   	 	playBufferSizeVec.record(playBufferSize);

   	 	bufferSizeVector.insert(bufferSizeVector.end(), playBufferSize);

   	 	determinePlayBufferState(playBufferSize); //determine the state of the playbuffer (RED, YELLOW or GREEN)

   	 	//TODO Here check the PlayBufferState and decide which type of trigger to send, if at all needed, to the FS4VP module.
   	 	//TODO calculate new playRate based on packet size!!

   	 	return;
	}
	else if(playBuffer.isEmpty() && (bytesRcvd < bytesToReceive))//if(!playBuffer.isEmpty() && (playBuffer.length() <= minAdvBufferSize))
	{

		EV <<"Buffer Starvation - PAUSE !!"<<endl;
		// OM: Count the number of buffer starvations
		numberOfBufferStarvations++;
		stats->addBufferStarvations(userClass);
		//std::cerr << "Buffer Starvation: " << getParentModule()->getName() << " " << simTime() << endl;
		/*
		 ** 1- Canceling the Play_Content Timer,
		 */
//		for(int x=0; x < playerControl.size(); x++)
//	    	{
//	    		EV<<"Iter # "<< x <<": PlayControl Vector has timer message:" <<playerControl[x]->getKind()<<endl;
//	    		if(playerControl[x]->getKind()== MK_PLAY_CONTENT)
//	    		{
//	    			EV<<"Canceling PLAY event timer "<< playerControl[x]->getName() <<" of type: "<<playerControl[x]->getKind()<<" scheduled at "<<playerControl[x]->getArrivalTime()<<endl;
//	    			if(playerControl[x]->isScheduled())
//	    				EV << "It was scheduled"<<endl;
//	    			if(playerControl[x]->isScheduled())
//	    				if(cancelEvent(playerControl[x]) == playerControl[x])
//	    					EV << "It was really canceled"<<endl;
//	    			//DEBUG:
//	    			ASSERT(playerControl[x]!=0); //to make sure that it is not a NULL pointer
//
//	    			if(playerControl[x]->isScheduled())
//	    				EV<<"PLAY timer is still scheduled at "<<playerControl[x]->getArrivalTime()<<" and is not cancelled"<<endl;
//	    			else
//	    				EV<<"PLAY timer is canceled successfully and is scheduled at "<<playerControl[x]->getArrivalTime()<<endl;
//	    			break;
//	    		}
//	    		EV<<"DEBUG: For-loop break exit"<<endl;
//	    	}
	    	/*
	    	 ** 2 - Schedule the timer for the PAUSE event
	    	 */

            pauseEvent = true;
            playBreakVec.record(0.0); //statistics
            starvationStart = simTime(); // Start time of the starvation


            // Starvation frequency
    		double starvationFrequency = 0.0;
    		starvationFrequency = SIMTIME_DBL(simTime() - starvationFreqStart);
    		starvationFrequencyVector.insert(starvationFrequencyVector.end(), starvationFrequency);
    		starvationFreqStart = simTime();


            createPauseTimer();
	}
	else //all the content has been delivered
	{
		EV<<"Content delivered - canceling and deleting PLAY/PAUSE timers!!"<<endl;
		std::cerr << "Client: " << getParentModule()->getName() << ", content delivered: " << simTime() << endl;
		//TODO add routine to cancelAndDelete() the timers for the PLAY & PAUSE events.

	    // Starvation frequency
		double starvationFrequency = 0.0;
		starvationFrequency = SIMTIME_DBL(simTime() - starvationFreqStart);
		starvationFrequencyVector.insert(starvationFrequencyVector.end(), starvationFrequency);
		starvationFreqStart = simTime();

		playTime = SIMTIME_DBL(simTime() - playTimeStart);

		//double avgFreq = (double)numberOfBufferStarvations / playTime;
		//stats->recordStarvationFrequency(avgFreq, userClass);


		//cancelAndDelete()
		if(contentDelivered == false)
		{

			if(userClass == 0)
			{
				stats->addGoldCompleted();
			}

			stats->addCompleted();

			contentDelivered = true;


			if(scheduleRestart == true)
			{
				//contentDelivered = false;
				bytesRcvd = 0;
				segmentsRcvd = 0;
				restart = true;
				//playBufferSize = 0;
				//playBuffer.clear();
			}

		}
	}
}

void TCPSinkAppQ::finish()
{
	//temp->clear();


	// Average buffer size
	double sum = 0.0;
	double avgbufferSize = 0.0;

	for(int i = 0; i < (int) bufferSizeVector.size(); i++)
	{
		sum = sum + bufferSizeVector[i];
	}

	if(sum != 0)
	{
		avgbufferSize = sum/(bufferSizeVector.size());
	}
	else
	{
		avgbufferSize = 0;
	}
	bufferSizeVector.clear();

	// Average packet size popped from the buffer
	double sumPacket = 0.0;
	double avgPacketSize = 0.0;

	for(int i = 0; i < (int) packetSizeVector.size(); i++)
	{
		sumPacket = sumPacket + packetSizeVector[i];
	}

	if(sumPacket != 0)
	{
		avgPacketSize = sumPacket/(packetSizeVector.size());
	}
	else
	{
		avgPacketSize = 0;
	}
	packetSizeVector.clear();

	// Average buffer starvation duration
	double sumDuration = 0.0;
	double avgDuration = 0.0;

	for(int i = 0; i < (int) starvationDurationVector.size(); i++)
	{
		sumDuration = sumDuration + starvationDurationVector[i];
	}

	if(sumDuration != 0)
	{
		avgDuration = sumDuration/(starvationDurationVector.size());
	}
	else
	{
		avgDuration = 0;
	}

	starvationDurationVector.clear();


	// Average buffer starvation frequency
	//double sumFreq = 0.0;
	double avgFreq = 0.0;

//	for(int i = 0; i < (int) starvationFrequencyVector.size(); i++)
//	{
//		sumFreq = sumFreq + starvationFrequencyVector[i];
//	}
//
//	if(sumFreq != 0)
//	{
//		avgFreq = sumFreq/(starvationFrequencyVector.size());
//	}
//	else
//	{
//		avgFreq = 0;
//	}
//	starvationFrequencyVector.clear();

	if(playTime == 0)
		playTime = simTime() - playTimeStart;

	double QoE_formula = 0.0;

	QoE_formula = sumDuration / SIMTIME_DBL(playTime);



	avgFreq = (double)numberOfBufferStarvations / playTime;

	recordScalar("playTimeStart", playTimeStart);
	recordScalar("avgPlayBufferSize", avgbufferSize);
	recordScalar("avgPacketSize", avgPacketSize);
	recordScalar("avgStarvationDuration", avgDuration);
	recordScalar("avgFrequencyStarvation", avgFreq);
    recordScalar("bytesRcvd", bytesRcvd);
    recordScalar("Number of buffer starvations", numberOfBufferStarvations);
    recordScalar("QoE formula", QoE_formula);
    recordScalar("sumDuration", sumDuration);

	std::string string1 = "BufferStarvations";

	std::string string2;          // string which will contain the result
	std::ostringstream convert;   // stream used for the conversion
	convert << (codecRate/1000000);      // insert the textual representation of 'Number' in the characters in the stream
	string2 = convert.str(); // set 'Result' to the contents of the stream
	std::string string3 = string1 + string2;

	recordScalar(string3.c_str(), numberOfBufferStarvations);



    //GOLD
    if(userClass == 0)
    {
    	recordScalar("Gold Starvations", numberOfBufferStarvations);
    	recordScalar("Gold avgStarvationDuration", avgDuration);
    	recordScalar("Gold avgFrequencyStarvation", avgFreq);
    	recordScalar("Gold playTime", playTime);
    }
    //SILVER
    else if(userClass == 1)
    {
    	recordScalar("Silver Starvations", numberOfBufferStarvations);
    	recordScalar("Silver avgStarvationDuration", avgDuration);
    	recordScalar("Silver avgFrequencyStarvation", avgFreq);
    	recordScalar("Silver playTime", playTime);
    }
    //BRONZE
    else if(userClass == 2)
    {
    	recordScalar("Bronze Starvations", numberOfBufferStarvations);
    	recordScalar("Bronze avgStarvationDuration", avgDuration);
    	recordScalar("Bronze avgFrequencyStarvation", avgFreq);
    	recordScalar("Bronze playTime", playTime);
    }
}

