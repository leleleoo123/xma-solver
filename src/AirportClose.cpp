//
//  AirportClose.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "AirportClose.hpp"
#include <sstream>
#include <iostream>
#include "Helper.hpp"

AirportClose::AirportClose(const std::string &row)
{
    std::stringstream stream(row);
    std::string term;
    
    // 机场ID
    getline(stream, term, ',');
    mnAirportId = std::stoi(term);
    
    // 关闭时间
    getline(stream, term, ',');
    mtBeginCloseTime = Helper::StrToTime(term);
    
    // 开放时间
    getline(stream, term, ',');
    mtEndCloseTime = Helper::StrToTime(term);
    
    // 生效日期
    getline(stream, term, ',');
    mtBeginDate = Helper::StrToDate(term);
    if (mtBeginDate < 0) {
        mtBeginDate = 0;
    }
    
    // 失效日期
    getline(stream, term, ',');
    mtEndDate = Helper::StrToDate(term);
    
}


bool AirportClose::IsClosed(const time_t t)
{
    // 只保留 Date 的时间戳
    time_t date = Helper::KeepDateOnly(t);
    
    if (date<mtBeginDate || date>=mtEndDate) {
        return false;
    }
    
    // 当天的时刻
    time_t hm = t - date;
    
    if (hm>mtBeginCloseTime && hm<mtEndCloseTime) {
        return true;
    }
    
    return false;
}


void AirportClose::AdjustTimeWindows(std::vector<std::pair<time_t, time_t> > &timeWindows,
                                     const time_t qqTime)
{
    
    if (timeWindows.empty()) {
        return;
    }
    
    // 找到最小的时刻和最大的时刻
    time_t tmin = timeWindows[0].first;
    time_t tmax = timeWindows[0].second;
    
    for (size_t i=1; i<timeWindows.size(); i++) {
        if (timeWindows[i].first < tmin) {
            tmin = timeWindows[i].first;
        }
        
        if (timeWindows[i].second > tmax) {
            tmax = timeWindows[i].second;
        }
    }
    
    // 最早和最晚的日期
    time_t date_min = Helper::KeepDateOnly(tmin);
    time_t date_max = Helper::KeepDateOnly(tmax);
    
    // 不要超过机场关闭限制的时间
    date_min = std::max(date_min, mtBeginDate);
    date_max = std::min(date_max, mtEndDate);
    int days = (int)((date_max-date_min) / 86400);
    
    for (int i=0; i<=days; i++) {
        time_t date = date_min + 86400 * i;
        Helper::SubtractInterval(timeWindows, std::make_pair(date+mtBeginCloseTime-qqTime, date+mtEndCloseTime-qqTime));
    }
    
}
