//
// Copyright 2008, 2009 Vasileios P. Kemerlis (aka vpk)
//                              vpk@cs.columbia.edu
//                              http://www.cs.columbia.edu/~vpk
//

// This file is part of BitTorrent Implementation for OMNeT++.

//    BitTorrent Implementation for OMNeT++ is free software: you can redistribute
//	  it and/or modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the License,
//	  or (at your option) any later version.

//    BitTorrent Implementation for OMNeT++ is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with BitTorrent Implementation for OMNeT++.
//    If not, see <http://www.gnu.org/licenses/>.


#include "BTTrackerMsg_m.h"
#include "BTSubTrackerClientBase.h"
//#include "TCPGenericCliAppBase.h"
#include "IPAddressResolver.h"
//#include "GenericAppMsg_m.h"

#define BEV	EV << "[" << hostModule()->peerId() << "]::[Tracker Client]: "
#define BEV2	EV << "[" << peerId_var << "]::[Tracker Client]: "

Register_Class(BTSubTrackerClientCommunicationBase);
Define_Module(BTSubTrackerClientBase);

/**
 * Constructor.
 */
BTSubTrackerClientBase::BTSubTrackerClientBase()
{
	//evtTout = new cMessage(NULL, EVT_TOUT);

}

/**
 * Destructor.
 */
BTSubTrackerClientBase::~BTSubTrackerClientBase()
{
	// delete any pending events
// 	cancelAndDelete(evtDelThread);
 	cancelAndDelete(evtTrackerComm);
	delete tcp;
}

/**
 * Called after the module creation.
 */
void BTSubTrackerClientBase::initialize()
{
	// parent initialize()
	//TCPGenericCliAppBase::initialize();
	TCPSrvHostApp::initialize();

	// member init
 	//state_var 			= ST_STARTED;
 	//transient_var		= 0;
	trackerId_var 		= "";
	connectGiveUp_var 	= (size_t)par("connectGiveUp");
	sessionTimeout_var	= (simtime_t)par("sessionTimeout");
	reconnectInterval_var 	= (simtime_t)par("reconnectInterval");
	infoHash_var 		= (string)par("infoHash");
	peerPort_var 		= (size_t)par("peerPort");
	compact_var 		= (bool)par("compact");
	noPeerId_var 		= (bool)par("noPeerId");
	numWant_var 		= (size_t)par("numWant");
	key_var				= (string)par("key");
	subtrackerNum 				= (int)par("subtrackerNum");
	announceInterval_var  = (simtime_t)par("announceInterval");

	//The peerID shall be unique for each peer, so it cannot be retrieved
	// from a parameter.
	peerId_var		 = generatePeerID();
	subtrackerIndex = 0;

 	evtTrackerComm = new cMessage("INTERNAL_TRACKER_COM_MSG", INTERNAL_TRACKER_COM_MSG);
// 	evtRemThread		= new cMessage(NULL, EVT_REM_THREAD);

	tcp = new TCP();

	setAnnounceInterval(announceInterval_var);

	// watches
 	//WATCH(state_var);
 	//WATCH(transient_var);
	WATCH(trackerId_var);
	WATCH(ipaddress_var);
	WATCH(connectGiveUp_var);
	WATCH(sessionTimeout_var);
	WATCH(reconnectInterval_var);
	WATCH(infoHash_var);
	WATCH(peerId_var);
	WATCH(peerPort_var);
	WATCH(compact_var);
	WATCH(noPeerId_var);
	WATCH(numWant_var);
	WATCH(key_var);

	scheduleAt(simTime(), evtTrackerComm);
}

/**
 * Called each time a new message is received.
 */
void BTSubTrackerClientBase::handleMessage(cMessage* msg)
{
	//if(msg->isSelfMessage() || msg->arrivedOn("btorrentIn") && msg->kind() == EVT_CONN)
	if((msg->kind() == INTERNAL_TRACKER_COM_MSG))
	{
		handleTimer(msg);
	}
 	else if ((msg->kind() == INTERNAL_REMOVE_THREAD_MSG))
 	{
 		handleThreadMessage(msg);
 	}
	else
	{
		// default handling				
		TCPSrvHostApp::handleMessage(msg);

	}
}

/**
 * Called from handleTimer() each time a self message is received.
 */
