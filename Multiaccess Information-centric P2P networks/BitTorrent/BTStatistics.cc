//
// Copyright 2009 Konstantinos V. Katsaros
//                              ntinos@aueb.gr
//                              http://mm.aueb.gr/~katsaros
//

// This file is part of BitTorrent Implementation for OMNeT++.

//    BitTorrent Implementation for OMNeT++ is free software: you can redistribute
//	  it and/or modify it under the terms of the GNU General Public License as
//    published by the Free Software Foundation, either version 2 of the License,
//	  or (at your option) any later version.

//    BitTorrent Implementation for OMNeT++ is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License
//    along with BitTorrent Implementation for OMNeT++.
//    If not, see <http://www.gnu.org/licenses/>.


#include "BTStatistics.h"


Define_Module(BTStatistics);

BTStatistics::~BTStatistics()
{
	delete dwSuccess;
	delete numBlockFail;
	delete dataProviders;
	delete numSeederBlocks;
	delete startupTime;
	delete downloadRate;
	delete uploadRate;
}


void BTStatistics::initialize()
{
	currentTerminalNum = 0;
	targetOverlayTerminalNum = par("targetOverlayTerminalNum");
	dwSuccess = new  cStdDev("BitTorrent:Download Duration");
	dwSuccess_vec.setName("BitTorrent:Download Duration");

	numBlockFail = new  cStdDev("BitTorrent:Failed Downloads:Number of Completed Blocks");
	numBlockFail_vec.setName("BitTorrent:Failed Downloads:Number of Completed Blocks");

	dataProviders = new cStdDev("BitTorrent:Number of Distinct Data Providing Peers");
	dataProviders_vec.setName("BitTorrent:Number of Distinct Data Providing Peers");

	numSeederBlocks = new cStdDev("BitTorrent:Number of Blocks Download From Seeder");
	numSeederBlocks_vec.setName("BitTorrent:Number of Blocks Download From Seeder");
	startupTime = new cStdDev("BitTorrent:Startup Time of The Peers");
	startupTime_vec.setName("BitTorrent:Startup Time of The Peers");

	downloadRate = new cStdDev("BitTorrent:Download rate of The Peers");
	downloadRate_vec.setName("BitTorrent:Download rate of The Peers");

	uploadRate = new cStdDev("BitTorrent:Upload rate of The Peers");
	uploadRate_vec.setName("BitTorrent:Upload rate of The Peers");
}


void BTStatistics::handleMessage(cMessage* msg)
{
	switch (msg->kind())
	{
		case BT_STATS_DWL:
		{
			BTStatisticsDWLMsg* dwMsg = dynamic_cast<BTStatisticsDWLMsg*>(msg);
			double dwTime = dwMsg->downloadTime();
			double rmBlocks = dwMsg->remainingBlocks();
			if (rmBlocks == 0 )
			{
				dwSuccess->collect(dwTime);
				dwSuccess_vec.record(dwTime);
			}
			else
			{
				numBlockFail->collect(rmBlocks);
				numBlockFail_vec.record(rmBlocks);
			}			
			checkFinish();
			delete msg;
			break;
		}
		case BT_STATS_PP:
		{
			BTStatisticsNumProvidersMsg* ppMsg = dynamic_cast<BTStatisticsNumProvidersMsg*>(msg);
			dataProviders->collect(ppMsg->numPeers());
			dataProviders_vec.record(ppMsg->numPeers());			
			delete msg;
			break;
		}
		case BT_STATS_NSB:
		{
			BTStatisticsNumSeederBlocksMsg* nsbMsg = dynamic_cast<BTStatisticsNumSeederBlocksMsg*>(msg);
			numSeederBlocks->collect(nsbMsg->numSeederBlocks());
			numSeederBlocks_vec.record(nsbMsg->numSeederBlocks());			
			delete msg;
			break;
		}

		case BT_STATS_EXIT:
		{
			delete msg;
			doFinish();
			break;
		}

    		case BT_STATS_STIME:
		{
        		BTStatisticsStartupTimeMsg* stMsg = dynamic_cast<BTStatisticsStartupTimeMsg*> (msg);
        		startupTime->collect(stMsg->startupTime());
        		startupTime_vec.record(stMsg->startupTime());			
        		delete msg;
        		break;
		}
		case BT_STATS_DRATE:
		{
        		BTStatisticsDownloadRateMsg* drMsg = dynamic_cast<BTStatisticsDownloadRateMsg*> (msg);
        		downloadRate->collect(drMsg->downloadRate());
        		downloadRate_vec.record(drMsg->downloadRate());			
        		delete msg;
        		break;
		}
		case BT_STATS_URATE:
		{
        		BTStatisticsUploadRateMsg* urMsg = dynamic_cast<BTStatisticsUploadRateMsg*> (msg);
        		uploadRate->collect(urMsg->uploadRate());
        		uploadRate_vec.record(urMsg->uploadRate());			
        		delete msg;
        		break;
		}
		default:
		{
			opp_error("Unknown message type %d",msg->kind());
			break;
		}
	}
}

void BTStatistics::checkFinish()
{
	currentTerminalNum++;
	cerr<<"******Check Finish Function************** Simtime: "<< simTime() << " seconds. Current terminal number: " << currentTerminalNum <<endl;
	//EV<<"******Check Finish Function************** Simtime: "<< simTime() << " seconds. Current terminal number: " << currentTerminalNum <<endl;
	if (currentTerminalNum == targetOverlayTerminalNum)
	{
		scheduleAt(simTime()+BT_STATS_MSG_TIME, new cMessage(NULL,BT_STATS_EXIT));
		//scheduleAt(simTime()+1000, new cMessage(NULL,BT_STATS_EXIT));
	}
}

void BTStatistics::doFinish()
{
	recordScalar("Simulation duration", simTime());
	dwSuccess->recordScalar();
	numBlockFail->recordScalar();
	dataProviders->recordScalar();
	numSeederBlocks->recordScalar();
	startupTime->recordScalar();
	downloadRate->recordScalar();
	uploadRate->recordScalar();
	endSimulation();
}

void BTStatistics::finish()
{

}

