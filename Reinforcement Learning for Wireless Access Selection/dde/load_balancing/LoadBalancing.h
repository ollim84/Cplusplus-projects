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

#ifndef __INETMANET_LOADBALANCING_H_
#define __INETMANET_LOADBALANCING_H_

#include <omnetpp.h>
#include "stdafx.h"
#include "sample.h"
#include "eventmanagerex.h"
//#include "expertsystem.h"

extern bool UseSubscriptionTypeInClientRanking;
extern bool UseLearning;
extern int PoAMargin;
extern double PoARankingLimit;
extern double ClientsPoAGradingLimit;
extern string SinglePoALoadBalancing;
extern int SimultaneousHOsPerCongestedPoA;
extern int ClientSatisfactionLimit;
extern int PoAStatusLimit;
extern bool ContinuousLoadBalancing;
extern int SimultaneousHOsToAvailablePoA;
extern bool GoldUsersPreferred;

/**
 * TODO - Generated class
 */
class LoadBalancing : public cSimpleModule
{
 public:
	vector<Event> doLoadBalancing(SampleEx poa_sample, SampleEx client_sample, map<string, int> subscription_types, map<string, string> poa_types);
 protected:
	double ClientsPoAGradingLimit_var;
	double PoARankingLimit_var;
    int  SimultaneousHOsPerCongestedPoA_var;
    int PoAMargin_var;
    const char* SinglePoALoadBalancing_var;
    int ClientSatisfactionLimit_var;
    int PoAStatusLimit_var;
    bool UseSubscriptionTypeInClientRanking_var;
    bool ContinuousLoadBalancing_var;
    bool UseLearning_var;
    int SimultaneousHOsToAvailablePoA_var;
    bool GoldUsersPreferred_var;
  protected:
    virtual void initialize();
    virtual void handleMessage(cMessage *msg);
};

#endif
