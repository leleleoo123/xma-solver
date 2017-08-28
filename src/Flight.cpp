//
//  Flight.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "Flight.hpp"
#include <sstream>
#include "Helper.hpp"


Flight::Flight() {}


Flight::Flight(const std::string &row)
: mbIsConnected(false), mnConnectedFlightId(-100), mbIsConnedtedPrePart(false)
{
    std::stringstream stream(row);
    std::string term;
    
    // 航班ID
    getline(stream, term, ',');
    mnFlightId = std::stoi(term);
    
    // 日期
    getline(stream, term, ',');
    mtDate = Helper::StrToDate(term);
    
    // 国际/国内
    getline(stream, term, ',');
    mbIsDomestic = (term == "国内");
    
    // 航班号
    getline(stream, term, ',');
    mnFlightNo = std::stoi(term);
    
    // 起飞/降落机场
    getline(stream, term, ',');
    mnStartAirport = std::stoi(term);
    getline(stream, term, ',');
    mnEndAirport = std::stoi(term);
    
    // 起飞/降落时间
    getline(stream, term, ',');
    mtStartDateTime = Helper::StrToDateTime(term);
    getline(stream, term, ',');
    mtEndDateTime = Helper::StrToDateTime(term);
    
    // 飞行时间
    mtFlyingTime = mtEndDateTime - mtStartDateTime;
    
    // 飞机ID
    getline(stream, term, ',');
    mnAirplaneId = std::stoi(term);
    
    // 机型
    getline(stream, term, ',');
    mnAirplaneType = std::stoi(term);
    
    // 旅客数
    getline(stream, term, ',');
    mnPassengers = std::stoi(term);
    
    // 联程旅客数
    getline(stream, term, ',');
    mnConnectPassengers = std::stoi(term);
    
    // 座位数
    getline(stream, term, ',');
    mnSeats = std::stoi(term);
    
    // 重要系数
    getline(stream, term, ',');
    mdImportanceRatio = std::stod(term);
    
}



void Flight::SetConnected(int connected_flight_id, bool isPrePart)
{
    mbIsConnected = true;
    mnConnectedFlightId = connected_flight_id;
    mbIsConnedtedPrePart = isPrePart;
}
