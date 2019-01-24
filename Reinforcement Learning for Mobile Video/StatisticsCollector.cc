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

#include "StatisticsCollector.h"
#include "InterfaceTableAccess.h"
#include "TCPSinkAppQ.h"

#define MK_WRITE_QMATRIX_MSG 10005

Define_Module(StatisticsCollector);


StatisticsCollector::~StatisticsCollector()
{
	goldDurationVector.clear();
	silverDurationVector.clear();
	bronzeDurationVector.clear();
	goldFreqVector.clear();
	silverFreqVector.clear();
	bronzeFreqVector.clear();

}


void StatisticsCollector::initialize()
{
	GoldStarvationDuration.setName("GoldStarvationDuration");
	SilverStarvationDuration.setName("SilverStarvationDuration");
	BronzeStarvationDuration.setName("BronzeStarvationDuration");

	goldDurationVector.clear();
	silverDurationVector.clear();
	bronzeDurationVector.clear();
	goldFreqVector.clear();
	silverFreqVector.clear();
	bronzeFreqVector.clear();

	numUsers = par("numUsers");
	numCompleted = 0;

	numberOfGold = par("numberOfGold");
	goldCompleted = 0;
	numberOfGoldStarvations = numberOfSilverStarvations = numberOfBronzeStarvations = 0;

	statsGoldStarvations = 0;
    statsSilverStarvations = 0;
	statsBronzeStarvations = 0;

	//fs4vp = simulation.getModuleByPath("fs4VP");

	fs4vp = getParentModule()->getSubmodule("DF1")->getSubmodule("fs4VP");


	WATCH(goldCompleted);


}

void StatisticsCollector::handleMessage(cMessage *msg)
{
	// TODO - Generated method body
}

void StatisticsCollector::recordStarvationDuration(double duration, int userClass)
{

	if(userClass == 0)
	{
		GoldStarvationDuration.record(duration);

	}
	else if (userClass == 1)
	{
		SilverStarvationDuration.record(duration);

	}
	else if (userClass == 2)
	{
		BronzeStarvationDuration.record(duration);

	}
}

void StatisticsCollector::recordClientStats(double freq, double duration, int starvations, int userClass)
{


	if(userClass == 0)
	{
		goldFreqVector.insert(goldFreqVector.end(), freq);
		goldDurationVector.insert(goldDurationVector.end(), duration);
		statsGoldStarvations += starvations;

	}
	else if (userClass == 1)
	{
		silverFreqVector.insert(silverFreqVector.end(), freq);
		silverDurationVector.insert(silverDurationVector.end(), duration);
		statsSilverStarvations += starvations;

	}
	else if (userClass == 2)
	{
		bronzeFreqVector.insert(bronzeFreqVector.end(), freq);
		bronzeDurationVector.insert(bronzeDurationVector.end(), duration);
		statsBronzeStarvations += starvations;

	}


}

void StatisticsCollector::addCompleted()
{
	numCompleted++;
	if(numCompleted == numUsers)
	{
		//EV << getFullPath() << ": Reached " << numCompleted << " completed downloads, ending simulation. " << endl;
		//endSimulation();
	}

}

void StatisticsCollector::addGoldCompleted()
{

	goldCompleted++;

	//Calculate statistics when gold users are active
	if(goldCompleted == numberOfGold)
	{
		std::cerr << "GOLD COMPLETED: " << simTime() << endl;

		// OM add: generate a function for the clients to send stats
		for(int i = 1; i < (numUsers+1); i++)
		{
			//cMessage *sendStats = new cMessage("sendStats", MK_SEND_STATS_MSG);
			std::string string1 = "FairTCP.UE_";
			std::string string3 = ".tcpHostApp[0]";

			int Number = i;       // number to be converted to a string
			std::string string2;          // string which will contain the result
			std::ostringstream convert;   // stream used for the conversion
			convert << Number;      // insert the textual representation of 'Number' in the characters in the stream
			string2 = convert.str(); // set 'Result' to the contents of the stream
			std::string string4 = string1 + string2 + string3;

			clientModule = simulation.getModuleByPath(string4.c_str());
			TCPSinkAppQ *clientApp = check_and_cast<TCPSinkAppQ *>(clientModule);
			clientApp->sendStats();

		}

		//writeQmatrix();



		EV << getFullPath() << ": Reached " << goldCompleted << " completed gold downloads, ending simulation. " << endl;
		endSimulation();
	}
}

void StatisticsCollector::writeQmatrix()
{
	//Generate cMessage to FS4VP to write Q-matrix
	Enter_Method_Silent();
	cMessage *writeQ = new cMessage("writeQ", MK_WRITE_QMATRIX_MSG);
	EV <<"Informing the FS4VP module to write the Q-matrix. " << endl;
	sendDirect(writeQ, fs4vp, "directInput"); //sending info to the FS4VP module

}

