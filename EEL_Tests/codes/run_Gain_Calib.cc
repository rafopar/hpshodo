/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   run_Gain_Calib.cc
 * Author: rafopar
 *
 * Created on May 6, 2019, 5:04 PM
 */

#include <cstdlib>
#include <iostream>
#include "Gain_Calib.C"

using namespace std;

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
    
    Gain_Calib t(run);
    t.Loop();

    return 0;
}

