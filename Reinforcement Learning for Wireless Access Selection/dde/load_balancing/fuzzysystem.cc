//=============================================================================
// Name        : fuzzysystem.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : FuzzySystem class implementation.
//=============================================================================

#include "stdafx.h"
#include "fuzzysystem.h"

#include <fl/Headers.h>
#include <typeinfo>
#include <iomanip>
#include <signal.h>

using namespace fl;

FuzzySystem::FuzzySystem()
{
}

FuzzySystem::~FuzzySystem(void)
{
	CloseEngine();
}

void FuzzySystem::CloseEngine()
{
	//if (engine) delete engine;
	//for (unsigned int i = 0; i < InputVariables.size(); i++)
	//{
	//	if (InputVariables[i]) delete InputVariables[i];
	//}
	//for (unsigned int i = 0; i < OutputVariables.size(); i++)
	//{
	//	if (OutputVariables[i]) delete OutputVariables[i];
	//}
}

bool ClientRankingFuzzySystem::InitializeEngine()
{
//	signal(SIGSEGV, fl::Exception::signalHandler);
//	signal(SIGABRT, fl::Exception::signalHandler);
//	signal(SIGILL, fl::Exception::signalHandler);
//	signal(SIGSEGV, fl::Exception::signalHandler);
//	signal(SIGFPE, fl::Exception::signalHandler);

	cout << "\nClientRankingFuzzySystem, " << fl::fuzzylite::name() << "!\n"
			<< "=================\n"
			<< "Version " << fl::fuzzylite::longVersion() << "\n"
			<< "Built on " << fl::fuzzylite::platform() << " "
			//<< "in " << fl::fuzzylite::configuration() << " mode\n"
			<< "Floating-point " << fl::fuzzylite::floatingPoint() << "\n"
			//<< "Precision is set to " << fl::fuzzylite::precision() << "\n"
			<< "Only " << fl::fuzzylite::decimals() << " decimals are printed\n"
			//<< std::setprecision(FL_DECIMALS) << std::fixed
			<< "Infinity is printed as: (" << (-std::numeric_limits<scalar>::infinity())
			<< " , " << std::numeric_limits<scalar>::infinity() << ")\n"
			<< "NaN values are printed as: " << std::numeric_limits<scalar>::quiet_NaN() << "\n";
			//<< "Defuzzifiers by default use " << fl::fuzzylite::defaultDivisions() << " divisions\n";
	if (fl::fuzzylite::logging()) {
		cout << "FL_LOG is enabled and prints in console as follows:\n";
		FL_LOG("message from FL_LOG");
		cout << "where " << FL_LOG_PREFIX << " indicates the origin of the call\n";
	} else {
		cout << "FL_LOG is NOT enabled and hence will not print anything\n";
	}

	return true;
}

