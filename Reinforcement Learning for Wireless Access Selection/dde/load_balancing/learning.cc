//=============================================================================
// Name        : learning.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : LearningComponent class implementation.
//=============================================================================

#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <math.h> 
#include "learning.h"
#include "random.h"
#include "cmn_utils.h"

using namespace std;

LearningComponent::LearningComponent():
poaType("unknown"),
bWaitingFeedback(false),
nStates(LEARNING_NB_OF_NETWORK_STATUSES*LEARNING_NB_OF_POA_STATUSES),
nActions(LEARNING_NB_OF_ACTIONS),
currentState(0),
nextState(0),
chosenAction(0),
//nextAction(0),
R(),
Q(),
explorationType("boltzmann"),
alpha(0.1),
epsilon(0.2),
gamma(0.8),
tau(1),
//savingCycle(10),
savingCycle(1),
nextPoAStatus(0),
nextNetworkStatus(0),
nextClientSatisfaction(0),
currentPoAStatus(0),
currentNetworkStatus(0),
currentClientSatisfaction(0),
nbOfHOs(0)
{
	qfile = "q_" + poaType + ".txt";
}

LearningComponent::LearningComponent(string poa_type, string fname): //ToDo: muutkin parametrit tiedostosta luettaviksi
poaType(poa_type),
bWaitingFeedback(false),
nStates(LEARNING_NB_OF_NETWORK_STATUSES*LEARNING_NB_OF_POA_STATUSES),
nActions(LEARNING_NB_OF_ACTIONS),
currentState(0),
nextState(0),
chosenAction(0),
//nextAction(0),
R(),
Q(),
explorationType("boltzmann"),
alpha(0.1),
epsilon(0.2),
gamma(0.8),
tau(1),
//savingCycle(10),
savingCycle(1),
nextPoAStatus(0),
nextNetworkStatus(0),
nextClientSatisfaction(0),
currentPoAStatus(0),
currentNetworkStatus(0),
currentClientSatisfaction(0),
nbOfHOs(0)
{
	string part1 = ::ExtractFileNameWithoutExtension(fname);
	string ext = ::ExtractFileExtension(fname);
	qfile = part1 + "_" + poaType + ext;
}

LearningComponent::~LearningComponent(void)
{
}

void LearningComponent::StartLearning()
{
	//InitializeRMatrix();
	InitializeQMatrix();
}

void LearningComponent::StopLearning()
{
	SaveQMatrix();
}

void LearningComponent::InitializeRMatrix()
{
	//vector<int> line;
	//line.push_back(-1);
	//line.push_back(0);
	//line.push_back(2);
	//line.push_back(1);
	//line.push_back(0);
	//line.push_back(-1);
	//line.push_back(-2);
	//line.push_back(-3);
	//line.push_back(-4);
	//line.push_back(-5);
	//line.push_back(-6);
	//R.push_back(line);
	//line.clear();

	//line.push_back(0);
	//line.push_back(2);
	//line.push_back(1);
	//line.push_back(0);
	//line.push_back(-1);
	//line.push_back(-2);
	//line.push_back(-3);
	//line.push_back(-4);
	//line.push_back(-5);
	//line.push_back(-6);
	//line.push_back(-7);
	//R.push_back(line);
	//line.clear();

	//line.push_back(2);
	//line.push_back(1);
	//line.push_back(0);
	//line.push_back(-1);
	//line.push_back(-2);
	//line.push_back(-3);
	//line.push_back(-4);
	//line.push_back(-5);
	//line.push_back(-6);
	//line.push_back(-7);
	//line.push_back(-8);
	//R.push_back(line);
	//line.clear();

	//line.push_back(4);
	//line.push_back(0);
	//line.push_back(-1);
	//line.push_back(-2);
	//line.push_back(-3);
	//line.push_back(-4);
	//line.push_back(-5);
	//line.push_back(-6);
	//line.push_back(-7);
	//line.push_back(-8);
	//line.push_back(-9);
	//R.push_back(line);
	//line.clear();
	//
	//line.push_back(6);
	//line.push_back(-1);
	//line.push_back(-2);
	//line.push_back(-3);
	//line.push_back(-4);
	//line.push_back(-5);
	//line.push_back(-6);
	//line.push_back(-7);
	//line.push_back(-8);
	//line.push_back(-9);
	//line.push_back(-10);
	//R.push_back(line);
	//line.clear();

	//line.push_back(8);
	//line.push_back(-2);
	//line.push_back(-3);
	//line.push_back(-4);
	//line.push_back(-5);
	//line.push_back(-6);
	//line.push_back(-7);
	//line.push_back(-8);
	//line.push_back(-9);
	//line.push_back(-10);
	//line.push_back(-11);
	//R.push_back(line);
	//line.clear();
}

