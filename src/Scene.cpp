//
//  Scene.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "Scene.hpp"
#include <sstream>
#include <iostream>
#include "Helper.hpp"

Scene::Scene(const std::string &row)
{
    std::stringstream stream(row);
    std::string term;
    
    // 开始时间
    getline(stream, term, ',');
    mtStartDateTime = Helper::StrToDateTime(term);
    
    // 结束时间
    getline(stream, term, ',');
    mtEndDateTime = Helper::StrToDateTime(term);
    
    // 影响类型
    getline(stream, term, ',');
    if (term == "降落") {
        mnType = 0;
    }
    else if (term == "起飞") {
        mnType = 1;
    }
    else if (term == "停机") {
        mnType = 2;
    } else {
        std::cerr << "影响类型不属于‘降落/起飞/停机’中的任一种哟!!!" << std::endl;
        abort();
    }
    
    // 机场ID
    getline(stream, term, ',');
    mnAirportId = std::stoi(term);
    
    // 航班ID
    getline(stream, term, ',');
    
    // 飞机
    getline(stream, term, ',');
    
    // 停机数
    getline(stream, term, ',');
    if (term.empty() || term == "\r") {
        mnMaxStopPlanes = 0;
    } else {
        mnMaxStopPlanes = std::stoi(term);
    }
    
}


bool Scene::IsInScene(const Flight &flight)
{
    if (mnType == 0) {      // 降落判断
        if (flight.mnEndAirport==mnAirportId && flight.mtEndDateTime>mtStartDateTime && flight.mtEndDateTime<mtEndDateTime) {
            return true;
        }
    }
    else if (mnType == 1) { // 起飞判断
        if (flight.mnStartAirport==mnAirportId && flight.mtStartDateTime>mtStartDateTime && flight.mtStartDateTime<mtEndDateTime) {
            return true;
        }
    }
    else if (mnType == 2) { // 停机判断
        if (flight.mnEndAirport == mnAirportId && flight.mtEndDateTime>mtStartDateTime && flight.mtEndDateTime<mtEndDateTime) {
            return true;
        }
    }
    
    return false;
}



bool Scene::IsStartInScene(const Flight &flight)
{
    if (mnType == 1) {
        if (flight.mnStartAirport==mnAirportId && flight.mtStartDateTime>mtStartDateTime && flight.mtStartDateTime<mtEndDateTime) {
            return true;
        }
    }
    
    return false;
}



bool Scene::IsEndInScene(const Flight &flight)
{
    if (mnType == 0) {
        if (flight.mnEndAirport==mnAirportId && flight.mtEndDateTime>mtStartDateTime && flight.mtEndDateTime<mtEndDateTime) {
            return true;
        }
    }
    
    return false;
}


bool Scene::IsStopInScene(const int airport, const time_t arriveTime, const time_t leaveTime)
{
    if (mnType == 2) {
        if (airport==mnAirportId && leaveTime>mtStartDateTime && arriveTime<mtEndDateTime) {
            return true;
        }
    }
    
    return false;
}