void BTSubTrackerClientBase::handleThreadMessage(cMessage *msg)
{
	      
	      TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->contextPointer();
	      BTSubTrackerClientCommunicationBase* handler = (BTSubTrackerClientCommunicationBase*)thread;

	      // different actions for each event
	      switch(msg->kind())
	      {
// 		      case EVT_DEL_THREAD:
// 			      BEV2<<"the connection to the tracker closed. Deleting serving thread."<<endl;
// 			      thread->socket()->abort();
// 			      scheduleAt(simTime(),evtRemThread);			
// 			      delete msg;
// 			      break;
		      case INTERNAL_REMOVE_THREAD_MSG:			
									      
			  //This socket may be removed only if it has closed or failed (due to a timeout e.g. the remote peer exited)
			      if ((thread->socket()->state()!= TCPSocket::CLOSED) && (thread->socket()->state()!= TCPSocket::SOCKERROR))
			      {
				      
				      scheduleAt(simTime()+100, msg);
			      }
			      else
			      {
				      
				      delete msg;
				      BEV2<<"Removing thread..."<<endl;
				      removeThread(thread);
			      }
			      
			      break;
		     default:
			      // report the error
 			      error("%s:%d at %s() unknown event(kind=%d) occured\n", __FILE__, __LINE__, __func__, msg->kind());
	      }
}
// 		      // complete event
// 		      case EVT_COMP:
// 			      // changing state to COMPLETED
// 			      handler->setCtrans(ST_TRANS_COMP);
// 			      //transient_var = ST_TRANS_COMP;
// 			      // this type of message was not self scheduled - dispose it
// 			      delete msg;
// 			      break;
// 
// 		      // stop event
// 		      case EVT_STOP:
// 			      //handler = getHandler();
// 			      handler->setCtrans(ST_TRANS_STOP);
// 			      //transient_var = ST_TRANS_STOP;
// 			      // this type of message was not self scheduled - dispose it
// 			      delete msg;
// 			      break;
// 
// 		      // connect event
// 		      case EVT_CONN:
// 			      // changing state to NORMAL
// 			      //transient_var = ST_TRANS_NORM;
// 			      handler->setCtrans(ST_TRANS_NORM);
// 
// 			      // this type of message was not self scheduled - dispose it
// 			      delete msg;
// 			      break;
// 
// 		      case EVT_DEL_THREAD:
// 			      BEV<<"the connection to the tracker closed. Deleting serving thread."<<endl;
// 			      thread->socket()->abort();
// 			      handler->TCPServerThreadBase::closed();
// 			      delete msg;
// 			      break;
// 
// 		      //session timeout and error events
// 		      case EVT_TOUT:
// 			      // logging
// 			      BEV2 << "session with Tracker[address=" << par("connectAddress") << ", port=" << par("connectPort") << "] expired\n";
// 
// 			      // close the pending connection - if it is still in pending state
// 			      if(thread->socket()->state() == TCPSocket::CONNECTING || thread->socket()->state() == TCPSocket::CONNECTED || thread->socket()->state() == TCPSocket::PEER_CLOSED)
// 				      close();
// 
// 			      // retry up to connectGiveUp_var times
// 			      if(--connectGiveUp_var > 0)
// 			      {
// 				      // logging
// 				      BEV2 << "scheduling another try at " << reconnectInterval_var << "secs\n";
// 
// 				      // schedule a future connect event
// 				      switch(handler->ctrans())
// 				      {
// 					      // we were about to complete - schedule another EVT_COMP
// 					      case ST_TRANS_COMP:
// 						      scheduleAt(simTime() + reconnectInterval_var, new cMessage(NULL, EVT_COMP));
// 						      break;
// 
// 					      // we were about to stop - schedule another EVT_STOP
// 					      case ST_TRANS_STOP:
// 						      scheduleAt(simTime() + reconnectInterval_var, new cMessage(NULL, EVT_STOP));
// 						      break;
// 
// 					      // normal - schedule another EVT_CONN
// 					      case ST_TRANS_NORM:
// 						      scheduleAt(simTime() + reconnectInterval_var, new cMessage(NULL, EVT_CONN));
// 						      break;
// 
// 					      // error - timeout while not being on a transient state
// 					      default:
// 						      // report the error
// 						      error("%s:%d at %s() timeout occured while not being on transient state(transient_var=%d)\n", __FILE__, __LINE__, __func__, handler->ctrans());
// 				      }
// 			      }
// 
// 			      // reset transient stuff
// 			      handler->setCtrans(0);
// 			      //transient_var = 0;
// 			      // finish - do not go any further
// 			      return;
// 			      break;
// 
// 		      // error - unknown event
// 		      default:
// 			      // report the error
// 			      error("%s:%d at %s() unknown event(kind=%d) occured\n", __FILE__, __LINE__, __func__, msg->kind());
// 	      }
// 
//  }

/**
 * Called from handleTimer() each time a message from btorrentIn gate is received.
 */