double ClientRankingFuzzySystem::ExecuteAlgorithm(vector<double> values)
{
	fl::Engine* engine = new fl::Engine;
	engine->setName("qtfuzzylite");

	//TODO:
//	engine->addHedge(new fl::Any);
//	engine->addHedge(new fl::Extremely);
//	engine->addHedge(new fl::Not);
//	engine->addHedge(new fl::Seldom);
//	engine->addHedge(new fl::Somewhat);
//	engine->addHedge(new fl::Very);

	fl::InputVariable* inputVariable1 = new fl::InputVariable;
	inputVariable1->setName("ClientSatisfaction");
	inputVariable1->setRange(0.000, 5.000);

	inputVariable1->addTerm(new fl::Ramp("LOW", 2.500,0.000));
	inputVariable1->addTerm(new fl::Trapezoid("REGULAR", 1.000,2.000,3.000,4.000));
	inputVariable1->addTerm(new fl::Ramp("HIGH", 2.500,5.000));
	engine->addInputVariable(inputVariable1);

	fl::InputVariable* inputVariable2 = new fl::InputVariable;
	inputVariable2->setName("NbOfAvailableNetworks");
	inputVariable2->setRange(0.000, 20.000);

	inputVariable2->addTerm(new fl::ZShape("LOW", 1.000,3.000));
	inputVariable2->addTerm(new fl::Bell("AVERAGE", 5.000,2.500,6.000));
	inputVariable2->addTerm(new fl::SShape("HIGH", 6.000,9.000));
	engine->addInputVariable(inputVariable2);

	fl::InputVariable* inputVariable3 = new fl::InputVariable;
	inputVariable3->setName("SignalStrength");
	inputVariable3->setRange(0.000, 100.000);

	inputVariable3->addTerm(new fl::ZShape("LOW", 38.000,60.000));
	inputVariable3->addTerm(new fl::Bell("AVERAGE", 65.000,15.000,5.000));
	inputVariable3->addTerm(new fl::Sigmoid("HIGH", 80.000,0.700)); // 0.7 ??
	engine->addInputVariable(inputVariable3);

	fl::InputVariable* inputVariable4 = new fl::InputVariable;
	inputVariable4->setName("SignalToNoise");
	inputVariable4->setRange(0.000, 50.000);

	inputVariable4->addTerm(new fl::Ramp("VERY_LOW", 20.000,10.000));
	inputVariable4->addTerm(new fl::Triangle("LOW", 10.000,20.000,30.000));
	inputVariable4->addTerm(new fl::Triangle("VERY_GOOD", 20.000,30.000,40.000));
	inputVariable4->addTerm(new fl::Ramp("EXCELLENT", 30.000,40.000));
	engine->addInputVariable(inputVariable4);

	fl::OutputVariable* outputVariable1 = new fl::OutputVariable;
	outputVariable1->setName("Potential");
	outputVariable1->setRange(0.000, 5.000);
	outputVariable1->setDefaultValue(fl::nan);
	//outputVariable1->setLockValidOutput(true);
	outputVariable1->setLockOutputValueInRange(true);
//	outputVariable1->setLockDefuzzifiedValue(true);
	outputVariable1->setDefuzzifier(new fl::Centroid(500));
	//outputVariable1->output()->setAccumulation(new fl::Maximum);
	outputVariable1->fuzzyOutput()->setAccumulation(new fl::Maximum);

	outputVariable1->addTerm(new fl::Ramp("REALLY_BAD", 1.000,0.000));
	outputVariable1->addTerm(new fl::Triangle("BAD", 0.000,1.000,2.000));
	outputVariable1->addTerm(new fl::Triangle("RELATIVELY_BAD", 1.000,2.000,3.000));
	outputVariable1->addTerm(new fl::Triangle("AVERAGE", 2.000,3.000,4.000));
	outputVariable1->addTerm(new fl::Triangle("RELATIVELY_GOOD", 3.000,4.000,5.000));
	outputVariable1->addTerm(new fl::Ramp("GOOD", 4.000,5.000));
	engine->addOutputVariable(outputVariable1);

	fl::RuleBlock* ruleblock1 = new fl::RuleBlock;
	ruleblock1->setName("");
	ruleblock1->setConjunction(new fl::Minimum);
	ruleblock1->setDisjunction(new fl::Maximum);
	ruleblock1->setActivation(new fl::Minimum);

	ruleblock1->addRule(fl::Rule::parse(
		"if NbOfAvailableNetworks is LOW then Potential is REALLY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is VERY_LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is EXCELLENT then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is VERY_LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is EXCELLENT then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is VERY_LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is EXCELLENT then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is VERY_LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is EXCELLENT then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is VERY_LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is EXCELLENT then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is VERY_LOW then Potential is GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is LOW and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is EXCELLENT then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is VERY_LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is LOW then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is VERY_GOOD then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is EXCELLENT then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is VERY_LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is LOW then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is VERY_GOOD then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is EXCELLENT then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is VERY_LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is LOW then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is VERY_GOOD then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is EXCELLENT then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is VERY_LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is LOW then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is VERY_GOOD then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is EXCELLENT then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is VERY_LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is LOW then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is VERY_GOOD then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is EXCELLENT then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is VERY_LOW then Potential is RELATIVELY_GOOD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is LOW then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is VERY_GOOD then Potential is AVERAGE", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is REGULAR and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is EXCELLENT then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is VERY_LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is LOW and SignalToNoise is EXCELLENT then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is VERY_LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is VERY_GOOD then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is AVERAGE and SignalToNoise is EXCELLENT then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is VERY_LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is VERY_GOOD then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is AVERAGE and SignalStrength is HIGH and SignalToNoise is EXCELLENT then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is VERY_LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is VERY_GOOD then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is LOW and SignalToNoise is EXCELLENT then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is VERY_LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is VERY_GOOD then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is AVERAGE and SignalToNoise is EXCELLENT then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is VERY_LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is LOW then Potential is RELATIVELY_BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is VERY_GOOD then Potential is BAD", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if ClientSatisfaction is HIGH and NbOfAvailableNetworks is HIGH and SignalStrength is HIGH and SignalToNoise is EXCELLENT then Potential is BAD", engine));
	engine->addRuleBlock(ruleblock1);

    inputVariable1->setInputValue(values[0]);
    inputVariable2->setInputValue(values[1]);
    inputVariable3->setInputValue(values[2]);
    inputVariable4->setInputValue(values[3]);

//    	std::cout << "ClientSatisfaction is: " << inputVariable1->fuzzify(values[0]) << "\n";
//    	std::cout << "NbOfAvailableNetworks is: " << inputVariable2->fuzzify(values[1]) << "\n";
//    	std::cout << "SignalStrength  is: " << inputVariable3->fuzzify(values[2]) << "\n";
//    	std::cout << "SignalToNoise is: " << inputVariable4->fuzzify(values[3]) << "\n";


    cout << "input: \t" << values[0] << "\t" << values[1] << "\t" << values[2] << "\t" << values[3] << "\t-> ";
    engine->process();
	//double ranking = outputVariable1->defuzzify();
    outputVariable1->defuzzify();
    double ranking = outputVariable1->getOutputValue();
    cout << "output: " << ranking << endl;
	
	return ranking;
}

