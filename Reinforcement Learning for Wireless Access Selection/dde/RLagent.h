/*
 * RLagent.h
 *
 *  Created on: Oct 1, 2012
 *      Author: Olli Mämmelä
 */

#ifndef RLAGENT_H_
#define RLAGENT_H_

#include <omnetpp.h>
#include <map>
//#include <boost/math/distributions/bernoulli.hpp>
//#include <boost/random/bernoulli_distribution.hpp>
//#include <boost/random/random_number_generator.hpp>
//#include <boost/random/variate_generator.hpp>
//#include <boost/random/mersenne_twister.hpp>
//#include <boost/random/uniform_int.hpp>
//#include <boost/random/uniform_int_distribution.hpp>
//#include <boost/random/normal_distribution.hpp>
//#include <boost/random/random_device.hpp>
#include "StatisticsCollector.h"

/**
 * TODO - Generated class
 */
class RLagent : public cSimpleModule
{
	public:
		// Desctructor
		~RLagent();

		void setDelay(double);
		void setState(int newState);
		int getState();
		const char* getFuzzyStateLiteral(int state);
		bool nextBool(double probability);
		bool giveFeedback(int ownState, int ownUsers, int bestState, int bestUsers, double reward, bool, bool);
		bool givePureFeedback(double, double, int, double, double, int, double,bool);
		void getControlAndReward(int curFuzzy, int curUsers, int bestState, int bestUsers, int &probability, double &reward);
		//void feedbackOnChange(int newState, double newBandwidth, int bestUsers, double delay);
		int getUsers();
		void setUsers(int bestUsers);
		bool getAction(int ownState, int ownUsers, int bestState, int bestUsers, double reward, bool video, bool sessionLock);
		bool getPureAction(int, int, int, int, int, int, double,bool);
		void getMaxFuture(int currentState, int ownUsers, int newState, int bestUsers, double &maxFuture, bool &futureAction);
		void getPureMaxFuture(int, int, int, int, int, int , double &maxFuture, bool &futureAction);
		int getUsersClassified(int bestUsers);
		int getPureUsersClassified(int bestUsers);
		int getSignalClassified(double);
		int getPureLossClassified(double);
		int getLossClassified(double);
		int getBandwidthClassified(double);
		const char* getUsersLiteral(int users);
		bool selectAction(double, double);

	    // Enumerated fuzzy classification states
	    enum fuzzyState {BAD, REGULAR, GOOD, EXCELLENT, countStates};

		// Enumerated signal strength
		enum signalState {BAD_SIGNAL, GOOD_SIGNAL, countSignal};

		// Enumerated Loss
		enum lossState {LOW_LOSS, MEDIUM_LOSS, HIGH_LOSS, countLoss};

		// Enumerated bandwidth
		enum bandwidthState {LOW_BW, MEDIUM_BW, HIGH_BW, countBW};

	    // Enumerated number of users
	    enum numberOfUsers {LOW, MEDIUM, HIGH, countUsers};

	    std::vector <double> probVector;


	    bool agentStarted;

	    struct stateInfo {
	    	int ownState;
	    	int ownUsers;
	    	int bestState;
	    	int bestUsers;
	    	bool changeAP;
	    	//int actionIndex;
	    	stateInfo() {
				ownState = -1; ownUsers = -1, bestState = -1, bestUsers = -1;
			}
	    };

	    struct pureStateInfo {
			int ownSS;
			int ownLoss;
			int ownUsers;
			int bestSS;
			int bestLoss;
			int bestUsers;
	    	bool changeAP;
	    	//int actionIndex;
	    	pureStateInfo() {
	    		ownSS = -1; ownLoss = -1, ownUsers = -1, bestSS = -1, bestLoss = -1, bestUsers = -1;
			}
	    };

	    pureStateInfo pureAgentState;

	    stateInfo agentState;

		struct transferInfo {
			int ownState;
			int ownUsers;
			int bestState;
			int bestUsers;
			//int probIndex;
			bool changeAP;
			double Qvalue;
			transferInfo() {
				ownState = -1; ownUsers = -1;
			}
		};

//		int ownSS;
//		int ownLoss;
//		int ownUsers;
//		int bestSS;
//		int bestLoss;
//		int bestUsers;

		struct pureTransferInfo {
			int ownSS;
			int ownLoss;
			int ownUsers;
			int bestSS;
			int bestLoss;
			int bestUsers;
	    	bool changeAP;
	    	double Qvalue;
			pureTransferInfo() {
				ownSS = -1; ownLoss = -1, ownUsers = -1, bestSS = -1, bestLoss = -1, bestUsers = -1;
			}
		};

	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();
		virtual transferInfo *lookupTransfer(int curFuzzy, int curUsers, int bestState, int bestUsers, bool change);
		virtual pureTransferInfo *lookupPureTransfer(int, int, int, int, int, int, bool);

		//Parameters
		int numUsers;
		double changeDelay;
		double bandwidth;
		int state;
		double reward;
		double transferPenalty;
		double gamma; //  discount factor
		double alpha; // learning rate
		std::vector <double> bandwidthVector;
		double epsilon;
		double beeta; // temperature parameter

		int numberOfChanges;
		int numberOfStays;

	 	 // Statistics
	 	 StatisticsCollector *stats;
	 	 cModule *modp;

	 	typedef std::list<transferInfo> TransferList;
	 	TransferList transferList; ///< list of Transfers, this list contains the actual Q-matrix

	 	typedef std::list<pureTransferInfo> PureTransferList;
	 	PureTransferList pureTransferList; ///< list of Transfers, this list contains the actual Q-matrix for pure Q-learning

	 	//boost::mt19937 rng;

	 	cOutVector RLagentReward;



};


#endif /* RLAGENT_H_ */

