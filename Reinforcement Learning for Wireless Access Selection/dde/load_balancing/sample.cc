//=============================================================================
// Name        : sample.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2009 VTT. All Rights Reserved.
// Description : Sample class implementation.
//=============================================================================

#include "stdafx.h"
#include "sample.h"
#include "cmn_utils.h"

extern int EnableDataTestLog;

Sample::Sample(void):
Items(),
LastTimestamps(),
SleepingSources(),
MaxWaitTime(60)
{
}

Sample::Sample(double max_wait_time):
Items(),
LastTimestamps(),
SleepingSources(),
MaxWaitTime(max_wait_time)
{
}

Sample::Sample(const Sample& _src):
Items(_src.Items),
LastTimestamps(_src.LastTimestamps),
SleepingSources(_src.SleepingSources),
MaxWaitTime(_src.MaxWaitTime)
{
}

Sample::~Sample(void)
{
}

Sample& Sample::operator=(const Sample& _src)
{
    if (&_src == this) return *this;

	Items = _src.Items;
	LastTimestamps = _src.LastTimestamps;
	SleepingSources = _src.SleepingSources;
	MaxWaitTime  = _src.MaxWaitTime;

    return *this;
}

void Sample::InitializeEvents(const string source, int isize)
{
	Event empty_event;
	vector<Event> items;
	items.assign(isize, empty_event);
	Items[source] = items;

	LastTimestamps[source] = 0; 
}
	
//void Sample::AddEvent(boost::shared_ptr<Event> pevent, int index) 
//{ 
//	Items[pevent->GetSource()][index] = *pevent; 
//	LastTimestamps[pevent->GetSource()] = pevent->GetTimeStamp(); 
//	UpdateSleepingSources(pevent->GetTimeStamp());
//
//	if (EnableDataTestLog)
//	{
//		cout << "TestLog: event " << pevent->GetEventID() << " " << pevent->GetName() << " added to the sample, index: " << index << endl;
//	}
//}

void Sample::UpdateSleepingSources(double last_timestamp)
{
	SleepingSources.clear();
	map< string, double >::const_iterator mapitr;
	for (mapitr = LastTimestamps.begin(); mapitr != LastTimestamps.end(); ++mapitr)
	{
		double td = last_timestamp - mapitr->second;
		if (td > MaxWaitTime) 
		{
			SleepingSources.push_back(mapitr->first);
		}
	}
	//if (EnableDataTestLog)
	//{
	//	cout << "TestLog: sleeping sources: ";
	//	if (SleepingSources.size() > 0)
	//	{
	//		for (unsigned int i = 0; i < SleepingSources.size(); i++)
	//		{
	//			cout << SleepingSources[i] << ", ";
	//		}
	//	}
	//	cout << endl;
	//}
}

bool Sample::IsSourceSleeping(const string source) const
{
	for (unsigned int i = 0; i < SleepingSources.size(); i++)
	{
		if (SleepingSources[i].find(source) != string::npos) 
		{
			//if (EnableDataTestLog)
			//{
			//	cout << "TestLog: source (" << source << ") ignored because it is sleeping" << endl;
			//}
			return true;
		}
	}

	return false;
}

string Sample::GetValue(string source, string event_name) const
{
	string value = "";
	map< string, vector<Event> >::const_iterator itr;
	for (itr = Items.begin(); itr != Items.end(); ++itr)
	{
		if (itr->first.find(source) != string::npos)
		{
			for (unsigned int i = 0; i < itr->second.size(); ++i)
			{
				if (itr->second[i].GetName().find(event_name) != string::npos) 
				{
					value = itr->second[i].GetValue();
					break;
				}
			}
		}
	}
	return value;
}

pair<double, string> Sample::GetTimestampAndValue(string source, string event_name) const
{
	double timestamp = DBL_MAX;
	string value = "";
	map< string, vector<Event> >::const_iterator itr;
	for (itr = Items.begin(); itr != Items.end(); ++itr)
	{
		if (itr->first.find(source) != string::npos)
		{
			for (unsigned int i = 0; i < itr->second.size(); ++i)
			{
				if (itr->second[i].GetName().find(event_name) != string::npos) 
				{
					timestamp = itr->second[i].GetTimeStamp();
					value = itr->second[i].GetValue();
					break;
				}
			}
		}
	}
	return make_pair(timestamp, value);
}

void Sample::SaveToFile(FILE* file)
{
	map< string, vector<Event> >::const_iterator mapitr;
	cout << "items " << Items.size() << endl;
	for (mapitr = Items.begin(); mapitr != Items.end(); ++mapitr)
	{
		cout << "events " << mapitr->second.size() << endl;

		vector<Event>::const_iterator itr;
		for (itr = mapitr->second.begin(); itr != mapitr->second.end(); ++itr)
		{
//			cout << "save: " << itr->GetTimeStamp() << "\t" << itr->GetSource().c_str() << "\t" << itr->GetName().c_str() << "\t" << itr->GetValue().c_str() << endl;
			fprintf(file, "%20.2f\t%s\t%s\t%s\n", itr->GetTimeStamp(), itr->GetSource().c_str(), itr->GetName().c_str(), itr->GetValue().c_str());
		}
		fflush(file);
	}
}

