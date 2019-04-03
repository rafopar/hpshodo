/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   run_Check_Assembled.cc
 * Author: rafopar
 *
 * Created on March 27, 2019, 12:57 AM
 */

#include <cstdlib>
#include <iostream>
#include "Check_Assembled_tiles.C"

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
    Check_Assembled_tiles t(run);
    t.Loop();
    
    
    return 0;
}

