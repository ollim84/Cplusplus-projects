//=============================================================================
// Name        : expertsystem.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : ExpertSystem class implementation.
//=============================================================================

#include "stdafx.h"
#include <algorithm>
#include "expertsystem.h"
#include "learning.h"

#ifdef linux
#include "LoadBalancing.h"
#endif

//extern int ClientSatisfactionLimit;
//extern int PoAStatusLimit;

ExpertSystem::ExpertSystem():
ClientRanking(),
PoARanking(),
Learning()
{
}

ExpertSystem::~ExpertSystem(void)
{
}

bool ExpertSystem::Initialize(map<string, string>& poa_types, string learning_fname)
{
	bool bsuccess = true;

	if (!ClientRanking.InitializeEngine()) bsuccess = false;
	if (!PoARanking.InitializeEngine()) bsuccess = false;

	map<string, string>::const_iterator itr;
	for (itr = poa_types.begin(); itr != poa_types.end(); ++itr) 
	{
		LearningComponent learning_component(itr->second, learning_fname);
		Learning[itr->first] = learning_component;
		Learning[itr->first].StartLearning();
	}
	
	return bsuccess;
}

void ExpertSystem::CloseExpertSystem()
{
	ClientRanking.CloseEngine();
	PoARanking.CloseEngine();

	map<string, LearningComponent>::iterator itr;
	for (itr = Learning.begin(); itr != Learning.end(); ++itr) 
	{
		itr->second.StopLearning();
	}
}

// The second elements are used to sort the items from the highest to the lowest value
bool ClientRankingSortFunctionUsingSecondElement(const pair<string, pair<int, double> >& a, const pair<string, pair<int, double> >& b)
{
    return b.second.second < a.second.second;
} 

// The first elements are sorted from the highest to the lowest value. If the first elements are the same, 
// the second elements are used to sort the items from the highest to the lowest.
bool ClientRankingSortFunctionUsingBothElements(const pair<string, pair<int, double> >& a, const pair<string, pair<int, double> >& b)
{

	// Gold users preferred
	if ( a.second.first < b.second.first ) return false;
    if ( b.second.first < a.second.first ) return true;
    return b.second.second < a.second.second;

}

bool ClientRankingSortFunctionBronze(const pair<string, pair<int, double> >& a, const pair<string, pair<int, double> >& b)
{
	// Bronze users preferred
	if ( b.second.first < a.second.first ) return false;
    if ( a.second.first < b.second.first ) return true;
    return a.second.second < b.second.second;

} 

