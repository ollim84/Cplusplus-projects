/*
 * RLagent.cc
 *
 *  Created on: Oct 1, 2012
 *      Author: Olli Mämmelä
 */
#include "RLagent.h"
#include <string.h>
#include <iostream>
#include <map>
#include <utility>
#include <algorithm> //for max_element
#include <sys/time.h>

#include "fisAP.h"
//#include "fuzzylite/FuzzyLite.h"
#include <limits>
#include <cstdlib>
//#include "fuzzylite/FunctionTerm.h"
#include <stdlib.h>
#include <fl/Headers.h>

template<class T>
double gen_normal_3(T &generator)
{
  return generator();
}

// Version that fills a vector
template<class T>
void gen_normal_3(T &generator,
              std::vector<bool> &res)
{
  for(size_t i=0; i<res.size(); ++i)
    res[i]=generator();
}

template<class T>
T gen_normal_4(T generator,
            std::vector<bool> &res)
{
  for(size_t i=0; i<res.size(); ++i)
    res[i]=generator();
  // Note the generator is returned back
  return  generator;
}


#define BEV	EV << "RL agent: "

Define_Module(RLagent);

using namespace std;

RLagent::~RLagent()
{
	bandwidthVector.clear();
}



std::ostream& operator<<(std::ostream& os, const RLagent::transferInfo& transfer)
{
    os << "ownState=" << transfer.ownState
       << " ownUsers=" << transfer.ownUsers
       << " bestState=" << transfer.bestState
       << " bestUsers=" << transfer.bestUsers
       << " changeAP=" << transfer.changeAP
       << " Qvalue=" << transfer.Qvalue;
    return os;
}

std::ostream& operator<<(std::ostream& os, const RLagent::stateInfo& agentState)
{
    os << "ownState=" << agentState.ownState
       << " ownUsers=" << agentState.ownUsers
       << " bestState=" << agentState.bestState
       << " bestUsers=" << agentState.bestUsers
       << " changeAP=" << agentState.changeAP;
    return os;
}

std::ostream& operator<<(std::ostream& os, const RLagent::pureTransferInfo& pureTransfer)
{
    os << "ownSS=" << pureTransfer.ownSS
       << " ownLoss=" << pureTransfer.ownLoss
       << " ownUsers=" << pureTransfer.ownUsers
       << " bestSS=" << pureTransfer.bestSS
       << " bestLoss=" << pureTransfer.bestLoss
       << " bestUsers=" << pureTransfer.bestUsers
       << " changeAP=" << pureTransfer.changeAP
       << " Qvalue=" << pureTransfer.Qvalue;
    return os;
}

std::ostream& operator<<(std::ostream& os, const RLagent::pureStateInfo& pureAgentState)
{
    os << "ownSS=" << pureAgentState.ownSS
       << " ownLoss=" << pureAgentState.ownLoss
       << " ownUsers=" << pureAgentState.ownUsers
       << " bestSS=" << pureAgentState.bestSS
       << " bestLoss=" << pureAgentState.bestLoss
       << " bestUsers=" << pureAgentState.bestUsers
       << " changeAP=" << pureAgentState.changeAP;
    return os;
}

void RLagent::initialize()
{
	changeDelay = 0.0;
	bandwidth = 0.0;
	state = EXCELLENT;
	reward = 0;
	transferPenalty = 3;
	numUsers = 0;
	agentStarted = false;
	numberOfChanges = numberOfStays = 0;

	modp = simulation.getModuleByPath("WirelessAccess.stats");
	stats = check_and_cast<StatisticsCollector *>(modp);

	RLagentReward.setName("RL agent reward");

	if (stats == NULL)
			opp_error("Wrong statisticsModulePath configuration");


	double prob = 0.0;

	for(int i = 0; i < 11 ; i++)
	{

		probVector.insert(probVector.end(), prob);
		prob += 0.1;
	}


	//Initialize the Q-matrix, which is a linked list data structure
	// Other data structures are possible too

	for(int a = 0 ; a < countStates ; a++) //current fuzzy
	{
		for(int b = 0; b < countUsers; b++) // current users
		{
			for(int c = 0; c < countStates; c++) // new fuzzy
			{
				for(int d = 0; d < countUsers; d++) // new users
				{
					// actions

					for(int e = 0; e < 2 ; e++)
					{
						transferInfo *transfer;
						transferList.push_back(transferInfo());
						transfer = &transferList.back();

						transfer->ownState = a;
						transfer->ownUsers = b;
						transfer->bestState = c;
						transfer->bestUsers = d;
						if(e == 0)
							transfer->changeAP = false;
						else
							transfer->changeAP = true;

						//Initial policy: set low Q-value if currentState != BAD
						// minus infinity
						if(transfer->ownState != BAD && transfer->changeAP == true)
						{
							transfer->Qvalue = -(std::numeric_limits<double>::infinity());
						}
						//Initial policy: set low Q-value if newState == BAD
						// minus infinity
						else if(transfer->ownState == BAD && transfer->bestState == BAD && transfer->changeAP == true)
						{
							transfer->Qvalue = -(std::numeric_limits<double>::infinity());

						}
						//Initial policy: set slightly higher Q-value for change
//						else if(transfer->ownState == BAD && transfer->bestState != BAD && transfer->changeAP == true)
//						{
//							transfer->Qvalue = 0.5;
//						}
						//Else: initialize the Q-value as zero
						else
						{
							transfer->Qvalue = 0;
						}
					}
				}
			}
		}
	}


	//initialize pure Q-learning matrix
	for(int ownSSIndex = 0 ; ownSSIndex < countSignal; ownSSIndex++) // ownSS
	{
		for(int ownLossIndex = 0; ownLossIndex < countLoss; ownLossIndex++) // ownLoss
		{
			for(int ownUsersIndex = 0; ownUsersIndex < countUsers; ownUsersIndex++) // ownUsers
			{
				for(int bestSSIndex = 0; bestSSIndex < countSignal; bestSSIndex++) // bestSS
				{
					for(int bestLossIndex = 0; bestLossIndex < countLoss; bestLossIndex++) // bestLoss
					{
						for(int bestUsersIndex = 0; bestUsersIndex < countUsers; bestUsersIndex++) // bestUsers
						{
							// actions
							for(int actionsIndex = 0; actionsIndex < 2 ; actionsIndex++)
							{
								pureTransferInfo *pureTransfer;
								pureTransferList.push_back(pureTransferInfo());
								pureTransfer = &pureTransferList.back();

								pureTransfer->ownSS = ownSSIndex;
								pureTransfer->ownLoss = ownLossIndex;
								pureTransfer->ownUsers = ownUsersIndex;
								pureTransfer->bestSS = bestSSIndex;
								pureTransfer->bestLoss = bestLossIndex;
								pureTransfer->bestUsers = bestUsersIndex;


								if(actionsIndex == 0)
									pureTransfer->changeAP = false;
								else
									pureTransfer->changeAP = true;

								//Initial policy:
								if(pureTransfer->bestSS == BAD_SIGNAL && pureTransfer->changeAP == true)
								{
									pureTransfer->Qvalue = -(std::numeric_limits<double>::infinity());
								}
								else if(pureTransfer->bestLoss == HIGH_LOSS && pureTransfer->changeAP == true)
								{
									pureTransfer->Qvalue = -(std::numeric_limits<double>::infinity());
								}
								else if(pureTransfer->ownLoss == LOW_LOSS && pureTransfer->changeAP == true)
								{
									pureTransfer->Qvalue = -(std::numeric_limits<double>::infinity());
								}
								//If all parameters are the same, do not change
								else if(pureTransfer->ownLoss == pureTransfer->bestLoss
										&& pureTransfer->ownUsers == pureTransfer->bestUsers
										&& pureTransfer->ownSS == pureTransfer->bestSS
										&& pureTransfer->changeAP == true)
								{
									pureTransfer->Qvalue = -(std::numeric_limits<double>::infinity());
								}
								else if(pureTransfer->ownLoss == HIGH_LOSS && pureTransfer->changeAP == true)
								{
									pureTransfer->Qvalue = 0.5;
								}
								else
								{
									pureTransfer->Qvalue = 0;
								}

							}

						}
					}
				}
			}
		}
	}


	// Discount factor
	// 0: immediate rewards considered
	// 1: long term rewards considered
	gamma = par("gamma");

	// Learning rate, to what extent newly acquired information will override old information
	// 0: agent will not learn anything
	// 1: agent considers only recent information
	alpha = par("alpha");

	// Exploration probability
	epsilon = par("epsilon");

	// Temperature parameter, determines how random the action selection is in Boltzmann Exploration
	// 0: agent selects actions almost randomly
	// inf: action selection approaches greedy action selection
	beeta = par("beeta");

	//boost::mt19937 rng(time(0));

	WATCH(changeDelay);
	WATCH(state);
	WATCH(reward);
	WATCH(numUsers);
	WATCH_LIST(transferList);
	WATCH(agentState);
	WATCH_LIST(pureTransferList);
	WATCH(pureAgentState);

}

