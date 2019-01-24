//
// Copyright 2004 Andras Varga
//
// This library is free software, you can redistribute it and/or modify
// it under  the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation;
// either version 2 of the License, or any later version.
// The library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// See the GNU Lesser General Public License for more details.
//


#include "SuperComServerApp.h"
#include "TCPSocket.h"
#include "TCPCommand_m.h"
#include "SuperComMsg_m.h"
#include "CPU.h"
#include "Core.h"
#include "Cache.h"
#include "RAMstick.h"
#include "Fan.h"
#include "HardDisk.h"
#include "StatisticsCollector.h"
#include "PSU.h"
#include "NIC.h"

#include "SuperComRmsApp.h" //MiM 7.9.2011 for dynamic PUE

#define INTERNAL_COMPLETE    1
#define INTERNAL_SLEEP_RESPONSE  2
#define INTERNAL_WAKEUP_RESPONSE 3
#define MSGKIND_REQUEST     11
#define MSGKIND_INPUT       13

#define MSGKIND_FINISH      15
#define MSGKIND_SLEEP		16
#define MSGKIND_SLEEP_RESPONSE		17
#define MSGKIND_WAKEUP  18
#define MSGKIND_WAKEUP_RESPONSE 19

#define BEV	EV << "[" << this->getParentModule()->getFullName() << "]: "

Define_Module(SuperComServerApp);


void SuperComServerApp::initialize()
{
    const char *address = par("address");
    int port = par("port");
    maxMsgDelay = 0;
    numRAMsticks = par("numRAMsticks");//number of RAMstics
    RAMstickSize = par("RAMstickSize");//size of each RAMstick; at first all sticks are of same size!
    numCPUs = par("numCPUs");//number of CPUs on this server
    numCaches = par("numCaches");
    numPSU = par("numPSU");
    memory = numRAMsticks * RAMstickSize;//Perhaps we use only this line always?
    usedMemory = 0;

    RAM_idle = FAN_idle = CPU_idle = HDD_idle = 0;
    numCores = par("numCores");
    TotalEnergy = energyCPU = energyRAM = energyFAN = energyHDD = energyPSU = energyNIC = energyMB = 0;
    totalEnergy_momentary = energyCPU_momentary = energyRAM_momentary = 0;
    energyFAN_momentary = energyHDD_momentary = energyPSU_momentary = energyNIC_momentary = 0;
    from = 0;
    energyRAM_vector = 0;
    runningJobs = 0;
    operationMode = 0; //Node is powered on

    EthModule = getParentModule()->getSubmodule("eth", 0)->getSubmodule("mac");

    server_idle = par("server_idle"); //power consumption of a server in idle state. Watts, 60-250W
    energyMode = par("energyMode"); // 0 = Hibernate, 1 = sleep/stdby

    //TODO: set delay as 5-10s for sleep/stdby
    delay = par("replyDelay");

    LoadedTimeVector.clear();
    msgsRcvd = msgsSent = bytesRcvd = bytesSent = 0;

    //MiM for CPU:
    for(int i=0;i<numCPUs;i++){

      CPU temp_cpu;
      temp_cpu.Architecture = par("CPU_Architecture"); //0=Intel, 1 = AMD
      temp_cpu.operatingSystem = par("operatingSystem"); // enumeration 0 = Linux, 1 = Windows
      temp_cpu.DVFS = par("DVFS");
      temp_cpu.Lithography = par("Lithography");
      temp_cpu.Power_idle = par("CPU_Power_idle");
      temp_cpu.Power_max = par("CPU_Power_max");

      //Each CPU has 2 cores
      for(int j=0;j<numCores;j++){
    	  Core temp_core;
    	  temp_core.Frequency = par("coreFrequency");//in GHz// TODO: read from parameter
    	  temp_core.Voltage = par("coreVoltage");//in W //TODO: read from parameter
    	  temp_core.Load = par("coreLoad"); //TODO: set to idle Load i.e. the load due to the operating system
    	  temp_cpu.cores.push_back(temp_core);
      }

      for(int k=0;k<numCaches;k++){
		  Cache temp_cache;
		  temp_cache.Level = 1;//TODO: read from parameter
		  temp_cache.Size = 1;//TODO: read from parameter

		  temp_cpu.caches.push_back(temp_cache);
      }

      cpu.push_back(temp_cpu); //insert the CPU object to the vector container
    }
     /////////////

    // RAMstick
    for(int i=0;i<numRAMsticks;i++)
    {
    	RAMstick temp_ram;
    	temp_ram.Size = RAMstickSize;
    	temp_ram.Vendor = par("RAMvendor"); //0=Kingston, 1 = Samsung, 2=Hynix
    	temp_ram.Voltage = par("RAMvoltage");
    	temp_ram.Type = par("RAMtype"); //2=DDR2, 3=DDR3
    	temp_ram.Buffered = par("RAMbuffered"); //true = buffered, false = unbuffered
    	temp_ram.Frequency = par("RAMfrequency"); //input freq in MHz
    	temp_ram.Loaded = par("RAMloaded"); // under load = true,  idle = false
    	RAM.push_back(temp_ram);

	}

    // FAN
    fan.Max_RPM = par("fanMaxRPM"); //4000 - 16000 rpm
    fan.Actual_RPM = par("fanActualRPM"); //1000 - 16000 rpm
    fan.Power_idle = par("fanPowerIdle");
    fan.Power_max = par("fanPowerMax");
    fan.Width = par("fanWidth"); //0.04 - 0.12 m
    fan.Depth = par("fanDepth"); //0.01 - 0.04 m

    //Hard Disk
    HDD.P_idle = par("HDDidlePower"); //4 - 8 W
    HDD.P_loaded = par("HDDloadedPower"); // 1.4 * idlePower  (6 - 12 W)
    HDD.P_stdby = par("HDDstdbyPower"); //0.13 - 1.0W
    HDD.HDDstate = par("HDDstate"); //1 = IDLE, 2 = LOADED, 3 = STDBY/SLEEP, 4 = STARTUP
    HDD.P_startup = par("HDDstartupPower"); // 3.7 * idlePower  (16 - 29 W)
    HDD.t_startup = par("HDDstartupTime"); // 10 - 20 s
    HDD.loadedTime = par("HDDloadedTime"); //Proportion of the walltime the job spends in loaded state

    //PSUs
    for(int i = 0; i<numPSU; i++)
    {
    	PSU temp_psu;
    	temp_psu.PSUefficiency = par("PSUefficiency");;
    	temp_psu.PSUpower = par("PSUpower");
    	psu.push_back(temp_psu);

    }

    //NIC
    nic.P_idle = par("NIC_powerIdle");
    nic.P_max = par("NIC_powerMax");

    // Statistics
    const char* ch1 = getParentModule()->getParentModule()->getFullName();
    char str1[200];
    strcpy(str1, ch1);

    char str2[80];
    sprintf(str2, ".stats");
    strcat(str1, str2);

    const char *statsString = str1;

    dcModp = simulation.getModuleByPath(statsString);
    dcStats = check_and_cast<StatisticsCollector *>(dcModp);

    //modp = simulation.getModuleByPath("datacenter.stats");
    modp = simulation.getModuleByPath("HPCNetwork.stats");
    stats = check_and_cast<StatisticsCollector *>(modp);

    if (stats == NULL)
    		opp_error("Wrong statisticsModulePath configuration");


    WATCH(msgsRcvd);
    WATCH(msgsSent);
    WATCH(bytesRcvd);
    WATCH(bytesSent);
    WATCH(memory);
    WATCH(numCores);
    WATCH(TotalEnergy);
    WATCH(energyCPU);
    WATCH(energyRAM);
    WATCH(energyFAN);
    WATCH(energyHDD);
    WATCH(runningJobs);

    TCPSocket socket;
    socket.setOutputGate(gate("tcpOut"));
    socket.bind(address[0] ? IPvXAddress(address) : IPvXAddress(), port);
    socket.listen();

}


