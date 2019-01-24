//=============================================================================
// Name        : fuzzysystem.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : FuzzSystem class declaration.
//=============================================================================

#pragma once
//#include "fl/Headers.h"
#include <assert.h>

using namespace std;

class FuzzySystem
{
public:
	FuzzySystem(void);
	virtual ~FuzzySystem(void);

	virtual bool InitializeEngine(){ assert(0); return 0; };
	virtual void CloseEngine();
	virtual double ExecuteAlgorithm(vector<double> values){ assert(0); return 0; };

protected:
	//fl::Engine* pEngine;

	//vector<fl::InputVariable*> InputVariables;
	//vector<fl::OutputVariable*> OutputVariables;
	//vector<fl::RuleBlock*> RuleBlocks;
};

class ClientRankingFuzzySystem : public FuzzySystem
{
public:
	ClientRankingFuzzySystem(void){};
	virtual ~ClientRankingFuzzySystem(void){ CloseEngine(); };

	bool InitializeEngine();
	double ExecuteAlgorithm(vector<double> values);
	void ExecuteTest();

private:
};

class PoARankingFuzzySystem : public FuzzySystem
{
public:
	PoARankingFuzzySystem(void){};
	virtual ~PoARankingFuzzySystem(void){ CloseEngine(); };

	bool InitializeEngine();
	double ExecuteAlgorithm(vector<double> values);

private:
};
