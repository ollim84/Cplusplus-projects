//=============================================================================
// Name        : eventmanager.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2009 VTT. All Rights Reserved.
// Description : EventManager class implementation.
//=============================================================================

#include "stdafx.h"
#include "eventmanagerex.h"
#include "sample.h"

#ifdef linux
#include "LoadBalancing.h"
#endif

//extern bool UseSubscriptionTypeInClientRanking;
//extern bool UseLearning;
//extern int PoAMargin;
//extern int PoARankingLimit;
//extern int ClientsPoAGradingLimit;
//extern string SinglePoALoadBalancing;
//extern int SimultaneousHOsPerCongestedPoA;

EventManagerEx::EventManagerEx(void):
pExpertSystem(0)
{
}

EventManagerEx::EventManagerEx(map<string, string>& poa_types, string learning_fname):
pExpertSystem(0)
{
	pExpertSystem = new ExpertSystem();
	pExpertSystem->Initialize(poa_types, learning_fname);
}

EventManagerEx::~EventManagerEx(void)
{
	if (pExpertSystem) delete pExpertSystem;
}

vector<Event> EventManagerEx::SampleProcessing(SampleEx& poa_sample, SampleEx& client_sample, map<string, int>& subscription_types)
{
	vector<Event> ho_recommendations;

	if (!pExpertSystem) 
	{
		stringstream smsg;
		smsg << "Error: Expert system not defined" << endl;
		throw logic_error(smsg.str());
	}

//	std::cout << "Event manager: " << endl;
//	std::cout << UseSubscriptionTypeInClientRanking << endl;
//	std::cout << UseLearning << endl;
//	std::cout << PoAMargin << endl;
//	std::cout << PoARankingLimit << endl;
//	std::cout << ClientsPoAGradingLimit << endl;
//	std::cout << SinglePoALoadBalancing << endl;
//	std::cout << SimultaneousHOsPerCongestedPoA << endl;

	//rank clients -> best candidates to be moved are in the beginning of the vector
	vector<pair<string, pair<int, double> > > client_ranking;

	// client_ranking[k].first = Client ID (string), e.g. wirelessHost[0]
	// client_ranking[k].second.first = Client user class (3, 2, 1)
	// client_ranking[k].second.second = Client's potential for handover ( 0 - 5)

	map<string, double> required_bw;
	double client_satisfaction = 0;
	client_ranking = pExpertSystem->ExecuteClientRanking(client_sample, subscription_types, required_bw, client_satisfaction, UseSubscriptionTypeInClientRanking, GoldUsersPreferred);

	//rank PoAs -> best PoA candidates for serving the new clients are in the beginning of the vector
	//vector<pair<string, pair<int, double> > > poa_ranking;
	vector<pair<pair<string, vector<int> >, pair<int, double> > > poa_ranking;
	// poa_ranking[j].first.first = PoA MAC address
	// poa_ranking[j].first.second[] = Number of Gold [0], Silver [1] and Bronze [2] users
	// poa_ranking[j].second.first = congestion (0 (false), 1 (true) )
	// poa_ranking[j].second.second = PoA potential (to accept clients) value (0 - 5, absolute not rounded value, e.g. 4.626)




	map<string, vector<double> > capacity;
	double network_status = 0;
	poa_ranking = pExpertSystem->ExecutePoARanking(poa_sample, capacity, network_status);

	//check some limits and rules
	string handled_poa = SinglePoALoadBalancing;
	bool blearning = UseLearning;
	if (blearning)
	{
		pExpertSystem->GiveFeedback(poa_sample, network_status, client_satisfaction);
	}

	//select a client or number of clients in congested PoAs and find appropriate PoA(s) for them for HO
	map<string, double>::const_iterator rbw_itr;
	map<string, vector<double> >::const_iterator maxcap_itr, abw_itr;

	map<string, vector<double> >::iterator cap_itr; // Capacity iterator used to substract capacity

	for (int m = poa_ranking.size()-1; m >= 0; m--)	//go through the PoA ranking list from the last one to the first one and find the congested PoAs (should be at the end of the vector)			
	{

//				cout << "poa_ranking[m].first: "<< poa_ranking[m].first << endl;
//				cout << "poa_ranking[m].second.first: "<< poa_ranking[m].second.first << endl;
//				cout << "poa_ranking[m].second.second: "<< poa_ranking[m].second.second << endl;


		if (poa_ranking[m].second.first == 1)	//congested PoA found
		{
			if (!handled_poa.empty() && poa_ranking[m].first.first.find(handled_poa) == string::npos) continue; //distributed load balancing is used and the PoA is not the handled one
	
			double relBW = DBL_MAX;
			if (blearning)
			{
				maxcap_itr = capacity.find(poa_ranking[m].first.first);
				if (maxcap_itr == capacity.end() || maxcap_itr->second.size() != 3) continue;	//skip the PoA if capacity info is not awailable
				relBW = pExpertSystem->ChooseAnAction(poa_ranking[m].first.first, maxcap_itr->second[0]);
			}			
			int nbofHOs = 0;
			double cumBW = 0;
			string congested_ap_id = poa_ranking[m].first.first;
			for (unsigned int i = 0; i < client_ranking.size(); i++)	//go through the vector of clients in the order of ranking
			{
				string client_id = client_ranking[i].first;

				string client_qoe_info = client_sample.GetValue(client_id, "Connection state");
				pair<string, double> client_qoe = client_sample.ParseClientQoE(client_qoe_info);

				string clients_current_poa = client_qoe.first;

				bool searchComplete = false;

				if (clients_current_poa == congested_ap_id)		//the client belongs to the congested PoA
				{
					string clients_poa_grading_list = client_sample.GetValue(client_id, "PoA list");
					vector<pair<string, double> > clients_poa_grading = client_sample.ParseClientsPoAGrading(clients_poa_grading_list);
					// clients_poa_grading[k].first = PoA MAC Address
					// clients_poa_grading[k].second = PoA Grading (0 - 5, floor rounded value)


					if (clients_poa_grading.empty()) continue;

					rbw_itr = required_bw.find(client_id);
					if (rbw_itr == required_bw.end()) continue;	//skip the client if bw requirement is not awailable

					bool HO_rule_made = false;
					vector<string> new_poas_for_client;
					cout << "HO for this client: " << client_ranking[i].first << " " << client_ranking[i].second.second << endl; 
					cout << "Client user class: " << client_ranking[i].second.first << endl;
					cout << "To one of these PoAs: " << endl;
					for (unsigned int j = 0; j < poa_ranking.size(); j++)	//go through the PoA ranking list and search for the PoAs which are good candidates for HO
					{
						if(searchComplete == true) break; // Stop search if a suitable AP has already been found

						if (poa_ranking[j].first.first == congested_ap_id) continue;

						abw_itr = capacity.find(poa_ranking[j].first.first);
						cap_itr = capacity.find(poa_ranking[j].first.first);

						if (abw_itr == capacity.end() || abw_itr->second.size() != 3) continue;	//skip the PoA if awailable capacity info is not awailable

						std::cout << "Number of gold users: " << poa_ranking[j].first.second[0] << endl;
						std::cout << "Number of silver users: " << poa_ranking[j].first.second[1] << endl;
						std::cout << "Number of bronze users: " << poa_ranking[j].first.second[2] << endl;

						//if(poa_ranking[j].first.second[0] != 0) continue; // skip PoA if there are more than 1 Gold users present 0, 1, 2

						cout << "client ID: "<< rbw_itr->first << endl;
						cout << "client required BW: "<< rbw_itr->second << endl;
						cout << "PoA ID: "<< poa_ranking[j].first.first << endl;
						cout << "PoA maximum BW: "<< abw_itr->second[0] << endl;
						cout << "PoA consumed BW: "<< abw_itr->second[1] << endl;
						cout << "PoA available BW: "<< abw_itr->second[2] << endl;
						cout << "PoAMargin/8: "<< PoAMargin/8 << endl;
						if (rbw_itr->second > (abw_itr->second[2] - PoAMargin/8)) continue;	//skip the PoA if client requires more bandwidth than the proposed PoA can offer

						cout << "poa_ranking[j].second.first: "<< poa_ranking[j].second.first << endl;
						cout << "poa_ranking[j].second.second: "<< poa_ranking[j].second.second << endl;
						cout << "PoARankingLimit: "<< PoARankingLimit<< endl;

						// Comparison with absolute (poa ranking) and int value (limit)
						bool poa_ranking_lower_than_allowed = (poa_ranking[j].second.second < PoARankingLimit) ? true : false;
						for (unsigned int k = 0; k < clients_poa_grading.size(); k ++)	//search whether the client can see the proposed PoA
						{
							if (clients_poa_grading[k].first == congested_ap_id) continue; // do not select AP which is congested

							bool clients_poa_grading_lower_than_allowed = (clients_poa_grading[k].second < ClientsPoAGradingLimit) ? true : false;

//							cout << "clients_poa_grading[k].first: "<< clients_poa_grading[k].first << endl;
							cout << "clients_poa_grading[k].second: "<< clients_poa_grading[k].second << endl;
							cout << "ClientsPoAGradingLimit: "<< ClientsPoAGradingLimit << endl;
//							cout << "poa_ranking[j].first: "<< poa_ranking[j].first << endl;


							if (clients_poa_grading[k].first == poa_ranking[j].first.first &&	//client can see the proposed PoA
								!clients_poa_grading_lower_than_allowed && 				//client values the PoA high enough
								!poa_ranking_lower_than_allowed)						//the PoA is a good candidate for the HO
							{
								cout << "\t" << clients_poa_grading[k].first << " accepted: (" << poa_ranking[j].second.second << ", " << clients_poa_grading[k].second << ") " << endl;
								new_poas_for_client.push_back(clients_poa_grading[k].first);
								cout << "\t"  << "PoA available BW: "<< abw_itr->second[2] << endl;
								cout << "\t"  << "PoA available BW cap: "<< cap_itr->second[2] << endl;
								cout << "\t"  << "client required BW: "<< rbw_itr->second << endl;

								// Substract client's required BW from the capacity
								cap_itr->second[2] = cap_itr->second[2] - rbw_itr->second;

								HO_rule_made = true;
								// Search is now complete since a suitable AP was found.
								searchComplete = true;

							}
							else if (clients_poa_grading_lower_than_allowed)	//no more potential PoAs in client's ranking list
							{
								cout << "\t" << clients_poa_grading[k].first << " rejected: client's PoA grading lower than allowed " << "(" << clients_poa_grading[k].second << ")" << endl;
								break;
							}
						}
						if (poa_ranking_lower_than_allowed)		//no more potential PoAs in PoA ranking vector
						{
								cout << "\t" << poa_ranking[j].first.first << " rejected: PoA ranking lower than allowed " << "(" << poa_ranking[j].second.second << ")" << endl;
							break;
						}
					}
					if (HO_rule_made) 
					{
						nbofHOs++;
						cumBW += rbw_itr->second;

						AddEvent(ho_recommendations, "Handover recommendation", poa_ranking[m].first.first, client_ranking[i].first, client_ranking[i].second.first, new_poas_for_client);
					}
				}
				if (!blearning) //if learning is not used, use fixed amount of HOs to be made in one step
				{
					if (nbofHOs >= SimultaneousHOsPerCongestedPoA) 
					{
						break;
					}
				}
				else //if learning is used, try to release the required amount of BW in one step
				{	
					if (cumBW >= relBW) 
					{
						break;
					}
				}
			}
			if (blearning) 
			{
				pExpertSystem->UpdateNbOfHOs(poa_ranking[m].first.first, nbofHOs);
				//Action vaihdetaan toteutuneen mukaiseksi jos ei pystyt� vapauttamaan riitt�v�sti kaistaa tai kaistaa vapautetaan enemm�n kuin oli tarkoitus
				//Jos ei tehd� n�in, niin palaute tulee v��r��n actioniin
				if (cumBW != relBW) pExpertSystem->UpdateAction(poa_ranking[m].first.first, cumBW, maxcap_itr->second[0]);
			}
		}
		else 
		{
			break; //no more congested PoAs
		}
	}

	return ho_recommendations;
}

void EventManagerEx::AddEvent(vector<Event>& events, const string name, const string source, const string target, const int type, const vector<string> vdata)
{
	double time_now = 0;

	Event evnt(name, source, time_now, type, "");

	if (vdata.size() <= 0) return;
	stringstream ss;
	ss << target << ";";
	for (unsigned int k = 0; k < vdata.size(); ++k)
	{
		ss << vdata[k];
		if (k < vdata.size()-1) ss << ",";
	}
	evnt.SetValue(ss.str());
	evnt.SetEventType(type);

	events.push_back(evnt);
}
