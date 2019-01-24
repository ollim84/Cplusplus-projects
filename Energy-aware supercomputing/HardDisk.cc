#include "HardDisk.h"

using namespace std;

HardDisk::HardDisk()
{


}

double HardDisk::computeEnergy(simtime_t from, simtime_t to)
{

	double E = 0.0;
	double E_loaded = 0.0;
	double E_idle = 0.0;
	double E_stdby = 0.0;
	double E_startup = 0.0;
	double elapsedIdleTime;
	double idleTimeThreshold = 60; // Time Threshold the HDD spends in idle mode until going into sleep

	//Idle state
	if (HDDstate == 1)
	{
		elapsedIdleTime = SIMTIME_DBL(to-from);
		if(elapsedIdleTime < idleTimeThreshold)
		{
			E = P_idle * SIMTIME_DBL(to-from);
		}
		else
		{
			E_idle = P_idle * idleTimeThreshold; // energy consumed during idle state
			E_stdby = P_stdby * (elapsedIdleTime - 60); // energy consumed during sleep/stdby state
			E = E_idle + E_stdby;
		}
	}
	//Loaded state = idle + load + energy in startup phase if needed
	else if (HDDstate == 2)
	{
		elapsedIdleTime = SIMTIME_DBL(to-from);
		if(elapsedIdleTime < idleTimeThreshold)
		{
			E_loaded = P_loaded * SIMTIME_DBL(to-from) * loadedTime; //Calculate the amount of energy in loaded state
			E_idle = P_idle * SIMTIME_DBL(to-from) * (1-loadedTime); //Calculate the amount of energy in the idle state
			E = E_loaded + E_idle; //Sum the energies
		}
		else
		{
			E_startup = P_startup * t_startup; //energy consumed during start up time
			E_loaded = P_loaded * (SIMTIME_DBL(to-from) - t_startup) * loadedTime; //Calculate the amount of energy in loaded state
			E_idle = P_idle * (SIMTIME_DBL(to-from) - t_startup) * (1-loadedTime); //Calculate the amount of energy in the idle state
			E = E_startup + E_loaded + E_idle; //Sum the energies
		}
	}
	//Standby state
	else if (HDDstate == 3)
	{
		E = P_stdby * SIMTIME_DBL(to-from);
	}
	//Waking up state
	else if(HDDstate == 4)
	{
		E_startup = P_startup * t_startup; //energy consumed during start up time
		E_idle = P_idle * (SIMTIME_DBL(to-from) - t_startup); //energy consumed during idle state (= 20s)
		E = E_startup + E_idle;
	}

	return E;
}

double HardDisk::computeStartUpPower()
{
	double E = 0.0;
	E = P_startup * t_startup;
	return E;
}