bool PoARankingFuzzySystem::InitializeEngine()
{
//	signal(SIGSEGV, fl::Exception::signalHandler);
//	signal(SIGABRT, fl::Exception::signalHandler);
//	signal(SIGILL, fl::Exception::signalHandler);
//	signal(SIGSEGV, fl::Exception::signalHandler);
//	signal(SIGFPE, fl::Exception::signalHandler);

	cout << "\nPoARankingFuzzySystem, " << fl::fuzzylite::name() << "!\n"
			<< "=================\n"
			<< "Version " << fl::fuzzylite::longVersion() << "\n"
			<< "Built on " << fl::fuzzylite::platform() << " "
			//<< "in " << fl::fuzzylite::configuration() << " mode\n"
			<< "Floating-point " << fl::fuzzylite::floatingPoint() << "\n"
			//<< "Precision is set to " << fl::fuzzylite::precision() << "\n"
			//<< "Only " << fl::fuzzylite::decimals() << " decimals are printed\n"
			//<< std::setprecision(FL_DECIMALS) << std::fixed
			<< "Infinity is printed as: (" << (-std::numeric_limits<scalar>::infinity())
			<< " , " << std::numeric_limits<scalar>::infinity() << ")\n"
			<< "NaN values are printed as: " << std::numeric_limits<scalar>::quiet_NaN() << "\n";
			//<< "Defuzzifiers by default use " << fl::fuzzylite::defaultDivisions() << " divisions\n";

	if (fl::fuzzylite::logging()) {
		cout << "FL_LOG is enabled and prints in console as follows:\n";
		FL_LOG("message from FL_LOG");
		cout << "where " << FL_LOG_PREFIX << " indicates the origin of the call\n";
	} else {
		cout << "FL_LOG is NOT enabled and hence will not print anything\n";
	}

	return true;
}

