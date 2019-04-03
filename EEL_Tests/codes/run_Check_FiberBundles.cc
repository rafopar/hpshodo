/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   run_Check_FiberBundles.cc
 * Author: rafopar
 *
 * Created on March 4, 2019, 10:19 AM
 */

#include <cstdlib>
#include <iostream>
#include "Check_FiberBundles.C"

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {
    
    if( argc < 2 ){
        cout<<"Provide the run number "<<endl;
        cout<<"Exiting "<<endl;
        exit(1);
    }

    int run = atoi(argv[1]);
    Check_FiberBundles t(run);
    t.Loop();
    
    return 0;
}