void BTSubTrackerClientBase::handleTimer(cMessage *msg) {

	// we should reach this point only if a timeout did not occur
	subtrackerIndex = 0;

	destAddr = IPAddressResolver().resolve(par("trackerConnectAddress"));
// 	//Convert string trackerConnectAddress to a vector
// 	std::vector<const char*> trackerConnectAddress; // holds result
// 
// 	//const char *trackerConnectAddress = par("trackerConnectAddress");
// 	cStringTokenizer tokenizer(par("trackerConnectAddress"));
// 	while (tokenizer.hasMoreTokens()) {
// 	    trackerConnectAddress.push_back((tokenizer.nextToken()));
// 	}
	//TCPServerThreadBase *thread;
	//BTTrackerClientBase* handler;
	// logging
	//if (!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder") && trackerNum > 1) {
// 	      for (subtrackerIndex = 0; subtrackerIndex < trackerConnectAddress.size(); subtrackerIndex++) {
			//	trackerConnAddr = trackerConnectAddress[subtrackerIndex];
				BEV2 <<"initializing a thread for the connection with tracker.. [address=" << destAddr <<endl;

				// new connection -- create new socket object and server process
				TCPSocket *newsocket = new TCPSocket();

				unsigned short port = tcp->getEphemeralPort();
				int connectPort = par("connectPort");

				//It could be the case that the peerWire port is freed so that we do not accept connections.
				while ((port==connectPort))
					port = tcp->getEphemeralPort();

				newsocket->bind(port);

				const char *serverThreadClass = (const char*)par("serverThreadClass");

				TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

				newsocket->setCallbackObject(proc);
				proc->init(this, newsocket);

				BTSubTrackerClientCommunicationBase* myProc = (BTSubTrackerClientCommunicationBase*) proc;
				//setThread(proc);
				myProc->setCtrans(ST_TRANS_NORM);
				//myProc->setActiveConnection(true);
				//myProc->setRemotePeerID(peer.peerId.c_str());
				//peerState.addPeer(peer,proc);

				newsocket->setOutputGate(gate("tcpOut"));

				newsocket->connect(destAddr, connectPort);
				socketMap.addSocket(newsocket);

				//increasePendingNumConnections();
				updateDisplay();
				//myProc->announce();
				
				// schedule a session timeout
				//proc->timerExpired(new cMessage(toString(TIMEOUT_TIMER),TIMEOUT_TIMER));
				//myProc->scheduleAt(simTime() + sessionTimeout_var, evtTout);
				//timerExpired(new cMessage(toString(TIMEOUT_TIMER),TIMEOUT_TIMER));
				
		    
				//trackerConnPort = par("trackerConnectPort[trackerIndex]");
				//BEV << "connecting to Tracker[address=" << par("connectAddress") << ", port=" << par("connectPort") << "]\n";
				//findAndSetIPAddress();	      
				//connectSeeder(trackerConnAddr);
	    //  }
}

void BTSubTrackerClientBase::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->socket());
    updateDisplay();
}
// 	else {
// 	      // grab my IP address
// 	      findAndSetIPAddress();
// 	      // fire the actual connect() - TCP
// 	      connect();
// 	}
/*
	BEV << "connecting to Tracker[address=" << par("connectAddress") << ", port=" << par("connectPort") << "]\n";
	EV << "Client Transient state: " << transient_var << "\n";
	EV << "Client state: " << state_var << "\n";
	EV << "Event: " << msg->kind() << "\n";  

	// grab my IP address
	findAndSetIPAddress();
	// fire the actual connect() - TCP
	//connect();

	if (!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder") && trackerNum > 1) {

	  if (connectFirstTracker == false) {
	    connect();
	    connectFirstTracker = true;
	  } 
	  else {
      
	    connectSeeder();
	    connectFirstTracker = false;
	    
	  }

	}
	else {

	  connect();

	}*/


// void BTTrackerClientBase::timerExpired(cMessage *msg)
// {
// 
//     TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->contextPointer();
//     BTTrackerClientBase* handler = (BTTrackerClientBase*)thread;
//     scheduleAt(simTime() + sessionTimeout_var, evtTout);
// 
// }

/**
 * Starts a new connection.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
// void BTTrackerClientBase::connect()
// {
// 	// logging
// 	BEV << "starting session timer[" << sessionTimeout_var << " secs]\n";
// 
// 	// schedule a session timeout and call default connect()
// 	scheduleAt(simTime() + sessionTimeout_var, evtTout);
// 	//TCPGenericCliAppBase::connect();
// 	
// }

// void BTTrackerClientBase::connectSeeder(const char *trackerConnAddr)
// {
// 	// logging
// 	cerr << "starting session timer[" << sessionTimeout_var << " secs]\n";
// 
// 	// schedule a session timeout and call default connect()
// 	scheduleAt(simTime() + sessionTimeout_var, evtTout);
// 	//TCPGenericCliAppBase::connectTracker(trackerConnAddr);
// 	
// }


/*void BTTrackerClientBase::connectSeeder()
{
	// logging
	BEV << "starting session timer[" << sessionTimeout_var << " secs]\n";

	// schedule a session timeout and call default connect()
	scheduleAt(simTime() + sessionTimeout_var, evtTout);
	

	// we need a new connId if this is not the first connection
	socket.renewSocket();

	// connect
	const char *connectAddress2 = par("connectAddress2");
	int connectPort2 = par("connectPort2");

	EV << "issuing OPEN command\n";
	setStatusString("connecting");

	socket.connect(IPAddressResolver().resolve(connectAddress2), connectPort2);
}*/

/**
 * Closes an active connection
 */
// void BTTrackerClientBase::close()
// {
// 
// 	// cancel the session timeout event and call default close()
// 	cancelEvent(evtTout);
// 	//TCPGenericCliAppBase::close();
// }

