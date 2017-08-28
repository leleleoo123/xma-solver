//
//  AirportClose.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef AirportClose_hpp
#define AirportClose_hpp

#include <string>
#include <vector>

class AirportClose
{
public:
    
    AirportClose(const std::string &row);
    
    // 判断时间戳是否落在机场关闭的时间窗内
    bool IsClosed(const time_t t);
    
    // 对时间窗口做调整, 使其不要违反机场关闭限制
    void AdjustTimeWindows(std::vector<std::pair<time_t, time_t> > &timeWindows, const time_t qqTime);
    
public:
    
    // 机场
    int mnAirportId;
    
    // 关闭时间
    time_t mtBeginCloseTime;
    
    // 开放时间
    time_t mtEndCloseTime;
    
    // 生效日期
    time_t mtBeginDate;
    
    // 失效日期
    time_t mtEndDate;
    
};

#endif /* AirportClose_hpp */