void SuperComServerApp::sendMessage(cMessage *msg)
{
	SuperComMsg *appmsg = dynamic_cast<SuperComMsg*>(msg);

	if (appmsg)
	{
		msgsSent++;
		bytesSent += appmsg->getByteLength();

		EV << "sending \"" << appmsg->getName() << "\" to TCP, " << appmsg->getByteLength() << " bytes\n";
	}
	else
	{
		EV << "sending \"" << msg->getName() << "\" to TCP\n";
	}

    send(msg, "tcpOut");
}

void SuperComServerApp::response(cMessage *msg)
{
	SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
	int connId = intMsg->getConnId();
	int needCores = intMsg->getCores();
	int needMemory = intMsg->getNeedMemory();
	int needLoad = intMsg->getLoad();
	IPvXAddress clientAddr = intMsg->getClientAddress();
	double loadedTime = intMsg->getLoadedTime();
	delete intMsg;
	int unloadedCores = 0;
	int outputDataLength = par("outputData");
	//int outputDataLength = intMsg->getInputData() / intMsg->getNodes(
	usedMemory = usedMemory - needMemory;

	if(usedMemory < 0)
		error("Used memory cannot be below zero.");


	SuperComMsg* appmsg = new SuperComMsg("complete");

	TCPSendCommand *cmd = new TCPSendCommand();
	cmd->setConnId(connId);
	appmsg->setControlInfo(cmd);

	// set length and send it back
	appmsg->setKind(TCP_C_SEND);
	appmsg->setType(MSGKIND_FINISH);
	appmsg->setClientAddress(clientAddr);
	appmsg->setCores(needCores);
	if(outputDataLength <= 0)
		error("Server cannot send negative output data.");
	appmsg->setByteLength(outputDataLength);

	BEV << "Finished executing the job, informing the RMS." << endl;
	BEV << "Amount of output data generated: "<< outputData << endl;
	//std::cerr << "Sending output data in Server. Simtime: " << simTime() << endl;
	BEV << "Calculating energy consumption from: " << from << " to: " << simTime() << endl;
	BEV << "Used memory: " <<  usedMemory << endl;
	BEV << "Loaded time: " << loadedTime << endl;



	outputData = 0;
	//Calculate energy for all components
	computeEnergy();

	//BEV << "CPU: " << energyCPU << " RAM: " << energyRAM << " FAN: " << energyFAN << endl;

	from = simTime(); //update

	//Decrement the number of running jobs
//	if(getFullPath() == "HPCNetwork.datacenter.server[26].tcpApp[0]")
//    {
//    	std::cerr << "Running jobs before job end: " << runningJobs << " simTime: "  << simTime() << endl;
//		for(int i = 0; i < numCPUs; i++)
//		{
//			for(int j = 0; j < numCores; j++)
//			{
//				std::cerr << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;
//
//			}
//		}
//
//    }
	runningJobs = runningJobs - 1;


	if(runningJobs < 0)
		error("Running jobs cannot be below zero.");

	//1. Update the load to the cores (remove the job load and set as idle load)
	for(int i = 0; i < numCPUs; i++)
	{
		if(unloadedCores == needCores)
			break;
		for(int j = 0; j < numCores; j++)
		{
			if(cpu[i].cores[j].Load == (needLoad + 0.5))
			{
				cpu[i].cores[j].Load = 0.5;
				unloadedCores++;
				if(unloadedCores == needCores)
					break;
			}

		}
	}

	//2. Update RAM
	if(usedMemory == 0)
	{
		for(int i = 0; i < numRAMsticks; i++)
		{
			RAM[i].Loaded = false;

		}
	}
	else
	{
		bool temp = true;
		for(int i = 0; i < numRAMsticks; i++)
		{
			RAM[i].Loaded = temp;
			if(usedMemory <= (i+1)*RAM[i].Size)
			{
				temp = false;
			}
		}
	}

	//3. Update HDD.

	//Set state as Idle if there ano running jobs
	if(runningJobs == 0)
	{
		HDD.HDDstate = 1;
	}
	// Else: there are jobs running--> loaded state
	else if(runningJobs > 0)
	{
		HDD.HDDstate = 2;
	}

	removeLoadedTime(loadedTime); //remove the loaded time of the job
	printLoadedTime();
	HDD.loadedTime = computeLoadedTime();


//	if(getFullPath() == "HPCNetwork.datacenter.server[26].tcpApp[0]")
//    {
//    	std::cerr << "Running jobs after job end: " << runningJobs << endl;
//    	std::cerr << "Job needs cores: " << needCores << endl;
//		for(int i = 0; i < numCPUs; i++)
//		{
//			for(int j = 0; j < numCores; j++)
//			{
//				std::cerr << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;
//
//			}
//		}
//
//    }

	for(int i = 0; i < numCPUs; i++)
	{
		for(int j = 0; j < numCores; j++)
		{
			EV << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;

		}
	}

	for(int i = 0; i < numRAMsticks; i++)
	{
		EV << "RAMstick: " << i << " Size: " << RAM[i].Size  << " Loaded: " << RAM[i].Loaded << endl;
	}

	EV << "Hard disk, state: " << HDD.HDDstate << " loaded time: " << HDD.loadedTime << endl;


	sendMessage(appmsg);

}

