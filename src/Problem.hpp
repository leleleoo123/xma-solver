//
//  Problem.hpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#ifndef Problem_hpp
#define Problem_hpp

#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <set>
#include <opencv2/opencv.hpp>
#include "Flight.hpp"
#include "AirportClose.hpp"
#include "Scene.hpp"
#include "Typhoon.hpp"
#include "ResultFlight.hpp"

class Problem
{
public:
    
    Problem(const std::string &dir);
    
    void Solve();
    
    double AdjustSingleAirline(int airplaneId);
    
    
    double AdjustPairedAirlines(int airplaneIdA, int airplaneIdB, bool saveResult);
    
    
    void SaveResults(const std::string &filename);
    
private:
    
    // 把起飞时间在调整窗口之前的航班分到一边去
    void SeparatePreAdjFlights();
    
    // 找到每个航班允许起飞的时间窗口
    void FindTimeWindowsOfFlights();
    
    // 找到所有可拉直的联程航班
    void FindPotentialStraightenFligths();
    
    // 试试看能不能连接两个航班
    bool TryConnectFlights(const Flight& flightA, const Flight& flightB);
    
    // 统计在单位时间容量限制时间段内起降的航班
    void UpdateSliceMaps(const ResultFlight& rf);
    
    //
    void CutOccupiedSlices(std::vector<Flight>& vFlights);
    
private:
    
    // ------------- 原始数据 -----------------------------
    // 所有航班
    std::map<int, Flight> mFlightMap;
    
    // 对应每一架飞机的全部航班
    std::map<int, std::vector<Flight> > mAllAirlineMap; // 所有的航班
    std::map<int, std::vector<Flight> > mAdjAirlineMap; // 代调整的航班
    std::map<int, std::vector<Flight> > mPreAirlineMap; // 在调整窗口之前的航班
    
    // 国内机场集合, 用于控制调机
    std::set<int> msDomesticAirports;
    
    // 航线-飞机限制
    std::map<std::pair<int, int>, std::set<int> > mAirlineLimitMap;
    
    // 机场关闭限制
    std::map<int, std::vector<AirportClose> > mAirportCloseMap;
    
    // 台风场景
    std::vector<Scene> mvScenes;
    std::map<int, Typhoon> mTyphoonMap;
    
    // 飞行时间 (机型, 起降机场, 飞行时间)
    std::map<int, std::map<std::pair<int, int>, time_t>  >  mTravelTimeMap;
    
    // 飞机过站时间
    std::map<std::pair<int, int>, time_t> mFlightIntervalTimeMap;
    
    // 结束时每个机场的飞机机型统计
    std::map<std::pair<int, int>, int> mEndAirportMap;
    
    // 所有可拉直的航班对
    std::set<std::pair<int, int> > msPSFpairs;
    
    
    // --- 恢复窗口 ---
    time_t mtRecoveryStart;
    time_t mtRecoveryEnd;
    
    
    
    // ------------- 调整后的数据 ------------------------
    std::map<int, ResultFlight> mResultFlightMap;
    
    
};

#endif /* Problem_hpp */
