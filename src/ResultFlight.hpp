//
//  ResultFlight.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/25.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef ResultFlight_hpp
#define ResultFlight_hpp

#include <iostream>
#include <map>
#include "Flight.hpp"

class ResultFlight
{
public:
    
    ResultFlight();
    
    ResultFlight(const std::string &row);
    
    ResultFlight(const Flight& flight);
    
    ResultFlight(const int flightId,
                 const int startAirport,
                 const int endAirport,
                 const int airplaneId,
                 const time_t startDateTime,
                 const time_t endDateTime);
    
    bool operator< (const ResultFlight& rf) const
    {
        return mtStartDateTime < rf.mtStartDateTime;
    }
    
    
public:
    
    // 航班ID
    int mnFlightId;
    
    // 起飞机场/降落机场
    int mnStartAirport;
    int mnEndAirport;
    
    // 起飞时间/降落时间
    time_t mtStartDateTime;
    time_t mtEndDateTime;
    
    // 飞机ID
    int mnAirplaneId;
    
    // 是否取消
    bool mbIsCancel;
    
    // 是否拉直
    bool mbIsStraighten;
    
    // 是否签转
    bool mbIsSignChange;
    
    // 旅客签转情况
    std::map<int, int> mSignChangePassInfo;
    
    
    
    
};

#endif /* ResultFlight_hpp */
