/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Make_hists_Asci.cc
 * Author: rafopar
 *
 * Created on December 7, 2018, 3:34 PM
 */

#include <TH1D.h>
#include <TFile.h>

#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    if( argc != 2 ){
        cout<<"Please provide the run number"<<endl;
        cout<<"Exiting"<<endl;
        exit(1);
    }
    
    int run = atoi(argv[1]);
    
    TFile *file_in = new TFile(Form("../Data/Analyze_Root_%d.root", run));
    
    const int n_ch = 16;
    
    for( int ich = 0; ich < n_ch; ich++ ){
    
        ofstream out_file(Form("../Data_ASCI/HPS_Hodo_%d_%d.dat", run, ich));
        
        TH1D *cur_hist = (TH1D*)file_in->Get(Form("h_signal_coincid1_0_%d", ich));
        
        int n_bins = cur_hist->GetNbinsX();
        
        for( int ibin = 0; ibin < n_bins; ibin++ ){
            double bin_center = cur_hist->GetBinCenter(ibin + 1);
            out_file<<bin_center<<setw(15)<<cur_hist->GetBinContent(ibin+1)<<endl;
        }
        
        out_file.close();
    }
    
    file_in->Close();
    
    return 0;
}