void RLagent::handleMessage(cMessage *msg)
{
	// TODO - Generated method body
}

/**
 * Function which provides a boolean value based on a probability
 * This can be implemented in multiple ways, but for this the boost library was chosen
 *
 */

bool RLagent::nextBool(double probability)
{

	bool value;
//	double value1;
//	double value2 = (double)RAND_MAX;
//	double value3;
//
//	value1 = rand();
//	value3 = value1/value2;
//
//	value = value3 < probability;
//
//	std::cout << "value: " << value << endl;
//	std::cout << "probability: " << probability << endl;

//	double testprob = 0.5;
//	double test;
//	double test2 = (double)RAND_MAX;
//	double test3;
//
//	int n = 0;
//	for(int i = 0; i < 100; i++)
//	{
//		test = rand();
//		test3 = test/test2;
//		if(test3 < testprob)
//			n++;
//	}
//	double percentage = (double)n/100;
//
//	std::cout << "n: " << n << endl;
//	std::cout << "n prob: " << percentage << endl;

//	RNGType rng( time(0) );
//    boost::uniform_int<> one_to_six( 1, 6 );
//
//    boost::variate_generator< RNGType, boost::uniform_int<> >
//                  dice(rng, one_to_six);
//
//    for ( int i = 0; i < 6; i++ ) {
//        int n  = dice();
//        std::cout << n << endl;
//   }

    timeval t1;
    gettimeofday(&t1, NULL);
    //t1.tv_usec

	bool testvalue;

	//time(0) boost::mt19937(time(0))
//	boost::variate_generator< boost::mt19937, boost::bernoulli_distribution<> > generator(boost::mt19937(time(0)), boost::bernoulli_distribution<>(probability));
//
//	testvalue = gen_normal_3(generator);

	//std::cout << "testvalue: " << testvalue << endl;

	value = testvalue;

	/*** time(0) has the resolution of 1 second. Using it multiple times as seed within a short time span will create the same generator. **/

//	double percentage;
//
//	int n = 0;
//
//    double p = 0.1;

//    std::cout << "time of day seconds: " << t1.tv_sec << endl;
//    std::cout << "time of day micros: " << t1.tv_usec << endl;

//    boost::mt19937 rng(t1.tv_usec);
//   // boost::mt19937 rng(boost::random_device());
//
//    boost::bernoulli_distribution<> d(p);
//    boost::variate_generator<
//        boost::mt19937&,
//        boost::bernoulli_distribution<>
//    > g(rng, d);
//
//
//    for(int i = 0; i < 10000; ++i) {
//    	if(g() == true)
//    		n++;
//    }

//    // creating random number generator
//   // boost::mt19937 randgen(static_cast<unsigned int>(std::time(0)));
//    boost::bernoulli_distribution<bool> prob;
//    boost::variate_generator<boost::mt19937,
//        boost::bernoulli_distribution<bool> > nD(randgen, prob);
//    bool test;
//    for (int i = 0; i < 10000; i++)
//    {
//        test = nD();
//		if(test == true)
//			n++;
//    }

//	percentage = (double)n/10000;
//
//	std::cout << "n: " << n << endl;
//	std::cout << "n prob: " << percentage << endl;


//	  std::vector<bool> res(10000);
//	  // Assigning back to the generator ensures the state is advanced
//	  generator=gen_normal_4(generator, res);
//
//	  for(size_t i=0; i<10000; ++i)
//	  {
//		  if(res[i] == true)
//			  n++;
//	  }
//
//		percentage = (double)n/10000;
//
//		std::cout << "n: " << n << endl;
//		std::cout << "n prob: " << percentage << endl;


//	  std::cout <<"\n" << endl;
//
//	  generator=gen_normal_4(generator, res);
//
//	  for(size_t i=0; i<10; ++i)
//	    std::cout<<res[i]
//	             <<std::endl;


//	n = 0;
//
//	for(int i = 0; i < 10000; i++)
//	{
//		testvalue = gen_normal_3(generator);
//		if(testvalue == true)
//			n++;
//	}
//
//		percentage = (double)n/10000;
//
//		std::cout << "n: " << n << endl;
//		std::cout << "n prob: " << percentage << endl;

//	n = 0;
//	for(int i = 0; i < 10000; i++)
//	{
//		testvalue = gen_normal_3(generator);
//		if(testvalue == true)
//			n++;
//	}
//
//		percentage = (double)n/10000;
//
//		std::cout << "n: " << n << endl;
//		std::cout << "n prob: " << percentage << endl;

//	bool testvalue = gen_normal_3(generator);


//	value = false;

    return value;
}