vector<pair<string, pair<int, double> > > ExpertSystem::ExecuteClientRanking(const SampleEx& sample, map<string, int>& subscription_types,
																	map<string, double>& required_bw, double& client_satisfaction, bool use_subscr_type, bool goldPreferred)
{
	cout << "Client ranking:" << endl; 
	cout << "use_subscr_type: " << use_subscr_type << endl;

	vector<pair<string, pair<int, double> > > client_ranking;
	int nclients_qoe_under_limit = 0;
	int nclients = 0;
	vector<double> values;
	map< string, vector<Event> >::const_iterator itr;
	for (itr = sample.Items.begin(); itr != sample.Items.end(); ++itr) //go through all clients
	{
		try
		{
			if (sample.IsSourceSleeping(itr->first)) continue; //ignore sleeping sources

			int subscription_type = subscription_types.find(itr->first)->second;
			//cout << "subscription_type: " << subscription_type << endl;
			values.clear();
			values.assign(4, NO_VALUE);
			for (unsigned int j = 0; j < itr->second.size(); ++j) //update variables from event's values
			{
				if (itr->second[j].GetName().find("Connection state") != string::npos)
				{
					pair<string, double> client_qoe = sample.ParseClientQoE(itr->second[j].GetValue());
					values[0] = client_qoe.second;
					if (client_qoe.second < ClientSatisfactionLimit) nclients_qoe_under_limit++;
				}
				if (itr->second[j].GetName().find("PoA list") != string::npos)  
				{
					values[1] = sample.ParseNbOfAvailablePoAs(itr->second[j].GetValue());
				}
				if (itr->second[j].GetName().find("RSSI/Noise") != string::npos)  
				{	
					pair<string, vector<int> > rssinoise = sample.ParseRSSINoise(itr->second[j].GetValue());
					values[2] = rssinoise.second[0]; //scaled between 0 - 100
					values[3] = ::abs(rssinoise.second[3]) - ::abs(rssinoise.second[1]); //from the original values SNR(dB) = RSSI(dBm) + Noise(dBm)
//					cout << "Signal strength: " << rssinoise.second[0] << endl;
//					cout << "RSSI(dBm): " << rssinoise.second[1] << endl;
//					cout << "Noise(dBm): " << rssinoise.second[3] << endl;
//					cout << "SNR(dB): " << values[3] << endl;
				}
				if (itr->second[j].GetName().find("BW requirement") != string::npos)
				{	
					double rbw = atof(sample.GetValue(itr->first, "BW requirement").c_str());
					required_bw[itr->first] = rbw/8; //kbps -> kBps
				}
			}
			if (values.size() == 4 && values[0] != NO_VALUE && values[1] != NO_VALUE && values[2] != NO_VALUE && values[3] != NO_VALUE) 
			{
				cout << itr->first << " "; 
				double res = ClientRanking.ExecuteAlgorithm(values); //execute ranking, res = potential for HO?
				client_ranking.push_back(make_pair(itr->first, make_pair(subscription_type, res)));
			}
			nclients++;
		}
		catch (exception &e)
		{
			printf("Warning: %s. Client %s ignored in ranking.", e.what(), itr->first.c_str());
		}
	}

//	cout << "CLIENT RANKING BEFORE SORT: " << endl;
//	for (unsigned int i = 0; i < client_ranking.size(); i++)
//	{
//		cout << "Client ID: " << client_ranking[i].first << endl;
//		cout << "Client user class: " << client_ranking[i].second.first << endl;
//		cout << "Client's potential for handover: " << client_ranking[i].second.second << endl;
//	}

	// Sort clients

	// User classes are used
	if (use_subscr_type) 
	{
		// Gold users preferred over silver and bronze
		if(goldPreferred == true)
		{
			sort(client_ranking.begin(), client_ranking.end(), ClientRankingSortFunctionUsingBothElements);
		}
		// Bronze users preferred over silver and gold
		else
		{
			sort(client_ranking.begin(), client_ranking.end(), ClientRankingSortFunctionBronze);
		}
	}
	else
	{
		sort(client_ranking.begin(), client_ranking.end(), ClientRankingSortFunctionUsingSecondElement);
	}

//	cout << "CLIENT RANKING AFTER SORT: " << endl;
//	for (unsigned int i = 0; i < client_ranking.size(); i++)
//	{
//		cout << "Client ID: " << client_ranking[i].first << endl;
//		cout << "Client user class: " << client_ranking[i].second.first << endl;
//		cout << "Client's potential for handover: " << client_ranking[i].second.second << endl;
//	}

	client_satisfaction = (nclients > 0) ? nclients_qoe_under_limit/nclients*100 : 0;

	return client_ranking;
}

// The first element sorted from the lowest to the highest value. If the first elements are the same,
// the second elements are used to sort the items from the highest to the lowest value

//ORIGINAL:
//bool PoARankingSortFunction(const pair<string, pair<int, double> >& a, const pair<string, pair<int, double> >& b)
//{
//	if ( a.second.first < b.second.first ) return true;
//    if ( b.second.first < a.second.first ) return false;
//    return b.second.second < a.second.second;
//}

bool PoARankingSortFunction(const pair<pair<string, vector<int> >, pair<int, double> >& a, const pair<pair<string, vector<int> >, pair<int, double> >& b)
{
	if ( a.second.first < b.second.first ) return true;
    if ( b.second.first < a.second.first ) return false;
    return b.second.second < a.second.second;
} 

