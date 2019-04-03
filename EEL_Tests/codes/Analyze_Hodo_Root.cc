/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Analyze_Hodo_Root.cc
 * Author: rafopar
 *
 * Created on October 28, 2018, 9:26 PM
 */

#include <cstdlib>

#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TTree.h>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Please provide the data Run number " << endl;
        cout << "Exiting" << endl;
        exit(1);
    }

    string strrunNo = argv[1];
    int runNo = atoi(argv[1]);

    const int n_ch = 16;
    const int n_sl = 2;
    const int n_samp = 112; // # of FADC samples

    double signal_1_[n_ch];
    double signal_2_[n_ch];
    double peak_1_[n_ch];
    double peak_2_[n_ch];
    double peak_pos_1_[n_ch];
    double peak_pos_2_[n_ch];
    double pedestal_1_[n_ch];
    double pedestal_2_[n_ch];

    TFile *file_in = new TFile(Form("../Data/Hodo_tests_%d.root", runNo));

    TTree *tr1 = (TTree*) file_in->Get("tr1");

    TBranch *b_signal_1_;
    TBranch *b_signal_2_;
    TBranch *b_peak_1_;
    TBranch *b_peak_2_;
    TBranch *b_peak_pos_1_;
    TBranch *b_peak_pos_2_;
    TBranch *b_pedestal_1_;
    TBranch *b_pedestal_2_;

    tr1->SetBranchAddress("signal_1_", &signal_1_, &b_signal_1_);
    tr1->SetBranchAddress("signal_2_", &signal_2_, &b_signal_2_);
    tr1->SetBranchAddress("peak_1_", &peak_1_, &b_peak_1_);
    tr1->SetBranchAddress("peak_2_", &peak_2_, &b_peak_2_);
    tr1->SetBranchAddress("peak_pos_1_", &peak_pos_1_, &b_peak_pos_1_);
    tr1->SetBranchAddress("peak_pos_2_", &peak_pos_2_, &b_peak_pos_2_);
    tr1->SetBranchAddress("pedestal_1_", &pedestal_1_, &b_pedestal_1_);
    tr1->SetBranchAddress("pedestal_2_", &pedestal_2_, &b_pedestal_2_);

    TFile *file_out = new TFile(Form("../Data/Analyze_Root_%d.root", runNo), "Recreate");
    TH1D * h_signal_all1_[n_sl][n_ch];
    TH1D * h_signal_coincid1_[n_sl][n_ch];


    for (int isl = 0; isl < n_sl; isl++) {
        for (int ich = 0; ich < n_ch; ich++) {
            h_signal_all1_[isl][ich] = new TH1D(Form("h_signal_all1_%d_%d", isl, ich), "", 400, -250., 2900);
            h_signal_coincid1_[isl][ich] = new TH1D(Form("h_signal_coincid1_%d_%d", isl, ich), "", 400, -250., 2900);
        }
    }

    int nev = tr1->GetEntries();
    cout << "Nev = " << nev << endl;

    for (int iev = 0; iev < nev; iev++) {

        tr1->GetEntry(iev);


        //        h_signal_7_all1->Fill(signal_1_[7]);
        //        h_signal_8_all1->Fill(signal_1_[8]);

        for (int ich = 0; ich < n_ch; ich++) {


            //  Pedestals might be calculated wring, we want to look events that have the peak
            //  in a correct time interval of the readout window
            if (peak_pos_1_[ich] <= 14. || peak_pos_1_[ich] >= 65.) {
                continue;
            }

            h_signal_all1_[0][ich]->Fill(signal_1_[ich]);
            if (peak_2_[14] > 155 && peak_2_[15] > 155) {
                h_signal_coincid1_[0][ich]->Fill(signal_1_[ich]);
            }

        }



    }

    gDirectory->Write();
    return 0;
}