bool RLagent::selectAction(double stayProbability, double changeProbability)
{
    double rnd = 0;
	rnd = dblrand();
	int selectIndex = -1;
	bool action;

	std::vector<double> probVector;
	probVector.insert(probVector.end(), stayProbability);
	probVector.insert(probVector.end(), changeProbability);

	std::cout << "rnd: " << rnd << endl;

	for (int i = 0; i < (int)probVector.size(); i++)
	{

		if(rnd < probVector[i])
		{
			selectIndex = i;
			break;
		}
		rnd -= probVector[i];

	}

	if(selectIndex == 0)
	{
		action = false;
	}
	else if (selectIndex == 1)
	{
		action = true;
	}
	else
	{
		error("Wrong selected index!");
	}

	return action;

}


void RLagent::setState(int newState)
{

	state = newState;
}

int RLagent::getState()
{
	return state;
}

int RLagent::getUsers()
{
	return numUsers;
}

void RLagent::setUsers(int bestUsers)
{

	numUsers = bestUsers;

}

/**
 * Gets the users classified to a finite state
 *
 *
 */

int RLagent::getUsersClassified(int bestUsers)
{
	int users;
	if(bestUsers <= 5)
	{
		users = LOW;
	}
	else if (bestUsers > 5 && bestUsers <= 10 )
	{
		users = MEDIUM;
	}
	else if(bestUsers >= 10)
	{
		users = HIGH;
	}

	return users;

}

int RLagent::getPureUsersClassified(int bestUsers)
{

    //Number of clients associated width the AP


	//Number of clients associated width the AP
	fl::InputVariable* users = new fl::InputVariable;
	users->setName("Users");
	users->setRange(0.000, 100.000);
	users->addTerm(new fl::Trapezoid("LOW", 0.000,0.000,5.000,10.000));
	users->addTerm(new fl::Triangle("MODERATE", 5.000, 10.000, 24.000));
	users->addTerm(new fl::Trapezoid("HIGH", 10.000,24.000,100.000,100.000));


    // Parse fuzzy output
    std::string temp = users->fuzzify(bestUsers);
    std::string lowStr = temp.substr(0,5);
    std::string medStr = temp.substr(12,5);
    std::string highStr = temp.substr(29,5);

    double lowValue = 0.0;
    double medValue = 0.0;
    double highValue = 0.0;


//    std::cout << "lowStr: " << lowStr << endl;
//    std::cout << "medStr: " << medStr << endl;
//    std::cout << "highStr: " << highStr << endl;

   // std::cout << "tempUsers: " << temp << endl;

    //Convert strings to double
    lowValue = atof(lowStr.c_str());
    medValue = atof(medStr.c_str());
    highValue = atof(highStr.c_str());

//    std::cout << "lowValue: " << lowValue << endl;
//    std::cout << "medValue: " << medValue << endl;
//    std::cout << "highValue: " << highValue << endl;

    double candidateValue = -1;
    double candidateIndex = 0;

    // Get the highest value
    //TODO: what to do when two values have 0.5?
    for(int i = 0 ; i < 3 ;  i++ )
    {
    	if(i == 0)
    	{
    		if(candidateValue == -1 || candidateValue < lowValue)
    		{
    			candidateIndex = 0;
    			candidateValue = lowValue;
    		}

    	}
    	else if(i == 1)
    	{
    		if(candidateValue == -1 || candidateValue < medValue)
    		{
    			candidateIndex = 1;
    			candidateValue = medValue;
    		}

    	}
    	else if(i == 2)
    	{
    		if(candidateValue == -1 || candidateValue < highValue)
    		{
    			candidateIndex = 2;
    			candidateValue = highValue;
    		}
    	}
    }

    int usersClassified;
    if(candidateIndex == 0)
    {
    	usersClassified = LOW;
    }
    else if(candidateIndex == 1)
    {
    	usersClassified = MEDIUM;
    }
    else if(candidateIndex == 2)
    {
    	usersClassified = HIGH;
    }

//	int users;
//	if(bestUsers < 5)
//	{
//		users = LOW;
//	}
//	else if (bestUsers >= 5 && bestUsers < 15 )
//	{
//		users = MEDIUM;
//	}
//	else if(bestUsers >= 15)
//	{
//		users = HIGH;
//	}

	return usersClassified;

}

int RLagent::getSignalClassified(double signal)
{
	int signalClassified;
	if(signal < 50)
	{
		signalClassified = BAD_SIGNAL;
	}
	else
	{
		signalClassified = GOOD_SIGNAL;
	}


	return signalClassified;

}

int RLagent::getLossClassified(double loss)
{
	int lossClassified;
	if(loss <= 2.5)
	{
		lossClassified = LOW_LOSS;
	}
	else if(loss <= 7.5 && loss > 2.5)
	{
		lossClassified = MEDIUM_LOSS;
	}
	else if(loss > 7.5)
	{
		lossClassified = HIGH_LOSS;
	}


	return lossClassified;

}

