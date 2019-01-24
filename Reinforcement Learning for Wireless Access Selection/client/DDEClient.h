//
// Copyright 2004 Andras Varga
//
// This library is free software, you can redistribute it and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

#ifndef __INET_DDEClient_H
#define __INET_DDEClient_H

#include <omnetpp.h>
#include "INETDefs.h"
#include <vector>
#include <list>
#include "TCPSocket.h"
#include "TCPSrvHostApp.h"
#include <AbstractRadioExtended.h>
#include <nsfis.h>
#include "ddeMsg_m.h"
#include "Ieee80211AgentSTA.h"
#include "NotificationBoard.h"
#include "IPv4InterfaceData.h"
#include "Ieee80211MgmtSTA.h"
#include <ThruputMeter.h>
#include "RLagent.h"
#include "StatisticsCollector.h"
#include "ClientMsg_m.h"
#include "TCP.h"
#include "TCPConnection.h"
#include "CliAppContext_m.h"
#include "InfoServer.h"



/**
 * A thread based TCP-model of the Resource management system subclassed from TCPSrvHostApp.
 * It serves requests coming in SuperComMsg messages.
 * Connection handlers are subclassed from TCPServerThreadBase.
 *
 */

class INET_API DDEClient : public TCPSrvHostApp, public INotifiable
{

  public:

    DDEClient();
    virtual ~DDEClient();


    // Enumerated fuzzy classification states
    enum fuzzyState {BAD, REGULAR, GOOD, EXCELLENT};

	 /** Describes associated AP */
	 struct AssociatedAPInfo {
		std::string ID;
		IPvXAddress apAddress;
		int numUsers;
		float load;
		float packetLoss;
		float classifyValue;
		bool cachedContent;
		float nsClass;
		float mnClass;
		float rssi;
		TCPServerThreadBase* thread;
		MACAddress macAddress;
		AssociatedAPInfo() {
			ID = -1; classifyValue = -1, load = -1, packetLoss = -1;
		}
	 };

	 /** Describes associated AP */
	 struct BestOtherAPInfo {
		std::string ID;
		IPvXAddress apAddress;
		int numUsers;
		float load;
		float packetLoss;
		float classifyValue;
		bool cachedContent;
		float nsClass;
		float mnClass;
		float rssi;
		TCPServerThreadBase* thread;
		MACAddress macAddress;
		BestOtherAPInfo() {
			ID = -1; classifyValue = -1, load = -1, packetLoss = -1;
		}
	 };

	 /** Describes an AP */
	 struct apInfo {
		std::string ID;
		IPvXAddress apAddress;
		bool suspended;
		apInfo() {
			ID = -1; suspended = false;
		}
	 };


    // GET/SET methods
    TCPServerThreadBase* getFileServerThread();
    void setFileServerThread(TCPServerThreadBase*);
    TCPServerThreadBase* getInfoServerThread();
    void setInfoServerThread(TCPServerThreadBase*);
    TCPServerThreadBase* getAPThread();
    void setAPThread(TCPServerThreadBase*);

    void addAPinfo(std::string ID, float classifyValue, int numUsers, float load, float packetLoss, IPvXAddress apAddr, bool cachedContent, float ns, float mn, const MACAddress& macAddress, float rssi);
    void addBestOtherApinfo(std::string ID, float classifyValue, int numUsers, float load, float packetLoss,
    		IPvXAddress apAddr, bool cachedContent, float ns, const MACAddress& macAddress);
    bool doHandover();

    void addAPListInfo(std::string ID, IPvXAddress apAddr);
    void suspendAP(std::string ID);
    void unSuspendAP(std::string ID);
    bool isAPSuspended(std::string ID);
    float getAssocAPclassification();
    float getAssocAPNSclassification();
    float getAssocAPMNclassification();
    int getAssocAPUsers();
    double getAssocAPLoad();
    double getAssocAPLoss();
    MACAddress getAssocAPMacAddress();
    float getAssocAPrssi();


