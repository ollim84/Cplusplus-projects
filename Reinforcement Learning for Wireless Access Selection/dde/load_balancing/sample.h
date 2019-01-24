//=============================================================================
// Name        : sample.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2009 VTT. All Rights Reserved.
// Description : Sample class declaration.
//=============================================================================

#pragma once

#include "event.h"

class Sample
{
public:
	Sample(void);
	Sample(double);
	Sample(const Sample& _src);
	Sample& operator = (const Sample& _src);
	virtual ~Sample(void);

	void Clear(){ Items.clear(); }
	void AddItem(string source, vector<Event> events){ Items[source] = events; }
	void InitializeEvents(const string source, int isize);
	//void AddEvent(boost::shared_ptr<Event> pevent, int index);
	//void AddOrReplaceEvent(boost::shared_ptr<Event> pevent);
	void UpdateSleepingSources(double last_timestamp);
	bool IsSourceSleeping(const string source) const;
	pair<double, string> GetTimestampAndValue(string source, string event_name) const;
	string GetValue(string source, string event_name) const;
	void SaveToFile(FILE* file);
	void PrintSample();

	virtual bool IsSamplePerfect(double maxtimedifference, bool& timedifference_exceded) { assert(0); return false; }
	virtual void ConstructDataVector(vector<double>& vdata, double& timestamp, const int cnt){ assert(0); }

	int ParseNbOfAvailablePoAs(string str) const;
	vector<pair<string, double> > ParseClientsPoAGrading(string str) const;
	pair<string, double> ParseClientQoE(string str) const;
	pair<string, vector<int> > ParseRSSINoise(string str) const;
	pair<string, vector<int> > ParseTXRXBps(string str) const;
	vector<double> ParseCapacity(string str) const;

	map<string, vector<Event> > Items; //first: source, second: events
	map<string, double> LastTimestamps; //first: source, second: the last timestamp of the events sent by the source
	vector<string> SleepingSources; //first: source, second: the last timestamp of the events sent by the source
	double MaxWaitTime; //wait time before the source is assumed to be sleeping
};

typedef Sample SampleEx;
