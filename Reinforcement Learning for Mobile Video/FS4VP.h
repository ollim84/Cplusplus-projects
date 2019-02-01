//
// Copyright 2011 Zarrar Yousaf (NEC)
//  Copyright 2013 Olli Mämmelä (VTT)
// This library is free software, you can redistribute it and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//

#ifndef __INET_FS4VP_H
#define __INET_FS4VP_H

#include <omnetpp.h>
#include "INETDefs.h"
#include "INotifiable.h"
#include"NotificationBoard.h"
#include "ContextInformation_m.h"
#include "TCP.h"
#include "TCPConnection.h"
#include "AppContext_m.h"
#include <clistener.h>
#include "CliAppContext_m.h"

/**
 * Fair Scheduler for Video Pacing. Designed specifically for progressively downloaded TCP video streams/content
 */
class INET_API FS4VP : public cSimpleModule, public INotifiable
{
public:
	FS4VP(); //constructor
	virtual ~FS4VP(); //destructor


  public:

	struct ContextInfoStruct
	{
		int appId;
		int connectPortId; //the client's port Id to which the server connects
		int fileSize; //total file size in Bytes
		int remainingBytes; //remaining content size in Bytes
		int chunkSize; //size in Bytes of the content segment
		double txRate; //the rate at which an app is allowed to transmit content data of specified chunk size
		//double virtualPlayRate; //the estimated rate at which content is being played at the client end - used for play buffer size estimation
		double playoutBufferSize; //estiamted size of the playOut buffer size at the client end. Used when estimating slient buffer size
		double codecRate;
		int playBufferState;
		bool initialPlayStarted; // OM add 11.02.2013
		int userClass; // OM 12.02.2013
		int playFinished;
		double origTxRate; // Original txRate, needed for burst
		double incRate;
		double decRate;
		//RemoteCliPlayBufferState playBufferState2; //experimental
		//cOutVector bytesSentVec_test;
		//	bool cxState; //True if ESTABLISHED else False
	};
//	ContextInfoStruct appContextData;

 	typedef std::map<int, ContextInfoStruct> AppContextDataMap; //the key of the map is the app "index" number
	typedef AppContextDataMap::iterator appMapIt;
	AppContextDataMap appMap;

	friend std::ostream& operator<<(std::ostream& os, const FS4VP::ContextInfoStruct& cix);

	// No classes separated

	/* A struct that specifies the states and actions in the Q-matrix */
	struct QInfo {
		// Portion of users in N1, N2 and N3
		int N1_fraction;
		int N2_fraction;
		int N3_fraction;
		// Actions for N1, N2 and N3
		int N1_action;
		int N2_action;
		int N3_action;
		// Q-value (= expected reward) for Q ( (N1_fraction, N1_fraction, N1_fraction), (N1_action, N2_action, N3_action) )
		double Qvalue;
		QInfo() {
			N1_fraction = -1; N2_fraction = -1;
		}
	};

    struct stateInfo {
		// Portion of users in N1, N2 and N3
		int N1_fraction;
		int N2_fraction;
		int N3_fraction;
		// Actions for N1, N2 and N3
		int N1_action;
		int N2_action;
		int N3_action;
    	stateInfo() {
    		N1_fraction = N2_fraction = N3_fraction = N1_action = N2_action = N3_action = -1;
		}
    };

    stateInfo agentState;

	struct probInfo {
		// Actions for N1, N2 and N3
		int N1_action;
		int N2_action;
		int N3_action;
		double probability;
		double qvalue;
		probInfo() {
			N1_action = N2_action = N3_action = -1;
		}
	};


	 TCP::TcpConnMap tcpConnMap;
	 TCP *tcp; //a pointer to the TCP layer

  protected:

	NotificationBoard *nb;
	ContextInformation *ci;
	cListener *listener;

	/*
	 ** Internal parameters that are sent to the relevant apps. TODO: make them dynamic.
	 */
	int numOfTcpApps;
	int bytesToSend;
	int chunkSize;
	double txRate;
	double iatChunk;
	bool bufferEstimation;
	double alpha;
	int mss; //the max segment size of the TCP layer

	bool rlAgentStarted;
	cOutVector chunkSize_UE1;
	cOutVector chunkSize_UE2;
	cOutVector chunkSize_UE3;
	cOutVector chunkSize_UE4;
	cOutVector chunkSize_UE5;
	cOutVector chunkSize_UE6;
	cOutVector chunkSize_UE7;
	cOutVector chunkSize_UE8;
	cOutVector chunkSize_UE9;
	cOutVector chunkSize_UE10;

	cOutVector txRate_UE1;
	cOutVector txRate_UE2;
	cOutVector txRate_UE3;
	cOutVector txRate_UE4;
	cOutVector txRate_UE5;

	cOutVector N1_action_vec;
	cOutVector N2_action_vec;
	cOutVector N3_action_vec;

	cOutVector numN1Gold_vec;
	cOutVector numN2Gold_vec;
	cOutVector numN3Gold_vec;

	cOutVector numN1Silver_vec;
	cOutVector numN2Silver_vec;
	cOutVector numN3Silver_vec;

	cOutVector numN1Bronze_vec;
	cOutVector numN2Bronze_vec;
	cOutVector numN3Bronze_vec;


	cOutVector reward_vec;

	//simsignal_t totalBytesCliRxed;

//	int polledAppIndex; // a simple variable that indicates which app (based on index number) out of the numOfTcpApps is allowed to transmit. It helps the round robin timer to
				   // determine which application tunr it is to give the tx slot
	int fileSize; //a temp fix: specify the file size that the app is allowed to send.

