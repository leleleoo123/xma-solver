//
//  Typhoon.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "Typhoon.hpp"
#include <iostream>

Typhoon::Typhoon() {}

Typhoon::Typhoon(const int airportId) : mnAirportId(airportId), mbStopLimitedOnly(true)
{
    mStopAirplaneSet.clear();
}

void Typhoon::ExtractInfo(const Scene &scene)
{
    if (mnAirportId != scene.mnAirportId) {
        return;
    }
    
    if (scene.mnType == 0) {
        mtNoLand = scene.mtStartDateTime;
        mtEnd = scene.mtEndDateTime;
        mbStopLimitedOnly = false;
    }
    else if (scene.mnType == 1)
    {
        mtNoTakeOff = scene.mtStartDateTime;
        mtEnd = scene.mtEndDateTime;
        mbStopLimitedOnly = false;
    }
    else if (scene.mnType == 2)
    {
        mtNoStop = scene.mtStartDateTime;
        mtEnd = scene.mtEndDateTime;
        mnMaxStopPlanes = scene.mnMaxStopPlanes;
    }
}


int Typhoon::NumStopAllowed()
{
    return (mnMaxStopPlanes - (int)mStopAirplaneSet.size());
}



bool Typhoon::IsTakeoffInTyphoon(const time_t tTakeoff)
{
    return ((tTakeoff>mtNoTakeOff) && (tTakeoff<mtEnd) && (!mbStopLimitedOnly));
}


bool Typhoon::IsLandInTyphoon(const time_t tLand)
{
    return ((tLand>mtNoLand) && (tLand<mtEnd) && (!mbStopLimitedOnly));
}


bool Typhoon::IsStopInTyphoon(const time_t tStopBegin, const time_t tStopEnd)
{
    return ((tStopEnd>mtNoTakeOff) && (tStopBegin<mtEnd));
}


bool Typhoon::IsInTyphoon(const Flight& flight)
{
    if (flight.mnStartAirport == mnAirportId)
    {
        return IsTakeoffInTyphoon(flight.mtStartDateTime);
    }
    else if (flight.mnEndAirport == mnAirportId)
    {
        return IsLandInTyphoon(flight.mtEndDateTime);
    }
    
    return false;
}


void Typhoon::CreateSliceMap()
{
    mtOneHourBeforeNoTakeoff = mtNoTakeOff - 3600;
    mtTwoHourAfterEnd = mtEnd + 3600 * 2;
    mSliceMap.clear();
    int loc = 0;
    while (mtOneHourBeforeNoTakeoff + loc*300 <= mtNoTakeOff) {
        mSliceMap[loc] = 0;
        loc++;
    }
    
    loc = (int)((mtEnd - mtOneHourBeforeNoTakeoff) / 300);
    while (mtOneHourBeforeNoTakeoff + loc*300 <= mtTwoHourAfterEnd) {
        mSliceMap[loc] = 0;
        loc++;
    }
}


void Typhoon::ResetSliceMap()
{
    for (auto& mit : mSliceMap) {
        mit.second = 0;
    }
}


void Typhoon::AddSliceCount(const time_t &t)
{
    if (mbStopLimitedOnly)
        return;
    
    if ((t>=mtOneHourBeforeNoTakeoff && t<=mtNoTakeOff) || (t>=mtEnd && t<=mtTwoHourAfterEnd)) {
        int loc = (int)((t-mtOneHourBeforeNoTakeoff) / 300);
        mSliceMap[loc]++;
    }
    
}


void Typhoon::ReduceSliceCount(const time_t &t)
{
    if (mbStopLimitedOnly)
        return;
    
    if ((t>=mtOneHourBeforeNoTakeoff && t<=mtNoTakeOff) || (t>=mtEnd && t<=mtTwoHourAfterEnd)) {
        int loc = (int)((t-mtOneHourBeforeNoTakeoff) / 300);
        if (mSliceMap[loc] < 1) {
            std::cout << "Error when reducing slice count" << std::endl;
            abort();
        }
        mSliceMap[loc]--;
    }
    
}





