//=============================================================================
// Name        : cmn_utils.cpp
// Author      : VTT/Heli Kokkoniemi-Tarkkanen
// Version     :
// Copyright   : Copyright (C) 2009 VTT. All Rights Reserved.
// Description : General functions for creating formatted strings etc.
//=============================================================================
#include "stdafx.h"
#include "cmn_utils.h"

using namespace std;

void SplitString(std::vector<std::string>& result, const std::string &source, char delim)
{
    std::stringstream ss(source);
    std::string item;
    while(std::getline(ss, item, delim))
	{
        result.push_back(item);
    }
}

std::string	ExtractFileExtension(const std::string& _sfilename)
{
	std::string tmp(_sfilename);
	int ipos = (int)tmp.find_last_of(".",tmp.size());
	if (ipos == -1) return string("");
	tmp = tmp.substr(ipos);
	return tmp;
}

std::string	ExtractFileNameWithoutExtension(const std::string& _sfilename)
{
	std::string tmp(_sfilename);
	int ipos = (int)tmp.find_last_of(".",tmp.size());
	if (ipos == -1) return _sfilename;
	tmp = tmp.substr(0,ipos);
	return tmp;
}

