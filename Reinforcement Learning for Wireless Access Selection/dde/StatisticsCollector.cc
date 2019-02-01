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
// // Copyright 2013 Olli Mämmelä (VTT)

#include "StatisticsCollector.h"
#include "InterfaceTableAccess.h"

Define_Module(StatisticsCollector);


StatisticsCollector::~StatisticsCollector()
{
	DownloadTime.clear();
}


void StatisticsCollector::initialize()
{
	downloadsCompleted = 0;
	numberOfHosts = par("numberOfHosts");
	downloadTime_vec.setName("download time");
	numberOfHandovers = 0;
	numberOfHandoverRecommendations = 0;
	numberOfStays = 0;
	numberOfBufferStarvations = 0;
	numberOfBufferStarvationsLearning = 0;
	numberOfBufferStarvationsNonLearning = 0;
	handoverFreqVec.setName("Handover_Frequency");
	numberOfBufferStarvationsGold = 0;
	numberOfBufferStarvationsSilver = 0;
	numberOfBufferStarvationsBronze = 0;
	numberOfGoldHandovers = 0;
	numberOfSilverHandovers = 0;
	numberOfBronzeHandovers = 0;

	starvationDurationGold_vec.setName("GoldStarvationDurationVector");
	starvationDurationSilver_vec.setName("SilverStarvationDurationVector");
	starvationDurationBronze_vec.setName("BronzeStarvationDurationVector");

	WATCH(downloadsCompleted);


}

void StatisticsCollector::handleMessage(cMessage *msg)
{
	// TODO - Generated method body
}


void StatisticsCollector::addDownloadTime(double downloadtime, bool fuzzy, int applicationType)
{

	downloadTime_vec.record(downloadtime);
	downloadTime_sca.collect(downloadtime);
	recordDownloadTime(downloadtime);

	// FTP application
	if(applicationType == 0)
		downloadTimeFTP_sca.collect(downloadtime);

	if(fuzzy == true)
		downloadTimeLearning_sca.collect(downloadtime);
	else
		downloadTimeNonLearning_sca.collect(downloadtime);

	downloadsCompleted++;
	if(downloadsCompleted == numberOfHosts)
	{
		EV << getFullPath() << ": Reached " << downloadsCompleted << " completed downloads, ending simulation. " << endl;
		endSimulation();
	}
}

void StatisticsCollector::addTotalBW(double value)
{
	totalBW_vec.record(value);
	totalBW_sca.collect(value);
}

void StatisticsCollector::addNonLearningTotalBW(double value)
{
	totalBWNonLearning_sca.collect(value);
}

void StatisticsCollector::addLearningTotalBW(double value)
{
	totalBWLearning_sca.collect(value);
}

void StatisticsCollector::addAvgDelay(double value)
{
	avgDelay_vec.record(value);
	avgDelay_sca.collect(value);
}

void StatisticsCollector::addNonLearningAvgDelay(double value)
{
	avgDelayNonLearning_sca.collect(value);
}

void StatisticsCollector::addLearningAvgDelay(double value)
{
	avgDelayLearning_sca.collect(value);
}

void StatisticsCollector::addStarvationFrequency(double frequency, bool fuzzy)
{
	starvationFrequency_vec.record(frequency);
	starvationFrequency_sca.collect(frequency);
	//recordStarvationDuration(duration);

	if(fuzzy == true)
		starvationFrequencyLearning_sca.collect(frequency);
	else
		starvationFrequencyNonLearning_sca.collect(frequency);
}

void StatisticsCollector::addStarvationDuration(double duration, bool fuzzy, int userClass)
{

	starvationDuration_vec.record(duration);
	starvationDuration_sca.collect(duration);
	recordStarvationDuration(duration);

	if(fuzzy == true)
		starvationDurationLearning_sca.collect(duration);
	else
		starvationDurationNonLearning_sca.collect(duration);

	// User class statistics
	if(userClass == 3)
	{
		starvationDurationGold_sca.collect(duration);
		starvationDurationGold_vec.record(duration);
	}
	else if(userClass == 2)
	{
		starvationDurationSilver_sca.collect(duration);
		starvationDurationSilver_vec.record(duration);
	}
	else if(userClass == 1)
	{
		starvationDurationBronze_sca.collect(duration);
		starvationDurationBronze_vec.record(duration);
	}
}


void StatisticsCollector::recordStarvationDuration(double duration)
{
	StarvationDuration.insert(StarvationDuration.end(), duration);
}

double StatisticsCollector::getAverageStarvationDuration()
{

	double sum = 0;
	double avgDuration = 0.0;
	for(int i = 0; i < (int) StarvationDuration.size(); i++)
	{
		sum = sum + StarvationDuration[i];
	}

	if(sum != 0)
	{
		avgDuration = sum/(StarvationDuration.size());
	}
	else
	{
		avgDuration = 0;
	}

	return avgDuration;
}

void StatisticsCollector::recordDownloadTime(double time)
{
	DownloadTime.insert(DownloadTime.end(), time);
}

void StatisticsCollector::recordHandoverTime(double time)
{
	handoverTime_sca.collect(time);
}

double StatisticsCollector::getAverageDownloadTime()
{
	double sum = 0;
	double avgDownloadTime = 0.0;
	for(int i = 0; i < (int) DownloadTime.size(); i++)
	{
		sum = sum + DownloadTime[i];
	}

	if(sum != 0)
	{
		avgDownloadTime = sum/(DownloadTime.size());
	}
	else
	{
		avgDownloadTime = 0;
	}

	return avgDownloadTime;
}

