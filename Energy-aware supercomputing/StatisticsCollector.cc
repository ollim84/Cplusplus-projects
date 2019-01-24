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

Define_Module(StatisticsCollector);


StatisticsCollector::~StatisticsCollector()
{

}


void StatisticsCollector::initialize()
{
	energyDC = 0;
	energyDCwithPUE = 0; //MiM 9.9.2011
	energyCPU = 0;
	energyRAM = 0;
	energyFAN = 0;
	energyHDD = 0;
	energyNetwork = 0; // Energy consumed by RMS NIC and Switching
	energyRouter = 0; // Energy consumed by Router NIC
	energyNIC = 0; // Energy consumed by the server NICs
	energyPSU = 0;
	energyMB = 0;

	energyDC_momentary = 0;
	energyDCWithPUE_momentary = 0; //MiM 9.9.2011
	energyCPU_momentary = 0;
	energyRAM_momentary = 0;
	energyFAN_momentary = 0;
	energyNetwork_momentary = 0;
	sleepMsgs = 0;
	sleepTime = 0;
	queueTime = 0;

	percentage = 0.0;
	jobsCompleted = 0;
	requestsSent = 0;
	wakeupTime = 0;
	wakeupMsgs = 0;
	receivedJobs = 0;

	totalPPS = 0;
	ppsCount = 0;
	from = 0;
	P_max = 69.316105370000002;
	P_idle = 69.316;
	E_switch = 0;
	meanTurnAroundTime = 0;
	siteEmissions = totalEmissions = 0;

	if(getFullPath() == "HPCNetwork.stats")
	{
		CUE_var = 0;
	}

	numberOfJobs = par("numberOfJobs");

	totalEnergy_vec.setName("Total Energy");
	totalEnergyWithPUE_vec.setName("Total energy with PUE");
	cpuEnergy_vec.setName("CPU Energy");
	ramEnergy_vec.setName("RAM Energy");
	fanEnergy_vec.setName("FAN Energy");
	networkEnergy_vec.setName("Network Energy");

	WATCH(jobsCompleted);
	WATCH(energyDC);
	WATCH(requestsSent);

}

void StatisticsCollector::handleMessage(cMessage *msg)
{
	// TODO - Generated method body
}

void StatisticsCollector::addTotalEnergy(double energy)
{
	energyDC += energy;

}

void StatisticsCollector::addTotalEnergyWithPUE(double energy)
{ //MiM 8.9.2011
	energyDCwithPUE += energy;
}

void StatisticsCollector::addCPUenergy(double CPUenergy)
{
	energyCPU += CPUenergy;

}

void StatisticsCollector::addRAMenergy(double RAMenergy)
{
	energyRAM += RAMenergy;

}

void StatisticsCollector::addFANenergy(double FANenergy)
{
	energyFAN += FANenergy;

}

void StatisticsCollector::addNetworkEnergy(double networkEnergy)
{
	energyNetwork += networkEnergy;

}

void StatisticsCollector::addRouterEnergy(double routerEnergy)
{
	energyRouter += routerEnergy;

}

void StatisticsCollector::addTotalPPS(double pps, cModule *module)
{

	IInterfaceTable *inet_ift;
	inet_ift = InterfaceTableAccess ().get();

	totalPPS += pps;
	ppsCount++;


	EV << " Router interfaces: " << inet_ift->getNumInterfaces() << endl;

	// Compute switching energy after all router NICs
	// have registered their pps
	//TODO: valid for only single router in the DC, should
	// update if there are more routers (federated case)
	if(ppsCount == (inet_ift->getNumInterfaces() - 1))
	{
		to = simTime();
		E_switch = P_idle + (P_max - P_idle) * totalPPS;
		E_switch = E_switch * SIMTIME_DBL(to-from);
		EV << " Router switching energy: " << E_switch << endl;
		EV << " Total PPS: " << totalPPS << endl;
		addRouterEnergy(E_switch);
		addTotalEnergy(E_switch);
		recordTotalEnergyVector(E_switch);

		totalPPS = 0;
		ppsCount = 0;
		from = simTime();
	}

}


void StatisticsCollector::addNICenergy(double nicEnergy)
{
	energyNIC += nicEnergy;

}

void StatisticsCollector::addHDDenergy(double hddEnergy)
{
	energyHDD += hddEnergy;

}

