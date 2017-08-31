//
//  Problem.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//


#include "Problem.hpp"
#include "Helper.hpp"
#include "ConfigParams.h"
#include <gurobi_c++.h>

#define AIRCRAFT_TYPES 4
#define TIME_UNIT 300
#define BIGNUM 1e8
#define STR_START_ADJUST_TIME "5/6/17 06:00"
#define STR_END_ADUST_TIME "5/9/17 00:00"


using namespace std;
using namespace cv;

Problem::Problem(const string& dir)
{
    // ----- 恢复窗口 -----
    string str_recovery_start_time = "5/6/17 06:00";
    mtRecoveryStart = Helper::StrToDateTime(str_recovery_start_time);
    string str_recovery_end_time = "5/9/17 00:00";
    mtRecoveryEnd = Helper::StrToDateTime(str_recovery_end_time);
    cout << "恢复开始/结束时间: " << mtRecoveryStart << ", " << mtRecoveryEnd << endl;
    cout << Helper::DateTimeToString(mtRecoveryStart) << ", " << Helper::DateTimeToString(mtRecoveryEnd) << endl;
    
    
    // ----- 航班 ---------------------------------------------------
    ifstream ifsFlights(dir + "/航班.csv");
    if (!ifsFlights.is_open()) {
        cerr << "Could not open file : " << dir << "/航班.csv" << endl;
        abort();
    }
    
    // 表头
    string oneLine;
    getline(ifsFlights, oneLine);
    
    // 数据
    mFlightMap.clear();
    mAllAirlineMap.clear();
    
    while (getline(ifsFlights, oneLine)) {
        Flight flight(oneLine);
        mFlightMap[flight.mnFlightId] = flight;
        
        if (mAllAirlineMap.count(flight.mnAirplaneId)) {
            mAllAirlineMap[flight.mnAirplaneId].push_back(flight);
        }
        else {
            vector<Flight> flightVector;
            flightVector.push_back(flight);
            mAllAirlineMap[flight.mnAirplaneId] = flightVector;
        }
    }
    
    // 关闭表格
    ifsFlights.close();
    
    
    // ----- 读取航线-飞机限制信息 ------------------------------------
    ifstream ifsAirplaneLimits(dir + "/航线-飞机限制.csv");
    if (!ifsAirplaneLimits.is_open()) {
        cerr << "Could not open file : " << dir << "/航线-飞机限制.csv" << endl;
        abort();
    }
    
    // 表头
    getline(ifsAirplaneLimits, oneLine);
    
    // 数据
    mAirlineLimitMap.clear();
    
    vector<int> intVector(10, 0);
    while (getline(ifsAirplaneLimits, oneLine)) {
        Helper::RowToIntVector(oneLine, intVector, 3);
        pair<int, int> airline(intVector[0], intVector[1]);
        if (mAirlineLimitMap.count(airline)) {
            mAirlineLimitMap[airline].insert(intVector[2]);
        }
        else {
            set<int> limitedAirplaneSet;
            limitedAirplaneSet.insert(intVector[2]);
            mAirlineLimitMap[airline] = limitedAirplaneSet;
        }
    }
    
    // 关闭表格
    ifsAirplaneLimits.close();
    
    
    
    // ----- 机场关闭限制信息 ---------------------------------------------
    ifstream ifsAirportClose(dir + "/机场关闭限制.csv");
    if (!ifsAirportClose.is_open()) {
        cerr << "Could not open file : " << dir << "/机场关闭限制.csv" << endl;
        abort();
    }
    
    // 表头
    getline(ifsAirportClose, oneLine);
    
    // 数据
    mAirportCloseMap.clear();
    
    while (getline(ifsAirportClose, oneLine)) {
        AirportClose airportClose(oneLine);
        int airportId = airportClose.mnAirportId;
        if (mAirportCloseMap.count(airportId)) {
            mAirportCloseMap[airportId].push_back(airportClose);
        }
        else {
            vector<AirportClose> airportCloseVector;
            airportCloseVector.push_back(airportClose);
            mAirportCloseMap[airportId] = airportCloseVector;
        }
    }
    
    // 关闭表格
    ifsAirportClose.close();
    
    
    // ----- 台风场景 ---------------------------------------
    ifstream ifsTyphoonScene(dir + "/台风场景.csv");
    if (!ifsTyphoonScene.is_open()) {
        cerr << "Could not open file : " << dir << "/台风场景.csv" << endl;
        abort();
    }
    
    // 表头
    getline(ifsTyphoonScene, oneLine);
    
    // 数据
    mvScenes.clear();
    
    while (getline(ifsTyphoonScene, oneLine)) {
        Scene scene(oneLine);
        mvScenes.push_back(scene);
    }
    
    // 关闭表格
    ifsTyphoonScene.close();
    
    // 数据整理:
    // 共有3个机场受台风影响, 先是限制着陆, 再是限制起飞(同时停机限制开始), 最后三个限制同时结束
    mTyphoonMap.clear();
    for (size_t i=0; i<mvScenes.size(); ++i) {
        const int airportId = mvScenes[i].mnAirportId;
        if (mTyphoonMap.count(airportId)) {
            mTyphoonMap[airportId].ExtractInfo(mvScenes[i]);
        }
        else {
            Typhoon typhoon(airportId);
            typhoon.ExtractInfo(mvScenes[i]);
            mTyphoonMap[airportId] = typhoon;
        }
    }
    
    for (auto& mit : mTyphoonMap) {
        if (!mit.second.mbStopLimitedOnly) {
            mit.second.CreateSliceMap();
        }
    }
    
    
    
    // ----- 飞行时间 --------------------------------------
    ifstream ifsTravelTime(dir + "/飞行时间.csv");
    if (!ifsTravelTime.is_open()) {
        cerr << "Could not open file : " << dir << "/飞行时间.csv" << endl;
        abort();
    }
    
    // 表头
    getline(ifsTravelTime, oneLine);
    
    // 数据
    mTravelTimeMap.clear();
    
    while (getline(ifsTravelTime, oneLine)) {
        Helper::RowToIntVector(oneLine, intVector, 4);
        int airplaneType = intVector[0];
        pair<int, int> airline(intVector[1], intVector[2]);
        time_t travelTime = intVector[3] * 60;
        mTravelTimeMap[airplaneType][airline] = travelTime;
    }
    
    // 关闭表格
    ifsTravelTime.close();
    
    
    
    // ----- 国内/国际机场 -----------------------
    ifstream ifsAirport(dir + "/机场.csv");
    if (!ifsAirport.is_open()) {
        cerr << "Could not open file : " << dir << "/机场.csv" << endl;
        abort();
    }
    
    // 表头
    getline(ifsAirport, oneLine);
    
    // 数据
    msDomesticAirports.clear();
    
    while (getline(ifsAirport, oneLine)) {
        Helper::RowToIntVector(oneLine, intVector, 2);
        if (intVector[1] > 0) {
            msDomesticAirports.insert(intVector[0]);
        }
    }
    
    // 关闭表格
    ifsAirport.close();
    
    
    // ----- 联程航班 ---------------------------
    mEndAirportMap.clear();
    mFlightIntervalTimeMap.clear();
    int connectedFlights = 0;
    
    for (auto& mit : mAllAirlineMap) {
        vector<Flight>& vFlights = mit.second;
        
        // 对每架飞机的航程从早到晚排序
        sort(vFlights.begin(), vFlights.end());
        
        // 判断是否联程航班
        for (size_t i=1; i<vFlights.size(); ++i) {
            Flight& preFlight = vFlights[i-1];
            Flight& flight = vFlights[i];
            
            if (preFlight.mnFlightNo == flight.mnFlightNo) {
                if (preFlight.mbIsConnected) {
                    cout << "Error when setting conneted flight!" << endl;
                    abort();
                }
                
                preFlight.SetConnected(flight.mnFlightId, true);
                flight.SetConnected(preFlight.mnFlightId, false);
                connectedFlights++;
            }
            
            pair<int, int> flightPair(preFlight.mnFlightId, flight.mnFlightId);
            mFlightIntervalTimeMap[flightPair] = flight.mtStartDateTime - preFlight.mtEndDateTime;
        }
        
        // 统计结束机场
        const int finalAirport = vFlights.back().mnEndAirport;
        pair<int, int> portAndPlane(finalAirport, vFlights[0].mnAirplaneType);
        if (mEndAirportMap.count(portAndPlane)) {
            mEndAirportMap[portAndPlane]++;
        } else {
            mEndAirportMap[portAndPlane] = 1;
        }
        
        if (vFlights[0].mtStartDateTime > mtRecoveryStart || vFlights.back().mtStartDateTime >= mtRecoveryEnd) {
            cout << "惨兮兮: 想假设(所有飞机最开始的航班都在恢复窗口开始之前, 而最后的航班在恢复窗口结束之前), 假设不成立呀!" << endl;
            cout << mit.first << ": " << Helper::DateTimeToString(vFlights[0].mtStartDateTime) << " -> " << Helper::DateTimeToString(vFlights.back().mtStartDateTime) << endl;
        }
        
        // ----- DEBUG -----
        if (finalAirport == 36) {
            cout << "Flight#" << vFlights.back().mnFlightId <<  " end in port36, plane#" << vFlights.back().mnAirplaneId << " type#" << vFlights.back().mnAirplaneType << endl;
        }
        
    }
    
    
    // ----- 打印基本信息 ----------
    cout << "航班总数: " << mFlightMap.size() << endl;
    cout << "飞机总数: " << mAllAirlineMap.size() << endl;
    cout << "国内机场: " << msDomesticAirports.size() << endl;
    cout << "联程航班: " << connectedFlights << endl;
    
    
}