    long getMsgsRcvd();
    void setMsgsRcvd(long);
    void incrementMsgsRcvd();
    long getMsgsSent();
    void setMsgsSent(long);
    void incrementMsgsSent();
    long getBytesRcvd();
    void setBytesRcvd(long);
    void incrementBytesRcvd(long);
    long getBytesSent();
    void setBytesSent(long);
    void incrementBytesSent(long);
    simtime_t getSessionTimeout() const;
    void setSessionTimeout(simtime_t);
    simtime_t getSuspendTimeout() const;
    void setSuspendTimeout(simtime_t);
    int getRequestLength();
    void setRequestLength(int);
    int getReplyLength();
    void setReplyLength(int);
    double getClassifyInterval();
    void setClassifyInterval(double);
    double getIdleInterval();
    void setIdleInterval(double);
    double getDecisionThreshold();
    void setDecisionThreshold(double);
    int getFileSize();
    void setFileSize(int);
    int getNumRequests();
    void setNumRequests(int);
    int getNumRequestsSent();
    void setNumRequestsSent(int);
    void incrementNumRequestsSent();
    long getBytesDownloaded();
    void setBytesDownloaded(long);
    void incrementBytesDownloaded(long);
    void getSignalStrength(float&, float&);
    void getMNclassification(int, float&, float&, float&);
    void sendScanRequest(const char* ssid, const MACAddress& addr, const MACAddress& newMAC);
    const char* getAssocApSSID();
	int getFuzzyState(float);
	const char* getFuzzyStateLiteral(int);
	bool getDownloadComplete();
	void setDownloadComplete(bool);
	void getEstimatedSignalStrength(const char*, float &, float &);
	void getAPSignalStrengths();
	simtime_t getChangeStart();
	void setChangeStart(simtime_t start);
	simtime_t getChangeEnd();
	void setChangeEnd(simtime_t end);
	//void feedbackOnChange(int);
	bool feedbackAgent(int, int, int, int);
	bool feedbackPureAgent(double, double, int, double, double, int);
	void addHandover(bool);
	int getUserClass();

	void setSessionLock(bool);
	bool getSessionLock();
	void setDecisionLock(bool);
	bool getDecisionLock();
	double getBandwidthLimit();


	//Algorithms
	bool getFuzzy();
	bool getLearning();
	bool getpureQLearning();
	bool getLoadBalancing();

	void setSignalStrengthValue(double ss);
	double getSignalStrengthValue();
	void setRSSIvalue(double rssi);
	double getRSSIvalue();
	void getNoiseLevel(float &, float &);

	void setRequestDelay(double);

	double computeDownloadThruput();
	double getDownloadThruput();
	void setDownloadThruput(double thruput);

	double computeUploadThruput();
	double getUploadThruput();
	void setUploadThruput(double thruput);

	double getTotalBandwidth();
	void setTotalBandwidth(double);

	double getAverageTotalBandwidth();
	double getAverageDelay();

	std::string getNewAPssid();
	void setNewAPssid(std::string ssid);

	MACAddress getNewAPMac();
	void setNewAPMac(MACAddress MAC);

	void recordDownloadTime();
	double getDownloadTime();
	void setDownloadTime(double time);

	void recordPlayTime();
	double getPlayTime();
	void setPlayTime(double time);

	// Application type
	int getApplicationType();

	bool getBackgroundTraffic();

	simtime_t getHandoverStart();
	void setHandoverStart(simtime_t);
	void scheduleAssociation();
	float getNSclassification();
	const MACAddress& getMacAddress();

	bool closeTcpConnections();

	// Video methods
	void updatePlayBuffer(GenericAppMsg* appmsg);
	double getTxRate();
	int getChunkSize();
	int getRemainingFileSize();
	void setRemainingFileSize(int);
	long getPlayBufferSize();
	int getInitAdvBufferSize();
	int getThreshYO();
	bool getPlayStarted();
	double getBufferedPlayTime();

