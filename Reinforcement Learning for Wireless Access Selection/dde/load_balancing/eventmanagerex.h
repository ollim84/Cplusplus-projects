//=============================================================================
// Name        : eventmanagerex.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : EventManagerEx class declaration.
//=============================================================================

#pragma once

#include "expertsystem.h"


class EventManagerEx
{
public:
	EventManagerEx(void);
	EventManagerEx(map<string, string>& poa_types, string learning_fname);
	virtual ~EventManagerEx(void);

	vector<Event> SampleProcessing(SampleEx& poa_sample, SampleEx& client_sample, map<string, int>& subscription_types);

protected:
	ExpertSystem* pExpertSystem;
	
//	bool UseSubscriptionTypeInClientRanking;
//	bool UseLearning;
//	int PoAMargin;
//	int PoARankingLimit;
//	int ClientsPoAGradingLimit;
//	string SinglePoALoadBalancing;
//	int SimultaneousHOsPerCongestedPoA;

	void AddEvent(vector<Event>& events, const string name, const string source, const string target, const int type, const vector<string> vdata);
};
