/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Draw_Signals.cc
 * Author: rafopar
 *
 * Created on September 11, 2018, 9:53 PM
 */

#include <cstdlib>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TLine.h>
#include <TLatex.h>
#include <TGraph.h>
#include <TGraphErrors.h>
#include <TCanvas.h>

#include <iostream>

#include <vector>

using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    TLine *line1 = new TLine();
    line1->SetLineColor(2);
    
    const int n_PMT_ch = 16;
    TCanvas *c1 = new TCanvas("c1", "", 1600, 1600);
    c1->Divide(4, 4);

    TLatex *lat1 = new TLatex();
    lat1->SetTextFont(52);
    lat1->SetNDC();

    map<int, vector<int> > ch_list;


    // In each run two fibers were inside the scintillator, and the 
    // rest have fibers hanging out free
    ch_list[141] = vector<int>{5, 6};
    ch_list[143] = vector<int>{7, 8};
    ch_list[144] = vector<int>{9, 10};
    ch_list[145] = vector<int>{11, 12};
    ch_list[146] = vector<int>{13, 14};
    ch_list[147] = vector<int>{15, 0};
    ch_list[148] = vector<int>{1, 2};
    ch_list[149] = vector<int>{3, 4};


    //    for (int i = 0; i < ch_list[74].size(); i++ ){
    //    
    //        cout<<ch_list[74].at(i)<<endl;
    //    }

    TGraphErrors *gr_signal_mean_val = new TGraphErrors();
    
    int ch_index = 0;
    for (map<int, vector<int> >::iterator it = ch_list.begin(); it != ch_list.end(); it++) {

        int run = it->first;
        vector<int> channel_list = it->second;
        TFile *cur_file = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");
        for (int i_ch = 0; i_ch < channel_list.size(); i_ch++) {
            int cur_ch = channel_list.at(i_ch);

            
            TH1D *h_cur_hist = (TH1D*) cur_file->Get(Form("h_signal_all1_0_%d", cur_ch));
            TH1D *h_cur_hist_coincid = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", cur_ch));
            TH1D *h_cur_hist_coincid_RB = (TH1D*)h_cur_hist_coincid->RebinX(4);
            //h_cur_hist_coincid->SetLineColor(2);

            c1->cd(cur_ch + 1);
            //h_cur_hist->DrawCopy();
            h_cur_hist_coincid_RB->DrawCopy("");
            double mean = h_cur_hist_coincid_RB->GetMean();
            double mean_err = h_cur_hist_coincid_RB->GetMeanError();
            lat1->DrawLatex(0.5, 0.7, Form("mean = %1.2f", mean));
            
            gr_signal_mean_val->SetPoint(ch_index, cur_ch, mean);
            gr_signal_mean_val->SetPointError(ch_index, 0., mean_err);
                    
            ch_index =   ch_index + 1;      
        }

        delete cur_file;
    }

    c1->Print("Figs/Signals_Allchannel.eps");
    c1->Print("Figs/Signals_Allchannel.pdf");
    c1->Print("Figs/Signals_Allchannel.png");


    gr_signal_mean_val->SetTitle("; Channel # ; ADC mean");
    gr_signal_mean_val->GetYaxis()->SetTitleOffset(1.4);
    gr_signal_mean_val->SetMarkerColor(4);
    gr_signal_mean_val->SetMarkerStyle(20);
    gr_signal_mean_val->SetMarkerSize(2.5);
    gr_signal_mean_val->SetMinimum(0.);
    c1->Clear();
    gr_signal_mean_val->Draw("AP");
    c1->Print("Figs/NPE_variations.eps");
    c1->Print("Figs/NPE_variations.pdf");
    c1->Print("Figs/NPE_variations.png");
    
    
    // Then three different runs (in addition to 83) were taken with channels 7 and 8 in the scintillator, just
    // the HV was different
    map<int, vector<int> > ch_list2;
    ch_list2[127] = vector<int>{7, 8};
    ch_list2[128] = vector<int>{7, 8};
    ch_list2[130] = vector<int>{7, 8};
    ch_list2[131] = vector<int>{7, 8};
    ch_list2[132] = vector<int>{7, 8};

    map<int, int> cols;
    cols[127] = 1;
    cols[128] = 2;
    cols[130] = 4;
    cols[131] = 6;
    cols[132] = 8;

    map<int, int> HVs;
    HVs[127] = 800;
    HVs[128] = 820;
    HVs[130] = 840;
    HVs[131] = 860;
    HVs[132] = 880;

    TCanvas *c2 = new TCanvas("c2", "", 1250, 600);
    c2->Divide(2, 1);

    bool first_run = true; // a flag that will be used for d
    int i_run = 0;
    for (map<int, vector<int> >::iterator it = ch_list2.begin(); it != ch_list2.end(); it++) {

        int run = it->first;
        vector<int> channel_list = it->second;
        TFile *cur_file = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

        for (int i_ch = 0; i_ch < channel_list.size(); i_ch++) {

            int cur_ch = channel_list.at(i_ch);

            TH1D *h_cur_hist = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", cur_ch));
            h_cur_hist->SetLineColor(cols[run]);
            h_cur_hist->Scale(10000. / h_cur_hist->GetMaximum());
            c2->cd(i_ch + 1);
            if (first_run) {
                h_cur_hist->DrawCopy("hist");
            } else {
                h_cur_hist->DrawCopy("Same hist");
            }

            lat1->SetTextColor(cols[run]);
            lat1->DrawLatex(0.5, 0.85 - i_run * 0.05, Form("HV: %d V", HVs[run]));
        }

        delete cur_file;
        first_run = false;
        i_run = i_run + 1;
    }

    c2->Print("Figs/HV_dep.eps");
    c2->Print("Figs/HV_dep.pdf");
    c2->Print("Figs/HV_dep.png");


    first_run = true; // a flag that will be used for d
    i_run = 0;
    
    for (map<int, vector<int> >::iterator it = ch_list2.begin(); it != ch_list2.end(); it++) {
        int run = it->first;
        vector<int> channel_list = it->second;
        TFile *cur_file = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

        for (int i_ch = 0; i_ch < channel_list.size(); i_ch++) {
            int cur_ch = channel_list.at(i_ch);

            TH1D *h_cur_hist = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", cur_ch));
            //h_cur_hist->SetAxisRange(4, 1199, "X");
            h_cur_hist->SetLineColor(cols[run]);
            h_cur_hist->Scale(10000. / h_cur_hist->GetMaximum());
            c2->cd(i_ch + 1);
            if (first_run) {
                h_cur_hist->DrawCopy("hist");
            } else {
                h_cur_hist->DrawCopy("Same hist");
            }

            lat1->SetTextColor(cols[run]);
            lat1->DrawLatex(0.5, 0.85 - i_run * 0.05, Form("HV: %d V", HVs[run]));

        }
        delete cur_file;
        first_run = false;
        i_run = i_run + 1;

    }
    c2->Print("Figs/HV_coincid_dep.eps");
    c2->Print("Figs/HV_coincid_dep.pdf");
    c2->Print("Figs/HV_coincid_dep.png");


    c1->Clear();
    c1->Divide(2, 2);
    int irun = 0;
    for (map<int, vector<int> >::iterator it = ch_list2.begin(); it != ch_list2.end(); it++) {

        int run = it->first;

        vector<int> channel_list = it->second;
        TFile *cur_file = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

        c1->cd(irun + 1);

        int cur_ch = channel_list.at(0);
        TH1D *h_cur_hist = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", cur_ch));
        TH1D *h_bgr_close_hist = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", 6));
        TH1D *h_bgr_far_hist = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", 0));

        h_cur_hist->SetLineColor(4);
        h_bgr_close_hist->SetLineColor(2);
        h_bgr_far_hist->SetLineColor(8);
        h_cur_hist->DrawCopy();
        h_bgr_close_hist->DrawCopy("Same");
        h_bgr_far_hist->DrawCopy("Same");

        lat1->SetTextColor(1);
        lat1->DrawLatex(0.5, 0.85, Form("HV: %d V", HVs[run]));
        lat1->SetTextColor(4);
        lat1->DrawLatex(0.5, 0.8, "ch #8");
        lat1->SetTextColor(2);
        lat1->DrawLatex(0.5, 0.75, "ch #7");
        lat1->SetTextColor(8);
        lat1->DrawLatex(0.5, 0.7, "ch #1");

        irun = irun + 1;

        delete cur_file;
    }
    c1->Print("Figs/Dark_curr.eps");
    c1->Print("Figs/Dark_curr.pdf");
    c1->Print("Figs/Dark_curr.png");
    
    
    // Finding Single photoelectron peak
    
    int run_NoAtten = 137;
    int run_Atten1 = 139;
    int ch_probe = 5;
    
    TFile *file_NoAtten = new TFile(Form("../Data/Analyze_Root_%d.root", run_NoAtten));
    TH1D *h_hist_NoAtten = (TH1D*)file_NoAtten->Get(Form("h_signal_coincid1_0_%d", ch_probe));
    h_hist_NoAtten->SetTitle("; ADC");

    TFile *file_Atten1 = new TFile(Form("../Data/Analyze_Root_%d.root", run_Atten1));
    TH1D *h_hist_Atten1 = (TH1D*)file_Atten1->Get(Form("h_signal_coincid1_0_%d", ch_probe));
    h_hist_Atten1->SetTitle("; ADC");
    
    c2->cd(1);
    h_hist_NoAtten->Draw();
    line1->DrawLine(900., 0., 900., 1.02*h_hist_NoAtten->GetMaximum());
    c2->cd(2);
    h_hist_Atten1->Draw();
    line1->DrawLine(100., 0., 90., 1.02*h_hist_Atten1->GetMaximum());
    c2->Print("Figs/Single_pe_peak1_test.eps");
    c2->Print("Figs/Single_pe_peak1_test.pdf");
    c2->Print("Figs/Single_pe_peak1_test.png");
    

    return 0;
}