void SuperComServerApp::sleepResponse(cMessage *msg)
{
	SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
	int connId = intMsg->getConnId();
	delete intMsg;

	SuperComMsg* appmsg = new SuperComMsg("sleepResponse");

	TCPSendCommand *cmd = new TCPSendCommand();
	cmd->setConnId(connId);
	appmsg->setControlInfo(cmd);

	// set length and send it back
	appmsg->setKind(TCP_C_SEND);
	appmsg->setByteLength(250); //TODO: define this better
	appmsg->setType(MSGKIND_SLEEP_RESPONSE);
	BEV << "Sending a sleep response to the RMS." << endl;
	sendMessage(appmsg);

}

void SuperComServerApp::wakeupResponse(cMessage *msg)
{
	SuperComInternalMsg* intMsg = (SuperComInternalMsg*)msg;
	int connId = intMsg->getConnId();
	IPvXAddress clientAddr = intMsg->getClientAddress();
	delete intMsg;

	SuperComMsg* appmsg = new SuperComMsg("wakeupResponse");

	TCPSendCommand *cmd = new TCPSendCommand();
	cmd->setConnId(connId);
	appmsg->setControlInfo(cmd);

	// set length and send it back
	appmsg->setKind(TCP_C_SEND);
	appmsg->setByteLength(250); //TODO: define this better
	appmsg->setType(MSGKIND_WAKEUP_RESPONSE);
	appmsg->setClientAddress(clientAddr);
	BEV << "Sending a wakeup response to the RMS." << endl;
	BEV << "Calculating energy consumption from: " << from << " to: " << simTime() << endl;

	stats->incrementWakeupTime(SIMTIME_DBL(simTime()-from));

	//Calculate energy for all components
	computeEnergy();

	from = simTime();

	//Update the load to the cores (load at IDLE state)
	for(int i = 0; i < numCPUs; i++)
	{
		for(int j = 0; j < numCores; j++)
		{
			cpu[i].cores[j].Load = 0.5;
		}
	}

	//Update HDD. Set state as Idle
	HDD.HDDstate = 1;

	sendMessage(appmsg);

}