// PoA ranking function that takes into account the user classes
bool PoARankingSortFunctionUserclasses(const pair<pair<string, vector<int> >, pair<int, double> >& a, const pair<pair<string, vector<int> >, pair<int, double> >& b)
{
	// Congestion (congested APs to the bottom of the list)
	if ( a.second.first < b.second.first ) return true;
    if ( b.second.first < a.second.first ) return false;


    // Capacity to accept clients (low capacity APs to the bottom)
	if ( a.second.second > b.second.second ) return true;
    if ( b.second.second > a.second.second ) return false;


    // Number of gold users (high number of gold users to the bottom)
	if ( a.first.second[0] < b.first.second[0] ) return true;
    if ( b.first.second[0] < a.first.second[0] ) return false;
    // Number of silver users
	if ( a.first.second[1] < b.first.second[1] ) return true;
    if ( b.first.second[1] < a.first.second[1] ) return false;
    // Number of bronze users
	if ( a.first.second[2] < b.first.second[2] ) return true;
    if ( b.first.second[2] < a.first.second[2] ) return false;

    // Capacity to accept clients
    return b.second.second < a.second.second;
}

//vector<pair<string, pair<int, double> > > ExpertSystem::ExecutePoARanking(const SampleEx& sample, map<string,
//												vector<double> >& capacity, double& network_status)
vector<pair<pair<string, vector<int> >, pair<int, double> > > ExpertSystem::ExecutePoARanking(const SampleEx& sample, map<string,
												vector<double> >& capacity, double& network_status)
{
	cout << "PoA ranking:" << endl; 

	//vector<pair<string, pair<int, double> > > poa_ranking;
	vector<pair<pair<string, vector<int> >, pair<int, double> > > poa_ranking;

	int npoas_status_under_limit = 0;
	int npoas = 0;
	vector<double> values;
	vector<int> userClasses;

	map< string, vector<Event> >::const_iterator itr;
	int numGold, numSilver, numBronze = 0;


	for (itr = sample.Items.begin(); itr != sample.Items.end(); ++itr) //go through all PoAs
	{
		try
		{
			if (sample.IsSourceSleeping(itr->first)) continue; //ignore sleeping sources

			int is_congested = 0;
			values.clear();
			userClasses.clear();
			userClasses.assign(3, NO_VALUE);
			values.assign(3, NO_VALUE);
			for (unsigned int j = 0; j < itr->second.size(); ++j) //update variables from event's values
			{
				if (itr->second[j].GetName().find("Congestion") == 0) 
				{
					is_congested = atoi(itr->second[j].GetValue().c_str());
					values[2] = is_congested;
				}
				if (itr->second[j].GetName().find("PoA status") == 0) 
				{
					values[0] = atof(itr->second[j].GetValue().c_str());
					if (values[0] < PoAStatusLimit) npoas_status_under_limit++;
				}
				if (itr->second[j].GetName().find("Capacity") == 0)
				{
					vector<double> capcty = sample.ParseCapacity(itr->second[j].GetValue());
					values[1] = (capcty[2]*8/1000); //Available BW, kBps -> Mbps
					capacity[itr->first] = capcty; //capacity kBps
				}
				// User classes
				if (itr->second[j].GetName().find("NumGold") == 0)
				{
					numGold = atoi(itr->second[j].GetValue().c_str());
					userClasses[0] = numGold;
				}
				if (itr->second[j].GetName().find("NumSilver") == 0)
				{
					numSilver = atoi(itr->second[j].GetValue().c_str());
					userClasses[1] = numSilver;
				}
				if (itr->second[j].GetName().find("NumBronze") == 0)
				{
					numBronze = atoi(itr->second[j].GetValue().c_str());
					userClasses[2] = numBronze;
				}
			}
			if (values.size() == 3 && values[0] != NO_VALUE && values[1] != NO_VALUE&& values[2] != NO_VALUE) 
			{
				cout << itr->first << " "; 
				double res = PoARanking.ExecuteAlgorithm(values); //execute ranking
				//poa_ranking.push_back(make_pair(itr->first, make_pair(is_congested, res)));
				poa_ranking.push_back(make_pair(make_pair(itr->first, userClasses), make_pair(is_congested, res)));

			}
			npoas++;
		}
		catch (exception &e)
		{
			printf("Warning: %s. PoA %s ignored in ranking.", e.what(), itr->first.c_str());
		}
	}

//	cout << "POA RANKING TEMP BEFORE: " << endl;
//	for (int m = 0; m < poa_ranking.size(); m++)
//	{
//		cout << "poa ID: "<< poa_ranking[m].first.first << endl;
//		cout << "poa numGold: "<< poa_ranking[m].first.second[0] << endl;
//		cout << "poa numSilver: "<< poa_ranking[m].first.second[1] << endl;
//		cout << "poa numBronze: "<< poa_ranking[m].first.second[2] << endl;
//		cout << "poa congestion: "<< poa_ranking[m].second.first << endl;
//		cout << "poa capacity: "<< poa_ranking[m].second.second << endl;
//	}

	//Rank of PoAs also according to the userClasses
	sort(poa_ranking.begin(), poa_ranking.end(), PoARankingSortFunctionUserclasses);


	//sort(poa_ranking.begin(), poa_ranking.end(), PoARankingSortFunction);

//	cout << "POA RANKING TEMP AFTER: " << endl;
//	for (int m = 0; m < poa_ranking.size(); m++)
//	{
//		cout << "poa ID: "<< poa_ranking[m].first.first << endl;
//		cout << "poa numGold: "<< poa_ranking[m].first.second[0] << endl;
//		cout << "poa numSilver: "<< poa_ranking[m].first.second[1] << endl;
//		cout << "poa numBronze: "<< poa_ranking[m].first.second[2] << endl;
//		cout << "poa congestion: "<< poa_ranking[m].second.first << endl;
//		cout << "poa capacity: "<< poa_ranking[m].second.second << endl;
//	}

	network_status = (npoas > 0) ? npoas_status_under_limit/npoas*100 : 0;

	return poa_ranking;
}