  protected:
     simtime_t delay;
     simtime_t maxMsgDelay;
     simtime_t sessionTimeout_var;
     simtime_t suspendTimeout_var;

     // Statistics
     long msgsRcvd_var;
     long msgsSent_var;
     long bytesRcvd_var;
     long bytesSent_var;

     bool sessionLock_var;
     bool decisionLock_var;

     double downloadTime_var;

     double ss_var;
     double rssi_var;
     double bandwidthLimit_var;
     TCP* tcp;

     //cOutVector bitpersecVector;

     int numRequestsPerSession_var;
     double startTime_var;
     int requestLength_var;
     int replyLength_var;
     int fileSize_var;
     int videoLength_var;
     double classifyInterval_var;
     double idleInterval_var;
     double decisionThreshold_var;
     int numRequests_var;
     int numRequestsSent_var;
     long bytesDownloaded_var;
     bool downloadComplete_var;
     simtime_t changeStart;
     simtime_t changeEnd;
     double requestDelay_var;
     int numberOfHandovers;
     float mnClass_var;

     // Algorithms
     bool fuzzy_var;
     bool learning_var;
     bool pureQLearning_var;
     bool loadBalancing_var;
     bool backgroundTraffic_var;


     std::vector <double> requestDelayVector;

     std::vector <double> handoverTime;
     simtime_t handoverStart;

	 cModule *module;
	 AbstractRadioExtended *radio;
	 Ieee80211AgentSTA *agent;
	 NotificationBoard* nb; // Notification board
	 InterfaceEntry* ie;    // interface to configure
	 Ieee80211MgmtSTA* mgmt;

	 TCPServerThreadBase* fileServerThread;
	 TCPServerThreadBase* infoServerThread;
	 TCPServerThreadBase* apThread;

	 cMessage* ThruputTimer;
	 double dlThruput_var;
	 double ulThruput_var;
	 double totalBW_var;

	 //DDEInternalMsg *timeoutAssoc;

	 AssociatedAPInfo assocAP;
	 BestOtherAPInfo bestOtherAp;

	 //New AP
	 std::string newSSID;
	 MACAddress newMAC;

	 cOutVector totalBW_vector;
	 bool debuggedNode;

 	 // Statistics
 	 StatisticsCollector *stats;
 	 cModule *modp;

 	 bool isDownloadStarted_var;
 	 simtime_t downloadStart;
 	 simtime_t downloadEnd;

 	 simtime_t playTimeStart;
 	 double playTime_var;

 	std::vector<double> TotalBW_vec;

	 // A linked list for the AP structure
	 typedef std::list<apInfo> APList;
	 APList apList; ///< list of APs

	 int applicationType; //5.9.2013 OM

	 bool handoverScan;
	 bool closeTcpConnections_var;


	  // --------- VIDEO client parameters ---------------

		cQueue playBuffer;	 	//ZY 24.10.2011
		long playBufferSize; 	//ZY 25.10.2011
		double playRate; 		//ZY 25.10.2011
		double pauseRate;		//ZY 07.03.2012
		long bytesRcvd;
		int segmentsRcvd;
		int initAdvBufferSize;
		int minAdvBufferSize;
		int port; //port Id
		bool pauseEvent;
		int userClass; // OM 12.02.2013
		bool contentDelivered; //OM
		int numberOfBufferStarvations; // OM
		std::vector<double> bufferSizeVector; //OM
		double txRate;
	    int chunkSize;
	    int remainingFileSize;
	    bool playFinished;
	    double codecRate;
	    bool playStarted;

	    std::vector<double> starvationDurationVector; //OM
	    simtime_t starvationStart; //OM


		typedef std::vector<cMessage*> PlayerControl; //Play/Pause message pointers are stored here
		PlayerControl playerControl;
	//	PlayerControl::iterator tcpAppListIt;

		enum PlayBufferState {
			RED,
			ORANGE,
			YELLOW,
			GREEN
		};

		enum PlayBufferState playBufferState;
		int currentBufferState; //keeps track of the current state of the playBuffer