void Problem::Solve()
{
    /* 先不引入调机空飞的操作: 对于竞赛中描述的场景, 调机空飞的架次应该非常非常少才对, 先不予考虑, 可以大大简化问题。
     * 留到最后再仔细分析下。
     * 嗯，很有道理哟!!! 哈哈哈!!! 哈哈哈!!! 哈哈哈!!!
     */
    
    // 1. 求出每个航班允许起飞的时间窗
    FindTimeWindowsOfFlights();
    
    // 2. 把处于调整窗口之内／之前 的航班分开
    SeparatePreAdjFlights();
    
    // ---- 找到一个相对不错的可行解 -----------
    FindAFeasibleSolution();
    
}



void Problem::FindAFeasibleSolution()
{
    /**
     *感觉单位时间容量限制特别容易违反, 而且也不好在Gurobi里面作限制.
     *先直接禁止在台风禁止起飞前一小时和台风结束后两小时起飞和降落吧.
     *不考虑调机和联程拉直, 即使有停机位也不停机, 就是这么任性哈哈哈!!!
     */
    
    // --- 单机调整 -------------------------------------------------
    int numPlanes = (int)mAdjAirlineMap.size();
    vector<double> vSAscores = vector<double>(numPlanes+1, -1000);
    
    for (auto& mit : mAdjAirlineMap)
    {
        const int airplaneId = mit.first;
        vector<int> vids(1,airplaneId);
        double cost = AdjustGroupedAirlines(vids, true, 0, 0);
        cout << airplaneId << ": " << cost << endl;
        vSAscores[airplaneId] = cost;
    }
    
    
    // --- 双机联调 -------------------------------------------------
    vector<size_t> vSortedIdxs = sort_indices(vSAscores);
    vSortedIdxs.resize(numPlanes);
    
    vector<vector<int> > planeGroups;
    
    while (vSortedIdxs.size() > 1)
    {
        int queryPlaneId = (int)vSortedIdxs[0];
        int trainIdx = -1;
        double mostReducedCost = -1000;
        vector<int> vids(2, queryPlaneId);
        
        for (int j=1; j<(int)vSortedIdxs.size(); j++)
        {
            int trialPlaneId = (int)vSortedIdxs[j];
            vids[1] = trialPlaneId;
            double pairCost = AdjustGroupedAirlines(vids, true, 0, 0);
            double reducedCost = vSAscores[queryPlaneId] + vSAscores[trialPlaneId] - pairCost;
            if (reducedCost > mostReducedCost)
            {
                trainIdx = j;
                mostReducedCost = reducedCost;
            }
        }
        if (trainIdx == -1) {
            cout << "双机联调时出错!!!" << endl;
            waitKey();
        }
        
        int bestPartnerId = (int)vSortedIdxs[trainIdx];
        vids[1] = bestPartnerId;
        
        if (mostReducedCost > 500) {
            planeGroups.push_back(vids);
            vSortedIdxs.erase(vSortedIdxs.begin() + trainIdx);
            vSortedIdxs.erase(vSortedIdxs.begin());
        } else {
            planeGroups.push_back(vector<int>(1, queryPlaneId));
            vSortedIdxs.erase(vSortedIdxs.begin());
        }
        
        cout << queryPlaneId << ", " << bestPartnerId << " : " << mostReducedCost << endl;
    }
    if (!vSortedIdxs.empty()) {
        planeGroups.push_back(vector<int>(1, (int)vSortedIdxs[0]));
    }
    
    
    // 整理双机联调的结果
    cout << endl << "双机联调结果:-------------------------------" << endl;
    
    double totalPairCost = 0;
    vector<double> groupCosts;
    for (size_t i=0; i<planeGroups.size(); i++) {
        const vector<int> &vids = planeGroups[i];
        double cost = AdjustGroupedAirlines(planeGroups[i], true, 0, 0);
        totalPairCost += cost;
        for (size_t j=0; j<vids.size(); j++) {
            cout << vids[j] << ", ";
        }
        cout << "cost: " << cost << endl;
    }
    cout << "双机联调: " << totalPairCost << endl;
    
    
    // --- 三机或四机联调 -------------------------------------------------
    vector<vector<int> > oldPlaneGroups = planeGroups;
    planeGroups.clear();
    
    vSortedIdxs = sort_indices(groupCosts);
    
    
    
    
    
    /*
    for (auto& mit : mTyphoonMap) {
        if (!mit.second.mbStopLimitedOnly) {
            mit.second.ResetSliceMap();
        }
    }
    */
    
    
}