int RLagent::getPureLossClassified(double loss)
{

    //Percentage of packet losses in the AP
	fl::InputVariable* losses = new fl::InputVariable;
	losses->setName("Losses");
	losses->setRange(0.000, 100.000);
	losses->addTerm(new fl::Ramp("LOW", 0.000,5.000));
	losses->addTerm(new fl::Triangle("MODERATE", 0.000, 5.000, 10.000));
	losses->addTerm(new fl::Trapezoid("HIGH",5.000,10.000,100.000,100.000));


    // Parse fuzzy output
    std::string temp = losses->fuzzify(loss);
    std::string lowStr = temp.substr(0,5);
    std::string medStr = temp.substr(12,5);
    std::string highStr = temp.substr(29,5);

    double lowValue = 0.0;
    double medValue = 0.0;
    double highValue = 0.0;


//    std::cout << "lowStr: " << lowStr << endl;
//    std::cout << "medStr: " << medStr << endl;
//    std::cout << "highStr: " << highStr << endl;

 //   std::cout << "temp: " << temp << endl;

    //Convert strings to double
    lowValue = atof(lowStr.c_str());
    medValue = atof(medStr.c_str());
    highValue = atof(highStr.c_str());

//    std::cout << "lowValue: " << lowValue << endl;
//    std::cout << "medValue: " << medValue << endl;
//    std::cout << "highValue: " << highValue << endl;

    double candidateValue = -1;
    double candidateIndex = 0;

    // Get the highest value
    //TODO: what to do when two values have 0.5?
    for(int i = 0 ; i < 3 ;  i++ )
    {
    	if(i == 0)
    	{
    		if(candidateValue == -1 || candidateValue < lowValue)
    		{
    			candidateIndex = 0;
    			candidateValue = lowValue;
    		}

    	}
    	else if(i == 1)
    	{
    		if(candidateValue == -1 || candidateValue < medValue)
    		{
    			candidateIndex = 1;
    			candidateValue = medValue;
    		}

    	}
    	else if(i == 2)
    	{
    		if(candidateValue == -1 || candidateValue < highValue)
    		{
    			candidateIndex = 2;
    			candidateValue = highValue;
    		}
    	}
    }


    int lossClassified;
    if(candidateIndex == 0)
    {
    	lossClassified = LOW_LOSS;
    }
    else if(candidateIndex == 1)
    {
    	lossClassified = MEDIUM_LOSS;
    }
    else if(candidateIndex == 2)
    {
    	lossClassified = HIGH_LOSS;
    }

//	int lossClassified;
//	if(loss < 1)
//	{
//		lossClassified = LOW_LOSS;
//	}
//	else if(loss < 10 && loss >= 1)
//	{
//		lossClassified = MEDIUM_LOSS;
//	}
//	else
//	{
//		lossClassified = HIGH_LOSS;
//	}


	return lossClassified;

}

int RLagent::getBandwidthClassified(double bandwidth)
{
	int bandwidthClassified;
	if(bandwidth > 0.7)
	{
		bandwidthClassified = LOW_BW;
	}
	else if(bandwidth < 0.7 && bandwidth > 0.33)
	{
		bandwidthClassified = MEDIUM_BW;
	}
	else
	{
		bandwidthClassified= HIGH_BW;
	}


	return bandwidthClassified;

}

void RLagent::setDelay(double delay)
{
	changeDelay = delay;
}

/**
 * Function for debugging
 *
 *
 */
const char* RLagent::getFuzzyStateLiteral(int state)
{
	const char* string;
	if(state == BAD)
	{
		string = "BAD";
	}
	else if(state == REGULAR)
	{
		string = "REGULAR";
	}
	else if(state == GOOD)
	{
		string = "GOOD";
	}
	else if(state == EXCELLENT)
	{
		string = "EXCELLENT";
	}

	return string;
}

/**
 * Function for debugging
 *
 *
 */

const char* RLagent::getUsersLiteral(int users)
{
	const char* string;
	if(users == LOW)
	{
		string = "LOW";
	}
	else if(users == MEDIUM)
	{
		string = "MEDIUM";
	}
	else if(users == HIGH)
	{
		string = "HIGH";
	}

	return string;
}

/**
 * The main function of the RL agent. The System (e.g. TCP application) calls this function
 * and it provides a boolean for either making a switch or staying at the current AP
 * The system provides its new Fuzzy state, users and bandwidth acquired when making
 * a previous decision
 */

bool RLagent::giveFeedback(int ownState, int ownUsers, int bestState, int bestUsers, double reward, bool video, bool sessionLock)
{

	bool changeAP;

	// Get users classified: low, medium, high
	int ownUsersClassified = getUsersClassified(ownUsers);
	int bestUsersClassified = getUsersClassified(bestUsers);

	//if(agentStarted == true)
	//{

		// Get the action based on the parameters
		changeAP = getAction(ownState, ownUsersClassified, bestState, bestUsersClassified, reward, video, sessionLock);




		//setState(newState);
		//setUsers(usersClassified);
		//bandwidth = newBandwidth;

		//bandwidthVector.insert(bandwidthVector.end(), newBandwidth);
		//std::cout << "RL agent, total bandwidth: " << bandwidth << endl;

		//std::cout << "action: " << changeAP << endl;

		// After selecting the first action, the agent has started
		// The first action is always a "stay"
		if(agentStarted == false)
		{
			agentStarted = true;
		}

		//changeAP = nextBool(probVector[actionIndex]);
//	}
//	else
//	{
//		changeAP = false;
//		std::cout << "Agent has not started." << endl;
//		std::cout << "ownState: " << newState << endl;
//		std::cout << "ownUsers: " << usersClassified << endl;
//		// Set initial state
//		agentState.ownState = newState;
//		agentState.ownUsers = usersClassified;
//		agentState.bestState = newState;
//		agentState.bestUsers = usersClassified;
//		agentState.changeAP = 0;
//
//		agentStarted = true;
//		//changeAP = nextBool(probVector[actionIndex]);
//	}

	return changeAP;
}

