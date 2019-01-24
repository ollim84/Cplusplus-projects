//=============================================================================
// Name        : event.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : Event class declaration.
//=============================================================================
#pragma once

const unsigned int DEFAULT_LIFETIME = 60;
const unsigned int DEFAULT_TRIGGER_LIFETIME = 7;
const unsigned int DEFAULT_STABILIZING_TIME = 5;
const int NO_VALUE = -999;

enum {
	EVENT_UNDEFINED = 0
};

enum {
	EVENT_TYPE_UNDEFINED = -1,
	EVENT_TYPE_KPI = 0,
	EVENT_TYPE_EVENT = 1
};

enum {
	NO_DESTINATION = 0,
	DESTINATION_VISUALISATION = 1,			//send event to visualisation
	DESTINATION_NES = 2,					//send event to network expert system (NES)
	DESTINATION_EC = 3,					//send event to DDE event cache
	DESTINATION_NES_AND_VISUALISATION = 4,	//send event to NES and visualisation
	DESTINATION_EC_AND_VISUALISATION = 5,	//send event to DDE event cache and visualisation
	DESTINATION_ALL = 6, 					//send event to all defined (e.g. NES, DDE event cache and visualisation, or ANC/NC, DDE event cache and visualisation)
	DESTINATION_OUT = 7,
	DESTINATION_OUT_AND_VISUALISATION = 8
};

enum {
	SOURCE_TYPE_UNKNOWN = 0, 	
	SOURCE_TYPE_NETWORK = 1,
	SOURCE_TYPE_POA = 2,
	SOURCE_TYPE_CLIENT = 3,
	SOURCE_TYPE_TRIGGER = 100 	
};

enum {
	VALUE_TYPE_UNKNOWN = 0,
	VALUE_TYPE_STATIC = 1,
	VALUE_TYPE_SEARCH_ANOTHER = 2
};

enum {
	LEVEL_UNKNOWN = 0, 	
	LEVEL_NETWORK = 1,
	LEVEL_POA = 2,
	LEVEL_CLIENT = 3
};

enum
{
	VARIABLE_TYPE_UNKNOWN = -1,
	VARIABLE_TYPE_USE_OLD = 0,
	VARIABLE_TYPE_UPDATE_REQUIRED = 1
};


class ConfigurationManager;

class Event
{
friend class ConfigurationManager;
public:
	Event(void);
	Event(string name, string src, double timestamp, int event_type, string value);
	Event(const Event& _src);
	Event& operator = (const Event& _src);
	virtual ~Event(void);

	virtual string GetName() const { return Name; }
	virtual void SetName(string name) { Name = name; }
	virtual string GetSource() const { return Source; }
	virtual void SetSource(string src) { Source = src; }
	virtual int GetDestination() const { return Destination; }
	virtual void SetDestination(int dest) { Destination = dest; }
	virtual int GetEventID() const { return EventID; }
	virtual void SetEventID(int event_id) { EventID = event_id; }
	virtual int GetEventType() const { return EventType; }
	virtual void SetEventType(int event_type) { EventType = event_type; }
	virtual double GetTimeStamp() const { return TimeStamp; }
	virtual void UpdateTime(double timestamp) { TimeStamp = timestamp; }
	virtual string GetValue() const { return Value; }
	virtual void SetValue(string value) { Value = value; }

	virtual int GetVariableType() const { return VariableType; }
	virtual void SetVariableType(int variable_type) { VariableType = variable_type; }
	virtual void SetValueType(int type) { ValueType = type; }
	virtual int GetValueType() const { return ValueType; }
	virtual void SetSourceType(int type) { SourceType = type; }
	virtual int GetSourceType() const { return SourceType; }
	virtual unsigned int GetLifetime() const { return Lifetime; }
	virtual void SetLifetime(unsigned int lifetime) { Lifetime = lifetime; }
	//virtual bool IsLifetimeExceeded() const;
	//virtual bool IsTriggerLifetimeExceeded() const;
	virtual unsigned int GetStabilizingTime() const { return StabilizingTime; }
	virtual void SetStabilizingTime(unsigned int stabtime) { StabilizingTime = stabtime; }
	int GetVariableType(map< string, vector<pair<string, int> > > poa_variables) const;

protected:
	string		Name;
	string		Source;
	double		TimeStamp;
	string		Value;	

	int			Destination;
	int			EventID;
	int			EventType;
	int			VariableType;

	int			ValueType;
	int			SourceType;
	unsigned int	Lifetime;		//s
	unsigned int	StabilizingTime; //s
};