void SuperComServerApp::handleMessage(cMessage *msg)
{
    if (msg->isSelfMessage())
    {
    	if(msg->getKind() == INTERNAL_COMPLETE)
    	{
    		response(msg);
    	}
    	else if(msg->getKind() == INTERNAL_SLEEP_RESPONSE)
    	{
    		sleepResponse(msg);

    	}
    	else if(msg->getKind() == INTERNAL_WAKEUP_RESPONSE)
		{
			wakeupResponse(msg);

		}
    	else
    	{
    		sendMessage(msg);
    	}
    }
    else if (msg->getKind()==TCP_I_PEER_CLOSED)
    {
        // we'll close too, but only after there's surely no message
        // pending to be sent back in this connection
        msg->setName("close");
        msg->setKind(TCP_C_CLOSE);
        sendMessage(msg);
    }
    else if (msg->getKind()==TCP_I_DATA || msg->getKind()==TCP_I_URGENT_DATA)
    {
    	SuperComMsg *appmsg = dynamic_cast<SuperComMsg*>(msg);

        if (!appmsg)
            error("Message (%s)%s is not a SuperComMsg -- "
                  "probably wrong client app, or wrong setting of TCP's "
                  "sendQueueClass/receiveQueueClass parameters "
                  "(try \"TCPMsgBasedSendQueue\" and \"TCPMsgBasedRcvQueue\")",
                  msg->getClassName(), msg->getName());

        msgsRcvd++;
        bytesRcvd += appmsg->getByteLength();

      //  long requestedBytes = appmsg->getExpectedReplyLength();

        simtime_t msgDelay = appmsg->getReplyDelay();
        if (msgDelay>maxMsgDelay)
            maxMsgDelay = msgDelay;

        switch(appmsg->getType())
        {
			case MSGKIND_REQUEST:
			{
				// bool doClose = appmsg->getServerClose();
				int connId = check_and_cast<TCPCommand *>(appmsg->getControlInfo())->getConnId();

				EV << "Before job allocation: " << endl;
				for(int i = 0; i < numCPUs; i++)
				{
					for(int j = 0; j < numCores; j++)
					{
						EV << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;

					}
				}

//				if(getFullPath() == "HPCNetwork.datacenter.server[26].tcpApp[0]")
//			    {
//					std::cerr << "Running jobs before job allocation: " << runningJobs << " simTime: "  << simTime() << endl;
//					std::cerr << "job load: " << appmsg->getLoad() << endl;
//					for(int i = 0; i < numCPUs; i++)
//					{
//						for(int j = 0; j < numCores; j++)
//						{
//							std::cerr << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;
//
//						}
//					}
//
//			    }

				//Increment the number of running jobs
				runningJobs++;



				if(runningJobs > 4)
					error("A server cannot have more than 4 jobs running simultaneously");

				// Get the runtime and schedule a complete event
				runTime = appmsg->getRunTime();
				needMemory = appmsg->getMemory();
				needCores = appmsg->getCores();
				needLoad = appmsg->getLoad();
				loadedTime = appmsg->getLoadedTime();
				SuperComInternalMsg* intMsg = new SuperComInternalMsg("complete", INTERNAL_COMPLETE);
				intMsg->setConnId(connId);
				intMsg->setCores(needCores);
				intMsg->setLoad(needLoad);
				intMsg->setNeedMemory(needMemory);
				intMsg->setInputData(appmsg->getInputData());
				intMsg->setNodes(appmsg->getNodes());
				intMsg->setClientAddress(appmsg->getClientAddress());
				intMsg->setLoadedTime(loadedTime);
				scheduleAt(simTime()+runTime,intMsg);

				// Insert the loaded time into the loadedTime vector
				LoadedTimeVector.insert(LoadedTimeVector.end(), loadedTime);

				//Output data is a proportion of the whole input data of a job
				//outputData = appmsg->getInputData() / appmsg->getNodes();
				usedMemory = usedMemory + needMemory;
				if(usedMemory > memory)
					error("Used memory cannot be larger than total memory.");
				//outputData = par("outputData");

				//std::cerr << "Input data received in Server. Simtime: " << simTime() << endl;

				BEV << "Received a job to be executed from the RMS." << endl;
				BEV << "Job running for: " << runTime << " need Memory: " << needMemory << " needCores: " << needCores << endl;
				BEV << "Load on the server: " << needLoad << endl;
				BEV << "Loaded time: " << loadedTime << endl;
				BEV << "Input data length: " << appmsg->getByteLength() << endl;
				BEV << "Calculating energy consumption from: " << from << " to: " << simTime() << endl;
				BEV << "Used memory: " <<  usedMemory << endl;
				BEV << "Total memory: " <<  memory << endl;

				//Calculate energy for all components
				computeEnergy();

				//BEV << "CPU: " << energyCPU << " RAM: " << energyRAM << " FAN: " << energyFAN << " PSU: " << energyPSU << endl;

				//Update as the last measured time
				from = simTime();

				int jobCores = 0; //How many cores are allocated to the job

				//1. Update the load to the cores (has to be updated after energy calculation for idle state)
				for(int i = 0; i < numCPUs; i++)
				{
					for(int j = 0; j < numCores; j++)
					{
						// Allocate the cores that have a load at idle state
						if(cpu[i].cores[j].Load == 0.5)
						{
							cpu[i].cores[j].Load = needLoad + 0.5;
							if(cpu[i].cores[j].Load > 100)
							{
								cpu[i].cores[j].Load = 100;
							}
							jobCores++; //increment the allocated cores
							if(jobCores == needCores) //if all needed cores are allocated, break
							{
								break;
							}
						}
					}
					if(jobCores == needCores) //if all needed cores are allocated, break
					{
						break;
					}
				}

//				if(getFullPath() == "HPCNetwork.datacenter.server[26].tcpApp[0]")
//			    {
//			    	std::cerr << "Running jobs after job allocation: " << runningJobs << endl;
//			    	std::cerr << "Job needs cores: " << needCores << endl;
//					for(int i = 0; i < numCPUs; i++)
//					{
//						for(int j = 0; j < numCores; j++)
//						{
//							std::cerr << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;
//
//						}
//					}
//
//			    }


				for(int i = 0; i < numCPUs; i++)
				{
					for(int j = 0; j < numCores; j++)
					{
						EV << "CPU: " << i << " Core: " << j  << " Load: " << cpu[i].cores[j].Load << endl;

					}
				}

				//2. Update RAM
				bool temp = true;

				for(int i = 0; i < numRAMsticks; i++)
				{
					RAM[i].Loaded = temp;
					if(usedMemory <= (i+1)*RAM[i].Size)
					{
						temp = false;
					}

				}

				for(int i = 0; i < numRAMsticks; i++)
				{
					EV << "RAMstick: " << i << " Size: " << RAM[i].Size  << " Loaded: " << RAM[i].Loaded << endl;
				}


				//3. Update HDD. Set state as loaded and update loaded time
				HDD.loadedTime = computeLoadedTime();
				HDD.HDDstate = 2;

				printLoadedTime();
				EV << "Hard disk, state: " << HDD.HDDstate << " loaded time: " << HDD.loadedTime << endl;

				delete appmsg;
				break;

			}
			case MSGKIND_SLEEP:
			{
				int connId = check_and_cast<TCPCommand *>(appmsg->getControlInfo())->getConnId();
				SuperComInternalMsg* intMsg = new SuperComInternalMsg("sleepResponse", INTERNAL_SLEEP_RESPONSE);
				intMsg->setConnId(connId);
				scheduleAt(simTime(),intMsg);

				stats->incrementSleepMsg();

				BEV << "Received a sleep msg from the RMS." << endl;
				BEV << "Sleep mode: " << appmsg->getSleepMode() << endl;
				BEV << "Calculating energy consumption from: " << from << " to: " << simTime() << endl;

				//Calculate energy for all components
				computeEnergy();

				from = simTime();

				//Update the load to the cores (load at down state)
				for(int i = 0; i < numCPUs; i++)
				{
					for(int j = 0; j < numCores; j++)
					{
						cpu[i].cores[j].Load = 0.0;
					}
				}

				//Update HDD. Set state as stdby/sleep
				HDD.HDDstate = 3;

				if(appmsg->getSleepMode() == 1) //STANDBY
				{
					delay = 5;
					operationMode = 1; //set operationMode as standby
					BEV << "Standby, Setting delay as: " << delay << endl;
				}
				else if(appmsg->getSleepMode() == 2) //HYBERNATED
				{
					delay = 90;
					operationMode = 2; //set operationMode as hybernated
					BEV << "Hybernate, Setting delay as: " << delay << endl;
				}

				delete appmsg;
				break;

			}
			case MSGKIND_WAKEUP:
			{
				int connId = check_and_cast<TCPCommand *>(appmsg->getControlInfo())->getConnId();
				SuperComInternalMsg* intMsg = new SuperComInternalMsg("wakeupResponse", INTERNAL_WAKEUP_RESPONSE);
				intMsg->setConnId(connId);
				intMsg->setClientAddress(appmsg->getClientAddress());
				scheduleAt(simTime() + delay,intMsg);
				BEV << "Received a wakeup msg from the RMS. Sending a reponse at: " << simTime() + delay << endl;

				BEV << "Calculating energy consumption from: " << from << " to: " << simTime() << endl;

				stats->incrementWakeupMsg();

				//Compute energy
				computeEnergy();

				operationMode = 0; //set operationMode as powered on

				stats->incrementSleepTime(SIMTIME_DBL(simTime()-from));

				from = simTime();

				//Update the load to the cores (load at waking up state)
				for(int i = 0; i < numCPUs; i++)
				{
					for(int j = 0; j < numCores; j++)
					{
						cpu[i].cores[j].Load = 0.5 + 20;
					}
				}

				//Update HDD. Set state as waking up
				HDD.HDDstate = 4;

				delete appmsg;
				break;

			}
			case MSGKIND_INPUT:
			{
				BEV << "Finished receiving input data from the RMS." << endl;
				BEV << "Amount of input data: " << appmsg->getByteLength()  << endl;
				delete appmsg;
				break;
			}
			default:
			{
				delete appmsg;
				break;
			}

        }

       // delete appmsg;

    }
    else
    {
        // Established message, some indication -- ignore
        delete msg;
    }

    if (ev.isGUI())
    {
        char buf[64];
        sprintf(buf, "rcvd: %ld pks %ld bytes\nsent: %ld pks %ld bytes", msgsRcvd, bytesRcvd, msgsSent, bytesSent);
        getDisplayString().setTagArg("t",0,buf);
    }
}


