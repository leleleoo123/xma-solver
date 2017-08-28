//
//  ConfigParams.h
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef ConfigParams_h
#define ConfigParams_h

#include "Helper.hpp"

// Cost parameters
const int AdjustFlightParam = 5000;
const int CancelFlightParam = 1000;
const int FlightTypeChangeParam = 1000;
const int ConnectFlightStraightenParam = 750;
const double DelayFlightParam = 100.0/3600.0;
const double AheadFlightParam = 150.0/3600.0;


// Penalty parameter for an infeasible solution
const double InfeasibilityPenaltyParam = 5 * 10e6;


// Penalty paramter for every single constraint violation
const double ConstraintViolationPenaltyParam = 10;


// Time constraints (in seconds)
const long MaxAheadTime = 6 * 60 * 60;
const long MaxIntervalTime = 50 * 60;
const long MaxDomesticDelayTime = 24 * 60 * 60;
const long MaxAbroadDelayTime = 36 * 60 * 60;


#endif /* ConfigParams_h */