bool RLagent::givePureFeedback(double ownSS, double ownLoss, int ownUsers, double bestSS, double bestLoss, int bestUsers, double reward, bool video)
{

	bool changeAP;

	// Get signal strength aggregated
	int ownSSClassified = getSignalClassified(ownSS);
	int bestSSClassified = getSignalClassified(bestSS);

	// Get loss aggregated
	int ownLossClassified = getLossClassified(ownLoss);
	int bestLossClassified = getLossClassified(bestLoss);

	// Get users classified: low, medium, high
	int ownUsersClassified = getUsersClassified(ownUsers);
	int bestUsersClassified = getUsersClassified(bestUsers);

	std::cout << "ownSS: " << ownSS << endl;
	std::cout << "ownLoss: " << ownLoss << endl;
	std::cout << "ownUsers: " << ownUsers << endl;
	std::cout << "bestSS: " << bestSS << endl;
	std::cout << "bestLoss: " << bestLoss << endl;
	std::cout << "bestUsers: " << bestUsers << endl;

	// Get bandwidth aggregated
	//int bandwidthClassified = getBandwidthClassified(newBandwidth);

	//if(agentStarted == true)
	//{

		// Get the action based on the parameters
		changeAP = getPureAction(ownSSClassified, ownLossClassified, ownUsersClassified, bestSSClassified, bestLossClassified, bestUsersClassified, reward, video);


		//setState(newState);
		//setUsers(usersClassified);
		bandwidth = reward;

		bandwidthVector.insert(bandwidthVector.end(), reward);
		std::cout << "RL agent, total reward: " << bandwidth << endl;

		//std::cout << "action: " << changeAP << endl;

		// After selecting the first action, the agent has started
		// The first action is always a "stay"
		if(agentStarted == false)
		{
			agentStarted = true;
		}

		//changeAP = nextBool(probVector[actionIndex]);
//	}
//	else
//	{
//		changeAP = false;
//		std::cout << "Agent has not started." << endl;
//		std::cout << "ownState: " << newState << endl;
//		std::cout << "ownUsers: " << usersClassified << endl;
//		// Set initial state
//		agentState.ownState = newState;
//		agentState.ownUsers = usersClassified;
//		agentState.bestState = newState;
//		agentState.bestUsers = usersClassified;
//		agentState.changeAP = 0;
//
//		agentStarted = true;
//		//changeAP = nextBool(probVector[actionIndex]);
//	}

	return changeAP;
}