double Problem::AdjustGroupedAirlines(const std::vector<int> &airplaneIds, bool cutWhole, bool showInfo, bool saveResult)
{
    const int nplanes = (int)airplaneIds.size();
    double cost = nplanes * (5e+7);
    
    // 每架飞机的机型 / 总的航班
    vector<int> planeTypes;
    vector<Flight> allAirlines;
    vector<size_t> entries;
    vector<int> initialAirports;
    vector<int> finalAirports;
    
    for (int i=0; i<nplanes; i++)
    {
        const int planeId = airplaneIds[i];
        const vector<Flight>& airline = mAdjAirlineMap[planeId];
        planeTypes.push_back(airline[0].mnAirplaneType);
        entries.push_back(allAirlines.size());
        allAirlines.insert(allAirlines.end(), airline.begin(), airline.end());
        initialAirports.push_back(airline[0].mnStartAirport);
        finalAirports.push_back(airline.back().mnEndAirport);
    }
    
    // 调整时间窗, 以满足单位时间容积约束
    CutOccupiedSlices(allAirlines, cutWhole);
    
    
    // 看两航班能不能相连
    const int N = (int)allAirlines.size();
    Mat connectivityMat = Mat::zeros(N, N, CV_8UC1);
    for (int i=0; i<N; ++i) {
        for (int j=0; j<N; ++j) {
            if (i==j) continue;
            if (TryConnectFlights(allAirlines[i], allAirlines[j])) {
                connectivityMat.at<uchar>(i,j) = 255;
            }
        }
    }
    //imshow("CMap", connectivityMat);
    //waitKey();
    
    
    // -------- Gurobi 优化 ------------------------
    const int M = N + 2*nplanes;
    
    GRBEnv *env = NULL;
    GRBVar *nodeVars = NULL;        // A node is a flight
    GRBVar **arcVars = NULL;        // links between nodes
    GRBVar *timeVars = NULL;        // take off time of flights
    GRBVar **windowVars = NULL;     // for multiple time windows
    GRBVar *btfVars = NULL;         // before-typhoon-flag
    GRBVar **planeVars = NULL;      // whether a plane serve a flight
    
    nodeVars = new GRBVar[M];       // 0~(N-1) flights | N~(M-1) initial & final nodes
    arcVars = new GRBVar*[M];
    for (int i=0; i<M; i++)
        arcVars[i] = new GRBVar[M];
    
    timeVars = new GRBVar[N];
    windowVars = new GRBVar*[N];
    for (int i=0; i<N; i++) {
        const int ntw = (int)allAirlines[i].mvTimeWindows.size();
        windowVars[i] = new GRBVar[ntw];
    }
    
    btfVars = new GRBVar[N];
    
    planeVars = new GRBVar*[nplanes];
    for (int i=0; i<nplanes; i++) {
        planeVars[i] = new GRBVar[M];
    }
    
    // var var var... so many vars...
    
    try {
        
        env = new GRBEnv();
        GRBModel model = GRBModel(*env);
        
        // --- Add variables into the model ---------------
        double totalPenalty = 0;

        for (int i=0; i<M; i++) {
            ostringstream ostr;
            ostr << "n_" << i;
            
            if (i>=N)
                nodeVars[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, ostr.str());
            else {
                double penalty = -CancelFlightParam * allAirlines[i].mdImportanceRatio;
                nodeVars[i] = model.addVar(0.0, 1.0, penalty, GRB_BINARY, ostr.str());
                totalPenalty -= penalty;
            }
            
            for (int j=0; j<M; j++) {
                ostringstream astr;
                astr << "a_" << i << j;
                arcVars[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, astr.str());
            }
        }
        
        
        for (int i=0; i<N; i++) {
            ostringstream tstr;
            tstr << "t_" << i;
            double t0 = allAirlines[i].mtStartDateTime;
            timeVars[i] = model.addVar(t0-6*3600, t0+36*3600, 0.0, GRB_CONTINUOUS, tstr.str());
            
            const int ntw = (int)allAirlines[i].mvTimeWindows.size();
            for (int j=0; j<ntw; j++) {
                ostringstream wstr;
                wstr << "w_" << i << j;
                windowVars[i][j] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, wstr.str());
            }
        }
        
        
        for (int i=0; i<M; i++) {
            if (i<N) {
                const Flight& flight = allAirlines[i];
                const int originalPlane = flight.mnAirplaneId;
                const int originalType = flight.mnAirplaneType;
                const double importRatio = flight.mdImportanceRatio;
                const double planeChangeCost = (flight.mtStartDateTime<=Date0506Clock16) ? PlaneChangeParamBefore : PlaneChangeParamAfter;
                
                for (int n=0; n<nplanes; n++) {
                    // 换机型
                    double coeff = FlightTypeChangeParams[originalType-1][planeTypes[n]-1];
                    
                    // 换飞机
                    if (originalPlane != airplaneIds[n]) {
                        coeff += planeChangeCost;
                    }
                    
                    // 重要系数
                    coeff *= importRatio;
                    
                    ostringstream pstr;
                    pstr << "plane_" << n << "_" << i;
                    planeVars[n][i] = model.addVar(0.0, 1.0, coeff, GRB_BINARY, pstr.str());
                }
            }
            else {
                for (int n=0; n<nplanes; n++) {
                    ostringstream pstr;
                    pstr << "plane_" << n << "_" << i;
                    planeVars[n][i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, pstr.str());
                }
            }
        }
        
        
        // --- Add constraints --------------------------
        // The initial/final node must be visited
        for (int i=N; i<M; i++) {
            nodeVars[i].set(GRB_DoubleAttr_LB, 1.0);
        }
        
        for (int i=0; i<M; i++) {
            GRBLinExpr nArcsIn = 0;
            GRBLinExpr nArcsOut = 0;
            for (int j=0; j<M; j++) {
                nArcsIn  += arcVars[j][i];
                nArcsOut += arcVars[i][j];
            }
            
            if (i < N) {    // Flight node
                model.addConstr(nArcsIn-nodeVars[i]==0);
                model.addConstr(nArcsOut-nodeVars[i]==0);
            }
            else if (i<N+nplanes)   // Initial node
            {
                model.addConstr(nArcsIn == 0);
                model.addConstr(nArcsOut == 1);
            }
            else {          // Final node
                model.addConstr(nArcsIn == 1);
                model.addConstr(nArcsOut == 0);
            }
        }
        
        // Set impossible arcs
        for (int n=0; n<nplanes; n++) {
            const int initialAirport = initialAirports[n];
            const int finalAirport = finalAirports[n];
            
            for (int i=0; i<N; i++) {
                if (allAirlines[i].mnStartAirport != initialAirport) {
                    arcVars[N+n][i].set(GRB_DoubleAttr_UB, 0);
                }
                if (allAirlines[i].mnEndAirport != finalAirport) {
                    arcVars[i][N+nplanes+n].set(GRB_DoubleAttr_UB, 0);
                }
            }
        }
        
        // 虽然一些飞机实际上是可以停在机场直到结束的, 但这里我还是禁止这种情况吧, 再看吧
        for (int i=N; i<M; i++) {
            for (int j=N; j<M; j++) {
                arcVars[i][j].set(GRB_DoubleAttr_UB, 0);
            }
        }
        
        for (int i=0; i<N; i++) {
            for (int j=0; j<N; j++) {
                if (connectivityMat.at<uchar>(i,j) == 0) {
                    arcVars[i][j].set(GRB_DoubleAttr_UB, 0);
                }
            }
        }
        
        // 联程航班限制: 如果两段都不取消, 则需要继续联程, 也就是不能在两段之间插入其他航班
        for (int i=0; i<N-1; i++) {
            if (allAirlines[i].mbIsConnected) {
                if (allAirlines[i+1].mbIsConnected && allAirlines[i].mnConnectedFlightId == allAirlines[i+1].mnFlightId) {
                    model.addConstr(nodeVars[i]+nodeVars[i+1]-arcVars[i][i+1]-1<=0);
                    i++;
                }
            }
        }
        
        
        // Time constraints
        for (int i=0; i<N; i++)
        {
            GRBLinExpr wsum = 0;
            const vector<pair<time_t, time_t> > &vTimeWindows = allAirlines[i].mvTimeWindows;
            const int ntw = (int)vTimeWindows.size();
            for (int j=0; j<ntw; ++j) {
                wsum += windowVars[i][j];
            }
            if (ntw > 0) {
                model.addConstr(wsum-nodeVars[i]>=0);
            }
            for (int j=0; j<ntw; ++j)
            {
                double taj = vTimeWindows[j].first;
                double tbj = vTimeWindows[j].second;
                model.addConstr(timeVars[i]-taj+(1-windowVars[i][j])*BIGNUM >= 0);
                model.addConstr(tbj-timeVars[i]+(1-windowVars[i][j])*BIGNUM >= 0);
            }
            
            double flyingTime = (double)allAirlines[i].mtFlyingTime;
            
            // 航班i的降落机场受台风影响吗?
            double typhoonEndTime = 0;
            bool mightStopInTyphoon = false;
            if (mTyphoonMap.count(allAirlines[i].mnEndAirport) && ntw > 0) {
                const Typhoon& typhoon = mTyphoonMap[allAirlines[i].mnEndAirport];
                if (allAirlines[i].mtTakeoffErliest + flyingTime <= typhoon.mtNoStop) {
                    mightStopInTyphoon = true;
                    typhoonEndTime = typhoon.mtEnd;
                    double tempTime = typhoon.mtNoStop - flyingTime;
                    ostringstream btfstr;
                    btfstr << "btf_" << i;
                    btfVars[i] = model.addVar(0.0, 1.0, 0.0, GRB_BINARY, btfstr.str());
                    model.addConstr(timeVars[i]-tempTime-60+btfVars[i]*BIGNUM >= 0);
                    model.addConstr(timeVars[i]-tempTime+(btfVars[i]-1)*BIGNUM <= 0);
                }
            }
            
            for (int j=0; j<N; j++) {
                if (connectivityMat.at<uchar>(i,j)) {
                    pair<int, int> flightIdPair(allAirlines[i].mnFlightId, allAirlines[j].mnFlightId);
                    double intervalTime = (double)MaxIntervalTime;
                    if (mFlightIntervalTimeMap.count(flightIdPair)) {
                        intervalTime = std::min(intervalTime, (double)mFlightIntervalTimeMap[flightIdPair]);
                    }
                    
                    // 航班衔接约束
                    model.addConstr(timeVars[i]-timeVars[j]+flyingTime+intervalTime-(1-arcVars[i][j])*BIGNUM <= 0);
                    
                    // 台风时限制停机的约束
                    if (mightStopInTyphoon) {
                        if (allAirlines[j].mtTakeoffLatest >= typhoonEndTime) {
                            model.addGenConstrIndicator(btfVars[i], 1, timeVars[j]-typhoonEndTime+60-(1-arcVars[i][j])*BIGNUM<=0);
                        }
                    }
                }
            }
            
            double t0i = (double)allAirlines[i].mtStartDateTime;
            double tai = t0i - 6*3600;
            double tbi = t0i + 36*3600;
            double importanceRatio = allAirlines[i].mdImportanceRatio;
            
            double tis[3] = {tai, t0i, tbi};
            double cost_tis[3] = {AheadFlightParam*importanceRatio*(t0i-tai), 0, DelayFlightParam*importanceRatio*(tbi-t0i)};
            
            if (ntw>0) {
                model.setPWLObj(timeVars[i], 3, tis, cost_tis);
            }
            
        }
        
        // Constraints from outside of the adjust window
        for (int n=0; n<nplanes; n++)
        {
            const vector<Flight>& preAirline = mPreAirlineMap[airplaneIds[n]];
            if (preAirline.empty()) {
                continue;
            }
            
            const Flight& preFlight = preAirline.back();
            double flyingTime = (double)preFlight.mtFlyingTime;
            double takeOffTime = (double)preFlight.mtStartDateTime;
            
            // 如果联程航班前半段在调整窗口之前, 则后半段是不能取消的
            const int entry = (int)entries[n];
            if (allAirlines[entry].mbIsConnected && !allAirlines[entry].mbIsConnedtedPrePart) {
                if (!preFlight.mbIsConnedtedPrePart || preFlight.mnConnectedFlightId!=allAirlines[entry].mnFlightId) {
                    cout << "Error: preFlight shall be connected pre part!" << endl;
                    abort();
                }
                nodeVars[entry].set(GRB_DoubleAttr_LB, 1);
                arcVars[N+n][entry].set(GRB_DoubleAttr_LB, 1);
            }
            
            
            // 台风停机限制
            bool mightStopInTyphoon = false;
            double typhoonEndTime = 0;
            if (mTyphoonMap.count(preFlight.mnEndAirport)) {
                const Typhoon& typhoon = mTyphoonMap[preFlight.mnEndAirport];
                typhoonEndTime = typhoon.mtEnd;
                mightStopInTyphoon = true;
            }
            
            // 时间限制
            for (int i=0; i<N; i++)
            {
                if (TryConnectFlights(preFlight, allAirlines[i])) {
                    pair<int, int> flightIdPair(preFlight.mnFlightId, allAirlines[i].mnFlightId);
                    double intervalTime = (double)MaxIntervalTime;
                    if (mFlightIntervalTimeMap.count(flightIdPair)) {
                        intervalTime = std::min(intervalTime, (double)mFlightIntervalTimeMap[flightIdPair]);
                    }
                    
                    // 航班衔接约束
                    model.addConstr(takeOffTime-timeVars[i]+flyingTime+intervalTime-(1-arcVars[N+n][i])*BIGNUM<=0);
                    
                    // 台风停机约束
                    if (mightStopInTyphoon) {
                        model.addConstr(timeVars[i] - typhoonEndTime+60-(1-arcVars[N+n][i])*BIGNUM <= 0);
                    }
                    
                } else {
                    arcVars[N+n][i].set(GRB_DoubleAttr_UB, 0);
                }
            }
        }
        
        // ----- 飞机各不相同 -------------------------
        // Initial nodes 是定好的哟
        for (int n=0; n<nplanes; n++) {
            planeVars[n][N+n].set(GRB_DoubleAttr_LB, 1);
        }
        
        // 航班之间连起来啦, 那必然是同一架飞机呀
        for (int i=0; i<M; i++) {
            for (int j=0; j<M; j++) {
                for (int n=0; n<nplanes; n++) {
                    model.addConstr(planeVars[n][i] - planeVars[n][j] + arcVars[i][j] - 1 <=0);
                }
            }
        }
        
        // 航线－飞机 限制
        for (int i=0; i<N; i++) {
            pair<int, int> portIdPair(allAirlines[i].mnStartAirport, allAirlines[i].mnEndAirport);
            if (mAirlineLimitMap.count(portIdPair)) {
                const set<int> limitedPlaneSet = mAirlineLimitMap[portIdPair];
                for (int n=0; n<nplanes; n++) {
                    if (limitedPlaneSet.count(airplaneIds[n])) {
                        planeVars[n][i].set(GRB_DoubleAttr_UB, 0);
                    }
                }
            }
        }
        
        // 每个航班只能由一个飞机执行
        for (int i=0; i<M; i++) {
            GRBLinExpr iplaneSum = 0;
            for (int n=0; n<nplanes; n++) {
                iplaneSum += planeVars[n][i];
            }
            model.addConstr(iplaneSum == nodeVars[i]);
        }
        
        // 全局基地平衡限制
        // 简单起见, 我这里限制一个结束Node不能接收一架和它原定机型不同的飞机
        for (int n=0; n<nplanes; n++)
        {
            for (int k=0; k<nplanes; k++) {
                if (planeTypes[k] != planeTypes[n]) {
                    planeVars[k][N+nplanes+n].set(GRB_DoubleAttr_UB, 0);
                }
            }
        }
        
        
        
        // 优化
       // model.set(GRB_IntParam_MIPFocus, 1);
       // model.set(GRB_DoubleParam_Heuristics, 1);
        model.set(GRB_IntParam_OutputFlag, showInfo);
        model.optimize();
        
        // --- Result ---
        int status = model.get(GRB_IntAttr_Status);
        if (status == GRB_INF_OR_UNBD || status == GRB_INFEASIBLE || status == GRB_UNBOUNDED) {
            if (showInfo) {
                cout << "The model cannot be solved " << "because it's infeasible or unbounded" << endl;
                cout << GRB_INF_OR_UNBD<< ", " << GRB_INFEASIBLE << ", " << GRB_UNBOUNDED << "->" <<status << endl;
            }
            return cost;
        }
        
        cost = model.get(GRB_DoubleAttr_ObjVal);
        cost += totalPenalty;
        
        
        // --- Save -----------------------
        for (int i=0; i<N; i++) {
            int iServed = round(nodeVars[i].get(GRB_DoubleAttr_X));
            long iStartTime = round(timeVars[i].get(GRB_DoubleAttr_X));
            
            ResultFlight resFlight(allAirlines[i]);
            if (iServed) {
                
                int totalServe = 0;
                for (int n=0; n<nplanes; n++) {
                    int nserve = round(planeVars[n][i].get(GRB_DoubleAttr_X));
                    totalServe += nserve;
                    if (nserve==1) {
                        resFlight.mnAirplaneId = airplaneIds[n];
                    }
                }
                if (totalServe != 1) {
                    cout << "错误: 执行某一航班的飞机数不为1" << endl;
                }
                
                resFlight.mtStartDateTime = iStartTime;
                resFlight.mtEndDateTime = iStartTime + allAirlines[i].mtFlyingTime;
                
            } else {
                resFlight.mbIsCancel = true;
            }
            
            // 更新 Slice Maps
            if (iServed) {
                UpdateSliceMaps(resFlight, true);
            }
            
            
            if (saveResult) {
                mResultFlightMap[resFlight.mnFlightId] = resFlight;
            }
            
        }
        
        
        
        
        
    } catch (GRBException exc) {
        cout << "Error code = " << exc.getErrorCode() << endl;
        cout << exc.getMessage() << endl;
    } catch (...) {
        cout << "Exception during optimization" << endl;
    }

    
    // --- Clear ---------------------
    delete[] nodeVars;
    for (int i=0; i<M; i++)
        delete[] arcVars[i];
    delete[] arcVars;
    delete[] timeVars;
    for (int i=0; i<N; i++)
        delete[] windowVars[i];
    delete[] windowVars;
    delete[] btfVars;
    for (int i=0; i<nplanes; i++)
        delete[] planeVars[i];
    delete[] planeVars;
    delete env;
    
    
    return cost;
}