void StatisticsCollector::addPSUenergy(double psuEnergy)
{
	energyPSU += psuEnergy;

}

void StatisticsCollector::addMBenergy(double mbEnergy)
{
	energyMB += mbEnergy;

}

void StatisticsCollector::recordTotalEnergyVector(double totalEnergy)
{
	energyDC_momentary += totalEnergy;
	totalEnergy_vec.record(energyDC_momentary);
}

void StatisticsCollector::recordTotalEnergyWithPUEVector(double totalEnergyWithPUE)
{ //MiM 8.9.2011
	energyDCWithPUE_momentary += totalEnergyWithPUE;
	totalEnergyWithPUE_vec.record(energyDCWithPUE_momentary);
}

void StatisticsCollector::recordNetworkEnergyVector(double E_network)
{
	energyNetwork_momentary += E_network;
	networkEnergy_vec.record(energyNetwork_momentary);
}

void StatisticsCollector::recordCPUEnergyVector(double cpuEnergy)
{
	energyCPU_momentary += cpuEnergy;
	cpuEnergy_vec.record(energyCPU_momentary);
}

void StatisticsCollector::recordRAMEnergyVector(double ramEnergy)
{
	energyRAM_momentary += ramEnergy;
	ramEnergy_vec.record(energyRAM_momentary);
}

void StatisticsCollector::recordFANEnergyVector(double fanEnergy)
{
	energyFAN_momentary += fanEnergy;
	fanEnergy_vec.record(energyFAN_momentary);
}

void StatisticsCollector::incrementSleepMsg()
{
	sleepMsgs++;
}

void StatisticsCollector::incrementSleepTime(double time)
{
	sleepTime += time;
}

void StatisticsCollector::incrementWakeupMsg()
{
	wakeupMsgs++;
}

void StatisticsCollector::incrementWakeupTime(double time)
{
	wakeupTime += time;
}

void StatisticsCollector::addQueueTime(double time)
{
	queueTime += time;
}

void StatisticsCollector::addRequestsSent()
{
	requestsSent++;
}

void StatisticsCollector::addJobsCompleted()
{
	jobsCompleted++;
	if(jobsCompleted == numberOfJobs)
	{
		EV << getFullPath() << ": Reached " << jobsCompleted << " jobs, ending simulation. " << endl;
		endSimulation();
	}
}

void StatisticsCollector::addReceivedJobs()
{
	receivedJobs++;
}


void StatisticsCollector::addTurnAroundTime(double time)
{
	TurnAroundTime.insert(TurnAroundTime.end(), time);
}

double StatisticsCollector::getAverageTurnAroundTime()
{
	double sum = 0;
	double avgTurnAroundTime = 0.0;
	for(int i = 0; i < (int) TurnAroundTime.size(); i++)
	{
		sum = sum + TurnAroundTime[i];
	}

	if(sum != 0)
	{
		avgTurnAroundTime = sum/(TurnAroundTime.size());
	}
	else
	{
		avgTurnAroundTime = 0;
	}

	return avgTurnAroundTime;
}

void StatisticsCollector::addWaitTime(double time)
{
	WaitTime.insert(WaitTime.end(), time);
}

double StatisticsCollector::getAverageWaitTime()
{
	double sum = 0;
	double avgWaitTime = 0.0;
	for(int i = 0; i < (int) WaitTime.size(); i++)
	{
		sum = sum + WaitTime[i];
	}

	if(sum != 0)
	{
		avgWaitTime = sum/(WaitTime.size());
	}
	else
	{
		avgWaitTime = 0;
	}

	return avgWaitTime;
}

void StatisticsCollector::setCUE(double CUE)
{
	CUE_var = 0.0;
	CUE_var = CUE;
	//std::cerr << "CUE: " << CUE_var << endl;

}

double StatisticsCollector::getCUE()
{
	return CUE_var;

}

void StatisticsCollector::addTotalEmissions(double siteEmissions)
{
	totalEmissions += siteEmissions;

}

void StatisticsCollector::addSiteEmissions(double emissions)
{ //MiM 8.9.2011
	siteEmissions += emissions;
}