/**
 * Starts a session and performs some logging.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
// void BTTrackerClientBase::socketEstablished(int connId, void *ptr)
// {
// 	// default handling
// 	//TCPGenericCliAppBase::socketEstablished(connId, ptr);
// 
// 	// logging
// 	BEV << "connected to Tracker[address=" << par("connectAddress") << ", port=" << par("connectPort") << "]\n";
// 	EV << "Client Transient state: " << transient_var << "\n";
// 	EV << "Client state: " << state_var << "\n";
// 	//EV << "Event: " << msg->kind() << "\n";  
// 
// 	// perform the announce
// 	//announce();
// }

/**
 * Generates an Azureus-style 20 character long, unique peer ID, with:
 * 	- client ID = 'OM' (OMnet++)
 * 	- version number  = 0001
 * (See  http://wiki.theory.org/BitTorrentSpecification for more information)
 *
 * The peer ID is augmented with the hosting module's name, for clarity/debugging purposes.
 *
 * This method should be re-implemented in future subclasses in order to change the.way peer IDs are generated.
 */
const char* BTSubTrackerClientBase::generatePeerID()
{
//	string strID("-OM0001-");
//	stringstream oss;
//	string  moduleName(parentModule()->fullName());
//	oss<< moduleName;
//	
//	oss << strID;
//
//    	for(int i=0; i<12; i++)
//        	oss <<intrand(10);
	
	return parentModule()->fullName();
}

/**
 * Schedule next communication with the tracker.
 *
 * 
 */
void BTSubTrackerClientBase::scheduleTrackerCommunication()
{

	scheduleAt(simTime() + announceInterval(), evtTrackerComm);

}





/**
 * Handles the connection teardown.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
// void BTTrackerClientBase::socketPeerClosed(int connId, void *yourPtr)
// {
// 	// the tracker closed the connection while been in a transient state
// 	if(transient_var != 0)
// 	{
// 		// logging
// 		BEV << "session with Tracker[address=" << par("connectAddress") << ", port=" << par("connectPort") << "] ended unexpectedly\n";
// 
// 		// fire the timeout event to handle possible errors
// 		cancelEvent(evtTout);
// 		scheduleAt(simTime(), evtTout);
// 
// 		// default handling
// 		EV << "remote TCP closed, closing here as well\n";
// 	}
// }


/**
 * Handles connection failures.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
// void BTTrackerClientBase::socketFailure(int connId, void *yourPtr, int code)
// {
// 	// default handling
// 	//TCPGenericCliAppBase::socketFailure(connId, yourPtr, code);
// 
// 	// error in the connection while been in a transient state
// 	if(transient_var != 0)
// 	{
// 		// logging
// 		BEV << "session with Tracker[address=" << par("connectAddress") << ", port=" << par("connectPort") << "] died unexpectedly\n";
// 
// 		// fire the timeout event to handle possible errors
// 		cancelEvent(evtTout);
// 		scheduleAt(simTime(), evtTout);
// 	}
// }


/**
 * Find the IP address of the client.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
// void BTSubTrackerClientBase::findAndSetIPAddress()
// {
// 	InterfaceTable* ift 	= NULL;
// 	InterfaceEntry* iff	= NULL;
// 	cModule* mod 		= this;
// 
// 	// traverse the hierarchy to grab the interface table
// 	while((ift = IPAddressResolver().findInterfaceTableOf(mod)) == NULL)
// 		mod = mod->parentModule();
// 
// 	// no interface table found? -- something nasty is happening here
// 	if(ift == NULL)
// 		// report the error
// 		error("%s:%d at %s() self-address resolution failed\n", __FILE__, __LINE__, __func__);
// 
// 	// traverse the interface table and grab the appropriate IP address
// 	for(int i=0; i<ift->numInterfaces(); i++)
// 	{
// 		iff = ift->interfaceAt(i);
// 
// 		// ignore loopbacks
// 		if(iff->isLoopback())
// 			continue;
// 
// 		// if the interface has an IPv4 address then use it
// 		if(iff->ipv4() != NULL)
// 		{
// 			// update the address value
// 			ipaddress_var = IPvXAddress(iff->ipv4()->inetAddress().str().c_str());
// 			return ipaddress_var;
// 			// update the address parameter - just in case other objects use it (e.g., TCPSocket...)
// 			//par("address").setStringValue(ipaddress_var.str().c_str());
// 			break;
// 		}
// 		// try with IPv6
// 		else
// 		{
// 			// update the address value
// 			ipaddress_var = IPvXAddress(iff->ipv6()->preferredAddress().str().c_str());
// 			return ipaddress_var;
// 			// update the address parameter - just in case other objects use it (e.g., TCPSocket...)
// 			//par("address").setStringValue(ipaddress_var.str().c_str());
// 			break;
// 		}
// 	}
// }


/**
 * Get the state of the client.
 */
// size_t BTTrackerClientBase::cstate() const
// {
// 	return state_var;
// }

/**
 * Set the state of the client.
 */