	//Specifying a vector for storing the pointer to the TCP Application instances inside teh server
	typedef std::vector<cModule*> TcpAppModuleList;
	TcpAppModuleList tcpAppList;
	TcpAppModuleList::iterator tcpAppListIt;

	//Specifying a vector for storing the pointer to the remote clients (i.e., UEs)
	typedef std::vector<cModule*> ClientAppModuleList;
	ClientAppModuleList clientAppList;
	TcpAppModuleList::iterator clientAppListIt;


	//=========================EXPERIMENTAL CODE (START)===================================
	/**
	 * Specifying a vector container for storing vector stats for each of the TCP application instance.
	 */

	struct ApplicationStatStruct
	{
		typedef std::vector<simtime_t> SimTimeVec;  //Vector for storing simTime
		SimTimeVec simTimeVec;
		typedef std::vector<int> TcpAppStatVec; //Vector for storing application statistics
		TcpAppStatVec tcpAppStatVec;
	};


	typedef std::map<int, ApplicationStatStruct> TCPStatMap; //the key of the map is the app "index" number
	TCPStatMap tcpStatMap;
//	typedef TCPStatMap::iterator statMapIt;

	//=========================EXPERIMEANTL CODE (END) ====================================
//
//	enum RemoteCliPlayBufferState {
//		RED,
//		ORANGE,
//		YELLOW,
//		GREEN
//	};

	//========================= RL AGENT 08.02.2013 OM  ====================================

	/* Specifies the possible actions for each group */
	enum actionSpace {
		nochange, // 0
		increase, // 1
		decrease, // 2

		countActions // just to know the amount of enumerations
	};

	/* Specifies the possible enumerated portions of users in each group */
	enum fractionSpace {
		zero, // 0 #0
		zeroTo25, // 0 - 25 #1
		Twenty5ToFifty, // 25 - 50 #2
		FiftyTo75, // 50 - 75 #3
		Seventy5To100, // 75 - 100 #4
		countFractionSpace // just to know the amount of enumerations
	};

	// Specifies the user class
	enum userClass {
		GOLD,
		SILVER,
		BRONZE
	};

	// No classes separated
 	typedef std::list<QInfo> QMatrix;
 	QMatrix qMatrix; /// a list that contains the Q-matrix

 	typedef std::list<probInfo> ProbList;


	double gamma; //  discount factor
	double alphaRL; // learning rate
	double agentControlTime; // control time of the agent
	double beeta; // temperature parameter
	bool learningEnabled; // whether learning is enabled
	bool burst; // whether temporary burst is enabled

	// Coefficients for reward function
	double uN1Gold;
	double uN2Gold;
	double uN3Gold;
	double uN1Silver;
	double uN2Silver;
	double uN3Silver;
	double uN1Bronze;
	double uN2Bronze;
	double uN3Bronze;

	// Increase/decrease percentages
	double goldIncPercentage;
	double silverIncPercentage;
	double bronzeIncPercentage;
	double goldDecPercentage;
	double silverDecPercentage;
	double bronzeDecPercentage;

	bool changeChunksize;
	bool readQmatrixFromFile; // Whether we want to read the Q-matrix from a file
	bool writeQmatrixToFile;  // Whether we want to write the Q-matrix into a file
	bool initialPolicy; //whether we use initial policy
	bool scheduleRestart_par; // used for starting content sending again to gold user after idle period
	double idlePeriod;
	bool randomQTable; // whether random Q table is used

  protected:
	virtual int numInitStages() const {return 4;} //this specifies into how many stages does one want the initialise function to be called.
    virtual void initialize(int stage);
    virtual void handleMessage(cMessage *msg);
    virtual void finish();
    virtual void sendTxTrigger(cMessage *msg);
    virtual void registerAppContext(cMessage *msg);
    virtual void virtualPlayAdvance(cMessage *msg); //ZY 19.06.2012
    virtual void processCliApplicationContextInfo(cMessage *cliContextInfo);//ZY 17.07.2012

    /* Added new functions for RL */
    virtual void processCliPlayStart(cMessage *msg); //OM 08.02.2013
    virtual void processCliPlayFinish(cMessage *msg); //OM
    virtual QInfo *lookupQInfo(int N1_fraction, int N2_fraction, int N3_fraction, int N1_action, int N2_action, int N3_action); //OM 11.02.2013
    virtual void getRLAgentAction(int &N1_action, int &N2_action, int &N3_action); //OM 11.02.2013
    virtual void getMaxFuture(int N1_fraction, int N2_fraction, int N3_fraction, double &maxFuture, int &futureActionN1, int &futureActionN2, int &futureActionN3); //OM 11.02.2013
    virtual double getInstantReward(); // OM 11.02.2013
    virtual void updateAgentState(); // OM 12.02.2013
    virtual void selectAction(int &actionN1, int &actionN2, int &actionN3); //OM
    virtual void doActions(int N1_action, int N2_action, int N3_action); //OM
    virtual void writeQmatrix();
    virtual void readQmatrix();
    virtual void pushDataToQmatrix(std::vector<std::string> strVec);
    virtual void initializeQmatrix();
	virtual void scheduleRestart(CliAppContext *cliAppContext, cMessage *triggerTimer);
 //   virtual char* printBufferState(int _cliBufferState);


  private:
   // virtual void receiveChangeNotification(int category, const cPolymorphic*);
    virtual void receiveChangeNotification(int category, const cPolymorphic *details);

    virtual void receiveSignal(cComponent *source, simsignal_t signalID, unsigned long l);

    //statistics

};

#endif