void StatisticsCollector::finish()
{

	percentage = (double)jobsCompleted/(double)requestsSent;
	meanTurnAroundTime = getAverageTurnAroundTime();
	double meanWaitTime = 0;
	meanWaitTime = getAverageWaitTime();

	if(CUE_var != 0)
	{

		//siteEmissions = CUE_var * energyDC; //commented out, see addSiteEmissions() (MiM 8.9.2011)
		modp = simulation.getModuleByPath("HPCNetwork.stats");
		stats = check_and_cast<StatisticsCollector *>(modp);
		stats->addTotalEmissions(siteEmissions);
		//std::cerr << "Site emissions: " << siteEmissions << endl;
	}
	else
	{
		//std::cerr << "Total emissions: " << totalEmissions << endl;
	}


	EV << getFullPath() << ": Data Centre Total Energy: " << energyDC << endl;
	EV << getFullPath() << ": Data Centre Total Energy With PUE: " << energyDCwithPUE << endl; //MiM 8.9.2011
	EV << getFullPath() << ": Data Centre CPU Energy: " << energyCPU << endl;
	EV << getFullPath() << ": Data Centre RAM Energy: " << energyRAM << endl;
	EV << getFullPath() << ": Data Centre FAN Energy: " << energyFAN << endl;
	EV << getFullPath() << ": Data Centre Network Energy: " << energyNetwork << endl;
	EV << getFullPath() << ": Data Centre Router Energy: " << energyRouter<< endl;
	EV << getFullPath() << ": Data Centre NIC Energy: " << energyNIC<< endl;
	EV << getFullPath() << ": Data Centre HDD Energy: " << energyHDD<< endl;
	EV << getFullPath() << ": Data Centre PSU Energy: " << energyPSU<< endl;
	EV << getFullPath() << ": Data Centre MB Energy: " << energyMB<< endl;
	EV << getFullPath() << ": Sleep Messages: " << sleepMsgs << endl;
	EV << getFullPath() << ": Sleep time: " << sleepTime << endl;
	EV << getFullPath() << ": Queue time: " << queueTime << endl;
	EV << getFullPath() << ": Wakeup Messages: " << wakeupMsgs << endl;
	EV << getFullPath() << ": Wakeup time: " << wakeupTime << endl;
	EV << getFullPath() << ": Percentage of completed jobs: " << percentage << endl;
	EV << getFullPath() << ": Completed jobs: " << jobsCompleted << endl;
	EV << getFullPath() << ": Requests sent: " << requestsSent << endl;
	EV << getFullPath() << ": Simulation time: " << simTime() << endl;
	EV << getFullPath() << ": Received jobs: " << receivedJobs << endl;

	EV << getFullPath() << ": Momentary Total Energy: " << energyDC_momentary << endl;
	EV << getFullPath() << ": Momentary CPU Energy: " << energyCPU_momentary << endl;
	EV << getFullPath() << ": Momentary RAM Energy: " << energyRAM_momentary << endl;
	EV << getFullPath() << ": Momentary FAN Energy: " << energyFAN_momentary  << endl;
	EV << getFullPath() << ": Momentary Network Energy: " << energyNetwork_momentary  << endl;


	recordScalar("Data Centre Total Energy", energyDC);
	recordScalar("Data Centre Total Energy With PUE", energyDCwithPUE); //MiM 8.9.2011
	recordScalar("Data Centre CPU Energy", energyCPU);
	recordScalar("Data Centre RAM Energy", energyRAM);
	recordScalar("Data Centre FAN Energy", energyFAN);
	recordScalar("Data Centre NIC Energy", energyNIC);
	recordScalar("Data Centre HDD Energy", energyHDD);
	recordScalar("Data Centre PSU Energy", energyPSU);
	recordScalar("Data Centre MB Energy", energyMB);
	recordScalar("Data Centre Network Energy", energyNetwork);
	recordScalar("Data Centre Router Energy", energyRouter);
	recordScalar("Data Centre Queue Time", queueTime);
	recordScalar("Data Centre Sleep time", sleepTime);
	recordScalar("Data Centre Wakeup time", wakeupTime);
	recordScalar("Data Centre percentage of completed jobs", percentage);
	recordScalar("Data Centre Completed jobs", jobsCompleted);
	recordScalar("Data Centre Requests sent", requestsSent);
	recordScalar("Data Centre Received jobs", receivedJobs);
	recordScalar("Data Centre Turnaround time", meanTurnAroundTime);
	recordScalar("Total Mean Wait time", meanWaitTime);
	recordScalar("Simulation duration", simTime());
	recordScalar("Site Emissions", siteEmissions);
	recordScalar("Total Emissions", totalEmissions);

}

