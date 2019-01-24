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

		void addTotalEnergy(double energy);
		void addCPUenergy(double CPUenergy);
		void addRAMenergy(double RAMenergy);
		void addFANenergy(double FANenergy);
		void addNetworkEnergy(double networkEnergy);
		void addRouterEnergy(double routerEnergy);
		void addTotalPPS(double pps, cModule *module);
		void addNICenergy(double nicEnergy);
		void addHDDenergy(double hddEnergy);
		void addPSUenergy(double psuEnergy);
		void addMBenergy(double mbEnergy);
		void recordTotalEnergyVector(double totalEnergy);
		void recordNetworkEnergyVector(double E_network);
		void recordCPUEnergyVector(double cpuEnergy);
		void recordRAMEnergyVector(double ramEnergy);
		void recordFANEnergyVector(double fanEnergy);
		void incrementSleepMsg();
		void incrementSleepTime(double time);
		void incrementWakeupMsg();
		void incrementWakeupTime(double time);
		void addQueueTime(double time);
		void addRequestsSent();
		void addJobsCompleted();
		void addReceivedJobs();
		double getAverageTurnAroundTime();
		void addTurnAroundTime(double time);
		void addWaitTime(double time);
		double getAverageWaitTime();
		void setCUE(double CUE);
		void addTotalEmissions(double siteEmissions);
		double getCUE();


	protected:
		virtual void initialize();
		virtual void handleMessage(cMessage *msg);
		virtual void finish();

		//Total energy consumed
		double energyDC;
		double energyCPU;
		double energyRAM;
		double energyFAN;
		double energyNetwork;
		double energyRouter;
		double energyNIC;
		double energyHDD;
		double energyPSU;
		double energyMB;

		//Momentary energy consumed
		double energyDC_momentary;
		double energyCPU_momentary;
		double energyRAM_momentary;
		double energyFAN_momentary;
		double energyNetwork_momentary;

		double siteEmissions;
		double totalEmissions;

		int sleepMsgs;
		double sleepTime;
		double queueTime;
		int wakeupMsgs;
		double wakeupTime;

		double totalPPS;
		int ppsCount;
		double P_idle;
		double P_max;
		simtime_t from;
		simtime_t to;
		double E_switch;
		int requestsSent;
		int jobsCompleted;
		double percentage;
		int numberOfJobs;
		int receivedJobs;
		double CUE_var;

	 	// Statistics
	 	StatisticsCollector *stats;
	 	cModule *modp;

	 	std::vector<double> TurnAroundTime;
	 	double meanTurnAroundTime;

	 	std::vector<double> WaitTime;
	 	double meanWaitTime;

		//Statistic vectors
		cOutVector totalEnergy_vec;
		cOutVector cpuEnergy_vec;
		cOutVector ramEnergy_vec;
		cOutVector fanEnergy_vec;
		cOutVector networkEnergy_vec;
};

#endif