//ToDo: Q-matriisin oletusarvojen m��rittely tiedoston kautta -HKo

void LearningComponent::InitializeQMatrix()
{
	//Reward matrix
	//int Q[nStates][nActions]
	//									action (free n% of BW i.e. 0, less or equal than 5 %, less or equal than 10 %, etc.)
	//								 0	 5	10	15	20	25	30	35	40	45	50		
	//	state		0	0-0:	{{	 7,  6,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//	netw.status	1	0-1:	 {	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4,	-5,	-6	},
	//	+PoAGrade	2	0-2:	 {	 1,	 0,	-1,	-2,	-3,	-4,	-5,	-6,	-7,	-8,	-9	},
	//				3	0-3:	 {	-2,	 6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//				4	0-4:	 {	-8,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4	},
	//				5	0-5:	 { -12,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4, -5	}}

	//								 0	 5	10	15	20	25	30	35	40	45	50		
	//				6	1-0:	{{	-1,	 5,	 6,  7,  6,	 5,	 4,	 3,	 2,	 1,	 0	},
	//				7	1-1:	 {	 3,	 6,  7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1	},
	//				8	1-2:	 {	 5,	 7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2	},
	//				9	1-3:	 {	 7,	 6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//				10	1-4:	 {	 9,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4	},
	//				11	1-5:	 {	11,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4, -5	}}

	//								 0	 5	10	15	20	25	30	35	40	45	50		
	//				12	2-0:	{{	-1,	 5,	 6,  7,  6,	 5,	 4,	 3,	 2,	 1,	 0	},
	//				13	2-1:	 {	 3,	 6,  7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1	},
	//				14	2-2:	 {	 5,	 7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2	},
	//				15	2-3:	 {	 7,	 6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//				16	2-4:	 {	 9,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4	},
	//				17	2-5:	 {	11,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4, -5	}}

	//								 0	 5	10	15	20	25	30	35	40	45	50		
	//				18	3-0:	{{	-1,	 5,	 6,  7,  6,	 5,	 4,	 3,	 2,	 1,	 0	},
	//				19	3-1:	 {	 3,	 6,  7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1	},
	//				20	3-2:	 {	 5,	 7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2	},
	//				21	3-3:	 {	 7,	 6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//				22	3-4:	 {	 9,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4	},
	//				23	3-5:	 {	11,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4, -5	}}

	//								 0	 5	10	15	20	25	30	35	40	45	50		
	//				24	4-0:	{{	-1,	 5,	 6,  7,  6,	 5,	 4,	 3,	 2,	 1,	 0	},
	//				25	4-1:	 {	 3,	 6,  7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1	},
	//				26	4-2:	 {	 5,	 7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2	},
	//				27	4-3:	 {	 7,	 6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//				28	4-4:	 {	 9,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4	},
	//				29	4-5:	 {	11,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4, -5	}}

	//								 0	 5	10	15	20	25	30	35	40	45	50		
	//				30	5-0:	{{	-1,	 5,	 6,  7,  6,	 5,	 4,	 3,	 2,	 1,	 0	},
	//				31	5-1:	 {	 3,	 6,  7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1	},
	//				32	5-2:	 {	 5,	 7,  6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2	},
	//				33	5-3:	 {	 7,	 6,  5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3	},
	//				34	5-4:	 {	 9,	 5,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4	},
	//				35	5-5:	 {	11,	 4,	 3,	 2,	 1,	 0,	-1,	-2,	-3,	-4, -5	}}

	srand((unsigned)time(0));

	Q.clear();
	ReadQMatrix();
	
	//if Q matrix file doesn't exist or reading fails, initialise an empty matrix
	if (Q.empty()) 
	{
		nStates = LEARNING_NB_OF_NETWORK_STATUSES * LEARNING_NB_OF_POA_STATUSES;
		nActions =  LEARNING_NB_OF_ACTIONS;
		Q.resize(nStates);
		for (unsigned int i = 0; i < nStates; i++)
		{
			Q[i].resize(nActions);
			for (unsigned int j = 0; j < nActions; j++)
			{
				Q[i][j] = 0;
			}
		}
	}
}