		//PlayBuffer Thresholds (in terms of number of chunks in playbuffer)
		int threshYO; //
		int threshGY;

		simsignal_t totalBytesReceived; //Signal to be emitted towards the listener (e.g., FS4VP module in the remote server)

		cModule *fs4vp;
		CliAppContext *cliAppContext; //a message for carrying client's context info such as client ID, bufferstate etc.

		// --------- VIDEO client parameters END---------------

  protected:

	 virtual void receiveChangeNotification(int category, const cPolymorphic *details);
	 /* Redefined methods from TCPSrvHostApp */
	virtual void initialize();
	virtual void handleMessage(cMessage *msg);
	virtual void handleSelfMessage(cMessage *msg);
	virtual void handleThreadMessage(cMessage *msg);
	virtual void removeThread(TCPServerThreadBase *);
	virtual void finish();

	// Video functions
    virtual void playContent();
    virtual void pauseContent();
    virtual void createPlayTimer();
    virtual void createPauseTimer();
    virtual void determinePlayBufferState(int playBufferSize);
    virtual void verifyBufferStateTransition(int _playBufferState);

	virtual apInfo *lookupAP(std::string ID);

    //statistics video
    cOutVector segmentsReceivedVec;
    cOutVector playBufferSizeVec;
    cOutVector playBreakVec;

    cOutVector playBufferSizeByteVec;
    //cOutVector playBufferDepletionVec;

    int playBufferIncreaseRate;
    int playBufferDepletionRate;

};

/**
 * The connection handler thread for servers.
 *
 */
class INET_API DDEClientServerHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		DDEClientServerHandler();
		/* Destructor */
		virtual ~DDEClientServerHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		DDEClient* hostModule();
		void sendRequest();
		void sendClassify();
		void callClose();
		void unRegister();

		// GET/SET methods
		void setInterfaceID(int);
		int getInterfaceID();
		void setRequestSentTime(simtime_t);
		simtime_t getRequestSentTime();
		double getDelay();
		void setFileServer(bool fileServer);
		bool getFileServer();

		void setCachedContent(bool);
		bool getCachedContent();


	protected:

		//Variables
		cMessage* evtTout;	// session timeout
		//cMessage* delThreadMsg;		// delete thread
		ddeMsg* internalMsg; // internal msg
		int interfaceID_var;
		bool classificationOK;
		bool firstClassifySent;
		simtime_t requestSent;
		bool fileServer_var;
		bool cachedContent_var;

		int timestep;


		//Reflection methods
		//virtual void sendRequest();
		virtual void sendMessage(cMessage *msg);

	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void closed();
		virtual void close();

};

/**
 * The connection handler thread for info server.
 *
 */
class INET_API DDEClientInfoHandler : public TCPServerThreadBase
{
	public:
		/* Constructor */
		DDEClientInfoHandler();
		/* Destructor */
		virtual ~DDEClientInfoHandler();

		virtual void init( TCPSrvHostApp* hostmodule, TCPSocket* socket);

		/* Redefined method from TCPServerThreadBase */
		DDEClient* hostModule();
		void sendRequest();
		void sendClassifyAP();
		void sendClassificationMN();
		float getNetworkClassification(float, const char*, bool);
		float computeEstimatedSignalStrength(const char* ssid);
		void unRegister();
		void callClose();

		// GET/SET methods
		void setInterfaceID(int);
		int getInterfaceID();


	protected:

		//Variables
		cMessage* evtTout;	// session timeout
		//cMessage* delThreadMsg;		// delete thread
		ddeMsg* internalMsg; // internal msg
		int interfaceID_var;

		//Reflection methods
		//virtual void sendRequest();
		virtual void sendMessage(cMessage *msg);

	protected:
		/* Redefined methods from TCPServerThreadBase */
		virtual void established();
		virtual void dataArrived(cMessage*, bool);
		virtual void timerExpired(cMessage*);
		virtual void peerClosed();
		virtual void closed();
		virtual void close();

};

#endif