void StatisticsCollector::addHandover(bool infoServerRecommendation)
{
	numberOfHandovers++;
	handoverFreqVec.record(1.0); //statistics
	if(infoServerRecommendation == true)
	{
		numberOfHandoverRecommendations++;
	}
}

void StatisticsCollector::addBufferStarvations(int bufferStarvations)
{
	numberOfBufferStarvations += bufferStarvations;
}

void StatisticsCollector::addLearningBufferStarvations(int bufferStarvations)
{
	numberOfBufferStarvationsLearning += bufferStarvations;
}

void StatisticsCollector::addNonLearningBufferStarvations(int bufferStarvations)
{
	numberOfBufferStarvationsNonLearning += bufferStarvations;
}

void StatisticsCollector::addStays(int stays)
{
	numberOfStays += stays;
}

void StatisticsCollector::addGoldStats(int starvations, double freq, int HOs)
{
	numberOfBufferStarvationsGold += starvations;
	starvationFrequencyGold_sca.collect(freq);
	numberOfGoldHandovers += HOs;

}

void StatisticsCollector::addSilverStats(int starvations, double freq, int HOs)
{
	numberOfBufferStarvationsSilver += starvations;
	starvationFrequencySilver_sca.collect(freq);
	numberOfSilverHandovers += HOs;

}

void StatisticsCollector::addBronzeStats(int starvations, double freq, int HOs)
{
	numberOfBufferStarvationsBronze += starvations;
	starvationFrequencyBronze_sca.collect(freq);
	numberOfBronzeHandovers += HOs;

}

void StatisticsCollector::finish()
{
	double meanDownloadTime = 0;
	meanDownloadTime = getAverageDownloadTime();

	double meanDuration = 0;
	meanDuration = getAverageStarvationDuration();



	EV << getFullPath() << ": Mean Download Time: " << meanDownloadTime  << endl;
	EV << getFullPath() << ": numberOfHandovers: " << numberOfHandovers  << endl;
	EV << getFullPath() << ": numberOfBufferStarvations: " << numberOfBufferStarvations  << endl;
	EV << getFullPath() << ": meanDuration: " << meanDuration  << endl;
	EV << getFullPath() << ": numberOfHandoverRecommendations: " << numberOfHandoverRecommendations << endl;
	EV << getFullPath() << ": numberOfBufferStarvationsGold: " << numberOfBufferStarvationsGold << endl;
	EV << getFullPath() << ": numberOfBufferStarvationsSilver: " << numberOfBufferStarvationsSilver << endl;
	EV << getFullPath() << ": numberOfBufferStarvationsBronze: " << numberOfBufferStarvationsBronze << endl;


	recordScalar("numberOfHandoversStats", numberOfHandovers);
	recordScalar("numberOfHandoverRecommendations", numberOfHandoverRecommendations);
	recordScalar("totalStays", numberOfStays);
	recordScalar("numberOfBufferStarvationsStats", numberOfBufferStarvations);
	recordScalar("numberOfBufferStarvationsLearningStats", numberOfBufferStarvationsLearning);
	recordScalar("numberOfBufferStarvationsNonLearningStats", numberOfBufferStarvationsNonLearning);

	//recordScalar("Mean Download time", meanDownloadTime);

	// Download time
	downloadTime_sca.recordAs("download time");
	totalBW_sca.recordAs("Total BW");

	downloadTimeFTP_sca.recordAs("download time FTP");

	downloadTimeNonLearning_sca.recordAs("download time NonLearning");
	totalBWNonLearning_sca.recordAs("Total BW NonLearning");

	downloadTimeLearning_sca.recordAs("download time Learning");
	totalBWLearning_sca.recordAs("Total BW Learning");

	// Starvation duration
	starvationDuration_sca.recordAs("starvationDuration");
	starvationDurationNonLearning_sca.recordAs("starvationDuration NonLearning");
	starvationDurationLearning_sca.recordAs("starvationDuration Learning");

	// Starvation frequency
	starvationFrequency_sca.recordAs("starvationFrequency");
	starvationFrequencyNonLearning_sca.recordAs("starvationFrequency NonLearning");
	starvationFrequencyLearning_sca.recordAs("starvationFrequency Learning");

	// Handover time
	handoverTime_sca.recordAs("handover time");

	// Avg delay
	avgDelay_sca.recordAs("avgDelay");
	avgDelayNonLearning_sca.recordAs("avgDelay NonLearning");
	avgDelayLearning_sca.recordAs("avgDelay Learning");

	// Gold
	recordScalar("GoldStarvationEvents", numberOfBufferStarvationsGold);
	starvationDurationGold_sca.recordAs("GoldStarvationDuration");
	starvationFrequencyGold_sca.recordAs("GoldStarvationFrequency");
	recordScalar("GoldHandoversTotal", numberOfGoldHandovers);

	// Silver
	recordScalar("SilverStarvationEvents", numberOfBufferStarvationsSilver);
	starvationDurationSilver_sca.recordAs("SilverStarvationDuration");
	starvationFrequencySilver_sca.recordAs("SilverStarvationFrequency");
	recordScalar("SilverHandoversTotal", numberOfSilverHandovers);

	// Bronze
	recordScalar("BronzeStarvationEvents", numberOfBufferStarvationsBronze);
	starvationDurationBronze_sca.recordAs("BronzeStarvationDuration");
	starvationFrequencyBronze_sca.recordAs("BronzeStarvationFrequency");
	recordScalar("BronzeHandoversTotal", numberOfBronzeHandovers);

}