int LearningComponent::GetState(double poa_status, double network_status)
{
	// Network status classification:
	// 0:   >= 90 % of PoAs heavily loaded (= under limit "PoAStatusLimit")
	// 1: 80 - 90 % of PoAs heavily loaded
	// 2: 70 - 80 % of PoAs heavily loaded
	// 3: 50 - 70 % of PoAs heavily loaded
	// 4: 30 - 50 % of PoAs heavily loaded
	// 5:    < 30 % of PoAs heavily loaded

	int ns = 0;
	if (network_status < 30) ns = 5;
	else if (network_status >= 30 && network_status < 50) ns = 4;
	else if (network_status >= 50 && network_status < 70) ns = 3;
	else if (network_status >= 70 && network_status < 80) ns = 2;
	else if (network_status >= 80 && network_status < 90) ns = 1;
	else if (network_status >= 90) ns = 0;

	int state = LEARNING_NB_OF_NETWORK_STATUSES * ns + ::Round(poa_status);
	return state;
}

void LearningComponent::SetInitialState(double poa_status, double network_status)
{
	currentPoAStatus = poa_status;
	currentNetworkStatus = network_status;

	currentState = GetState(poa_status, network_status);
}

int LearningComponent::ChooseAnAction() 
{
    //Choose a possible action connected to the current state.
	chosenAction = Explore(currentState);
	bWaitingFeedback = true;

	return chosenAction;
}

void LearningComponent::GiveFeedback(double poa_status, double network_status, double client_satisfaction)
{
	if (!bWaitingFeedback) return;

	nextState = GetState(poa_status, network_status);

	int reward = CalculateReward(poa_status, network_status, client_satisfaction);
	UpdateQMatrix(reward);

	cout << "Old state: " << currentState << "\t" << currentPoAStatus << "\t" << currentNetworkStatus << "\t" << currentClientSatisfaction << "\t" << nbOfHOs << endl;
	cout << "New state: " << nextState << "\t" << poa_status << "\t" << network_status << "\t" << client_satisfaction << endl;
	cout << "Reward: " << reward << endl;

    currentState = nextState;
	currentPoAStatus = poa_status;
	currentNetworkStatus = network_status;
	currentClientSatisfaction = client_satisfaction;
	nbOfHOs = 0;
	
	bWaitingFeedback = false;
}

int LearningComponent::CalculateReward(double poa_status, double network_status, double client_satisfaction)
{
	int reward = 0;
	
	//Tukiaseman PoA gradin pit�isi parantua
	reward += (poa_status > currentPoAStatus) ? 10 : 0;

	//HO-k�skyjen ei pit�isi aiheuttaa ongelmia niiss� tukiasemissa, joiden tilannetta muutettiin
	reward += (network_status >= currentNetworkStatus) ? 10 : 0;

	//K�ytt�j�tyytyv�isyyden pit�isi joko pysy� samana tai parantua tukiasemassa kiinni olevilla clienteilla
	reward += (client_satisfaction >= currentClientSatisfaction) ? 10 : 0;

	//Osuus k�ytt�jist�, joilla tila 0 tai 1 penaltya? 
	
	//Lis�ksi voisi olla:
	//	- Yhteydenottoyritykset soluihin, joiden tilannetta muutettiin ei pit�isi ep�onnistua
	//	- HO-k�skyjen ei pit�isi aiheuttaa ping-pong:ia -> vaatii erillisen ping-pong-laskurin/eventin

	//HOs should only be made if necessary; large number of HOs should be avoided 
	reward -= nbOfHOs * 2 - 1;

    return reward;
}