bool RLagent::getPureAction(int ownSSClassified, int ownLossClassified, int ownUsersClassified, int bestSSClassified, int bestLossClassified, int bestUsersClassified, double reward, bool video)
{
	bool actionIndex;
	actionIndex = false;
	bool futureAction;
	double maxFuture = 0.0;
	double rewardMbs;

	// FTP service
	if(video == false)
	{
		rewardMbs = reward / 1000000;
	}
	// Video service
	else
	{
		rewardMbs = reward / 100;
		//rewardMbs = log(1 + reward);
	}

	std::cout << "Reward: " << reward << endl;
	std::cout << "Reward in Mbps: " << rewardMbs << endl;


	//Set bestState to old action. This is the actual state that the agent goes after action selection
	//pureAgentState.ownSS = ownSSClassified;
	//pureAgentState.ownLoss = ownLossClassified;
	//pureAgentState.ownUsers = ownUsersClassified;
	pureAgentState.bestSS = bestSSClassified;
	pureAgentState.bestLoss = bestLossClassified;
	pureAgentState.bestUsers = bestUsersClassified;



	// The first action is always a "stay", afterwards the agent is started
	if(agentStarted == true)
	{


		// 1. Update Q values
		std::cout << "OLD ACTION: " << endl;

		std::cout << "ownSS: " << pureAgentState.ownSS << endl;
		std::cout << "ownLoss: " << pureAgentState.ownLoss << endl;
		std::cout << "ownUsers: " << pureAgentState.ownUsers << endl;
		std::cout << "bestSS: " << pureAgentState.bestSS << endl;
		std::cout << "bestLoss: " << pureAgentState.bestLoss<< endl;
		std::cout << "bestUsers: " << pureAgentState.bestUsers<< endl;
		std::cout << "action: " << pureAgentState.changeAP << endl;

		//lookup a transfer that was previously done
		pureTransferInfo *pureTransfer = lookupPureTransfer(pureAgentState.ownSS, pureAgentState.ownLoss, pureAgentState.ownUsers,
				pureAgentState.bestSS, pureAgentState.bestLoss, pureAgentState.bestUsers, pureAgentState.changeAP);


		if(pureTransfer == 0)
		{
			opp_error("Error! Transfer not found by the RL agent");
		}

		// Max future value if action is chosen: Q(next state, all actions)
		// all actions: true (switch), false (stay)
		std::cout << "ownSS: " << ownSSClassified << endl;
		std::cout << "ownLoss: " << ownLossClassified << endl;
		std::cout << "ownUsers: " << ownUsersClassified << endl;
		std::cout << "bestSS: " << bestSSClassified << endl;
		std::cout << "bestLoss: " << bestLossClassified << endl;
		std::cout << "bestUsers: " << bestUsersClassified << endl;

		getPureMaxFuture(ownSSClassified, ownLossClassified, ownUsersClassified, bestSSClassified, bestLossClassified, bestUsersClassified, maxFuture, futureAction);

		std::cout << "Old Q value: " << pureTransfer->Qvalue << endl;
		std::cout << "instant reward received from last move: " << rewardMbs << endl;
		std::cout << "max future Q value: " << maxFuture << endl;
		std::cout << "max future action: " << futureAction << endl;

		//update transfer Q value based on the instant reward that was retrieved from previous action
		// Q-value --> estimated reward
		// new Q value = (1-alpha) * old Q value + alpha * ( instantReward + gamma * (max future value if action is chosen))
		pureTransfer->Qvalue = (1-alpha) * pureTransfer->Qvalue + alpha * (rewardMbs + gamma * maxFuture);

		std::cout << "New Q value: " << pureTransfer->Qvalue << endl;

	}

	// 2. Choose an action

	double actionProbability = 0.0;
	double stayProbability = 0.0;

	// Boltzmann exploration: select actions based on (softmax action selection) weighted probabilities

	// P(stay) = e^(beeta * Q(s1n1 s2n2, stay) / ( e^(beeta * Q(s1n1 s2n2, stay)) + e^(beeta * Q(s1n1 s2n2, change)))

	//lookup a transfer for staying
	pureTransferInfo *stayTransfer = lookupPureTransfer(ownSSClassified, ownLossClassified, ownUsersClassified, bestSSClassified, bestLossClassified, bestUsersClassified, false);

	//lookup a transfer for changing
	pureTransferInfo *changeTransfer = lookupPureTransfer(ownSSClassified, ownLossClassified, ownUsersClassified, bestSSClassified, bestLossClassified, bestUsersClassified, true);

	// This is the actual Boltzmann formula, which computes the probability
	actionProbability = exp(beeta * (changeTransfer->Qvalue)) / ( exp(beeta * (stayTransfer->Qvalue)) + exp(beeta * (changeTransfer->Qvalue)) );

	stayProbability = exp(beeta * (stayTransfer->Qvalue)) / ( exp(beeta * (stayTransfer->Qvalue)) + exp(beeta * (changeTransfer->Qvalue)) );

	// Get the boolean value based on that probability
	//actionIndex = nextBool(actionProbability);

	actionIndex = selectAction(stayProbability, actionProbability);

	//actionIndex = true;

	std::cout << "Action probability for changing: " << actionProbability << endl;
	std::cout << "Action probability for staying: " << stayProbability << endl;
	std::cout << "Action: " << actionIndex << endl;
	std::cout << "changeTransfer->Qvalue: " << changeTransfer->Qvalue << endl;
	std::cout << "stayTransfer->Qvalue: " << stayTransfer->Qvalue << endl;
	std::cout << "exp(beeta * (stayTransfer->Qvalue)): " << exp(beeta * (stayTransfer->Qvalue)) << endl;
	std::cout << "exp(beeta * (changeTransfer->Qvalue)): " << exp(beeta * (changeTransfer->Qvalue)) << endl;

	// Check if current policy is followed
	//TODO
	if(bestSSClassified == BAD_SIGNAL)
	{
		// If a change was selected, correct it as false
		// We do not change into an AP which is also BAD
		if(actionIndex == true)
		{
			actionIndex = false;
			//error("Wrong action selected!");
		}
	}
	else
	{
		// Count the number of changes
		if(actionIndex == true)
		{
			numberOfChanges++;

		}
		// Count the number of stays
		else
		{
			numberOfStays++;
		}

	}

	// Set new agent state
	pureAgentState.ownSS = ownSSClassified;
	pureAgentState.ownLoss = ownLossClassified;
	pureAgentState.ownUsers = ownUsersClassified;
	pureAgentState.bestSS = bestSSClassified;
	pureAgentState.bestLoss = bestLossClassified;
	pureAgentState.bestUsers = bestUsersClassified;
	pureAgentState.changeAP = actionIndex;

	std::cout << "NEW ACTION: " << endl;

	std::cout << "ownSS: " << pureAgentState.ownSS << endl;
	std::cout << "ownLoss: " << pureAgentState.ownLoss << endl;
	std::cout << "ownUsers: " << pureAgentState.ownUsers << endl;
	std::cout << "bestSS: " << pureAgentState.bestSS << endl;
	std::cout << "bestLoss: " << pureAgentState.bestLoss<< endl;
	std::cout << "bestUsers: " << pureAgentState.bestUsers<< endl;
	std::cout << "action: " << pureAgentState.changeAP << endl;


	return actionIndex;
}
/**
 * This function gets the action (switch or stay) and updates the Q-matrix
 *
 *
 */