void StatisticsCollector::addBufferStarvations(int userClass)
{
	if(userClass == 0)
	{
		numberOfGoldStarvations++;

	}
	else if (userClass == 1)
	{
		numberOfSilverStarvations++;

	}
	else if (userClass == 2)
	{
		numberOfBronzeStarvations++;

	}

}



void StatisticsCollector::finish()
{
	// Average buffer starvation duration Gold
	double sumDurationGold = 0.0;
	double avgDurationGold = 0.0;

	for(int i = 0; i < (int) goldDurationVector.size(); i++)
	{
		sumDurationGold = sumDurationGold + goldDurationVector[i];
	}

	if(sumDurationGold != 0)
	{
		avgDurationGold = sumDurationGold/(goldDurationVector.size());
	}
	else
	{
		avgDurationGold = 0;
	}
	goldDurationVector.clear();

	// Average buffer starvation duration Silver
	double sumDurationSilver = 0.0;
	double avgDurationSilver = 0.0;

	for(int i = 0; i < (int) silverDurationVector.size(); i++)
	{
		sumDurationSilver = sumDurationSilver + silverDurationVector[i];
	}

	if(sumDurationSilver != 0)
	{
		avgDurationSilver = sumDurationSilver/(silverDurationVector.size());
	}
	else
	{
		avgDurationSilver = 0;
	}
	silverDurationVector.clear();

	// Average buffer starvation duration Bronze
	double sumDurationBronze = 0.0;
	double avgDurationBronze = 0.0;

	for(int i = 0; i < (int) bronzeDurationVector.size(); i++)
	{
		sumDurationBronze = sumDurationBronze + bronzeDurationVector[i];
	}

	if(sumDurationBronze != 0)
	{
		avgDurationBronze= sumDurationBronze/(bronzeDurationVector.size());
	}
	else
	{
		avgDurationBronze = 0;
	}
	bronzeDurationVector.clear();


	// Average buffer starvation freq Gold
	double sumFreqGold = 0.0;
	double avgFreqGold = 0.0;

	for(int i = 0; i < (int) goldFreqVector.size(); i++)
	{
		sumFreqGold = sumFreqGold + goldFreqVector[i];
	}

	if(sumFreqGold != 0)
	{
		avgFreqGold = sumFreqGold/(goldFreqVector.size());
	}
	else
	{
		avgFreqGold = 0;
	}
	goldFreqVector.clear();

	// Average buffer starvation Freq Silver
	double sumFreqSilver = 0.0;
	double avgFreqSilver = 0.0;

	for(int i = 0; i < (int) silverFreqVector.size(); i++)
	{
		sumFreqSilver = sumFreqSilver + silverFreqVector[i];
	}

	if(sumFreqSilver != 0)
	{
		avgFreqSilver = sumFreqSilver/(silverFreqVector.size());
	}
	else
	{
		avgFreqSilver = 0;
	}
	silverFreqVector.clear();

	// Average buffer starvation Freq Bronze
	double sumFreqBronze = 0.0;
	double avgFreqBronze = 0.0;

	for(int i = 0; i < (int) bronzeFreqVector.size(); i++)
	{
		sumFreqBronze = sumFreqBronze + bronzeFreqVector[i];
	}

	if(sumFreqBronze != 0)
	{
		avgFreqBronze= sumFreqBronze/(bronzeFreqVector.size());
	}
	else
	{
		avgFreqBronze = 0;
	}
	bronzeFreqVector.clear();

	recordScalar("StatsGoldDuration_active", avgDurationGold);
	recordScalar("StatsSilverDuration_active", avgDurationSilver);
	recordScalar("StatsBronzeDuration_active", avgDurationBronze);

	recordScalar("StatsGoldFrequency_active", avgFreqGold);
	recordScalar("StatsSilverFrequency_active", avgFreqSilver);
	recordScalar("StatsBronzeFrequency_active", avgFreqBronze);

	recordScalar("StatsGoldStarvations_active", statsGoldStarvations);
	recordScalar("StatsSilverStarvations_active", statsSilverStarvations);
	recordScalar("StatsBronzeStarvations_active", statsBronzeStarvations);



	recordScalar("StatsGoldStarvations", numberOfGoldStarvations);
	recordScalar("StatsSilverStarvations", numberOfSilverStarvations);
	recordScalar("StatsBronzeStarvations", numberOfBronzeStarvations);
//	GoldStarvationDuration.recordAs("GoldStarvationDuration");
//	SilverStarvationDuration.recordAs("SilverStarvationDuration");
//	BronzeStarvationDuration.recordAs("BronzeStarvationDuration");
}

