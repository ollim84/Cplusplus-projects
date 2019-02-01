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

#ifndef __STATISTICSCOLLECTOR_H__
#define __STATISTICSCOLLECTOR_H__

#include <omnetpp.h>

/**
 * TODO - Generated class
 */
class StatisticsCollector : public cSimpleModule
{
	public:
		// Desctructor
		~StatisticsCollector();

		void addDownloadTime(double downloadtime, bool fuzzy, int applicationType);
		void addHandover(bool);
		void addTotalBW(double value);
		void addNonLearningTotalBW(double value);
		void addLearningTotalBW(double value);
		void addNonLearningAvgDelay(double value);
		void addLearningAvgDelay(double value);
		void addAvgDelay(double value);
		void addStays(int);
		void addBufferStarvations(int);
		void addLearningBufferStarvations(int);
		void addNonLearningBufferStarvations(int);
		void addStarvationDuration(double duration, bool fuzzy, int userClass);
		void addStarvationFrequency(double frequency, bool fuzzy);
		void addGoldStats(int, double, int);
		void addSilverStats(int,double, int);
		void addBronzeStats(int, double, int);
		void recordHandoverTime(double time);


	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();

		double getAverageDownloadTime();
		double getAverageStarvationDuration();
		void recordDownloadTime(double time);
		void recordStarvationDuration(double duration);


		int downloadsCompleted;
		int numberOfHosts;
		int numberOfHandovers;
		int numberOfHandoverRecommendations; // from the infoServer
		int numberOfStays;
		int numberOfBufferStarvations;
		int numberOfBufferStarvationsLearning;
		int numberOfBufferStarvationsNonLearning;
		std::vector<double> DownloadTime;
		std::vector<double> StarvationDuration;

		//Statistic vectors
		cOutVector downloadTime_vec;
		cStdDev downloadTime_sca; // All nodes
		cStdDev downloadTimeNonLearning_sca; //Non-learning nodes
		cStdDev downloadTimeLearning_sca; // Learning nodes

		cStdDev downloadTimeFTP_sca; // FTP nodes

		cStdDev handoverTime_sca;

		// Starvation duration
		cOutVector starvationDuration_vec;
		cStdDev starvationDuration_sca; // All nodes
		cStdDev starvationDurationNonLearning_sca; //Non-learning nodes
		cStdDev starvationDurationLearning_sca; // Learning nodes

		// Starvation frequency
		cOutVector starvationFrequency_vec;
		cStdDev starvationFrequency_sca; // All nodes
		cStdDev starvationFrequencyNonLearning_sca; //Non-learning nodes
		cStdDev starvationFrequencyLearning_sca; // Learning nodes

		// Bandwidth
		cOutVector totalBW_vec;
		cStdDev totalBW_sca;
		cStdDev totalBWNonLearning_sca;
		cStdDev totalBWLearning_sca;

		cOutVector avgDelay_vec;
		cStdDev avgDelay_sca;
		cStdDev avgDelayNonLearning_sca;
		cStdDev avgDelayLearning_sca;

		cOutVector handoverFreqVec;


		// Gold statistics
		int numberOfBufferStarvationsGold;
		cStdDev starvationDurationGold_sca;
		cStdDev starvationFrequencyGold_sca;
		cOutVector starvationDurationGold_vec;
		int numberOfGoldHandovers;

		// Silver statistics
		int numberOfBufferStarvationsSilver;
		cStdDev starvationDurationSilver_sca;
		cStdDev starvationFrequencySilver_sca;
		cOutVector starvationDurationSilver_vec;
		int numberOfSilverHandovers;

		// Bronze statistics
		int numberOfBufferStarvationsBronze;
		cStdDev starvationDurationBronze_sca;
		cStdDev starvationFrequencyBronze_sca;
		cOutVector starvationDurationBronze_vec;
		int numberOfBronzeHandovers;

};

#endif
