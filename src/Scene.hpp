//
//  Scene.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef Scene_hpp
#define Scene_hpp

#include <string>
#include "Flight.hpp"

class Scene
{
public:
    
    Scene(const std::string &row);
    
    // 判断是否受影响
    bool IsInScene(const Flight& flight);
    
    // 判断起飞是否受此场景影响
    bool IsStartInScene(const Flight& flight);
    
    // 判断降落是否是否受此场景影响
    bool IsEndInScene(const Flight& flight);
    
    // 判断停机时间段是否落在受影响的场景内
    bool IsStopInScene(const int airport, const time_t arriveTime, const time_t leaveTime);
    
public:
    
    // 机场ID
    int mnAirportId;
    
    // 开始时间
    time_t mtStartDateTime;
    
    // 结束时间
    time_t mtEndDateTime;
    
    // 影响类型
    int mnType;
    
    // 最大停机数
    int mnMaxStopPlanes;
    
};

#endif /* Scene_hpp */
