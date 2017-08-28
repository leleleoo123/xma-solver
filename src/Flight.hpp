//
//  Flight.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef Flight_hpp
#define Flight_hpp

#include <string>
#include <time.h>
#include <vector>

class Flight
{
public:
    
    Flight();
    
    Flight(const std::string &row);
    
    void SetConnected(int connected_flight_id, bool isPrePart);
    
    bool operator< (const Flight& fl) const
    {
        return mtStartDateTime < fl.mtStartDateTime;
    }
    
    
public:
    
    // 航班ID
    int mnFlightId;
    
    // 日期
    time_t mtDate;
    
    // 国际/国内
    bool mbIsDomestic;
    
    // 航班号
    int mnFlightNo;
    
    // 起飞/降落机场
    int mnStartAirport;
    int mnEndAirport;
    
    // 起飞/降落/飞行时间
    time_t mtStartDateTime;
    time_t mtEndDateTime;
    time_t mtFlyingTime;
    
    // 飞机ID
    int mnAirplaneId;
    
    // 机型
    int mnAirplaneType;
    
    // 旅客数/联程旅客数/普通旅客数
    int mnPassengers;
    int mnConnectPassengers;
    int mnNormalPassengers;
    
    // 座位数
    int mnSeats;
    
    // 重要系数
    double mdImportanceRatio;
    
    // 是否联程航班
    bool mbIsConnected;
    int mnConnectedFlightId;
    
    // 是联程航班前半段吗
    bool mbIsConnedtedPrePart;
    
    
    // ------------For Solver --------------------- //
    std::vector<std::pair<time_t, time_t> > mvTimeWindows;
    time_t mtTakeoffErliest;
    time_t mtTakeoffLatest;
    
    
    
};

#endif /* Flight_hpp */
