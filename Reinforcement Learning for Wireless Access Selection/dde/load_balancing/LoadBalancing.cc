//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#include "LoadBalancing.h"


Define_Module(LoadBalancing);

 bool UseSubscriptionTypeInClientRanking;
 bool UseLearning;
 int PoAMargin;
 double PoARankingLimit;
 double ClientsPoAGradingLimit;
 string SinglePoALoadBalancing;
 int SimultaneousHOsPerCongestedPoA;
 int ClientSatisfactionLimit;
 int PoAStatusLimit;
 bool ContinuousLoadBalancing;
 int SimultaneousHOsToAvailablePoA;
 bool GoldUsersPreferred;


void LoadBalancing::initialize()
{

	ClientsPoAGradingLimit_var = par("ClientsPoAGradingLimit");
	PoARankingLimit_var =  par("PoARankingLimit");
    SimultaneousHOsPerCongestedPoA_var = par("SimultaneousHOsPerCongestedPoA");
    PoAMargin_var = par("PoAMargin");
    SinglePoALoadBalancing_var = par("SinglePoALoadBalancing");
    ClientSatisfactionLimit_var = par("ClientSatisfactionLimit");
    PoAStatusLimit_var = par("PoAStatusLimit");
    UseSubscriptionTypeInClientRanking_var = par("UseSubscriptionTypeInClientRanking");
    ContinuousLoadBalancing_var = par("ContinuousLoadBalancing");
    UseLearning_var = par("UseLearning");
    SimultaneousHOsToAvailablePoA_var = par("SimultaneousHOsToAvailablePoA");
    GoldUsersPreferred_var = par("GoldUsersPreferred");

}

vector<Event> LoadBalancing::doLoadBalancing(SampleEx poa_sample, SampleEx client_sample, map<string, int> subscription_types, map<string, string> poa_types)
{
	std::cout << "Doing load balancing!" << endl;

	//rules
	ClientsPoAGradingLimit = ClientsPoAGradingLimit_var;
	PoARankingLimit = PoARankingLimit_var;
	SimultaneousHOsPerCongestedPoA = SimultaneousHOsPerCongestedPoA_var;
	PoAMargin = PoAMargin_var;
	SinglePoALoadBalancing  =  SinglePoALoadBalancing_var;
	ClientSatisfactionLimit = ClientSatisfactionLimit_var;
	PoAStatusLimit = PoAStatusLimit_var;

	UseSubscriptionTypeInClientRanking = UseSubscriptionTypeInClientRanking_var;
	ContinuousLoadBalancing = ContinuousLoadBalancing_var;
	UseLearning = UseLearning_var;
	SimultaneousHOsToAvailablePoA = SimultaneousHOsToAvailablePoA_var;
	GoldUsersPreferred = GoldUsersPreferred_var;

	string learning_file = "nc_q.txt";

	EventManagerEx emanager(poa_types, learning_file);
	vector<Event> returnEvents;
	vector<Event> events;
	Event evnt;

	returnEvents = emanager.SampleProcessing(poa_sample, client_sample, subscription_types);

	cout << "Step 1 (action)" << endl;
	for (unsigned int i = 0; i < returnEvents.size(); i++)
	{
		cout << returnEvents[i].GetName() << " " << returnEvents[i].GetValue() << endl;
	}

//	//Step 2 (feedback)
//	evnt = Event("Congestion", "00:1A:30:6A:0D:10", 0, 0, "0");
//	events.push_back(evnt);
//	evnt = Event("PoA status", "00:1A:30:6A:0D:10", 0, 0, "5");
//	events.push_back(evnt);
//	evnt = Event("Network type", "00:1A:30:6A:0D:10", 0, 0, "WLAN");
//	events.push_back(evnt);
//	evnt = Event("Nb of active clients", "00:1A:30:6A:0D:10", 0, 0, "2");
//	events.push_back(evnt);
//	evnt = Event("Capacity", "00:1A:30:6A:0D:10", 0, 0, "3400.55, 279.606813525, 3120.94318648");
//	events.push_back(evnt);
//	poa_sample.AddItem("00:1A:30:6A:0D:10", events);
//
//	events.clear();
//	evnt = Event("Congestion", "00:1A:30:6A:0A:70", 0, 0, "0");
//	events.push_back(evnt);
//	evnt = Event("PoA status", "00:1A:30:6A:0A:70", 0, 0, "4");
//	events.push_back(evnt);
//	evnt = Event("Network type", "00:1A:30:6A:0A:70", 0, 0, "WLAN");
//	events.push_back(evnt);
//	evnt = Event("Nb of active clients", "00:1A:30:6A:0A:70", 0, 0, "3");
//	events.push_back(evnt);
//	evnt = Event("Capacity", "00:1A:30:6A:0A:70", 0, 0, "3918.85, 3596.98147884, 321.868521161");
//	events.push_back(evnt);
//	poa_sample.AddItem("00:1A:30:6A:0A:70", events);
//
//	events.clear();
//	evnt = Event("RSSI/Noise", "Gold1", 0, 0, "00:22:fb:68:0b:34, 00:1A:30:6A:0D:10, 72.5, -46, 50.0, -256");
//	events.push_back(evnt);
//	evnt = Event("BW requirement", "Gold1", 0, 0, "4000.000000");
//	events.push_back(evnt);
//	evnt = Event("PoA list", "Gold1", 0, 0, "2;00:1A:30:6A:0D:10,4.73267728072;00:1A:30:6A:0A:70,3.0");
//	events.push_back(evnt);
//	evnt = Event("Connection state", "Gold1", 0, 0, "00:1A:30:6A:0D:10,4.65852709533");
//	events.push_back(evnt);
//	client_sample.AddItem("Gold1", events);
//
//	events.clear();
//	events = emanager.SampleProcessing(poa_sample, client_sample, subscription_types);

//	cout << "Step 2 (feedback)" << endl;
//	for (unsigned int i = 0; i < events.size(); i++)
//	{
//		cout << events[i].GetName() << " " << events[i].GetValue() << endl;
//	}

	return returnEvents;

}

void LoadBalancing::handleMessage(cMessage *msg)
{
    // TODO - Generated method body
}