void SuperComServerApp::computeEnergy()
{

	//The server was powered on
	//if(powerOn == true)
	if(operationMode == 0)
	{
		totalEnergy_momentary = 0;

		energyCPU_vector = 0;
		//1. Calculate CPU Energy (energy consumption during job execution)
		for(int i=0;i<numCPUs;i++){

			energyCPU_momentary = cpu[i].computePower(from, simTime());
			energyCPU += energyCPU_momentary;
			energyCPU_vector += energyCPU_momentary;
			totalEnergy_momentary += energyCPU_momentary;
			EV << "Energy CPU momentary: " << energyCPU_momentary << endl;
		}
		stats->recordCPUEnergyVector(energyCPU_vector);


		energyRAM_vector = 0;
		//2. Calculate RAM Energy
		for(int i=0;i<numRAMsticks;i++)
		{
			energyRAM_momentary = RAM[i].computeEnergy(from, simTime());
			energyRAM += energyRAM_momentary;
			energyRAM_vector += energyRAM_momentary;
			totalEnergy_momentary += energyRAM_momentary;
			EV << "Energy RAM momentary: " << energyRAM_momentary << endl;
		}
		stats->recordRAMEnergyVector(energyRAM_vector);

		//3. Calculate HDD energy
		energyHDD_momentary = HDD.computeEnergy(from, simTime());
		energyHDD += energyHDD_momentary;
		totalEnergy_momentary += energyHDD_momentary;
		EV << "Energy HDD momentary: " << energyHDD_momentary << endl;

		//4. Calculate NIC energy
		energyNIC_momentary = nic.computeEnergy(from, simTime(), EthModule);
		energyNIC += energyNIC_momentary;
		totalEnergy_momentary += energyNIC_momentary;
		EV << "Energy NIC momentary: " << energyNIC_momentary << endl;
		//std::cerr << "Energy NIC momentary: " << energyNIC_momentary << endl;

		//5. Calculate Mainboard Energy
		energyMB_momentary = (40 * SIMTIME_DBL(simTime()-from));
		energyMB += energyMB_momentary;
		totalEnergy_momentary += energyMB_momentary;
		EV << "Energy MB momentary: " << energyMB_momentary << endl;

		//6. Calculate FAN Energy
		energyFAN_momentary = fan.computeEnergy(from, simTime());
		energyFAN += energyFAN_momentary;
		stats->recordFANEnergyVector(energyFAN_momentary);
		totalEnergy_momentary += energyFAN_momentary;
		EV << "Energy FAN momentary: " << energyFAN_momentary << endl;

		double serverPower = 0.0;
		serverPower = totalEnergy_momentary/(SIMTIME_DBL(SimTime()-from));
		std::cerr << "serverPower = " << serverPower << endl;
		//7. Calculate PSU energy
		for(int i=0;i<numPSU;i++)
		{
			energyPSU_momentary = psu[i].computePSUEnergy(from, simTime(), numPSU, serverPower); //Power unknown
			//energyPSU_momentary = psu[i].computeEnergy(from, simTime()); //Power known
			energyPSU += energyPSU_momentary;
			totalEnergy_momentary += energyPSU_momentary;
			EV << "Energy PSU momentary: " << energyPSU_momentary << endl;
		}

		EV << "Total energy momentary: " << totalEnergy_momentary << endl;


		//Total energy vector
		stats->recordTotalEnergyVector(totalEnergy_momentary);

		//for dynamic PUE (MiM 7.9.2011):
		//update totalEnergy*PUE and totalEnergy*PUE*ESC=totalEmissions

		//get the current PUE from RMS
		EV << "getting RMS module... " << endl;
		cModule *RMSmodule = getParentModule()->getParentModule()->getSubmodule("rms")->getSubmodule("tcpApp", 0);
		SuperComRmsApp *rmsapplication = check_and_cast<SuperComRmsApp*>(RMSmodule);
		double currentPUE = rmsapplication->getPUE();
		EV << "currentPUE = " << currentPUE << endl;

		//calculate ICTenergy * PUE
		stats->recordTotalEnergyWithPUEVector(totalEnergy_momentary * currentPUE);
		stats->addTotalEnergyWithPUE(totalEnergy_momentary * currentPUE);
		EV << "Total Energy momentary with PUE = " << totalEnergy_momentary * currentPUE << endl;
		//update totalEmissions = ICTenergy*ESC*PUE=ICTenergy*CUE
		double currentCUE = rmsapplication->getCUE();
		//stats->addSiteEmissions(totalEnergy_momentary * currentCUE);
		dcStats->addSiteEmissions(totalEnergy_momentary * currentCUE);
		EV << "Site Emissions momentary = " << totalEnergy_momentary * currentCUE << endl;

		if (isnan(totalEnergy_momentary) == true)
		{
			std::cerr << "totalEnergy_momentary is NaN = " << totalEnergy_momentary << endl;

		}
		//end of dynamic PUE changes (MiM)
	}
	//The server was NOT powered on
	else
	{
		//Sleep/stdby mode
		if(operationMode == 1)
		{
			//In sleep/stdby mode, the server consumes 1-6W of power by the RAM
			//With 4 RAMsticks, the server consums approx. 6.7 W
			// --> 95 % reduction in 500s compared to 150W power in idle
			//TODO: Wakeup time with sleep/stdby should be smaller
			//TODO: PSU energy in sleep mode?

			totalEnergy_momentary = 0;
			energyRAM_vector = 0;
			//Calculate RAM Energy
			for(int i=0;i<numRAMsticks;i++)
			{
				energyRAM_momentary = RAM[i].computeEnergy(from, simTime());
				energyRAM += energyRAM_momentary;
				energyRAM_vector += energyRAM_momentary;
				totalEnergy_momentary += energyRAM_momentary;
				EV << "Energy RAM momentary: " << energyRAM_momentary << endl;
			}
			stats->recordRAMEnergyVector(energyRAM_vector);

//			//Calculate PSU energy
//			for(int i=0;i<numPSU;i++)
//			{
//				energyPSU_momentary = psu[i].computeEnergy(from, simTime());
//				energyPSU += energyPSU_momentary;
//				totalEnergy_momentary += energyPSU_momentary;
//			}

			//Total energy vector
			stats->recordTotalEnergyVector(totalEnergy_momentary);

			EV << "Total momentary: " << totalEnergy_momentary << endl;

			//for dynamic PUE (MiM 7.9.2011):
			//update totalEnergy*PUE and totalEnergy*PUE*ESC=totalEmissions

			//get the current PUE from RMS
			EV << "getting RMS module... " << endl;
			cModule *RMSmodule = getParentModule()->getParentModule()->getSubmodule("rms")->getSubmodule("tcpApp", 0);
			SuperComRmsApp *rmsapplication = check_and_cast<SuperComRmsApp*>(RMSmodule);
			double currentPUE = rmsapplication->getPUE();
			EV << "currentPUE = " << currentPUE << endl;

			//calculate ICTenergy * PUE
			stats->recordTotalEnergyWithPUEVector(totalEnergy_momentary * currentPUE);
			stats->addTotalEnergyWithPUE(totalEnergy_momentary * currentPUE);
			EV << "Total Energy momentary with PUE = " << totalEnergy_momentary * currentPUE << endl;
			//update totalEmissions = ICTenergy*ESC*PUE=ICTenergy*CUE
			double currentCUE = rmsapplication->getCUE();
			stats->addSiteEmissions(totalEnergy_momentary * currentCUE);
			EV << "Site Emissions momentary = " << totalEnergy_momentary * currentCUE << endl;
			//end of dynamic PUE changes (MiM)

		}
		//Hibernation
		else if (operationMode == 2)
		{
			EV << "No energy consumed during hibernation.." << endl;

		}
	}

	from = simTime(); //update it inside here, not where computeEnergy() is called; RMS->computeEnergy() needs it here (MiM 8.9.2011)

}