// void BTTrackerClientBase::setCstate(size_t state)
// {
// 	state_var = state;
// }

/**
 * Get the transient state of the client.
 */
// size_t BTTrackerClientBase::ctrans() const
// {
// 	return transient_var;
// }

/**
 * Set the transient state of the client.
 */
// void BTTrackerClientBase::setCtrans(size_t trans)
// {
// 	transient_var = trans;
// }

/**
 * Get the tracker id.
 */
const string& BTSubTrackerClientBase::trackerId() const
{
	return trackerId_var;
}

/**
 * Set the tracker id.
 */
void BTSubTrackerClientBase::setTrackerId(const string& trackerId)
{
	trackerId_var = trackerId;
}

/**
 * Get the re-connect tries.
 */
size_t BTSubTrackerClientBase::connectGiveUp() const
{
	return connectGiveUp_var;
}

/**
 * Set the re-connect tries.
 */
void BTSubTrackerClientBase::setConnectGiveUp(size_t connectGiveUp)
{
	connectGiveUp_var = connectGiveUp;
}

/**
 * Get the session timeout (seconds).
 */
simtime_t BTSubTrackerClientBase::sessionTimeout() const
{
	return sessionTimeout_var;
}

/**
 * Set the session timeout (seconds).
 */
void BTSubTrackerClientBase::setSessionTimeout(simtime_t sessionTimeout)
{
	sessionTimeout_var = sessionTimeout;
}

/**
 * Get the re-connect interval.
 */
simtime_t BTSubTrackerClientBase::reconnectInterval() const
{
        return reconnectInterval_var;
}

/**
 * Set the re-connect interval.
 */
void BTSubTrackerClientBase::setReconnectInterval(simtime_t reconnectInterval)
{
        reconnectInterval_var = reconnectInterval;
}

int BTSubTrackerClientBase::announceInterval()
{
	return (int)announceInterval_var;
}

void BTSubTrackerClientBase::setAnnounceInterval(int announceInterval)
{
	announceInterval_var = announceInterval;
}

/**
 * Get the info hash.
 */
const string& BTSubTrackerClientBase::infoHash() const
{
	return infoHash_var;
}

/**
 * Set the info hash.
 */
void BTSubTrackerClientBase::setInfoHash(const string& infoHash)
{
	infoHash_var = infoHash;
}

/**
 * Get the peer id.
 */
const string& BTSubTrackerClientBase::peerId() const
{
	return peerId_var;
}

/**
 * Set the peer id.
 */
void BTSubTrackerClientBase::setPeerId(const string& peerId)
{
	peerId_var = peerId;
}

/**
 * Get the peer port.
 */
size_t BTSubTrackerClientBase::peerPort() const
{
	return peerPort_var;
}

/**
 * Set the peer port.
 */
void BTSubTrackerClientBase::setPeerPort(size_t peerPort)
{
	peerPort_var = peerPort;
}

/**
 * Get the compact flag.
 */
bool BTSubTrackerClientBase::compact() const
{
	return compact_var;
}

/**
 * Set the compact flag.
 */
void BTSubTrackerClientBase::setCompact(bool compact)
{
	compact_var = compact;
}

/**
 * Get the no_peer_id flag.
 */
bool BTSubTrackerClientBase::noPeerId() const
{
	return noPeerId_var;
}

/**
 * Set the no_peer_id flag.
 */
void BTSubTrackerClientBase::setNoPeerId(bool noPeerId)
{
	noPeerId_var = noPeerId;
}

/**
 * Get the numWant peers count.
 */
size_t BTSubTrackerClientBase::numWant() const
{
	return numWant_var;
}

/**
 * Set the numWant peers count.
 */
void BTSubTrackerClientBase::setNumWant(size_t numWant)
{
	numWant_var = numWant;
}

/**
 * Get the peer's key.
 */
const string& BTSubTrackerClientBase::key() const
{
	return key_var;
}

/**
 * Set the peer's key.
 */
void BTSubTrackerClientBase::setKey(const string& key)
{
	key_var = key;
}

/**
 * Set the thread.
 */
void BTSubTrackerClientBase::setThread(TCPServerThreadBase* thread)
{
	thread_var = thread;
}

/**
 * Get the thread.
 */
TCPServerThreadBase* BTSubTrackerClientBase::getThread()
{
	return thread_var;
}

/**
 * Set the thread handler.
 */
void BTSubTrackerClientBase::setHandler(BTSubTrackerClientCommunicationBase* handler)
{
	handler_var = handler;
}

/**
 * Get the thread handler.
 */
BTSubTrackerClientCommunicationBase* BTSubTrackerClientBase::getHandler()
{
	return handler_var;
}

// const char* BTSubTrackerClientBase::toString(int type)
// {
// 
// 	switch (type)
// 	{
// 		case TIMEOUT_TIMER :
// 			return "TIMEOUT_TIMER";
// 			break;
// 	}
// }

/**
 * Constructor.
 */
