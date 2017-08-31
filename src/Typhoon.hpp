//
//  Typhoon.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef Typhoon_hpp
#define Typhoon_hpp

#include "Scene.hpp"
#include "Flight.hpp"
#include "ResultFlight.hpp"
#include <iostream>
#include <map>

class Typhoon
{
public:
    
    Typhoon();
    
    Typhoon(const int airportId);
    
    void ExtractInfo(const Scene &scene);
    
    bool IsTakeoffInTyphoon(const time_t tTakeoff);
    
    bool IsLandInTyphoon(const time_t tLand);
    
    bool IsStopInTyphoon(const time_t tStopBegin, const time_t tStopEnd);
    
    bool IsInTyphoon(const Flight& flight);
    
    void CreateSliceMap();
    void ResetSliceMap();
    void AddSliceCount(const time_t& t);
    void ReduceSliceCount(const time_t& t);
    
    
    
public:
    
    // 机场ID
    int mnAirportId;
    
    // 限制降落开始的时间
    time_t mtNoLand;
    
    // 限制起飞开始的时间
    time_t mtNoTakeOff;
    
    // 限制停机开始的时间
    time_t mtNoStop;

    // 台风影响结束的时间
    time_t mtEnd;
    
    // 最大停机数
    int mnMaxStopPlanes;
    
    // 只是单纯限制停机吗
    bool mbStopLimitedOnly;
    
    // 单位时间容量限制
    time_t mtOneHourBeforeNoTakeoff;
    time_t mtTwoHourAfterEnd;
    
    
    // -------- for solver -------
    // 台风期间在该机场停机的飞机
    std::vector<int> mvStopAirplanes;
    
    // 台风前一小时/台风后两小时, 每5分钟内起飞或者降落的航班数
    std::map<int, int> mSliceMap;
    
    
};

#endif /* Typhoon_hpp */