double SuperComServerApp::computeEstimate(int cores, int memory, int coreLoad, double loadedTime, int runTime)
{
	double totalEnergy_temp = 0.0;
	double energyFAN_temp = 0.0;
	double energyCPU_temp = 0.0;
	double energyMem_temp = 0.0;
	double energyHDD_temp = 0.0;
	double energyPSU_temp = 0.0;
	double energyMB_temp = 0.0;
	double energyNIC_temp = 0.0;


	//1. Compute CPU power estimate
	energyCPU_temp = getCPUEstimate(cores, coreLoad, runTime);
	EV << "Power CPU: " << energyCPU_temp/runTime << endl;
	totalEnergy_temp += energyCPU_temp;

	//2. Compute memory power estimate
	energyMem_temp = getMemoryEstimate(memory, runTime);
	EV << "Power Mem: " << energyMem_temp/runTime << endl;
	totalEnergy_temp += energyMem_temp;

	//3. Compute HDD estimate. Set state as loaded and update loaded time
	HardDisk tempHDD;
	tempHDD = HDD;
	tempHDD.loadedTime = loadedTime;
	tempHDD.HDDstate = 2;

	energyHDD_temp = tempHDD.computeEnergy(simTime(), simTime()+runTime);
	totalEnergy_temp += energyHDD_temp;
	EV << "Power HDD: " << energyHDD_temp/runTime << endl;

	//4. Compute FAN estimate
	energyFAN_temp = fan.computeEnergy(simTime(), simTime()+runTime);
	totalEnergy_temp += energyFAN_temp;
	EV << "Power fan: " << energyFAN_temp/runTime << endl;

	//5. Compute mainboard estimate
	energyMB_temp = (40 * runTime);
	totalEnergy_temp += energyMB_temp;;
	EV << "Power MB: " << energyMB_temp/runTime << endl;

	//6. Compute NIC estimate. Just compute idle power?
	NIC tempNIC;
	tempNIC = nic;
	energyNIC_temp = tempNIC.P_idle * runTime;
	totalEnergy_temp += energyNIC_temp;
	EV << "Power NIC: " << energyNIC_temp/runTime << endl;

	//7. Compute PSU estimate
	std::vector<PSU> tempPSU;
	tempPSU = psu;
	for(int i=0;i<numPSU;i++)
	{
		energyPSU_temp = psu[i].computePSUEnergy(simTime(), simTime()+runTime, numPSU, totalEnergy_temp/runTime); //Power unknown
		//energyPSU_temp = tempPSU[i].computeEnergy(simTime(), simTime()+runTime); //Power known
		totalEnergy_temp += energyPSU_temp;
	}
	EV << "Power psu: " << energyPSU_temp/runTime << endl;
	std::cerr << "Power psu: " << energyPSU_temp/runTime << endl;

	EV << "Power Total: " << totalEnergy_temp/runTime << endl;

	return totalEnergy_temp;
}

