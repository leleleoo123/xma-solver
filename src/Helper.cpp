//
//  Helper.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include "Helper.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>

const time_t T2000_01_01 = 946656000;

time_t Helper::StrToDate(std::string &str)
{
    // 把简写的年份补全 (5/7/17 ~ 5/7/2017)
    size_t idx = str.find("/", 3);
    str.insert(idx+1, "20");
    
    // 记得要初始化tm结构, 否则会出现bug
    struct tm date = {0};
    strptime(str.c_str(), "%m/%d/%Y", &date);
    
    return mktime(&date);
}


time_t Helper::StrToDateTime(std::string &str)
{
    // 把简写的年份补全 (5/20/17 ~ 5/20/2017)
    size_t idx = str.find("/", 3);
    str.insert(idx+1, "20");
    
    // 记得要初始化tm结构, 否则会出现bug
    struct tm dateTime = {0};
    strptime(str.c_str(), "%m/%d/%Y %H:%M", &dateTime);
    
    return mktime(&dateTime);
}


time_t Helper::StrToTime(std::string &str)
{
    size_t idx = str.find(":");
    int hour = std::stoi(str.substr(0, idx));
    int minute = std::stoi(str.substr(idx+1, 2));
    
    return (hour*60 + minute) * 60;
}


std::string Helper::DateTimeToString(const time_t dateTime)
{
    std::tm *ltm = localtime(&dateTime);
    std::stringstream stream;
    stream << std::put_time(ltm, "%m/%d_%H:%M");
    return stream.str();
}


std::string Helper::DateTimeToFullString(const time_t dateTime)
{
    std::tm *ltm = localtime(&dateTime);
    std::stringstream stream;
    stream << std::put_time(ltm, "%Y/%m/%d %H:%M");
    return stream.str();
}



time_t Helper::KeepDateOnly(const time_t dateTime)
{
    time_t temp = dateTime - T2000_01_01;
    
    return (T2000_01_01 +  (temp / 86400)*86400);
}


void Helper::RowToIntVector(const std::string &str, std::vector<int> &vec, const int N)
{
    std::stringstream stream(str);
    std::string term;
    
    for (int i=0; i<N; i++) {
        getline(stream, term, ',');
        vec[i] = std::stoi(term);
    }
}


void Helper::SubtractInterval(std::vector<std::pair<time_t, time_t> > &timeWindows,
                              const std::pair<time_t, time_t> badTimeInterval)
{
    if (timeWindows.empty() || badTimeInterval.first >= badTimeInterval.second)
        return;
    
    std::vector<std::pair<time_t, time_t> > newTimeWindows;
    
    time_t tBadLow = badTimeInterval.first;
    time_t tBadUp = badTimeInterval.second;
    
    for (size_t i=0; i<timeWindows.size(); ++i)
    {
        std::pair<time_t, time_t> twindow = timeWindows[i];
        
        if (tBadLow>=twindow.second || tBadUp<=twindow.first)
            newTimeWindows.push_back(twindow);
        else
        {
            if (tBadLow >= twindow.first)
                newTimeWindows.push_back(std::make_pair(twindow.first, tBadLow));
            
            if (tBadUp <= twindow.second)
                newTimeWindows.push_back(std::make_pair(tBadUp, twindow.second));
        }
        
    }
    
    timeWindows = newTimeWindows;
}
