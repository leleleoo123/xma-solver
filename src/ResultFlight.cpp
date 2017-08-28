//
//  ResultFlight.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/25.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "ResultFlight.hpp"

ResultFlight::ResultFlight() : mbIsCancel(false), mbIsStraighten(false), mbIsSignChange(false) {}


ResultFlight::ResultFlight(const Flight& flight)
: mbIsCancel(false), mbIsStraighten(false), mbIsSignChange(false)
{
    mnFlightId = flight.mnFlightId;
    mnStartAirport = flight.mnStartAirport;
    mnEndAirport = flight.mnEndAirport;
    mtStartDateTime = flight.mtStartDateTime;
    mtEndDateTime = flight.mtEndDateTime;
    mnAirplaneId = flight.mnAirplaneId;
}



