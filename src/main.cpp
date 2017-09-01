//
//  main.cpp
//  HelloLele
//
//  Created by 谭智丹 on 2017/8/21.
//  Copyright © 2017年 谭智丹. All rights reserved.
//

#include <iostream>
#include "Helper.hpp"
#include "Problem.hpp"

using namespace std;

void LoadSolution(const string& file, vector<vector<int> > &planeGroups, vector<double>& costs)
{
    ifstream ifs(file);
    
    string aline, s;
    vector<double> data;
    
    while (!ifs.eof()) {
        getline(ifs, aline);
        stringstream stream(aline);
        data.clear();
        while (getline(stream, s, ','))
        {
            data.push_back(stod(s));
        }
        vector<int> ids(data.size()-1, 0);
        for (size_t i=0; i<data.size()-1; i++) {
            ids[i] = (int)data[i];
        }
        planeGroups.push_back(ids);
        costs.push_back(data.back());
    }
}


int main(int argc, const char * argv[]) {
    
    string dir = "/Users/tanzhidan/Documents/aaaxacompetition/data_20170814";
    
    
    Problem prob(dir);
    
    clock_t tbegin = clock();
    prob.Solve();
    clock_t tend = clock();
    cout << "Time: " << (double)(tend-tbegin)/CLOCKS_PER_SEC << endl;
    
    prob.SaveResults("/Users/tanzhidan/Documents/java-xma/results/leleleoo123_680000.00_1.csv");
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
