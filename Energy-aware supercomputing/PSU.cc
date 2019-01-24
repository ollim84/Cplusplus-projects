#include "PSU.h"

using namespace std;

PSU::PSU(){

}

/**
 *  Function for computing PSU energy when the measured power of
 *  the PSU is known
 */
double PSU::computeEnergy(simtime_t from, simtime_t to)
{
	double E = 0.0;

	E = (PSUpower * (100 - PSUefficiency)) / 100;

	//multiply E with the time elapsed
	E = E * SIMTIME_DBL(to-from);

	return E;

}

/**
 * Function for computing PSU energy when the measured power of
 * the PSU is not known
 */

double PSU::computePSUEnergy(simtime_t from, simtime_t to, int numPSU, double serverPower)
{
	double E = 0.0;

	E = ((serverPower/(numPSU * PSUefficiency)) * 100) - (serverPower * numPSU);

	//EV << "PSU: " << (serverPower/(numPSU * PSUefficiency)) * 100  << endl;
	//std::cerr << "PSU power: " << E << endl;

	//multiply E with the time elapsed
	E = E * SIMTIME_DBL(to-from);

	return E;
}
