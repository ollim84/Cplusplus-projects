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
#include "BTTrackerClientBase.h"
//#include "TCPGenericCliAppBase.h"
#include "IPAddressResolver.h"
#include <IPControlInfo.h>
//#include "GenericAppMsg_m.h"

#define BEV	EV << "[" << hostModule()->peerId() << "]::[Tracker Client]: "
#define BEV2	EV << "[" << peerId_var << "]::[Tracker Client]: "

Register_Class(BTTrackerClientCommunicationBase);
Define_Module(BTTrackerClientBase);

/**
 * Constructor.
 */
BTTrackerClientBase::BTTrackerClientBase()
{
	//evtTout = new cMessage(NULL, EVT_TOUT);

}

/**
 * Destructor.
 */
BTTrackerClientBase::~BTTrackerClientBase()
{
	// delete any pending events
	delete tcp;
}

/**
 * Called after the module creation.
 */
void BTTrackerClientBase::initialize()
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
	key_var			= (string)par("key");
	subtrackerNum 		= (int)par("subtrackerNum");
	netinf_var		= (bool)par("netinf");

	//The peerID shall be unique for each peer, so it cannot be retrieved
	// from a parameter.
	peerId_var		 = generatePeerID();
	connectFirstTracker = false;
	subtrackerIndex = 0;
// 	evtTrackerComm = new cMessage("INTERNAL_SUBTRACKER_COM_MSG", INTERNAL_SUBTRACKER_COM_MSG);

// 	if (strncmp(this->parentModule()->moduleType()->fullName(), "SubTracker", 10) == 0) {	      
// 	      scheduleAt(simTime(), evtTrackerComm);
// 	}

// 	evtDelThread		= new cMessage(NULL, EVT_DEL_THREAD);
// 	evtRemThread		= new cMessage(NULL, EVT_REM_THREAD);

	tcp = new TCP();

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
	WATCH(netinf_var);

}

/**
 * Called each time a new message is received.
 */