//double LearningComponent::CalculatePoASatisfaction(map<string, double> affected_poas)
//{
//	int npoas = affected_poas.size();
//	if (npoas <= 0) return 0;
//
//	double cum_poa_grade = 0;
//	map<string, double>::const_iterator itr;
//	for (itr = affected_poas.begin(); itr != affected_poas.end(); ++itr) 
//	{
//		cum_poa_grade += itr->second;
//	}
//
//	return (cum_poa_grade/npoas); //ToDo: average PoA satisfaction used so far, could be also PoA grades under a certain limit, etc.
//}
//
//double LearningComponent::CalculateClientSatisfaction(map<string, double> moved_clients)
//{
//	int nclients = moved_clients.size();
//	if (nclients <= 0) return 0;
//
//	double cum_client_qoe = 0;
//	map<string, double>::const_iterator itr;
//	for (itr = moved_clients.begin(); itr != moved_clients.end(); ++itr) 
//	{
//		cum_client_qoe += itr->second;
//	}
//
//	return (cum_client_qoe/nclients); //ToDo: average clients satisfaction used so far, could be also QoEs under a certain limit, etc.
//}

void LearningComponent::UpdateQMatrix(int reward)
{
	static int count = 0;

	//Q(x,a) = (1 - alpha) * Q(x,a) + alpha * (r + gamma * max Q(y,b)) 
	double maxQs = Max(Q[nextState]);
	Q[currentState][chosenAction] += (int)(alpha * (reward + gamma*maxQs - Q[currentState][chosenAction]));
	count++;
	
	if (count == savingCycle) 
	{
		SaveQMatrix();
		count = 0;
	}
}

//ToDo: siirr� erilliseen luokkaan -HKo
int LearningComponent::Explore(int state) 
{
    if (explorationType.compare("boltzmann") == 0) 
	{
        return Boltzmann(state);
    } 
	else if (explorationType.compare("egreedy") == 0) 
	{
        return EGreedy(state);
    } 
	else 
	{
		stringstream smsg;
		smsg << "Error: undefined exploration type: " << explorationType.c_str() << endl;
		throw logic_error(smsg.str());
    }
}

//ToDo: siirr� erilliseen luokkaan -HKo
int LearningComponent::Boltzmann(int state) 
{
    vector<int> Qs = Q[state];
    //double maxQs = Max(Qs);
	
    double sumQs = 0;
	vector<double> p;
	p.resize(Qs.size());
	for (unsigned int a = 0; a < Qs.size(); a++) 
	{
        p[a] = exp(Qs[a] / tau); //miksi joissakin esimerkeiss� k�ytetty exp((Qs[a]-maxQs)/tau)?
        sumQs += p[a];
    }

	for (unsigned int a = 0; a < p.size(); a++) 
	{
        p[a] /= sumQs;
    }

    double rnd = drand48();
    double total = p[0];
	unsigned int a = 0;
    while (total < rnd) 
	{
        a++;
        total += p[a];
        if (a >= nActions) 
		{
			stringstream smsg;
			smsg << "Error: something went wrong in Bolzman: "<< endl;
            smsg << total << " " << rnd << endl;
            for (unsigned int _a = 0; _a < nActions; _a++) 
			{
                smsg << p[_a] << " ";
            }
            smsg << endl;
            for (unsigned int _a = 0; _a < nActions; _a++) 
			{
                smsg << Qs[_a] << " ";
            }
            smsg << endl;
			throw logic_error(smsg.str());
        }
    }

	return a;
}

//ToDo: siirr� erilliseen luokkaan -HKo
int LearningComponent::EGreedy(int state) 
{
	int action;
    if (drand48() < epsilon) 
	{
        action = GetRandomAction();
    } 
	else 
	{
        action = GetMaxAction(state);
    }
	return action;
}