void Problem::FindTimeWindowsOfFlights()
{
    for (auto& mit : mAllAirlineMap) {
        
        vector<Flight> &airline = mit.second;
        
        for (size_t i=0; i<airline.size(); i++) {
            
            Flight& flight = airline[i];
            const time_t tTakeOff = flight.mtStartDateTime;
            
            // ====== 如果在调整窗口外, 则不能调整 ==============
            if (tTakeOff<mtRecoveryStart || tTakeOff>mtRecoveryEnd) {
                flight.mvTimeWindows.clear();
                flight.mvTimeWindows.push_back(make_pair(tTakeOff, tTakeOff));
                flight.mtTakeoffErliest = tTakeOff;
                flight.mtTakeoffLatest = tTakeOff;
                continue;
            }
            
            // ====== 可调整的航班 ==========================
            const int startAirport = flight.mnStartAirport;
            const int endAirport = flight.mnEndAirport;
            const time_t flyingTime = flight.mtFlyingTime;
            
            // 是国内航班吗? 起飞受台风影响的国内航班才能提前
            const bool isDomestic = flight.mbIsDomestic;
            
            // 起飞受台风影响吗?
            bool takeoffHit = false;
            if (mTyphoonMap.count(startAirport))
                takeoffHit = mTyphoonMap[startAirport].IsTakeoffInTyphoon(flight.mtStartDateTime);
            
            // --- 时间窗计算 --------
            vector<pair<time_t, time_t> > vTimeWindows;
            
            // 最大提前/延迟范围
            time_t tmin = tTakeOff - ((takeoffHit && isDomestic) ? MaxAheadTime : 0);
            time_t tmax = tTakeOff + (isDomestic ? MaxDomesticDelayTime : MaxAbroadDelayTime);
            vTimeWindows.push_back(make_pair(tmin, tmax));
            
            
            // 从时间窗里减去因台风而禁起飞的时间段
            if (mTyphoonMap.count(startAirport)) {
                const Typhoon& typhoon = mTyphoonMap[startAirport];
                if (!typhoon.mbStopLimitedOnly) {
                    Helper::SubtractInterval(vTimeWindows, make_pair(typhoon.mtNoTakeOff, typhoon.mtEnd));
                }
            }
            
            // 从时间窗里减去因台风而禁着陆(而影响起飞)的时间段
            if (mTyphoonMap.count(endAirport)) {
                const Typhoon& typhoon = mTyphoonMap[endAirport];
                if (!typhoon.mbStopLimitedOnly) {
                    Helper::SubtractInterval(vTimeWindows, make_pair(typhoon.mtNoLand-flyingTime, typhoon.mtEnd-flyingTime));
                }
            }
            
            // 还要注意不要违反机场关闭限制, OMG!!!
            if (mAirportCloseMap.count(startAirport)) {
                vector<AirportClose> &vAirportCloses = mAirportCloseMap[startAirport];
                for (size_t k=0; k<vAirportCloses.size(); ++k)
                    vAirportCloses[k].AdjustTimeWindows(vTimeWindows, 0);
            }
            
            if (mAirportCloseMap.count(endAirport)) {
                vector<AirportClose> &vAirportCloses = mAirportCloseMap[endAirport];
                for (size_t k=0; k<vAirportCloses.size(); ++k)
                    vAirportCloses[k].AdjustTimeWindows(vTimeWindows, flyingTime);
            }
            
            // 算好啦, LOL!!!
            flight.mvTimeWindows = vTimeWindows;
            if (!vTimeWindows.empty()) {
                flight.mtTakeoffErliest = vTimeWindows[0].first;
                flight.mtTakeoffLatest = vTimeWindows.back().second;
            } else {
                flight.mtTakeoffErliest = flight.mtStartDateTime;
                flight.mtTakeoffLatest = flight.mtStartDateTime;
            }
        }
    }
    
    /*
     // --- Debug Print ----
     for (auto &mit : mAllAirlineMap) {
         const vector<Flight>& airline = mit.second;
         for (size_t i=0; i<airline.size(); ++i) {
             const Flight& flight = airline[i];
             const vector<pair<time_t, time_t> > &timeWindows = flight.mvTimeWindows;
     
             cout << "FlightNo: " << flight.mnFlightNo << "," << flight.mbIsConnected << ", " << flight.mnStartAirport << " to " << flight.mnEndAirport << ", ";
             for (size_t j=0; j<timeWindows.size(); ++j) {
                 cout << Helper::DateTimeToString(timeWindows[j].first) << "->" << Helper::DateTimeToString(timeWindows[j].second) << ", ";
             }
             cout << endl;
         }
         cout << endl;
     }
    */
}



