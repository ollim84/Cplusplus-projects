//=============================================================================
// Name        : expertsystem.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : ExpertSystem class declaration.
//=============================================================================

#pragma once
#include "fuzzysystem.h"
#include "sample.h"
#include "learning.h"

//#ifdef linux
//#include "LoadBalancing.h"
//#endif

const double EPS = 0.00000001;

class ExpertSystem
{
public:
	ExpertSystem(void);
	virtual ~ExpertSystem(void);

	bool Initialize(map<string, string>& poa_types, string learning_fname);
	void CloseExpertSystem();
	
	vector<pair<string, pair<int, double> > > ExecuteClientRanking(const SampleEx& sample, map<string, int>& subscription_types,
															map<string, double>& required_bw, double& client_satisfaction, bool use_subscr_type = false, bool goldPreferred = true);
//	vector<pair<string, pair<int, double> > > ExecutePoARanking(const SampleEx& sample, map<string, vector<double> >& capacity,
//															double& network_status);
	vector<pair<pair<string, vector<int> >, pair<int, double> > > ExecutePoARanking(const SampleEx& sample, map<string, vector<double> >& capacity,
															double& network_status);

	void SetInitialState(string current_poa);
	double ChooseAnAction(string current_poa, double max_capacity);
	void GiveFeedback(const SampleEx& poa_sample, double network_status, double _satisfaction);
	void UpdateNbOfHOs(string current_poa, int nb_of_hos);
	void UpdateAction(string current_poa, double cum_bw, double max_capacity);
	//void UpdateHOInfo(string current_poa, string affected_poa, double poa_grade, string moved_client, double connection_state);

private:
	ClientRankingFuzzySystem ClientRanking;
	PoARankingFuzzySystem PoARanking;
	//int ClientSatisfactionLimit;
	//int PoAStatusLimit;
	
	map<string, LearningComponent> Learning;
};
