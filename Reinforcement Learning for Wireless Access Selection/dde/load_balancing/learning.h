//=============================================================================
// Name        : learning.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : LearningComponent class declaration.
//=============================================================================

#pragma once
#include <vector>

using namespace std;

const int LEARNING_NB_OF_NETWORK_STATUSES = 6;
const int LEARNING_NB_OF_POA_STATUSES = 6;
const int LEARNING_NB_OF_ACTIONS = 11;

class LearningComponent
{
public:
	LearningComponent(void);
	LearningComponent(string poa_type, string fname);
	virtual ~LearningComponent(void);

	void StartLearning();
	void StopLearning();
	int  ChooseAnAction();
	bool IsWaitingFeedback() const { return bWaitingFeedback; }
	void GiveFeedback(double poa_status, double network_status, double client_satisfaction);
	void PrintOutQMatrix();

	int GetNbOfActions() const { return nActions; }
	int GetState(double poa_status, double network_status);
	void SetInitialState(double poa_status, double network_status);
	void SetPoASatisfaction(double poa_satisfaction);
	void SetClientSatisfaction(double client_satisfaction);
	void CancelWaitingFeedback() { bWaitingFeedback = false; }
	void UpdateNbOfHOs(int nb_of_hos) { nbOfHOs = nb_of_hos; }
	void UpdateAction(int action) { chosenAction = action; }

private:
	string poaType;
	bool bWaitingFeedback;

	unsigned int nStates;
	unsigned int nActions;
	unsigned int currentState;
	unsigned int nextState;
	unsigned int chosenAction;
	//int nextAction;
	vector<vector<int> > R;
	vector<vector<int> > Q;

	string explorationType;
	double alpha;		//Learning rate. Wiki:st�: Determines to what extent the newly acquired information will override the old information. 
						//A factor of 0 will make the algorithm not learn anything, while a factor of 1 would make the algorithm consider only the most recent information. 
						//In fully deterministic environments, a learning rate of alpha_t = 1 is optimal. When the problem is stochastic, the algorithms still 
						//converges under some technical conditions on the learning rate, that require it to decrease to zero. In practice, often a constant 
						//learning rate is used, such as alpha_t = 0.1 for all t.
	double epsilon;
	double gamma;
	double tau;			//Temperature. Netist�: Tau controls the amount of exploration (the probability of executing actions other than the one with the highest Q-value). 
						//If tau is high, or if Q-values are all the same, this will pick a random action. If tau is low and Q-values are different, it will tend to
						//pick the action with the highest Q-value.

	string qfile;
	int savingCycle;

	int nbOfHOs;
	double nextPoAStatus;
	double nextNetworkStatus;
	double nextClientSatisfaction;
	double currentPoAStatus;
	double currentNetworkStatus;
	double currentClientSatisfaction;

	void InitializeRMatrix();
	void InitializeQMatrix();
	int Explore(int state);
	int Boltzmann(int state);
	int EGreedy(int state);
	int GetMaxAction(int state);
	int GetRandomAction();

	//double CalculatePoASatisfaction(map<string, double> affected_poas);		//map<"affected PoA", "PoA grade">>
	//double CalculateClientSatisfaction(map<string, double> moved_clients);	//map<"client id", "connection state">>
	void UpdatePoAGrade(double poa_grade);
	int CalculateReward(double poa_status, double network_status, double client_satisfaction);
	void UpdateQMatrix(int reward);
	void SaveQMatrix();
	void ReadQMatrix();

	double Max(vector<int> Qs);
	vector<int> ArgMaxAll(vector<int> Qs);
};