// 把起飞时间在调整窗口之前的航班分开
// 需要在对航班按时间先后排完序并标记完联程航班后再执行这一步
void Problem::SeparatePreAdjFlights()
{
    int nadj = 0;
    mPreAirlineMap.clear();
    mAdjAirlineMap.clear();
    
    for (auto& mit : mAllAirlineMap) {
        
        const int airplaneId = mit.first;
        const vector<Flight>& allAirline = mit.second;
        
        vector<Flight> preAirline;  preAirline.clear();
        vector<Flight> adjAirline;  adjAirline.clear();
        
        for (size_t i=0; i<allAirline.size(); i++) {
            
            Flight flight = allAirline[i];
            if (flight.mtStartDateTime < mtRecoveryStart) {
                preAirline.push_back(flight);
                ResultFlight resFlight(flight);
                mResultFlightMap[resFlight.mnFlightId] = resFlight;
            } else {
                adjAirline.push_back(flight);
            }
        }
        
        mPreAirlineMap[airplaneId] = preAirline;
        mAdjAirlineMap[airplaneId] = adjAirline;
              
        nadj += (int)adjAirline.size();
    }
    
    cout << "待调整航班数: " << nadj << endl;
}





bool Problem::TryConnectFlights(const Flight &flightA, const Flight &flightB)
{
    // flightA的降落机场
    const int endAirportA = flightA.mnEndAirport;
    if (!(endAirportA==flightB.mnStartAirport)) {
        return false;
    }
    
    // 航班的时间窗口不能为空
    if (flightA.mvTimeWindows.empty() || flightB.mvTimeWindows.empty()) {
        return false;
    }
    
    pair<int, int> flightIDpair(flightA.mnFlightId, flightB.mnFlightId);
    
    // flightA 和 flightB 之间的转场时间
    time_t flightIntervalTime = MaxIntervalTime;
    if (mFlightIntervalTimeMap.count(flightIDpair)) {
        flightIntervalTime = std::min(MaxIntervalTime, mFlightIntervalTimeMap[flightIDpair]);
    }
    
    // 如果flightA尽可能早地起飞, 落地转场之后仍然超出了flightB的时间窗, 那就连接不起来呀
    if (flightA.mtTakeoffErliest + flightA.mtFlyingTime + flightIntervalTime > flightB.mtTakeoffLatest) {
        return false;
    }
    
    // fligthA的降落机场有台风吗?
    if (mTyphoonMap.count(flightB.mnStartAirport)) {
        const Typhoon& typhoon = mTyphoonMap[flightB.mnStartAirport];
        if (typhoon.mnMaxStopPlanes == 0) {
            // 限制停机, 如果flightA最晚落地时间在限制停机开始之前, 而flighB最早起飞时间在台风之后, 那连不起来
            if ((flightB.mtTakeoffErliest>typhoon.mtEnd)
                && (flightA.mtTakeoffLatest+flightA.mtFlyingTime<typhoon.mtNoStop))
            {
                return false;
            }
        }
    }
    
    return true;
    
}