double ExpertSystem::ChooseAnAction(string current_poa, double max_capacity)
{ 
	int action = Learning[current_poa].ChooseAnAction();
	cout << "Action for " << current_poa << ": " << action << endl;

	return (action*5*max_capacity/100);
}

void ExpertSystem::GiveFeedback(const SampleEx& poa_sample, double network_status, double client_satisfaction)
{
	//k�yd��n l�pi kaikki tukiasemat, joille tehtiin h�nd�reit� ja lasketaan rewardit niist� actioneista, joita niille tehtiin
	map<string, LearningComponent>::iterator itr;
	for (itr = Learning.begin(); itr != Learning.end(); ++itr) //go through all PoAs
	{
		try
		{
			if (poa_sample.IsSourceSleeping(itr->first))	//ignore sleeping sources
			{
				cout << itr->first << " stopped waiting feedback (sleeping)." << endl;
				itr->second.CancelWaitingFeedback();
				continue;
			}
			if (!itr->second.IsWaitingFeedback()) continue; //ignore PoAs that aren't waiting for feedback

			string poa_status_str = poa_sample.GetValue(itr->first, "PoA status").c_str();
			if (poa_status_str.empty())						//problem occured so PoA satus cannot be found -> stop waiting feedback
			{
				cout << itr->first << " stopped waiting feedback (unknown PoA status)." << endl;
				itr->second.CancelWaitingFeedback();
				continue;
			}

			cout << "Feedback for " << itr->first << ":" << endl;
			int poa_status = atoi(poa_status_str.c_str());
			itr->second.GiveFeedback(poa_status, network_status, client_satisfaction);
		}
		catch (exception &e)
		{
			printf("Warning: %s. PoA %s ignored in giving feecback.", e.what(), itr->first.c_str());
		}
	}
}

void ExpertSystem::UpdateNbOfHOs(string current_poa, int nb_of_hos)
{
	map<string, LearningComponent>::iterator lc_itr;
	lc_itr = Learning.find(current_poa);
	if (lc_itr == Learning.end()) return;	//return if (for some reason) a learning component for that poa cannot be found

	lc_itr->second.UpdateNbOfHOs(nb_of_hos);
}

void ExpertSystem::UpdateAction(string current_poa, double cum_bw, double max_capacity)
{
	map<string, LearningComponent>::iterator lc_itr;
	lc_itr = Learning.find(current_poa);
	if (lc_itr == Learning.end()) return;	//return if (for some reason) a learning component for that poa cannot be found

	int action = 0;
	if (cum_bw > 0)
	{
		action = (int)(cum_bw/max_capacity*100/5 - EPS) + 1;
		int max_action = lc_itr->second.GetNbOfActions() - 1;
		if (action > max_action) action = max_action;
	}
	lc_itr->second.UpdateAction(action);
}
