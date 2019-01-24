//=============================================================================
// Name        : event.h
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2013 VTT. All Rights Reserved.
// Description : General functions for creating formatted strings etc.
//=============================================================================
#pragma once


void SplitString(std::vector<std::string>& result, const std::string &source, char delim);
std::string	ExtractFileExtension(const std::string& _sfilename);
std::string	ExtractFileNameWithoutExtension(const std::string& _sfilename);

// Rounds the given T val to the closest integer value.
template <class T>
int Round(const T&_val)
{
    double dval = (_val < 0.0) ? -0.5 : 0.5;
    return (int)(_val + dval);
}