BTSubTrackerClientCommunicationBase::BTSubTrackerClientCommunicationBase()
{

	evtTout		= new cMessage(NULL, EVT_TOUT);
  	initDelThreadMsg	=	new cMessage("INTERNAL_INIT_REMOVE_THREAD_MSG",INTERNAL_INIT_REMOVE_THREAD_MSG);
// 	evtRemThread		= new cMessage(NULL, EVT_REM_THREAD);
	delThreadMsg = new cMessage("INTERNAL_REMOVE_THREAD_MSG",INTERNAL_REMOVE_THREAD_MSG);
	state_var 			= ST_STARTED;
 	transient_var		= 0;
	//connectGiveUp_var 	= hostModule()->connectGiveUp();


}

/**
 * Destructor.
 */
BTSubTrackerClientCommunicationBase::~BTSubTrackerClientCommunicationBase()
{
	// cancel any pending messages
	hostModule()->cancelAndDelete(evtTout);
  	hostModule()->cancelAndDelete(initDelThreadMsg);
 	hostModule()->cancelAndDelete(delThreadMsg);
}

void BTSubTrackerClientCommunicationBase::established()
{
	// start the session timer
	scheduleAt(simTime() + (hostModule()->sessionTimeout()), evtTout);
	
	// logging
	BEV << "connection with tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "] established\n";
	BEV << "starting session timer[" << hostModule()->sessionTimeout() << " secs] for tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "]\n";
	announce();


}

/**
 * Sends an announce to the tracker.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTSubTrackerClientCommunicationBase::announce()
{
	// get an annouse message
	BTTrackerMsgAnnounce *msg = new BTTrackerMsgAnnounce();

	// mandatory fields
	msg->setInfoHash(hostModule()->infoHash().c_str());
	msg->setPeerId(hostModule()->peerId().c_str());
	//msg->setPeerPort(hostModule()->peerPort());
	msg->setPeerPort(81);
	msg->setTrackerId(hostModule()->trackerId().c_str()); // will be "" at the beginning
	msg->setCompact(hostModule()->compact());
	msg->setNoPeerId(hostModule()->noPeerId());
	msg->setNumWant(hostModule()->numWant());
	msg->setKey(hostModule()->key().c_str());

	

	// differentiate based on the actual state (we should be on a transient state)
	switch(ctrans())
	{
		// going to completed
		case ST_TRANS_COMP:
			msg->setEvent(A_COMPLETED); // completed event
			break;

		// going to stopped
		case ST_TRANS_STOP:
			msg->setEvent(A_STOPPED); // stopped event
			break;

		// going to normal
		case ST_TRANS_NORM:
			if(cstate() == ST_STARTED)
			{
				msg->setEvent(A_STARTED); // started event
			}
			else
			{
				msg->setEvent(A_NORMAL); // normal event
			}
			break;

		// error - invalid state
		case 0:
		default:
			// report the error
			hostModule()->error("%s:%d at %s() invalid state occured, should a transient state(state_var=%d, transient_var=%d) occured (simTime() = %f)\n", __FILE__, __LINE__, __func__, cstate(), ctrans(), simTime());
	}
	string trackerId_var = hostModule()->trackerId();
	// set the announce size
	findAndSetAnnounceSize(msg, trackerId_var);

	// update the statistics
	//packetsSent++;
	//bytesSent+=msg->byteLength();

	// logging
	BEV << "sending announce to Tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "]\n";
 	EV << "Client Transient state: " << ctrans() << "\n";
 	EV << "Client state: " << cstate() << "\n";
// 	EV << "Event: " << msg->kind() << "\n";  

	// send the announce
	socket()->send(msg);

	// in case of a stopped event issue a session close immediately after the announce
	if(ctrans() == ST_TRANS_STOP)
	{
		setCtrans(0);
		cancelEvent(evtTout);
		close();
		//socket()->close();
	}
}

/**
 * Handles the response from the tracker.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
//void BTTrackerClientBase::socketDataArrived(int connId, void *ptr, cMessage *msg, bool urgent)
void BTSubTrackerClientCommunicationBase::dataArrived(cMessage *msg, bool urgent)
{

	// cast the reponse
	BTTrackerMsgResponse* mmsg = dynamic_cast<BTTrackerMsgResponse*>(msg);
	// invalid message type received
	if(!mmsg)
	{
		// report the error
		hostModule()->error("%s:%d at %s() invalid message received, should be a BTTrackerMsgResponse\n", __FILE__, __LINE__, __func__);
	}

	// logging
	BEV << "announce reply from Tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "]\n";
	EV << "Client Transient state: " << ctrans() << "\n";
 	EV << "Client state: " << cstate() << "\n";
	//EV << "Event: " << msg->kind() << "\n"; 

	// we have a valid reply - reset previous failures
	hostModule()->setConnectGiveUp((size_t)hostModule()->par("connectGiveUp"));
	//connectGiveUp_var = hostModule()->connectGiveUp();


// 	BTTrackerClientCommunicationBase *handler;
// 	handler = hostModule()->getHandler();

	// differentiate based on the actual state (we should be on a transient state)
	switch(ctrans())
	{
		// move to COMPLETED state
		case ST_TRANS_COMP:
			setCstate(ST_COMPLETED);
			//state_var = ST_COMPLETED;
			break;

		// move to STOPPED state
		case ST_TRANS_STOP:
			/*if ((!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder")) && trackerNum > 1) {
			  if(connectFirstTracker == false) {
			    state_var = ST_STOPPED;
			  }
			}
			else {
			  state_var = ST_STOPPED;
			}*/
			setCstate(ST_STOPPED);
			//state_var = ST_STOPPED;
			break;

		// move to NORMAL state
		case ST_TRANS_NORM:
			/*if ((!strcmp(this->parentModule()->moduleType()->fullName(), "BTHostSeeder")) && trackerNum > 1) {
			  if(connectFirstTracker == false) {
			    state_var = ST_NORMAL;
			  }
			}
			else {
			  state_var = ST_NORMAL;
			}*/
			setCstate(ST_NORMAL);
			//state_var = ST_NORMAL;
			break;

		// error - invalid state
		case 0:
		default:
		{

			// report the error
			hostModule()->error("%s:%d at %s() invalid state occured, should a transient state(state_var=%d, transient_var=%d) occured (simTime() = %f)\n", __FILE__, __LINE__, __func__, cstate(), ctrans(), simTime());

		}

	}

	EV << "Client Transient state: " << ctrans() << "\n";
 	EV << "Client state: " << cstate() << "\n";