//ToDo: siirr� erilliseen luokkaan -HKo
int LearningComponent::GetMaxAction(int state) 
{
	int action;

    vector<int> maxA = ArgMaxAll(Q[state]);
    int n = maxA.size();
    action = maxA[(int)(n*drand48())];

	return action;
}

//ToDo: siirr� erilliseen luokkaan -HKo
int LearningComponent::GetRandomAction() 
{
    int action = (int)(nActions*drand48());
	return action;
}

//ToDo: siirr� erilliseen luokkaan -HKo
double LearningComponent::Max(vector<int> Qs)
{
    int maxX = Qs[0];
	for (unsigned int i = 1; i < Qs.size(); i++) 
	{
        if (Qs[i] > maxX) 
		{
            maxX = Qs[i];
        }
    }
    return maxX ;
}


//ToDo: siirr� erilliseen luokkaan -HKo
vector<int> LearningComponent::ArgMaxAll(vector<int> Qs) 
{
    int maxX = Qs[0];
    std::vector<int> maxI;
    maxI.push_back(0);

    for (unsigned int i = 1; i < Qs.size(); i++) 
	{
        int X = Qs[i];
        if (X > maxX) 
		{
            maxX = X;
            maxI.clear();
            maxI.push_back(i);
        } 
		else if (X == maxX) 
		{
            maxI.push_back(i);
        }
    }
    return maxI;
}

void LearningComponent::PrintOutQMatrix()
{
    //Print out Q matrix.
    for (unsigned int i = 0; i < nStates; i++)
	{
        for (unsigned int j = 0; j < nActions; j++)
		{
            cout << setw(5) << Q[i][j];
			if (j < nActions - 1)
			{
				cout << ",";
			}
		}
        cout << "\n";
	}
    cout << "\n";
}

void LearningComponent::SaveQMatrix()
{
	if (!qfile.empty())
	{
		FILE *f;
		f = fopen(qfile.c_str(), "w");
		if(!f)
		{
			fprintf(stderr, "The Q matrix file '%s' could not be opened\n", qfile.c_str());
		}

		//Save Q matrix into a file
		for (unsigned int i = 0; i < nStates; i++)
		{
			for (unsigned int j = 0; j < nActions; j++)
			{
				fprintf(f, "\t%d", Q[i][j]);	
			}
			fprintf(f, "\n");
			fflush(f);
		}
		fprintf(f, "\n");
		fflush(f);

		fclose(f);
	}
}

void LearningComponent::ReadQMatrix()
{
	if (qfile.empty()) return;

	ifstream fs;
	fs.open(qfile.c_str());

	bool bsuccess = true;
	try
	{
		//Read Q matrix from a file
		Q.clear();

		string sbuffer;
		int prev_size = 0;
		while (getline(fs, sbuffer))
		{
			vector<int> actions;
			if (sbuffer.empty() || (sbuffer[0] == '\n')) continue;
			else if (sbuffer[0] == '#') continue;
			else 
			{
				vector<string> vparts;
				::SplitString(vparts, sbuffer, '\t');
				if (prev_size != 0 && vparts.size() != prev_size)
				{
					bsuccess = false;
					stringstream smsg;
					smsg << "Error reading Q-matrix.";
					throw logic_error(smsg.str());
				}
				for (unsigned int i = 0; i < vparts.size(); i++)
				{
					actions.push_back(atoi(vparts[i].c_str()));
				}

				Q.push_back(actions);
				prev_size = vparts.size();
			}
		}
		nStates = Q.size();
		nActions = (Q.size() > 0) ? Q[0].size() : 0;
				
		fs.close();
	}
	catch (logic_error& _e)
	{
		//end of file rearched, do nothing
		
		//if an error occured, throw an exception
		if (!bsuccess)
		{
			stringstream smsg;
			smsg << "Error reading Q-matrix.";
			Q.clear();
			throw logic_error(_e.what());
		}
	}
}