double SuperComServerApp::getCPUEstimate(int cores, int coreLoad, int runTime)
{
	double energyCPU_temp = 0.0;
	double energyCPU_tempMomentary = 0.0;

	std::vector<CPU> tempCPU;
	tempCPU = cpu;
	int jobCores = 0; //How many cores are allocated to the job

	// Update the load to the cores (has to be updated after energy calculation for idle state)
	for(int i = 0; i < numCPUs; i++) {
		for(int j = 0; j < numCores; j++) {
			// Allocate the cores that have a load at idle state
			if(tempCPU[i].cores[j].Load == 0.5) {
				tempCPU[i].cores[j].Load = coreLoad + 0.5;
				if(tempCPU[i].cores[j].Load > 100) {
					tempCPU[i].cores[j].Load = 100;
				}
				jobCores++; //increment the allocated cores
				if(jobCores == cores) //if all needed cores are allocated, break
					break;
			}
		}
		if(jobCores == cores) //if all needed cores are allocated, break
			break;
	}

	//Compute estimated energy
	for(int i=0;i<numCPUs;i++){
		energyCPU_tempMomentary = tempCPU[i].computePower(simTime(), simTime()+runTime);
		energyCPU_temp += energyCPU_tempMomentary;
	}

	return energyCPU_temp;
}

