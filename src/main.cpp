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

int main(int argc, const char * argv[]) {
    
    string dir = "/Users/tanzhidan/Documents/aaaxacompetition/data_20170814";
    
    Problem prob(dir);
    
    clock_t tbegin = clock();
    prob.Solve();
    clock_t tend = clock();
    cout << "Time: " << (double)(tend-tbegin)/CLOCKS_PER_SEC << endl;
    
   // prob.SaveResults("/Users/tanzhidan/Documents/java-xma/results/leleleoo123_680000.00_1.csv");
    
    
    std::cout << "Hello, World!\n";
    return 0;
}