// 	EV << "TrackerFlag: " << connectFirstTracker << "\n"; 

	// grab tracker id, if the tracker returned it to us
	if(strlen(mmsg->trackerId()) > 0)
	{
		hostModule()->setTrackerId(mmsg->trackerId());
		//trackerId_var = mmsg->trackerId();
	}
	// default handling
// 	packetsRcvd++;
// 	bytesRcvd += mmsg->byteLength();
	// we changed state
	setCtrans(0);
	//transient_var = 0;
	
	//get the announce interval from tracker response
	//hostModule()->setAnnounceInterval(mmsg->announceInterval());

	//schedule next communication with the tracker
	hostModule()->scheduleTrackerCommunication();

	// don't forward the message to the btorrent application, since the subtracker doesn't have one
	//hostModule()->send(mmsg, "btorrentOut");

	// connection teardown
	delete mmsg;
	close();
	//socket()->close();
}

/**
 * Set the size of the announce message.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTSubTrackerClientCommunicationBase::findAndSetAnnounceSize(cMessage* msg, string trackerId_var) const
{
        // cast the message
        BTTrackerMsgAnnounce* mmsg = dynamic_cast<BTTrackerMsgAnnounce*>(msg);
        if(mmsg) // success
        {
			string httpHeader = "Content-Type: text/plain; \n\n";
			//FIXME: use some readable macro here...
			int msgLength = (httpHeader.length() * sizeof(char)) + 20 + 20 + 2 + (trackerId_var.length() * sizeof(char)) + 1 + 1 + 2 + 4;
			mmsg->setByteLength(msgLength); // FIXME
        }
        else // failure
        {
			// report the error
			//hostModule()->error("%s:%d at %s() invalid message, should be of BTTrackerMsgAnnounce type\n", __FILE__, __LINE__, __func__);
        }
}


/**
 * Handles a session timeout event.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTSubTrackerClientCommunicationBase::timerExpired(cMessage* msg)
{

	    // logging
	    //BEV << "session with tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "] expired\n";
	
	    switch(msg->kind())
	    {
		// complete event
		case EVT_COMP:
			// changing state to COMPLETED
			setCtrans(ST_TRANS_COMP);
			//transient_var = ST_TRANS_COMP;
			// this type of message was not self scheduled - dispose it
			delete msg;
			break;

		// stop event
		case EVT_STOP:
			// changing state to STOPPED
			setCtrans(ST_TRANS_STOP);
			//transient_var = ST_TRANS_STOP;
			// this type of message was not self scheduled - dispose it
			delete msg;
			break;

		// connect event
		case EVT_CONN:
			// changing state to NORMAL
			//transient_var = ST_TRANS_NORM;
			setCtrans(ST_TRANS_NORM);

			// this type of message was not self scheduled - dispose it
			delete msg;
			break;

		//session timeout and error events
		case EVT_TOUT:
			BEV << "session with Tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "] expired\n";
			delete msg;
			// close the pending connection - if it is still in pending state
			if(socket()->state() == TCPSocket::CONNECTING || socket()->state() == TCPSocket::CONNECTED || socket()->state() == TCPSocket::PEER_CLOSED)
				close();

			size_t connectGiveUp_var;
			connectGiveUp_var = hostModule()->connectGiveUp();
			connectGiveUp_var--;
			hostModule()->setConnectGiveUp(connectGiveUp_var);
			// retry up to connectGiveUp_var times
			if((connectGiveUp_var) > 0)
			{
				// logging
				BEV << "scheduling another try at " << hostModule()->reconnectInterval() << "secs\n";

				// schedule a future connect event
				switch(ctrans())
				{
					// we were about to complete - schedule another EVT_COMP
					case ST_TRANS_COMP:
						scheduleAt(simTime() + hostModule()->reconnectInterval(), new cMessage(NULL, EVT_COMP));
						break;

					// we were about to stop - schedule another EVT_STOP
					case ST_TRANS_STOP:
						scheduleAt(simTime() + hostModule()->reconnectInterval(), new cMessage(NULL, EVT_STOP));
						break;

					// normal - schedule another EVT_CONN
					case ST_TRANS_NORM:
						scheduleAt(simTime() + hostModule()->reconnectInterval(), new cMessage(NULL, EVT_CONN));
						break;

					// error - timeout while not being on a transient state
					default:
						// report the error
						hostModule()->error("%s:%d at %s() timeout occured while not being on transient state(transient_var=%d)\n", __FILE__, __LINE__, __func__, ctrans());
				}
			}

			// reset transient stuff
			setCtrans(0);
			//transient_var = 0;
			// finish - do not go any further
			return;
			break;

		case INTERNAL_INIT_REMOVE_THREAD_MSG:
			BEV<<"the connection to the tracker closed. Deleting serving thread."<<endl;
			socket()->abort();
			scheduleAt(simTime(), delThreadMsg);			
			delete msg;
			break;

// 		case EVT_REM_THREAD:			
// 						
// 			//TCPServerThreadBase::closed();
// 			//delete msg;
// // 			//This socket may be removed only if it has closed or failed (due to a timeout e.g. the remote peer exited)
// 			if ((socket()->state()!= TCPSocket::CLOSED) && (socket()->state()!= TCPSocket::SOCKERROR))
// 			{
// 				
// 				scheduleAt(simTime()+100, msg);
// 			}
// 			else
// 			{
// 				
// 				delete msg;
// 				//BEV<<"Removing thread..."<<endl;
// 				TCPServerThreadBase::closed();
// 			}
// 			
// 			break;
			

		// error - unknown event
		default:
			// report the error
			hostModule()->error("%s:%d at %s() unknown event(kind=%d) occured\n", __FILE__, __LINE__, __func__, msg->kind());
	  }
	
}

/**
 * Handles the teardown of the thread.
 *
 * This method should re-implemented in future subclasses in order to extend/add/change the behavior of the tracker.
 */