double SuperComServerApp::getMemoryEstimate(int memory, int runTime)
{
	double energyRAM_temp = 0.0;
	double energyRAM_tempMomentary = 0.0;

	std::vector<RAMstick> tempRAM;
	tempRAM = RAM;

	//Set estimated memory usage
	bool temp = true;
	for(int i = 0; i < numRAMsticks; i++) {
		tempRAM[i].Loaded = temp;
		if(memory <= (i+1)*tempRAM[i].Size) {
			temp = false;
		}
	}
	//Compute estimated energy
	for(int i = 0;i < numRAMsticks; i++)
	{
		energyRAM_tempMomentary = tempRAM[i].computeEnergy(simTime(), simTime()+runTime);
		energyRAM_temp += energyRAM_tempMomentary;
	}

	return energyRAM_temp;

}

double SuperComServerApp::computeLoadedTime()
{
	double min = LoadedTimeVector[0];
	double max = 0;
	double avg = 0;

	// Get the minimum value (maximum value of the elements)
	for(int i = 0; i < (int) LoadedTimeVector.size(); i++)
	{
		if(LoadedTimeVector[i] < min)
		{
			min = LoadedTimeVector[i];
		}
	}

	EV << "Minimum value: " << min << endl;

	// Compute the maximum value (sum of all elements)
	for(int i = 0; i < (int) LoadedTimeVector.size(); i++)
	{
		max = max + LoadedTimeVector[i];
	}

	EV << "Maximum value: " << max << endl;

	// Compute the average
	avg = (min+max)/2;
	if(avg > 1)
		avg = 1;

	return avg;
}

void SuperComServerApp::removeLoadedTime(double loadedTime)
{

	// Erase the specified loaded time from the LoadedTimeVector
	for(int i = 0; i < (int) LoadedTimeVector.size(); i++)
	{
		if(LoadedTimeVector[i] == loadedTime)
		{
			LoadedTimeVector.erase(LoadedTimeVector.begin()+i);
		}
	}

}

void SuperComServerApp::printLoadedTime()
{

	// Erase the specified loaded time from the LoadedTimeVector
	for(int i = 0; i < (int) LoadedTimeVector.size(); i++)
	{
		EV << "Loaded time index: " << i << " value: " << LoadedTimeVector[i] << endl;
	}

}

void SuperComServerApp::finish()
{

	BEV << "Calculating energy consumption from: " << from << " to: " << simTime() << endl;
	//Calculate energy for all components
	computeEnergy();

    //Calculate the total energy
    TotalEnergy = energyCPU + energyRAM + energyFAN + energyHDD + energyPSU + energyNIC + energyMB;

    //Add the consumed energies to the global statistics
    stats->addTotalEnergy(TotalEnergy);
    stats->addCPUenergy(energyCPU);
    stats->addRAMenergy(energyRAM);
    stats->addFANenergy(energyFAN);
    stats->addNICenergy(energyNIC);
    stats->addHDDenergy(energyHDD);
    stats->addPSUenergy(energyPSU);
    stats->addMBenergy(energyMB);

    dcStats->addTotalEnergy(TotalEnergy);


    EV << getFullPath() << ": sent " << bytesSent << " bytes in " << msgsSent << " packets\n";
    EV << getFullPath() << ": received " << bytesRcvd << " bytes in " << msgsRcvd << " packets\n";
    BEV << getFullPath() << ": Total Energy: " << TotalEnergy << endl;
    BEV << getFullPath() << ": CPU energy: " << energyCPU << endl;
    BEV << getFullPath() << ": RAM energy: " << energyRAM << endl;
    BEV << getFullPath() << ": FAN energy: " << energyFAN << endl;
    BEV << getFullPath() << ": HDD energy: " << energyHDD << endl;
    BEV << getFullPath() << ": PSU energy: " << energyPSU << endl;
    BEV << getFullPath() << ": MB energy: " << energyMB << endl;
    BEV << getFullPath() << ": NIC energy: " << energyNIC << endl;
    BEV << getFullPath() << ": Sim time: " << simTime() << endl;

    recordScalar("packets sent", msgsSent);
    recordScalar("packets rcvd", msgsRcvd);
    recordScalar("bytes sent", bytesSent);
    recordScalar("bytes rcvd", bytesRcvd);

    recordScalar("Total Energy", TotalEnergy);
	recordScalar("CPU energy", energyCPU);
	recordScalar("RAM energy", energyRAM);
	recordScalar("FAN energy", energyFAN);
	recordScalar("HDD energy", energyFAN);
	recordScalar("MB energy", energyMB);
	recordScalar("NIC energy", energyNIC);
	recordScalar("PSU energy", energyPSU);


}
