//=============================================================================
// Name        : event.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2009 VTT. All Rights Reserved.
// Description : Event class implementation.
//=============================================================================

#include "stdafx.h"
//#include "c_time.h"
#include "event.h"

Event::Event(void):
Name(""),
Source(""),
TimeStamp(0),
Value(""),
Destination(NO_DESTINATION), 
EventID(EVENT_UNDEFINED),
EventType(EVENT_TYPE_UNDEFINED),
VariableType(VARIABLE_TYPE_UNKNOWN),
ValueType(VALUE_TYPE_UNKNOWN),
SourceType(SOURCE_TYPE_UNKNOWN),
Lifetime(DEFAULT_LIFETIME),
StabilizingTime(DEFAULT_STABILIZING_TIME)
{
}

Event::Event(string name, string src, double timestamp, int event_type, string value):
Name(name),
Source(src),
TimeStamp(timestamp),
Value(value),
Destination(NO_DESTINATION), 
EventID(EVENT_UNDEFINED),
EventType(event_type),
VariableType(VARIABLE_TYPE_UNKNOWN),
ValueType(VALUE_TYPE_UNKNOWN),
SourceType(SOURCE_TYPE_UNKNOWN),
Lifetime(DEFAULT_LIFETIME),
StabilizingTime(DEFAULT_STABILIZING_TIME)
{
}

Event::Event(const Event& _src):
Name(_src.Name),
Source(_src.Source),
TimeStamp(_src.TimeStamp),
Value(_src.Value),
Destination(_src.Destination), 
EventID(_src.EventID),
EventType(_src.EventType),
VariableType(_src.VariableType),
ValueType(_src.ValueType),
SourceType(_src.SourceType),
Lifetime(_src.Lifetime),
StabilizingTime(_src.StabilizingTime)
{
}

Event& Event::operator=(const Event& _src)
{
    if (&_src == this) return *this;

	Name = _src.Name;
	Source = _src.Source;
	TimeStamp = _src.TimeStamp;
	Value = _src.Value;
	Destination = _src.Destination;
	EventID = _src.EventID;
	EventType = _src.EventType;
	VariableType = _src.VariableType;
	ValueType = _src.ValueType;
	SourceType = _src.SourceType;
	Lifetime = _src.Lifetime;
	StabilizingTime = _src.StabilizingTime;

    return *this;
}
	
Event::~Event(void)
{
}

//bool Event::IsLifetimeExceeded() const
//{
//	double time_now = GetMilliTime();
//	bool bret = (time_now <= TimeStamp + Lifetime) ? false : true;
//	return bret;
//}
//
//bool Event::IsTriggerLifetimeExceeded() const
//{
//	double time_now = GetMilliTime();
//	bool bret = (time_now <= TimeStamp + DEFAULT_TRIGGER_LIFETIME) ? false : true;
//	return bret;
//}