void BTSubTrackerClientCommunicationBase::peerClosed()
{
	// the tracker closed the connection while been in a transient state
 	if(ctrans() != 0)
 	{
 		// logging
 		BEV << "session with Tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remoteAddress() << "] ended unexpectedly\n";
 
 		// fire the timeout event to handle possible errors
 		cancelEvent(evtTout);
 		scheduleAt(simTime(), evtTout);
 
 		// default handling
 		EV << "remote TCP closed, closing here as well\n";
 	}

}

/**
 * Handles connection failures.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTSubTrackerClientCommunicationBase::failure(int code)
{
	// default handling
	//TCPServerThreadBase::failure(int code);

	// error in the connection while been in a transient state
	if(ctrans() != 0)
	{
		// logging
		BEV << "session with Tracker[address=" << socket()->remoteAddress() << ", port=" << socket()->remotePort() << "] died unexpectedly\n";

		// fire the timeout event to handle possible errors
		cancelEvent(evtTout);
		scheduleAt(simTime(), evtTout);
	}
}

/**
 * Closes an active connection
 */
void BTSubTrackerClientCommunicationBase::close()
{

	// cancel the session timeout event and call default close()
	cancelEvent(evtTout);
	socket()->close();	
// 	socket()->abort();	
	//TCPServerThreadBase::peerClosed();
// 	BEV<<"Socket state =  "<< socket()->state()<<endl;
// 	TCPServerThreadBase::closed();
	//socket()->close();
// 	if( evtDelThread->isScheduled())
// 			cancelEvent	(evtDelThread);
   	scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);
	//scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);
//	scheduleAt(simTime() + 2, initDelThreadMsg);
}


/**
 * Get the state of the client.
 */
size_t BTSubTrackerClientCommunicationBase::cstate() const
{
	return state_var;
}

/**
 * Set the state of the client.
 */
void BTSubTrackerClientCommunicationBase::setCstate(size_t state)
{
	state_var = state;
}

/**
 * Get the transient state of the client.
 */
size_t BTSubTrackerClientCommunicationBase::ctrans() const
{
	return transient_var;
}

/**
 * Set the transient state of the client.
 */
void BTSubTrackerClientCommunicationBase::setCtrans(size_t trans)
{
	transient_var = trans;
}


/**
 * Get the host module (i.e., the instance of BTSubTrackerClientBase which spawns the thread).
 */
BTSubTrackerClientBase* BTSubTrackerClientCommunicationBase::hostModule()
{
	// get the host module and check its type
	BTSubTrackerClientBase* hostMod = dynamic_cast<BTSubTrackerClientBase*>(TCPServerThreadBase::hostModule());

	// return the correct module
	return hostMod;
}