void Sample::PrintSample()
{
	map< string, vector<Event> >::const_iterator mapitr;
	cout << "items " << Items.size() << endl;
	for (mapitr = Items.begin(); mapitr != Items.end(); ++mapitr)
	{
		cout << "events " << mapitr->second.size() << endl;

		vector<Event>::const_iterator itr;
		for (itr = mapitr->second.begin(); itr != mapitr->second.end(); ++itr)
		{
//			cout << "save: " << itr->GetTimeStamp() << "\t" << itr->GetSource().c_str() << "\t" << itr->GetName().c_str() << "\t" << itr->GetValue().c_str() << endl;
			printf("%20.2f\t%s\t%s\t%s\n", itr->GetTimeStamp(), itr->GetSource().c_str(), itr->GetName().c_str(), itr->GetValue().c_str());
		}
	}
}

pair<string, vector<int> > Sample::ParseTXRXBps(string str) const
{
	pair<string, vector<int> > txrxbps;
	vector<string> vparts;
	::SplitString(vparts, str, ',');
	if (vparts.size() == 4) 
	{
		vector<int> values;
		values.resize(2);
		values[0] = atoi(vparts[2].c_str());
		values[1] = atoi(vparts[3].c_str());
		txrxbps = make_pair(vparts[1].c_str(), values);
	}
	else
	{
		stringstream smsg;
		smsg << "Cannot parse client's TX and RX Bps.";
		throw logic_error(smsg.str());
	}
	return txrxbps;
}

int Sample::ParseNbOfAvailablePoAs(string str) const
{
	int nb_of_poas = 0;
	vector<string> vparts;
	::SplitString(vparts, str, ';');
	if (vparts.empty())
	{
		stringstream smsg;
		smsg << "Cannot parse client's nb of available PoAs.";
		throw logic_error(smsg.str());
	}
	nb_of_poas = atoi(vparts[0].c_str());
	return nb_of_poas;
}

vector<pair<string, double> > Sample::ParseClientsPoAGrading(string str) const
{
	vector<pair<string, double> > clients_poa_grading;
	vector<string> vparts;
	::SplitString(vparts, str, ';');
	if (vparts.size() >= 3)
	{
		if (atof(vparts[0].c_str()) == vparts.size()-1)
		{
			for (unsigned int i = 1; i < vparts.size(); i++)
			{
				vector<string> vpoas;
				::SplitString(vpoas, vparts[i], ',');
				if (vpoas.size() == 2) 
				{
					clients_poa_grading.push_back(make_pair(vpoas[0], atof(vpoas[1].c_str())));
				}
				else
				{
					stringstream smsg;
					smsg << "Cannot parse client's PoA grading.";
					throw logic_error(smsg.str());
				}
			}
		}
	}
	else
	{
		stringstream smsg;
		smsg << "Cannot parse client's PoA grading. vparts.size() < 3" << endl;
		smsg << "str: " << str << endl;
		throw logic_error(smsg.str());
	}
	return clients_poa_grading;
}

pair<string, double> Sample::ParseClientQoE(string str) const
{
	pair<string, double> client_qoe;
	vector<string> vparts;
	::SplitString(vparts, str, ',');
	if (vparts.size() == 2) 
	{
		client_qoe = make_pair(vparts[0], atof(vparts[1].c_str()));
	}
	else
	{
		stringstream smsg;
		smsg << "Cannot parse client's QoE.";
		throw logic_error(smsg.str());
	}
	return client_qoe;
}

pair<string, vector<int> > Sample::ParseRSSINoise(string str) const
{
	pair<string, vector<int> > rssinoise;
	vector<string> vparts;
	::SplitString(vparts, str, ',');
	if (vparts.size() == 6) 
	{
		vector<int> values;
		for (unsigned int i = 2; i < 6; i++)
		{
			values.push_back(atoi(vparts[i].c_str()));
		}
		rssinoise = make_pair(vparts[1].c_str(), values);
	}
	else
	{
		stringstream smsg;
		smsg << "Cannot parse client's RSSI and noise.";
		throw logic_error(smsg.str());
	}
	return rssinoise;
}

vector<double> Sample::ParseCapacity(string str) const
{
	vector<double> capacity;
	vector<string> vparts;
	::SplitString(vparts, str, ',');
	if (vparts.size() == 3) 
	{
		capacity.resize(3);
		capacity[0] = atoi(vparts[0].c_str());
		capacity[1] = atoi(vparts[1].c_str());
		capacity[2] = atoi(vparts[2].c_str());
	}
	else
	{
		stringstream smsg;
		smsg << "Cannot parse capacity.";
		throw logic_error(smsg.str());
	}
	return capacity;
}


