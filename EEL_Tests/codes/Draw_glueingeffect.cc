/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Draw_glueingeffect.cc
 * Author: rafopar
 *
 * Created on January 3, 2019, 4:57 PM
 */

#include <cstdlib>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TLatex.h>
#include <TCanvas.h>

using namespace std;

/*
 * 
 */
void Draw_glueingeffect() {
    
    const int n_ch = 16;
    
    TLatex *lat1 = new TLatex();
    lat1->SetNDC(); 
    
    TFile *file_no_Glue = new TFile("../Data/Hodo_tests_Analyze_215.root");
    TFile *file_with_Glue = new TFile("../Data/Hodo_tests_Analyze_221.root");
    
    TH1D *h_signal_1_noGlue_[n_ch];
    TH1D *h_signal_1_WithGlue_[n_ch];
    
    for( int ich = 0; ich < n_ch; ich++ ){
        h_signal_1_noGlue_[ich] = (TH1D*)file_no_Glue->Get(Form("h_signal_1_%d", ich));
        h_signal_1_noGlue_[ich]->Scale(1000./h_signal_1_noGlue_[ich]->GetMaximum());
        h_signal_1_noGlue_[ich]->SetLineColor(4);
        
        h_signal_1_WithGlue_[ich] = (TH1D*)file_with_Glue->Get(Form("h_signal_1_%d", ich));
        h_signal_1_WithGlue_[ich]->Scale(1000./h_signal_1_WithGlue_[ich]->GetMaximum());
        h_signal_1_WithGlue_[ich]->SetLineColor(2);
    }
    
    TCanvas *c1 = new TCanvas("c1", "", 1200, 1200);
    c1->Divide(4, 4, 0., 0.);
    
    for( int ich = 0; ich < n_ch; ich++ ){
        c1->cd( int(ich/4)*4 + ich%4 + 1);
        h_signal_1_noGlue_[ich]->Draw("hist");
        h_signal_1_WithGlue_[ich]->Draw("Same hist");
    }

    
    cout<<"PMT ch#3 No glue mean = "<<h_signal_1_noGlue_[2]->GetMean()<<endl;
    cout<<"PMT ch#3 With glue mean = "<<h_signal_1_WithGlue_[2]->GetMean()<<endl;
    cout<<"PMT ch#6 No glue mean = "<<h_signal_1_noGlue_[5]->GetMean()<<endl;
    cout<<"PMT ch#6 With glue mean = "<<h_signal_1_WithGlue_[5]->GetMean()<<endl;
    
    return 0;
}

