/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   run_Fill_SPE_hists.cc
 * Author: rafopar
 *
 * Created on April 2, 2019, 12:18 AM
 */

#include "Fill_SPE_hists.C"
#include <cstdlib>

#include <iostream>

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

    Fill_SPE_hists t(run);
    t.Loop();
    
    return 0;
}