bool RLagent::getAction(int ownState, int ownUsers, int bestState, int bestUsers, double reward, bool video, bool sessionLock)
{

//	int currentState = getState();
//	int ownUsers = getUsers();
	bool actionIndex;
	double maxFuture = 0.0;
	bool futureAction;
	double rewardMbs;

	if(video == false)
	{
		rewardMbs = reward / 1000000;
	}
	else
	{
		//rewardMbs = reward / 100;
		if(reward >= 0)
			rewardMbs = log(reward);
		else
			rewardMbs = reward / 100;
	}

	//double rewardMbs = reward / 100;

	std::cout << "Reward: " << reward << endl;
	std::cout << "Reward in Mbps: " << rewardMbs << endl;

	// Record the reward
	RLagentReward.record(rewardMbs);


	//Set bestState to old action. This is the actual state that the agent goes after action selection
	//agentState.bestState = ownState;
	//agentState.bestUsers = ownUsers;


	// The first action is always a "stay", afterwards the agent is started
	if(agentStarted == true)
	{

		// 1. Update Q values
		std::cout << "OLD ACTION: " << endl;

		std::cout << "ownState: " << getFuzzyStateLiteral(agentState.ownState) << endl;
		std::cout << "ownUsers: " << getUsersLiteral(agentState.ownUsers) << endl;
		std::cout << "bestState: " << getFuzzyStateLiteral(agentState.bestState) << endl;
		std::cout << "bestUsers: " << getUsersLiteral(agentState.bestUsers) << endl;
		std::cout << "action: " << agentState.changeAP << endl;

		//lookup a transfer that was previously done
		transferInfo *transfer = lookupTransfer(agentState.ownState, agentState.ownUsers, agentState.bestState, agentState.bestUsers, agentState.changeAP);


		if(transfer == 0)
		{
			opp_error("Error! Transfer not found by the RL agent");
		}

		// Max future value if action is chosen: Q(next state, all actions)
		// all actions: true (switch), false (stay)
		std::cout << "MAX FUTURE: " << endl;
		std::cout << "ownState: " << getFuzzyStateLiteral(ownState) << endl;
		std::cout << "ownUsers: " << getUsersLiteral(ownUsers) << endl;
		std::cout << "bestState: " << getFuzzyStateLiteral(bestState) << endl;
		std::cout << "bestUsers: " << getUsersLiteral(bestUsers) << endl;

		getMaxFuture(ownState, ownUsers, bestState, bestUsers, maxFuture, futureAction);

		std::cout << "Old Q value: " << transfer->Qvalue << endl;
		std::cout << "instant reward received from last move: " << rewardMbs << endl;
		std::cout << "max future Q value: " << maxFuture << endl;
		std::cout << "max future action: " << futureAction << endl;

		//update transfer Q value based on the instant reward that was retrieved from previous action
		// Q-value --> estimated reward
		// new Q value = (1-alpha) * old Q value + alpha * ( instantReward + gamma * (max future value if action is chosen))
		transfer->Qvalue = (1-alpha) * transfer->Qvalue + alpha * (rewardMbs + gamma * maxFuture);

		std::cout << "New Q value: " << transfer->Qvalue << endl;

	}
	// 2. Choose an action


	double actionProbability = 0.0;
	double stayProbability = 0.0;

	// Boltzmann exploration: select actions based on (softmax action selection) weighted probabilities

	// P(stay) = e^(beeta * Q(s1n1 s2n2, stay) / ( e^(beeta * Q(s1n1 s2n2, stay)) + e^(beeta * Q(s1n1 s2n2, change)))

	//lookup a transfer for staying
	transferInfo *stayTransfer = lookupTransfer(ownState, ownUsers, bestState, bestUsers, false);

	//lookup a transfer for changing
	transferInfo *changeTransfer = lookupTransfer(ownState, ownUsers, bestState, bestUsers, true);

	// This is the actual Boltzmann formula, which computes the probability
	actionProbability = exp(beeta * (changeTransfer->Qvalue)) / ( exp(beeta * (stayTransfer->Qvalue)) + exp(beeta * (changeTransfer->Qvalue)) );
	stayProbability = exp(beeta * (stayTransfer->Qvalue)) / ( exp(beeta * (stayTransfer->Qvalue)) + exp(beeta * (changeTransfer->Qvalue)) );

	std::cout << "Action probability for changing: " << actionProbability << endl;
	std::cout << "Action probability for staying: " << stayProbability << endl;

	std::cout << "changeTransfer->Qvalue: " << changeTransfer->Qvalue << endl;
	std::cout << "stayTransfer->Qvalue: " << stayTransfer->Qvalue << endl;

	// Get the boolean value based on that probability
	//actionIndex = nextBool(actionProbability);

	//TODO: Both change and stay transfers at minus infinity, codefix
	if(changeTransfer->Qvalue == -(std::numeric_limits<double>::infinity()) && stayTransfer->Qvalue == -(std::numeric_limits<double>::infinity()))
	{
		//error("Both change and stay at minus infinity!");
		actionIndex = 0;
	}
	else
	{
		actionIndex = selectAction(stayProbability, actionProbability);
	}



	std::cout << "Action: " << actionIndex << endl;



	//actionIndex = true;

	//std::cout << "exp(beeta * (stayTransfer->Qvalue)): " << exp(beeta * (stayTransfer->Qvalue)) << endl;
	//std::cout << "exp(beeta * (changeTransfer->Qvalue)): " << exp(beeta * (changeTransfer->Qvalue)) << endl;

	// Check if current policy is followed
	if(ownState != BAD || bestState == BAD || sessionLock == true)
	{
		// If a change was selected, correct it as false
		// We do not change into an AP which is also BAD
		if(actionIndex == true)
		{
			actionIndex = false;
			numberOfStays++;
			//error("Wrong action selected!");
		}
	}
	else
	{
		// Count the number of changes
		if(actionIndex == true)
		{
			numberOfChanges++;

		}
		// Count the number of stays
		else
		{
			numberOfStays++;
		}

	}


//	if(currentState != BAD)
//	{
//		double actionProbability = 0.0;
//
//		// Boltzmann exploration: select actions based on (softmax action selection) weighted probabilities
//
//		// P(stay) = e^(beeta * Q(s1n1 s2n2, stay) / ( e^(beeta * Q(s1n1 s2n2, stay)) + e^(beeta * Q(s1n1 s2n2, change)))
//
//		//lookup a transfer for staying
//		transferInfo *stayTransfer = lookupTransfer(currentState, ownUsers, newState, bestUsers, false);
//
//		//lookup a transfer for changing
//		transferInfo *changeTransfer = lookupTransfer(currentState, ownUsers, newState, bestUsers, true);
//
//		actionProbability = exp(beeta * (changeTransfer->Qvalue)) / ( exp(beeta * (stayTransfer->Qvalue)) + exp(beeta * (changeTransfer->Qvalue)) );
//
//		actionIndex = nextBool(actionProbability);
//
//		//actionIndex = false;
//
//		std::cout << "Action probability for changing: " << actionProbability << endl;
//		std::cout << "Action: " << actionIndex << endl;
//		std::cout << "changeTransfer->Qvalue: " << changeTransfer->Qvalue << endl;
//		std::cout << "stayTransfer->Qvalue: " << stayTransfer->Qvalue << endl;
//		std::cout << "exp(beeta * (stayTransfer->Qvalue)): " << exp(beeta * (stayTransfer->Qvalue)) << endl;
//		std::cout << "exp(beeta * (changeTransfer->Qvalue)): " << exp(beeta * (changeTransfer->Qvalue)) << endl;
//
//		if(actionIndex == true)
//			error("Wrong action selected!");
//
//		//actionIndex = false;
//	}
//	else
//	{
//
//		// Choose an action (= probability) that maximizes long term reward
//		//TODO: have to determine when the q values have converged
//		// the agent needs to explore first
//
//		if(newState == BAD)
//		{
//			actionIndex = false;
//		}
//		else
//		{
//
//			double actionProbability = 0.0;
//
//			// Boltzmann exploration: select actions based on (softmax action selection) weighted probabilities
//
//			// P(stay) = e^(beeta * Q(s1n1 s2n2, stay) / ( e^(beeta * Q(s1n1 s2n2, stay)) + e^(beeta * Q(s1n1 s2n2, change)))
//
//			//lookup a transfer for staying
//			transferInfo *stayTransfer = lookupTransfer(currentState, ownUsers, newState, bestUsers, false);
//
//			//lookup a transfer for changing
//			transferInfo *changeTransfer = lookupTransfer(currentState, ownUsers, newState, bestUsers, true);
//
//			actionProbability = exp(beeta * (changeTransfer->Qvalue)) / ( exp(beeta * (stayTransfer->Qvalue)) + exp(beeta * (changeTransfer->Qvalue)) );
//
//			actionIndex = nextBool(actionProbability);
//
//			//actionIndex = false;
//
//			std::cout << "Action probability for changing: " << actionProbability << endl;
//			std::cout << "Action: " << actionIndex << endl;
//			std::cout << "changeTransfer->Qvalue: " << changeTransfer->Qvalue << endl;
//			std::cout << "stayTransfer->Qvalue: " << stayTransfer->Qvalue << endl;
//			std::cout << "exp(beeta * (stayTransfer->Qvalue)): " << exp(beeta * (stayTransfer->Qvalue)) << endl;
//			std::cout << "exp(beeta * (changeTransfer->Qvalue)): " << exp(beeta * (changeTransfer->Qvalue)) << endl;
//
//		}
//	}



//	std::cout << "ownState: " << getFuzzyStateLiteral(currentState) << endl;
//	std::cout << "ownUsers: " << getUsersLiteral(ownUsers) << endl;
//	std::cout << "bestState: " << getFuzzyStateLiteral(newState) << endl;
//	std::cout << "bestUsers: " << getUsersLiteral(bestUsers) << endl;
//	std::cout << "action: " << action << endl;

	// Set new agent state
	agentState.ownState = ownState;
	agentState.ownUsers = ownUsers;
	agentState.bestState = bestState;
	agentState.bestUsers = bestUsers;
	agentState.changeAP = actionIndex;

	std::cout << "NEW ACTION: " << endl;
	std::cout << "ownState: " << getFuzzyStateLiteral(agentState.ownState) << endl;
	std::cout << "ownUsers: " << getUsersLiteral(agentState.ownUsers) << endl;
	std::cout << "bestState: " << getFuzzyStateLiteral(agentState.bestState) << endl;
	std::cout << "bestUsers: " << getUsersLiteral(agentState.bestUsers) << endl;
	std::cout << "action: " << agentState.changeAP << endl;


	return actionIndex;
}

