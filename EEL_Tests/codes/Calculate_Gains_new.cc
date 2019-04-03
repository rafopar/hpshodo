/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Calculate_Gains.cc
 * Author: rafopar
 *
 * Created on November 4, 2018, 3:14 PM
 */

#include <TF1.h>
#include <TH1D.h>
#include <TH1D.h>
#include <TFile.h>
#include <TStyle.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TMultiGraph.h>
#include <TGraphErrors.h>
#include <map>
#include "../../Rafo_HD/Work/Soft/MyFuncs/FitFuncs.cc"

#include <fstream>
#include <cstdlib>

using namespace std;

/*
 * 
 */
int Calculate_Gains() {

    gStyle->SetOptFit(1);
    TCanvas *c1 = new TCanvas("c1", "", 750, 750);

    TLatex *lat1 = new TLatex();
    lat1->SetTextFont(52);
    lat1->SetTextSize(0.08);
    lat1->SetNDC();    
    
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
    
    double anode_unif_spec[n_pmt_ch] = {87., 72., 72., 76., 79., 81., 83., 73., 81., 84., 87., 79., 100., 87., 89., 91.};
    double PMT_channels[n_pmt_ch] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
    TGraph *gr_Anode_uniformity_datasheet = new TGraph(n_pmt_ch, PMT_channels, anode_unif_spec);
    
    
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


    map<int, double> ch_mean;
    map<int, double> ch_spe;
    map<int, double> ch_spe_err;

    map<int, vector<int> > ch_list_signal;
    map<int, vector<int> > ch_list_sphe;

//    ch_list_signal[155] = vector<int>{8, 9, 10, 11, 12, 13, 14, 15};
//    ch_list_signal[156] = vector<int>{0, 1, 2, 3, 4, 5, 6, 7};
//
//    ch_list_sphe[153] = vector<int>{0, 1, 2, 3, 4, 5, 6, 7};
//    ch_list_sphe[154] = vector<int>{8, 9, 10, 11, 12, 13, 14, 15};

    ch_list_signal[170] = vector<int>{8, 9, 10, 11, 12, 13, 14, 15};
    ch_list_signal[171] = vector<int>{0, 1, 2, 3, 4, 5, 6, 7};

    ch_list_sphe[198] = vector<int>{0, 1, 2, 3, 4, 5, 6, 7};
    ch_list_sphe[199] = vector<int>{8, 9, 10, 11, 12, 13, 14, 15};

    TGraphErrors *gr_mean = new TGraphErrors();

    int gr_mean_index = 0;

    TCanvas *c2 = new TCanvas("c2", "", 750, 750);
    c2->Divide(4, 4, 0., 0.);
    
    for (map<int, vector<int> >::iterator it = ch_list_signal.begin(); it != ch_list_signal.end(); it++) {

        int run = it->first;
        vector<int> channel_list = it->second;

        TFile *cur_file = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

        for (unsigned int i_ch = 0; i_ch < channel_list.size(); i_ch++) {
            int cur_ch = channel_list.at(i_ch);

            TH1D *h_cur_hist_coincid = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", cur_ch));
            TH1D *h_cur_hist_coincid_RB = (TH1D*) h_cur_hist_coincid->RebinX(4);

            double mean = h_cur_hist_coincid->GetMean();
            double mean_err = h_cur_hist_coincid->GetMeanError();

            ch_mean[cur_ch] = mean;
            
            gr_mean->SetPoint(gr_mean_index, cur_ch, mean);
            gr_mean->SetPointError(gr_mean_index, 0., mean_err);
            
            c2->cd(cur_ch + 1)->SetRightMargin(0.01);
            c2->cd(cur_ch + 1)->SetTopMargin(0.01);
            h_cur_hist_coincid_RB->SetStats(0);
            h_cur_hist_coincid_RB->DrawCopy();
            lat1->DrawLatex(0.45, 0.9, Form("mean = %1.1f", mean) );
            
            gr_mean_index = gr_mean_index + 1;
        }

        cur_file->Close();
    }

    c2->Print("Figs/Signals_All_ch.eps");
    c2->Print("Figs/Signals_All_ch.pdf");
    c2->Print("Figs/Signals_All_ch.png");
    
    c1->cd();
    
    gr_mean->SetTitle("; Channel # ; ADC mean");
    gr_mean->GetYaxis()->SetTitleOffset(1.4);
    gr_mean->SetMarkerColor(4);
    gr_mean->SetMarkerStyle(20);
    gr_mean->SetMarkerSize(1.2);
    gr_mean->SetMinimum(0.);
    
    gr_mean->Draw("AP");
    c1->Print("Figs/Mean_val_variations.eps");
    c1->Print("Figs/Mean_val_variations.pdf");
    c1->Print("Figs/Mean_val_variations.png");


    TGraphErrors *gr_Q1 = new TGraphErrors();

    unsigned int gr_Q1_ind = 0;

    ofstream out_SPE("PMT_SPE.dat");

    for (map<int, vector<int> >::iterator it = ch_list_sphe.begin(); it != ch_list_sphe.end(); it++) {

        int run = it->first;
        vector<int> channel_list = it->second;

        TFile *cur_file = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

        for (unsigned int i_ch = 0; i_ch < channel_list.size(); i_ch++) {
            int cur_ch = channel_list.at(i_ch);

            TH1D *h_cur_hist_coincid = (TH1D*) cur_file->Get(Form("h_signal_coincid1_0_%d", cur_ch));

            h_cur_hist_coincid->Fit(f_Sum_Responce, "MeV", "", -200, x_max);

            double Q1 = f_Sum_Responce->GetParameter(4);
            double Q1_Err = f_Sum_Responce->GetParError(4);

            ch_spe[cur_ch] = Q1;
            ch_spe_err[cur_ch] = Q1_Err;
            
            gr_Q1->SetPoint(gr_Q1_ind, cur_ch, Q1);
            gr_Q1->SetPointError(gr_Q1_ind, 0., Q1_Err);

	    out_SPE<<(cur_ch + 1)<<"    "<<Q1<<endl;

            gr_Q1_ind = gr_Q1_ind + 1;
        }


        cur_file->Close();
    }

    c1->Clear();
    
    gr_Q1->SetTitle("; Channel # ; Mean of SPE");
    gr_Q1->GetYaxis()->SetTitleOffset(1.4);
    gr_Q1->SetMarkerColor(4);
    gr_Q1->SetMarkerStyle(20);
    gr_Q1->SetMarkerSize(1.1);
    gr_Q1->SetMinimum(0.);
    gr_Q1->Draw("AP");
    
    c1->Print("Figs/Q1_variations.eps");
    c1->Print("Figs/Q1_variations.pdf");
    c1->Print("Figs/Q1_variations.png");
    
    c1->Clear();
    gr_Anode_uniformity_datasheet->SetMarkerStyle(25);
    gr_Anode_uniformity_datasheet->SetMarkerColor(2);
    gr_Anode_uniformity_datasheet->SetMinimum(0.);
    gr_Anode_uniformity_datasheet->Draw("AP");
    c1->Print("Figs/Gain_data_sheet.eps");
    c1->Print("Figs/Gain_data_sheet.pdf");
    c1->Print("Figs/Gain_data_sheet.png");
    
    c1->Clear();
    TMultiGraph *mtgr_gain_uniformity = new TMultiGraph();
    
    mtgr_gain_uniformity->Add(gr_Q1);
    mtgr_gain_uniformity->Add(gr_Anode_uniformity_datasheet);
    mtgr_gain_uniformity->Draw("AP");
    lat1->SetTextSize(0.035);
    lat1->SetTextColor(2);
    lat1->DrawLatex(0.15, 0.84, Form("EBB = %1.1f V", Ebb_sheet));
    lat1->SetTextColor(4);
    lat1->DrawLatex(0.15, 0.78, Form("HV EEL = %1.1f V", EEL_HV));
    c1->Print("Figs/Compare_gain_uniformity.eps");
    c1->Print("Figs/Compare_gain_uniformity.pdf");
    c1->Print("Figs/Compare_gain_uniformity.png");
    
    
    TGraphErrors *gr_avg_Nphe = new TGraphErrors();
    unsigned int ind_gr_avg_nphe = 0;
    for( map<int, double>::iterator it = ch_mean.begin(); it != ch_mean.end(); it++ ){
        
        int cur_ch = it->first;
        double mean = ch_mean[cur_ch];
        double spe = ch_spe[cur_ch];
        double spe_err = ch_spe_err[cur_ch];
        
        gr_avg_Nphe->SetPoint(ind_gr_avg_nphe, cur_ch, mean/spe);
        gr_avg_Nphe->SetPointError(ind_gr_avg_nphe, 0., (spe_err/spe)*mean/spe);
        
        ind_gr_avg_nphe = ind_gr_avg_nphe + 1;
    }
    
    c1->Clear();
    
    gr_avg_Nphe->SetTitle("; Channel # ; avg NPE");
    gr_avg_Nphe->GetYaxis()->SetTitleOffset(1.4);
    gr_avg_Nphe->SetMarkerColor(4);
    gr_avg_Nphe->SetMarkerStyle(20);
    gr_avg_Nphe->SetMarkerSize(1.1);
    gr_avg_Nphe->SetMinimum(0.);
    gr_avg_Nphe->Draw("AP");
    c1->Print("Figs/Nphe_variations.eps");
    c1->Print("Figs/Nphe_variations.pdf");
    c1->Print("Figs/Nphe_variations.png");
    
    
    double mean_4fibers[n_pmt_ch];
    mean_4fibers[2] = 1587.; // Run 215
    mean_4fibers[5] = 1922.; // Run 215
    
    double boost_4fibers[n_pmt_ch];
    boost_4fibers[2] = mean_4fibers[2]/ch_mean[2];
    boost_4fibers[5] = mean_4fibers[5]/ch_mean[5];
    
    double mean_4fibers_withglue[n_pmt_ch];
    mean_4fibers_withglue[2] = 1418.; // Run 215
    mean_4fibers_withglue[5] = 1412.; // Run 215
    
    double boost_4fibers_withglue[n_pmt_ch];
    boost_4fibers_withglue[2] = mean_4fibers_withglue[2]/ch_mean[2];
    boost_4fibers_withglue[5] = mean_4fibers_withglue[5]/ch_mean[5];
    
    // This is the command to get mean nphe values for single/double and glued/non-glued cases
    // Q1 = 82.5 for PMT ch #13 and 80 for ch#14
    //tr1->Draw("signal_1_[13]/80.>>hh_signal_1(600, 0., 50.)", "peak_1_[6] > 150 && peak_1_[7] > 150 && peak_1_[10] > 150 && peak_1_[11] > 150", "")
    double mean_single_fiber_noglue = 5.405; // Run 225
    double mean_double_fiber_noglue = 8.42;  // Run 225
    double mean_single_fiber_withglue = 6.807; // Run 226
    double mean_double_fiber_withglue = 10.08;  // Run 226
    
    TGraph *gr_single_no_glue = new TGraph();
    gr_single_no_glue->SetPoint(0, 12, mean_single_fiber_noglue);
    gr_single_no_glue->SetMarkerColor(2);
    gr_single_no_glue->SetMarkerSize(2);
    gr_single_no_glue->SetMarkerSize(2);
    gr_single_no_glue->SetMarkerStyle(27);
    TGraph *gr_single_with_glue = new TGraph();
    gr_single_with_glue->SetPoint(0, 12, mean_single_fiber_withglue);
    gr_single_with_glue->SetMarkerColor(2);
    gr_single_with_glue->SetMarkerSize(2);
    gr_single_with_glue->SetMarkerSize(2);
    gr_single_with_glue->SetMarkerStyle(33);
    
    TGraph *gr_double_no_glue = new TGraph();
    gr_double_no_glue->SetPoint(0, 13, mean_double_fiber_noglue);
    gr_double_no_glue->SetMarkerColor(2);
    gr_double_no_glue->SetMarkerSize(2);
    gr_double_no_glue->SetMarkerSize(2);
    gr_double_no_glue->SetMarkerStyle(42);
    TGraph *gr_double_with_glue = new TGraph();
    gr_double_with_glue->SetPoint(0, 13, mean_double_fiber_withglue);
    gr_double_with_glue->SetMarkerColor(2);
    gr_double_with_glue->SetMarkerSize(2);
    gr_double_with_glue->SetMarkerSize(2);
    gr_double_with_glue->SetMarkerStyle(43);
    
    
    
    TGraph *gr_4fibers = new TGraph();
    gr_4fibers->SetPoint(0, 2, mean_4fibers[2]/ch_spe[2]);
    gr_4fibers->SetPoint(1, 5, mean_4fibers[5]/ch_spe[5]);
    gr_4fibers->SetMarkerColor(95);
    gr_4fibers->SetMarkerSize(2);
    gr_4fibers->SetMarkerStyle(29);
   
    TGraph *gr_4fibers_withglue = new TGraph();
    gr_4fibers_withglue->SetPoint(0, 2, mean_4fibers_withglue[2]/ch_spe[2]);
    gr_4fibers_withglue->SetPoint(1, 5, mean_4fibers_withglue[5]/ch_spe[5]);
    gr_4fibers_withglue->SetMarkerColor(95);
    gr_4fibers_withglue->SetMarkerSize(2);
    gr_4fibers_withglue->SetMarkerStyle(29);
   
    
    TMultiGraph *mtgr_nphe_1 = new TMultiGraph();
    mtgr_nphe_1->Add(gr_avg_Nphe);
    mtgr_nphe_1->Add(gr_4fibers);
//    mtgr_nphe_1->Add(gr_single_no_glue);
//    mtgr_nphe_1->Add(gr_single_with_glue);
//    mtgr_nphe_1->Add(gr_double_no_glue);
//    mtgr_nphe_1->Add(gr_double_with_glue);
//    mtgr_nphe_1->Add(gr_4fibers_withglue);
    mtgr_nphe_1->Draw("AP");
    mtgr_nphe_1->SetMinimum(0.);
    mtgr_nphe_1->SetTitle("; Channel # ; avg NPE");
    lat1->DrawLatex(0.27, 0.86, Form("Gain = #times %1.3f", boost_4fibers[2]));
    lat1->DrawLatex(0.42, 0.75, Form("Gain = #times %1.3f", boost_4fibers[5]));
    c1->Print("Figs/Nphe_Gain_with4_fibers.eps");
    c1->Print("Figs/Nphe_Gain_with4_fibers.pdf");
    c1->Print("Figs/Nphe_Gain_with4_fibers.png");
    
    return 0;
}

