//
//  Helper.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.

/******* WARNING ***** 温馨提示 *********************
*
*Note that this helper was designed for 21 century.
*
**************************************************/

#ifndef Helper_hpp
#define Helper_hpp

#include <string>
#include <vector>
#include <time.h>
#include <ctime>


template <typename T>
std::vector<size_t> sort_indices(const std::vector<T> &v)
{
    // Initialize original index locations
    std::vector<size_t> idx(v.size());
    for (size_t i=0; i<idx.size(); i++) {
        idx[i] = i;
    }
    
    // Sort indices based on comparing values in v
    std::sort(idx.begin(), idx.end(), [&v](size_t i1, size_t i2) {return v[i1] > v[i2];});
    
    return idx;
}



class Helper
{
public:
    
    // 把格式如 "5/7/17"(月/日/年) 的字符串转成 time_t
    static time_t StrToDate(std::string& str);
    
    // 把格式如 "10/20/17 18:30" 的字符串转成 time_t
    static time_t StrToDateTime(std::string& str);
    
    // 把格式如 "0:15" 或 "23:00" 的字符串转成 time_t
    static time_t StrToTime(std::string& str);
    
    // 把 time_t 转成 字符串
    static std::string DateTimeToString(const time_t dateTime);
    
    // 把 time_t 转成 字符串
    static std::string DateTimeToFullString(const time_t dateTime);
    
    // 只保留日期的time_t
    static time_t KeepDateOnly(const time_t dateTime);
    
    // 把csv表格的一行转成多个整数
    static void RowToIntVector(const std::string& str, std::vector<int>& vec, const int N);
    
    // 多个时间窗口减去某一时间区间
    static void SubtractInterval(std::vector<std::pair<time_t, time_t> > &timeWindows,
                                 const std::pair<time_t, time_t> badTimeInterval);
};

#endif /* Helper_hpp */