/**
 * Gets the max future action and its Q-value
 *
 *
 */

void RLagent::getMaxFuture(int currentState, int ownUsers, int newState, int bestUsers, double &maxFuture, bool &futureAction)
{

	bool action;
	std::vector <double> qVector;
	double candidateValue = -1;
	double qvalue = 0.0;
	bool maxFutureAction;
	transferInfo *futureTransfer;

	//for(int i = 0; i < (int)probVector.size(); i++)
	for(int i = 0; i < 2 ; i++)
	{

		if(i == 0)
			action = false;
		else
			action = true;

		futureTransfer = lookupTransfer(currentState, ownUsers, newState, bestUsers, action);
		qvalue = futureTransfer->Qvalue;
		if(candidateValue == -1 || qvalue > candidateValue)
		{
			candidateValue = qvalue;
			maxFutureAction = action;
		}

	}

	maxFuture = candidateValue;
	futureAction = maxFutureAction;


	//maxFuture = *max_element(qVector.begin(), qVector.end());


	return;
}

void RLagent::getPureMaxFuture(int ownSSClassified, int ownLossClassified, int ownUsersClassified, int bestSSClassified, int bestLossClassified, int bestUsersClassified,
		double &maxFuture, bool &futureAction)
{

	bool action;
	std::vector <double> qVector;
	double candidateValue = -1;
	double qvalue = 0.0;
	bool maxFutureAction;
	pureTransferInfo *futureTransfer;

	//for(int i = 0; i < (int)probVector.size(); i++)
	for(int i = 0; i < 2 ; i++)
	{

		if(i == 0)
			action = false;
		else
			action = true;

		futureTransfer = lookupPureTransfer(ownSSClassified, ownLossClassified, ownUsersClassified, bestSSClassified, bestLossClassified,
				bestUsersClassified, action);
		qvalue = futureTransfer->Qvalue;
		if(candidateValue == -1 || qvalue > candidateValue)
		{
			candidateValue = qvalue;
			maxFutureAction = action;
		}

	}

	maxFuture = candidateValue;
	futureAction = maxFutureAction;


	//maxFuture = *max_element(qVector.begin(), qVector.end());


	return;
}




//Looks up Transfer from the transfer list
RLagent::transferInfo *RLagent::lookupTransfer(int curFuzzy, int curUsers, int bestState, int bestUsers, bool change)
{

	int test;

    for (TransferList::iterator it=transferList.begin(); it!=transferList.end(); ++it)
    {
//        if (it->ownState == curFuzzy && it->ownUsers == curUsers && it->bestState == bestState && it->bestUsers == bestUsers && it->probability == probability)
//        {
//            return &(*it);
//        }

        if ((it->ownState == curFuzzy) && (it->ownUsers == curUsers) && (it->bestState == bestState) && (it->bestUsers == bestUsers))
        {
        	test = it->changeAP;
        	if(test == change)
        	{
        		return &(*it);
        	}

        }
    }
    return NULL;
}

//Looks up Transfer from the transfer list
RLagent::pureTransferInfo *RLagent::lookupPureTransfer(int ownSSClassified, int ownLossClassified, int ownUsersClassified, int bestSSClassified,
		int bestLossClassified, int bestUsersClassified, bool change)
{

	int test;

    for (PureTransferList::iterator it=pureTransferList.begin(); it!=pureTransferList.end(); ++it)
    {

        if ((it->ownSS == ownSSClassified) && (it->ownLoss == ownLossClassified) && (it->ownUsers == ownUsersClassified)
        		&& (it->bestSS == bestSSClassified) && (it->bestLoss == bestLossClassified) && (it->bestUsers == bestUsersClassified))
        {
        	test = it->changeAP;
        	if(test == change)
        	{
        		return &(*it);
        	}

        }
    }
    return NULL;
}



void RLagent::finish()
{
	recordScalar("numberOfChanges", numberOfChanges);
	recordScalar("numberOfStays", numberOfStays);
	stats->addStays(numberOfStays);


}
