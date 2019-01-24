#include "NIC.h"


using namespace std;

NIC::NIC(){

    PrevRcvd = PrevSent = CurSent = CurRcvd = Rcvd = Sent = pps = 0;

}

/**
 *  Function for computing NIC energy with a linear model. The model is
 *  mainly valid for low to medium traffic (up to 70 000 packets per second).
 *  It becomes quite unlinear for higher rates.
 */
double NIC::computeEnergy(simtime_t from, simtime_t to, cModule *module)
{


	ethMac = check_and_cast<EtherMAC*> (module);
	double E = 0.0;

	if(from != to)
	{
		CurSent= ethMac->getSent();
		CurRcvd = ethMac->getRcvd();

		Rcvd = CurRcvd - PrevRcvd;
		Sent = CurSent - PrevSent;
		EV << " PrevSent: " << PrevSent << " PrevRcvd: " << PrevRcvd << endl;
		PrevRcvd = CurRcvd;
		PrevSent = CurSent;

		EV << " CurSent: " << CurSent << " CurRcvd: " << CurRcvd << endl;
		EV << " Packets sent: " << Sent << " Packets rcvd: " << Rcvd << endl;

		pps = (Rcvd + Sent) / (SIMTIME_DBL(to-from));

		EV << " Packets per second: " << pps << endl;

		E = P_idle + (P_max - P_idle) * pps;
	}

	//multiply E with the time elapsed
	E = E * SIMTIME_DBL(to-from);
	return E;

}