void Problem::UpdateSliceMaps(const ResultFlight &rf, bool addOrReduce)
{
    if (rf.mbIsCancel)
        return;
    
    if (mTyphoonMap.count(rf.mnStartAirport)) {
        if (addOrReduce) {
            mTyphoonMap[rf.mnStartAirport].AddSliceCount(rf.mtStartDateTime);
        } else {
            mTyphoonMap[rf.mnStartAirport].ReduceSliceCount(rf.mtStartDateTime);
        }
    }
    
    if (mTyphoonMap.count(rf.mnEndAirport)) {
        if (addOrReduce) {
            mTyphoonMap[rf.mnEndAirport].AddSliceCount(rf.mtEndDateTime);
        }
        else {
            mTyphoonMap[rf.mnEndAirport].ReduceSliceCount(rf.mtEndDateTime);
        }
    }
}



void Problem::CutOccupiedSlices(std::vector<Flight> &vFlights, bool cutWhole)
{
    if (cutWhole)
    {
        for (size_t i=0; i<vFlights.size(); i++)
        {
            Flight& flight = vFlights[i];
            if (mTyphoonMap.count(flight.mnStartAirport)) {
                const Typhoon& typhoon = mTyphoonMap[flight.mnStartAirport];
                if (!typhoon.mbStopLimitedOnly) {
                    Helper::SubtractInterval(flight.mvTimeWindows, make_pair(typhoon.mtNoTakeOff-3670, typhoon.mtEnd+7270));
                }
            }
            if (mTyphoonMap.count(flight.mnEndAirport)) {
                const Typhoon& typhoon = mTyphoonMap[flight.mnEndAirport];
                if (!typhoon.mbStopLimitedOnly) {
                    Helper::SubtractInterval(flight.mvTimeWindows, make_pair(typhoon.mtNoLand-flight.mtFlyingTime, typhoon.mtEnd-flight.mtFlyingTime+7270));
                }
            }
        }
    }
    else
    {
        for (size_t i=0; i<vFlights.size(); i++)
        {
            Flight& flight = vFlights[i];
            
            if (mTyphoonMap.count(flight.mnStartAirport) && !mTyphoonMap[flight.mnStartAirport].mbStopLimitedOnly) {
                const Typhoon& typhoon = mTyphoonMap[flight.mnStartAirport];
                for (auto& mit : typhoon.mSliceMap) {
                    if (mit.second >= 1) {
                        int loc = mit.first;
                        time_t tfront = typhoon.mtOneHourBeforeNoTakeoff + loc * 300;
                        Helper::SubtractInterval(flight.mvTimeWindows, pair<time_t, time_t>(tfront-6, tfront+300));
                    }
                }
            }
            
            if (mTyphoonMap.count(flight.mnEndAirport) && !mTyphoonMap[flight.mnEndAirport].mbStopLimitedOnly) {
                const Typhoon& typhoon = mTyphoonMap[flight.mnEndAirport];
                for (auto& mit : typhoon.mSliceMap) {
                    if (mit.second >= 1) {
                        int loc = mit.first;
                        time_t tfront = typhoon.mtOneHourBeforeNoTakeoff + loc * 300 - flight.mtFlyingTime;
                        Helper::SubtractInterval(flight.mvTimeWindows, pair<time_t, time_t>(tfront-6, tfront+300));
                    }
                }
            }
        }
    }
}



void Problem::SaveResults(const string& filename)
{
    ofstream fout(filename);
    
    for (auto& mit : mResultFlightMap) {
        
        const ResultFlight& rf = mit.second;
        
        fout << rf.mnFlightId << "," << rf.mnStartAirport << "," << rf.mnEndAirport << ",";
        fout << Helper::DateTimeToFullString(rf.mtStartDateTime) << ",";
        fout << Helper::DateTimeToFullString(rf.mtEndDateTime) << ",";
        fout << rf.mnAirplaneId << ",";
        fout << rf.mbIsCancel << "," << rf.mbIsStraighten << "," << 0 << "," << rf.mbIsSignChange << ",";
        fout << endl;
        
    }
    
    fout.close();
    
}
