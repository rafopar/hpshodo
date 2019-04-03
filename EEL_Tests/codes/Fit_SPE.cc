/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Fit_SPE.cc
 * Author: rafopar
 *
 * Created on April 2, 2019, 10:03 AM
 */


#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>
#include "/Work/Soft/MyFuncs/FitFuncs.cc"

#include <fstream>

#include <cstdlib>

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

    const int n_par = 10;
    const double x_max = 500;
    const double Ebb_sheet = 800.;
    const double EEL_HV = 880.;

    
    TF1 *f_Sum_Responce = new TF1("f_Sum_Responce", Sum_Responce, -400., x_max, n_par);
    f_Sum_Responce->SetNpx(4500);
    
    const int n = 4;
    double scale = 1.49;
    double mu = 0.028;
    double omega = 0.74;
    double Q0 = 4.5;
    double Q1 = 50;
    double alpha = 2.e-5;
    double sigma0 = 12;
    double sigma1 = 21.;
    double height = 84000;

    const int n_pmt_ch = 16;
    
    const int n_slot = 2;
    
    string HV_PMTs[n_slot] = {"910V", "985V"};
    string PMT_Names[n_slot] = {"SA3857", "SA2060"};
    
    ofstream out_Gains[2];
    
    out_Gains[0] = ofstream(Form("Gains_%s_%s.dat", PMT_Names[0].c_str(), HV_PMTs[0].c_str()));
    out_Gains[1] = ofstream(Form("Gains_%s_%s.dat", PMT_Names[1].c_str(), HV_PMTs[1].c_str()));
            
    TH1D *h_signal_slot[n_slot][n_pmt_ch];
    
    
    int sl_numbers[n_slot] = {3, 4};
    
    f_Sum_Responce->SetParameters(n, mu, omega, Q0, Q1, alpha, sigma0, sigma1, scale, height);

    TF1 * f_sub_peak_[n];

    f_Sum_Responce->FixParameter(0., n);
    f_Sum_Responce->SetParLimits(1, 0.0, 100);
    f_Sum_Responce->SetParLimits(2, 0., 1.);
    f_Sum_Responce->SetParLimits(3, -200, 200);
    f_Sum_Responce->SetParLimits(4, 10, 200);
    f_Sum_Responce->SetParLimits(5, 0.000000001, 1);
    f_Sum_Responce->SetParLimits(6, 0.1, 75);
    f_Sum_Responce->SetParLimits(7, 20, 1000);
    f_Sum_Responce->SetParNames("n", "#mu", "#omega", "Q_{0}", "Q_{1}", "#alpha", "#sigma_{0}", "#sigma_{1}", "scale", "height");
    
    
    TFile *file_in = new TFile(Form("../Data/SPE_hists_%d.root", run));
    
    TCanvas *c_[2];
    
    for( int i = 0; i < 2; i++ ){
        c_[i] = new TCanvas(Form("c_%d", i), "", 1200, 1200);
        c_[i]->Divide(4, 4);
    }
    
    for( int i_sl = 0; i_sl < n_slot; i_sl++ ){
        
        for( int i_ch = 0; i_ch < n_pmt_ch; i_ch++ ){
            
            c_[i_sl]->cd(i_ch + 1)->SetLogy();
            h_signal_slot[i_sl][i_ch] = (TH1D*)file_in->Get(Form("h_signal_slot%d_%d", sl_numbers[i_sl], i_ch));
             h_signal_slot[i_sl][i_ch]->SetAxisRange(-50., x_max, "X");
            h_signal_slot[i_sl][i_ch]->Fit(f_Sum_Responce, "MeV", "", -50, x_max);

            double Q1 = f_Sum_Responce->GetParameter(4);
            double Q1_Err = f_Sum_Responce->GetParError(4);

            out_Gains[i_sl]<<i_ch<<setw(12)<<Q1<<endl;
        }
        
    }
    
    for( int i_sl = 0; i_sl < n_slot; i_sl++ ){
        c_[i_sl]->Print(Form("SPE_Fits_Slot%d.eps", sl_numbers[i_sl]));
        c_[i_sl]->Print(Form("SPE_Fits_Slot%d.pdf", sl_numbers[i_sl]));
        c_[i_sl]->Print(Form("SPE_Fits_Slot%d.png", sl_numbers[i_sl]));
    }
    
    return 0;
}

