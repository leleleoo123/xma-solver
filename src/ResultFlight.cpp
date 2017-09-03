//
//  ResultFlight.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/25.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "ResultFlight.hpp"
#include "Helper.hpp"
#include <sstream>
#include <iostream>

ResultFlight::ResultFlight() : mbIsCancel(false), mbIsStraighten(false), mbIsSignChange(false) {}


ResultFlight::ResultFlight(const std::string& row)
{
    std::stringstream stream(row);
    std::string term;
    
    // 航班ID
    getline(stream, term, ',');
    mnFlightId = std::stoi(term);
    
    // 起飞/降落机场
    getline(stream, term, ',');
    mnStartAirport = std::stoi(term);
    getline(stream, term, ',');
    mnEndAirport = std::stoi(term);
    
    // 起飞/降落时间
    getline(stream, term, ',');
    mtStartDateTime = Helper::ResStrToDateTime(term);
    getline(stream, term, ',');
    mtEndDateTime = Helper::ResStrToDateTime(term);
    
    // 飞机ID
    getline(stream, term, ',');
    mnAirplaneId = std::stoi(term);
    
    // 是否取消
    getline(stream, term, ',');
    mbIsCancel = std::stoi(term);
    
    //
    mbIsStraighten = false;
    mbIsSignChange = false;
    
    // --- DEBUG PRINT --------
//    std::cout << "ResFlight: " << mnFlightId << ", " << mnStartAirport << "->" << mnEndAirport << ", " <<
//    Helper::DateTimeToFullString(mtStartDateTime) << ", " << Helper::DateTimeToFullString(mtEndDateTime)
//    << ", " << mnAirplaneId << ", " << mbIsCancel << std::endl;
    
}



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