void BTTrackerClientBase::handleMessage(cMessage* msg)
{
	//if(msg->isSelfMessage() || msg->arrivedOn("btorrentIn") && msg->kind() == EVT_CONN)
	if(msg->arrivedOn("btorrentIn"))
	{
		handleTimer(msg);
	}
// 	else if((msg->kind() == INTERNAL_SUBTRACKER_COM_MSG))
// 	{
// 		handleTimer(msg);
// 	}
 	else if ((msg->kind() == INTERNAL_REMOVE_THREAD_MSG))
 	{
 		handleThreadMessage(msg);
 	}
	else if ((msg->kind() == INTERNAL_INIT_TRACKER_CONNECTION_MSG))
	{
 		handleSelfMessage(msg);
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
void BTTrackerClientBase::handleThreadMessage(cMessage *msg)
{
	      
	      TCPServerThreadBase *thread = (TCPServerThreadBase *)msg->contextPointer();
	      BTTrackerClientCommunicationBase* handler = (BTTrackerClientCommunicationBase*)thread;

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
void BTTrackerClientBase::handleTimer(cMessage *msg) {

	// we should reach this point only if a timeout did not occur
	subtrackerIndex = 0;

	destAddr = IPAddressResolver().resolve(par("trackerConnectAddress"));

	//Convert string trackerConnectAddress to a vector
// 	std::vector<const char*> trackerConnectAddress; // holds result
// 
// 	cStringTokenizer tokenizer(par("trackerConnectAddress"));
// 	while (tokenizer.hasMoreTokens()) {
// 	    trackerConnectAddress.push_back((tokenizer.nextToken()));
// 	}

	InterfaceTable* ift 	= NULL;
	InterfaceEntry* iff	= NULL;
	cModule* mod 		= this;

	// traverse the hierarchy to grab the interface table
	while((ift = IPAddressResolver().findInterfaceTableOf(mod)) == NULL)
		mod = mod->parentModule();

	// no interface table found? -- something nasty is happening here
	if(ift == NULL)
		// report the error
		error("%s:%d at %s() self-address resolution failed\n", __FILE__, __LINE__, __func__);
	
	// traverse the interface table and grab the appropriate IP address
	for(int i=0; i<ift->numInterfaces(); i++) {

		      iff = ift->interfaceAt(i);

		      if(iff->isLoopback()) {
			      continue;
		      }
		      // if the interface has an IPv4 address then use it
		      if(iff->ipv4() != NULL)
		      {
			      // update the address value
			      //interfaceId = i;
			      localAddr = IPvXAddress(iff->ipv4()->inetAddress().str().c_str());

			      BEV2 <<"initializing a thread for the connection with tracker.. [address=" << destAddr <<endl;
			      //BEV2 <<"Using interface.. " << interfaceId <<endl;

			      // new connection -- create new socket object and server process
			      TCPSocket *newsocket = new TCPSocket();

			      unsigned short port = tcp->getEphemeralPort();
			      int connectPort = par("connectPort");

			      //It could be the case that the peerWire port is freed so that we do not accept connections.
			      while ((port==connectPort))
				      port = tcp->getEphemeralPort();

			     // newsocket->bind(port);
			      newsocket->bind(localAddr, port);

			      const char *serverThreadClass = (const char*)par("serverThreadClass");

			      TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

			      newsocket->setCallbackObject(proc);
			      proc->init(this, newsocket);

			      BTTrackerClientCommunicationBase* myProc = (BTTrackerClientCommunicationBase*) proc;

			      myProc->setCtrans(ST_TRANS_NORM);

			      newsocket->setOutputGate(gate("tcpOut"));

			      //newsocket->connect(destAddr, connectPort, localAddr);
			      newsocket->connect(destAddr, connectPort);
			      socketMap.addSocket(newsocket);

			      //increasePendingNumConnections();
			      updateDisplay();

			      continue;
		      }

      }
      delete msg;
}

void BTTrackerClientBase::removeThread(TCPServerThreadBase *thread)
{
    socketMap.removeSocket(thread->socket());
    updateDisplay();
}

void BTTrackerClientBase::handleSelfMessage(cMessage *msg)
{
	BTInternalMsg* intMsg = (BTInternalMsg*)msg;
	PEER peer = intMsg->peer();
	initConnection(peer);
	delete msg;
}

void BTTrackerClientBase::initConnection(PEER peer) {

	BEV2 <<"initializing a thread for the connection with SubTracker: " << peer.peerId <<". Port: "<< peer.peerPort << endl;

	// new connection -- create new socket object and server process
	TCPSocket *newsocket = new TCPSocket();

	unsigned short port = tcp->getEphemeralPort();
	int connectPort = peer.peerPort;

	//It could be the case that the peerWire port is freed so that we do not accept connections.
	while ((port==connectPort))
		port = tcp->getEphemeralPort();

	newsocket->bind(port);

	const char *serverThreadClass = (const char*)par("serverThreadClass");

	TCPServerThreadBase *proc = check_and_cast<TCPServerThreadBase *>(createOne(serverThreadClass));

	newsocket->setCallbackObject(proc);
	proc->init(this, newsocket);

	BTTrackerClientCommunicationBase* myProc = (BTTrackerClientCommunicationBase*) proc;
	//setThread(proc);
	myProc->setCtrans(ST_TRANS_NORM);


	newsocket->setOutputGate(gate("tcpOut"));

	newsocket->connect(peer.ipAddress, connectPort);
	socketMap.addSocket(newsocket);

	//increasePendingNumConnections();
	updateDisplay();
}



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
const char* BTTrackerClientBase::generatePeerID()
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
 * Find the IP address of the client.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
IPvXAddress BTTrackerClientBase::findAndSetIPAddress()
{
	InterfaceTable* ift 	= NULL;
	InterfaceEntry* iff	= NULL;
	cModule* mod 		= this;

	// traverse the hierarchy to grab the interface table
	while((ift = IPAddressResolver().findInterfaceTableOf(mod)) == NULL)
		mod = mod->parentModule();

	// no interface table found? -- something nasty is happening here
	if(ift == NULL)
		// report the error
		error("%s:%d at %s() self-address resolution failed\n", __FILE__, __LINE__, __func__);

	// traverse the interface table and grab the appropriate IP address
	for(int i=0; i<ift->numInterfaces(); i++)
	{
		iff = ift->interfaceAt(i);

		// ignore loopbacks
		if(iff->isLoopback())
			continue;

		// if the interface has an IPv4 address then use it
		if(iff->ipv4() != NULL)
		{
			// update the address value
			ipaddress_var = IPvXAddress(iff->ipv4()->inetAddress().str().c_str());
			return ipaddress_var;
			// update the address parameter - just in case other objects use it (e.g., TCPSocket...)
			//par("address").setStringValue(ipaddress_var.str().c_str());
			break;
		}
		// try with IPv6
		else
		{
			// update the address value
			ipaddress_var = IPvXAddress(iff->ipv6()->preferredAddress().str().c_str());
			return ipaddress_var;
			// update the address parameter - just in case other objects use it (e.g., TCPSocket...)
			//par("address").setStringValue(ipaddress_var.str().c_str());
			break;
		}
	}
}


/**
 * Get the tracker id.
 */
const string& BTTrackerClientBase::trackerId() const
{
	return trackerId_var;
}

/**
 * Set the tracker id.
 */
void BTTrackerClientBase::setTrackerId(const string& trackerId)
{
	trackerId_var = trackerId;
}

/**
 * Get the re-connect tries.
 */
size_t BTTrackerClientBase::connectGiveUp() const
{
	return connectGiveUp_var;
}

/**
 * Set the re-connect tries.
 */
void BTTrackerClientBase::setConnectGiveUp(size_t connectGiveUp)
{
	connectGiveUp_var = connectGiveUp;
}

/**
 * Get the session timeout (seconds).
 */
simtime_t BTTrackerClientBase::sessionTimeout() const
{
	return sessionTimeout_var;
}

/**
 * Set the session timeout (seconds).
 */
void BTTrackerClientBase::setSessionTimeout(simtime_t sessionTimeout)
{
	sessionTimeout_var = sessionTimeout;
}

/**
 * Get the re-connect interval.
 */
simtime_t BTTrackerClientBase::reconnectInterval() const
{
        return reconnectInterval_var;
}

/**
 * Set the re-connect interval.
 */
void BTTrackerClientBase::setReconnectInterval(simtime_t reconnectInterval)
{
        reconnectInterval_var = reconnectInterval;
}

/**
 * Get the info hash.
 */
const string& BTTrackerClientBase::infoHash() const
{
	return infoHash_var;
}

/**
 * Set the info hash.
 */
void BTTrackerClientBase::setInfoHash(const string& infoHash)
{
	infoHash_var = infoHash;
}

/**
 * Get the peer id.
 */
const string& BTTrackerClientBase::peerId() const
{
	return peerId_var;
}

/**
 * Set the peer id.
 */
void BTTrackerClientBase::setPeerId(const string& peerId)
{
	peerId_var = peerId;
}

/**
 * Get the peer port.
 */
size_t BTTrackerClientBase::peerPort() const
{
	return peerPort_var;
}

/**
 * Set the peer port.
 */
void BTTrackerClientBase::setPeerPort(size_t peerPort)
{
	peerPort_var = peerPort;
}

/**
 * Get the compact flag.
 */
bool BTTrackerClientBase::compact() const
{
	return compact_var;
}

/**
 * Set the compact flag.
 */
void BTTrackerClientBase::setCompact(bool compact)
{
	compact_var = compact;
}

/**
 * Get the no_peer_id flag.
 */
bool BTTrackerClientBase::noPeerId() const
{
	return noPeerId_var;
}

/**
 * Set the no_peer_id flag.
 */
void BTTrackerClientBase::setNoPeerId(bool noPeerId)
{
	noPeerId_var = noPeerId;
}

/**
 * Get the numWant peers count.
 */
size_t BTTrackerClientBase::numWant() const
{
	return numWant_var;
}

/**
 * Set the numWant peers count.
 */
void BTTrackerClientBase::setNumWant(size_t numWant)
{
	numWant_var = numWant;
}

/**
 * Get the peer's key.
 */
const string& BTTrackerClientBase::key() const
{
	return key_var;
}

/**
 * Set the peer's key.
 */
void BTTrackerClientBase::setKey(const string& key)
{
	key_var = key;
}

/**
 * Set the thread.
 */
void BTTrackerClientBase::setThread(TCPServerThreadBase* thread)
{
	thread_var = thread;
}

/**
 * Get the thread.
 */
TCPServerThreadBase* BTTrackerClientBase::getThread()
{
	return thread_var;
}

/**
 * Set the thread handler.
 */
void BTTrackerClientBase::setHandler(BTTrackerClientCommunicationBase* handler)
{
	handler_var = handler;
}

/**
 * Get the thread handler.
 */
BTTrackerClientCommunicationBase* BTTrackerClientBase::getHandler()
{
	return handler_var;
}

bool BTTrackerClientBase::netInfMode()
{
	return netinf_var;
}

void BTTrackerClientBase::setNetInfMode(bool value)
{
	netinf_var = value;
}


/**
 * Constructor.
 */
BTTrackerClientCommunicationBase::BTTrackerClientCommunicationBase()
{

	evtTout		= new cMessage(NULL, EVT_TOUT);
  	initDelThreadMsg	=	new cMessage("INTERNAL_INIT_REMOVE_THREAD_MSG",INTERNAL_INIT_REMOVE_THREAD_MSG);
// 	evtRemThread		= new cMessage(NULL, EVT_REM_THREAD);
	delThreadMsg = new cMessage("INTERNAL_REMOVE_THREAD_MSG",INTERNAL_REMOVE_THREAD_MSG);
	//evtTrackerComm = new cMessage("INTERNAL_TRACKER_COMMUNICATION_MSG",INTERNAL_TRACKER_COMMUNICATION_MSG);
	state_var 			= ST_STARTED;
 	transient_var		= 0;
	//connectGiveUp_var 	= hostModule()->connectGiveUp();


}

/**
 * Destructor.
 */
BTTrackerClientCommunicationBase::~BTTrackerClientCommunicationBase()
{
	// cancel any pending messages
	hostModule()->cancelAndDelete(evtTout);
  	hostModule()->cancelAndDelete(initDelThreadMsg);
 	hostModule()->cancelAndDelete(delThreadMsg);
	initDelThreadMsg = NULL;
	delThreadMsg = NULL;
	evtTout = NULL;
}

void BTTrackerClientCommunicationBase::established()
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
void BTTrackerClientCommunicationBase::announce()
{
	// get an annouse message
	BTTrackerMsgAnnounce *msg = new BTTrackerMsgAnnounce();

	// mandatory fields
	msg->setInfoHash(hostModule()->infoHash().c_str());
	msg->setPeerId(hostModule()->peerId().c_str());
	msg->setPeerPort(hostModule()->peerPort());
	msg->setTrackerId(hostModule()->trackerId().c_str()); // will be "" at the beginning
	msg->setCompact(hostModule()->compact());
	msg->setNoPeerId(hostModule()->noPeerId());
	msg->setNumWant(hostModule()->numWant());
	msg->setKey(hostModule()->key().c_str());
//  	msg->setIpAddress(hostModule()->findAndSetIPAddress());

		

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
	//cerr << "\nState: \n" << ctrans() << endl;
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
void BTTrackerClientCommunicationBase::dataArrived(cMessage *msg, bool urgent)
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

	
	//Regular BitTorrent
	if (hostModule()->netInfMode() == false) {

	      // forward the message to the btorrent application
	      hostModule()->send(mmsg, "btorrentOut");
	} 

	//NETINF
	else {

	
	      // forward the message to the btorrent application
	      if(hostModule()->trackerId() == "SubTrackerID") {
		    hostModule()->send(mmsg, "btorrentOut");
	      }

	      else if (hostModule()->trackerId() == "TrackerID") {

		    
		    for (unsigned int i=0;i<mmsg->peersArraySize();i++)
		    {
			PEER peer = mmsg->peers(i);
		      
			if(strncmp(peer.peerId.c_str(), "overlayTerminal_", 16) != 0) {
	      

			      BEV<< "received SubTracker "<< peer.peerId.c_str()<< " in tracker's response. Attempting connection ..."<<endl;
			      BTInternalMsg* intMsg = new BTInternalMsg("INTERNAL_INIT_TRACKER_CONNECTION_MSG",INTERNAL_INIT_TRACKER_CONNECTION_MSG);
			      intMsg->setPeer(peer);
			      scheduleAt(simTime(),intMsg);
			}
			else {

			      BEV<< "received Peer "<< peer.peerId.c_str()<< " in tracker's response. NOT Attempting connection ..."<<endl;	

			}
		    }
		    delete mmsg;

	      }
	}
	// connection teardown
	close();
	//socket()->close();
}

/**
 * Set the size of the announce message.
 *
 * This method should be re-implemented in future subclasses in order to extend/add/change the behavior of the client.
 */
void BTTrackerClientCommunicationBase::findAndSetAnnounceSize(cMessage* msg, string trackerId_var) const
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
void BTTrackerClientCommunicationBase::timerExpired(cMessage* msg)
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
			//if (sockstate!=CONNECTED && sockstate!=PEER_CLOSED && sockstate!=CONNECTING && sockstate!=LISTENING)
			if(socket()->state() != TCPSocket::CONNECTING && socket()->state() != TCPSocket::CONNECTED && socket()->state() != TCPSocket::PEER_CLOSED && socket()->state() != TCPSocket::LISTENING) {
			//if(socket()->state() == TCPSocket::CONNECTING || socket()->state() == TCPSocket::CONNECTED || socket()->state() == TCPSocket::PEER_CLOSED)
				close();
			}
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
void BTTrackerClientCommunicationBase::peerClosed()
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
void BTTrackerClientCommunicationBase::failure(int code)
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
void BTTrackerClientCommunicationBase::close()
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
	///TCP_TIMEOUT_2MSL = 240s
   	scheduleAt(simTime() + TCP_TIMEOUT_2MSL, initDelThreadMsg);
//	scheduleAt(simTime() + 2, initDelThreadMsg);
}


/**
 * Get the state of the client.
 */
size_t BTTrackerClientCommunicationBase::cstate() const
{
	return state_var;
}

/**
 * Set the state of the client.
 */
void BTTrackerClientCommunicationBase::setCstate(size_t state)
{
	state_var = state;
}

/**
 * Get the transient state of the client.
 */
size_t BTTrackerClientCommunicationBase::ctrans() const
{
	return transient_var;
}

/**
 * Set the transient state of the client.
 */
void BTTrackerClientCommunicationBase::setCtrans(size_t trans)
{
	transient_var = trans;
}


/**
 * Get the host module (i.e., the instance of BTTrackerClientBase which spawns the thread).
 */
BTTrackerClientBase* BTTrackerClientCommunicationBase::hostModule()
{
	// get the host module and check its type
	BTTrackerClientBase* hostMod = dynamic_cast<BTTrackerClientBase*>(TCPServerThreadBase::hostModule());

	// return the correct module
	return hostMod;
}