double PoARankingFuzzySystem::ExecuteAlgorithm(vector<double> values)
{
	fl::Engine* engine = new fl::Engine;
	engine->setName("qtfuzzylite");

	//TODO:
//	engine->addHedge(new fl::Any);
//	engine->addHedge(new fl::Extremely);
//	engine->addHedge(new fl::Not);
//	engine->addHedge(new fl::Seldom);
//	engine->addHedge(new fl::Somewhat);
//	engine->addHedge(new fl::Very);

	fl::InputVariable* inputVariable1 = new fl::InputVariable;
	inputVariable1->setName("APStatus");
	inputVariable1->setRange(0.000, 5.000);

	inputVariable1->addTerm(new fl::Ramp("VERY_BAD", 1.000,0.000));
	inputVariable1->addTerm(new fl::Triangle("BAD", 0.000,1.000,2.000));
	inputVariable1->addTerm(new fl::Triangle("MEDIUM", 1.000,2.000,3.000));
	inputVariable1->addTerm(new fl::Triangle("RELATIVELY_GOOD", 2.000,3.000,4.000));
	inputVariable1->addTerm(new fl::Triangle("GOOD", 3.000,4.000,5.000));
	inputVariable1->addTerm(new fl::Ramp("VERY_GOOD", 4.000,5.000));
	engine->addInputVariable(inputVariable1);

	fl::InputVariable* inputVariable2 = new fl::InputVariable;
	inputVariable2->setName("AvailableBandwidth");

	// WLAN G
//	inputVariable2->setRange(0.000, 35.000);
//
//	inputVariable2->addTerm(new fl::Ramp("VERY_LOW", 4.000,0.000));
//	inputVariable2->addTerm(new fl::Triangle("LOW", 1.000,4.000,8.000));
//	inputVariable2->addTerm(new fl::Triangle("RELATIVELY_LOW", 4.000,8.000,13.000));
//	inputVariable2->addTerm(new fl::Triangle("MEDIUM", 8.000,13.000,19.000));
//	inputVariable2->addTerm(new fl::Triangle("HIGH", 13.000,19.000,26.000));
//	inputVariable2->addTerm(new fl::Ramp("VERY_HIGH", 19.000,30.000));

	// WLAN B
	inputVariable2->setRange(0.000, 6.000);

//	inputVariable2->addTerm(new fl::Ramp("VERY_LOW", 0.400,0.000));
//	inputVariable2->addTerm(new fl::Triangle("LOW", 0.100,0.400,0.800));
//	inputVariable2->addTerm(new fl::Triangle("RELATIVELY_LOW", 0.400,0.800,1.300));
//	inputVariable2->addTerm(new fl::Triangle("MEDIUM", 0.800,1.300,1.900));
//	inputVariable2->addTerm(new fl::Triangle("HIGH", 1.300,1.900,2.600));
//	inputVariable2->addTerm(new fl::Ramp("VERY_HIGH", 1.900,3.000));

	inputVariable2->addTerm(new fl::Ramp("VERY_LOW", 0.700,0.000));
	inputVariable2->addTerm(new fl::Triangle("LOW", 0.200,0.700,1.400));
	inputVariable2->addTerm(new fl::Triangle("RELATIVELY_LOW", 0.700,1.400,2.200));
	inputVariable2->addTerm(new fl::Triangle("MEDIUM", 1.400,2.200,3.300));
	inputVariable2->addTerm(new fl::Triangle("HIGH", 2.200,3.300,4.500));
	inputVariable2->addTerm(new fl::Ramp("VERY_HIGH", 3.300,5.100));

	engine->addInputVariable(inputVariable2);

	fl::InputVariable* inputVariable3 = new fl::InputVariable;
	inputVariable3->setName("APCongestion");
	inputVariable3->setRange(0.000, 1.000);

	inputVariable3->addTerm(new fl::Rectangle("NOT_CONGESTED", 0.000,0.500));
	inputVariable3->addTerm(new fl::Rectangle("CONGESTED", 0.500,1.000));
	engine->addInputVariable(inputVariable3);

	fl::OutputVariable* outputVariable1 = new fl::OutputVariable;
	outputVariable1->setName("Capacity");
	outputVariable1->setRange(0.000, 5.000);
	//outputVariable1->setLockOutputRange(true);
	outputVariable1->setLockOutputValueInRange(true);
	outputVariable1->setDefaultValue(fl::nan);
	//outputVariable1->setLockValidOutput(true);
//	outputVariable1->setLockDefuzzifiedValue(true);
	outputVariable1->setDefuzzifier(new fl::Centroid(500));
	//outputVariable1->output()->setAccumulation(new fl::Maximum);
	outputVariable1->fuzzyOutput()->setAccumulation(new fl::Maximum);

	outputVariable1->addTerm(new fl::Ramp("VERY_LOW", 1.000,0.000));
	outputVariable1->addTerm(new fl::Triangle("LOW", 0.000,1.000,2.000));
	outputVariable1->addTerm(new fl::Triangle("RELATIVELY_LOW", 1.000,2.000,3.000));
	outputVariable1->addTerm(new fl::Triangle("RELATIVELY_HIGH", 2.000,3.000,4.000));
	outputVariable1->addTerm(new fl::Triangle("HIGH", 3.000,4.000,5.000));
	outputVariable1->addTerm(new fl::Ramp("VERY_HIGH", 4.000,5.000));
	engine->addOutputVariable(outputVariable1);

	fl::RuleBlock* ruleblock1 = new fl::RuleBlock;
	ruleblock1->setName("");
	ruleblock1->setConjunction(new fl::Minimum);
	ruleblock1->setDisjunction(new fl::Maximum);
	ruleblock1->setActivation(new fl::Minimum);

	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_GOOD and AvailableBandwidth is VERY_LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_GOOD and AvailableBandwidth is LOW and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_GOOD and AvailableBandwidth is RELATIVELY_LOW and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_GOOD and AvailableBandwidth is MEDIUM and APCongestion is NOT_CONGESTED then Capacity is HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_GOOD and AvailableBandwidth is HIGH and APCongestion is NOT_CONGESTED then Capacity is VERY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_GOOD and AvailableBandwidth is VERY_HIGH and APCongestion is NOT_CONGESTED then Capacity is VERY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is GOOD and AvailableBandwidth is VERY_LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is GOOD and AvailableBandwidth is LOW and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is GOOD and AvailableBandwidth is RELATIVELY_LOW and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is GOOD and AvailableBandwidth is MEDIUM and APCongestion is NOT_CONGESTED then Capacity is HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is GOOD and AvailableBandwidth is HIGH and APCongestion is NOT_CONGESTED then Capacity is VERY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is GOOD and AvailableBandwidth is VERY_HIGH and APCongestion is NOT_CONGESTED then Capacity is VERY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is RELATIVELY_GOOD and AvailableBandwidth is VERY_LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is RELATIVELY_GOOD and AvailableBandwidth is LOW and APCongestion is NOT_CONGESTED then Capacity is LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is RELATIVELY_GOOD and AvailableBandwidth is RELATIVELY_LOW and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is RELATIVELY_GOOD and AvailableBandwidth is MEDIUM and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is RELATIVELY_GOOD and AvailableBandwidth is HIGH and APCongestion is NOT_CONGESTED then Capacity is HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is RELATIVELY_GOOD and AvailableBandwidth is VERY_HIGH and APCongestion is NOT_CONGESTED then Capacity is VERY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is MEDIUM and AvailableBandwidth is VERY_LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is MEDIUM and AvailableBandwidth is LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is MEDIUM and AvailableBandwidth is RELATIVELY_LOW and APCongestion is NOT_CONGESTED then Capacity is LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is MEDIUM and AvailableBandwidth is MEDIUM and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is MEDIUM and AvailableBandwidth is HIGH and APCongestion is NOT_CONGESTED then Capacity is RELATIVELY_HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is MEDIUM and AvailableBandwidth is VERY_HIGH and APCongestion is NOT_CONGESTED then Capacity is HIGH", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is BAD and AvailableBandwidth is VERY_LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is BAD and AvailableBandwidth is LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is BAD and AvailableBandwidth is RELATIVELY_LOW and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is BAD and AvailableBandwidth is MEDIUM and APCongestion is NOT_CONGESTED then Capacity is LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is BAD and AvailableBandwidth is HIGH and APCongestion is NOT_CONGESTED then Capacity is LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is BAD and AvailableBandwidth is VERY_HIGH and APCongestion is NOT_CONGESTED then Capacity is LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APStatus is VERY_BAD and APCongestion is NOT_CONGESTED then Capacity is VERY_LOW", engine));
	ruleblock1->addRule(fl::Rule::parse(
		"if APCongestion is CONGESTED then Capacity is VERY_LOW", engine));
	engine->addRuleBlock(ruleblock1);
	
	//TODO:
	//engine->configure("Minimum", "Maximum", "Minimum", "Maximum", "Centroid", FL_DIVISIONS);

//   cout << FclExporter().toString(engine) << "\n" << std::endl;

//	std::cout << "APStatus is: " << inputVariable1->fuzzify(values[0]) << "\n";
//	std::cout << "AvailableBandwidth is: " << inputVariable2->fuzzify(values[1]) << "\n";
//	std::cout << "APCongestion  is: " << inputVariable3->fuzzify(values[2]) << "\n";

	inputVariable1->setInputValue(values[0]);
    inputVariable2->setInputValue(values[1]);
    inputVariable3->setInputValue(values[2]);
    engine->process();
	//double ranking = outputVariable1->defuzzify();
    outputVariable1->defuzzify();
    double ranking = outputVariable1->getOutputValue();
    cout << "input: \t" << values[0] << "\t" << values[1] << "\t" << values[2]<< "\t-> output: " << ranking << endl;
	
	return ranking;
}
