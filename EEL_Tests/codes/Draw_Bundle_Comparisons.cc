/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Draw_Bundle_Comparisons.cc
 * Author: rafopar
 *
 * Created on March 5, 2019, 8:50 PM
 */

#include <map>
#include <cstdlib>
#include <TFile.h>
#include <TH1D.h>
#include <TLatex.h>
#include <TCanvas.h>

using namespace std;

/*
 * 
 */
void Draw_Bundle_Comparisons() {

    const int n_Bundle = 3;
    
    const int n_runs = 6;
    const int n_pair = 8;
    
    string runs_[n_runs] = {"273", "274", "276_277_280", "278_279", "281", "282"};
    
    TLatex *lat1 = new TLatex();
    lat1->SetNDC();
    
    int cols_[n_Bundle] = {1, 2, 4};
    
    map< pair<int, int>, string > bundle_tile_run_map;
    map<string, TFile*> file_in;
    map< pair<int, int>, TH1D* > h_nphe;
    map< pair<int, int>, TH1D* > h_signal;
    map< pair<int, int>, TH1D* > h_nphe_individual;
    
    for( int i_run = 0; i_run < n_runs; i_run++ ){
        file_in[runs_[i_run]] = new TFile(Form("../Data/Check_FiberBundles_%s.root", runs_[i_run].c_str()));
    }
    
    
    bundle_tile_run_map[make_pair(0, 0)] = "282";
    bundle_tile_run_map[make_pair(0, 1)] = "281";
    bundle_tile_run_map[make_pair(0, 2)] = "281";
    bundle_tile_run_map[make_pair(0, 3)] = "281";
    bundle_tile_run_map[make_pair(0, 4)] = "281";
    bundle_tile_run_map[make_pair(0, 5)] = "281";
    bundle_tile_run_map[make_pair(0, 6)] = "281";
    bundle_tile_run_map[make_pair(0, 7)] = "282";

    bundle_tile_run_map[make_pair(1, 0)] = "274";
    bundle_tile_run_map[make_pair(1, 1)] = "273";
    bundle_tile_run_map[make_pair(1, 2)] = "273";
    bundle_tile_run_map[make_pair(1, 3)] = "273";
    bundle_tile_run_map[make_pair(1, 4)] = "273";
    bundle_tile_run_map[make_pair(1, 5)] = "273";
    bundle_tile_run_map[make_pair(1, 6)] = "273";
    bundle_tile_run_map[make_pair(1, 7)] = "274";

    bundle_tile_run_map[make_pair(2, 0)] = "278_279";
    bundle_tile_run_map[make_pair(2, 1)] = "276_277_280";
    bundle_tile_run_map[make_pair(2, 2)] = "276_277_280";
    bundle_tile_run_map[make_pair(2, 3)] = "276_277_280";
    bundle_tile_run_map[make_pair(2, 4)] = "276_277_280";
    bundle_tile_run_map[make_pair(2, 5)] = "276_277_280";
    bundle_tile_run_map[make_pair(2, 6)] = "276_277_280";
    bundle_tile_run_map[make_pair(2, 7)] = "278_279";
    
    TCanvas *c1 = new TCanvas("c1", "", 750, 750);
    c1->Divide(3, 3, 0., 0.);

    TCanvas *c2 = new TCanvas("c2", "", 750, 750);
    c2->Divide(3, 3, 0., 0.);

    TCanvas *c3 = new TCanvas("c3", "", 1050, 1050);
    c3->Divide(4, 4, 0., 0.);
    
    for( int i_bundle = 0; i_bundle < n_Bundle; i_bundle++ ){
        
        for( int i_pair = 0; i_pair < n_pair; i_pair++ ){
            
            pair<int, int> cur_pair = make_pair(i_bundle, i_pair);
            string run = bundle_tile_run_map[cur_pair];
            
            int ch1 = 2*i_pair + 1;
            int ch2 = 2*i_pair + 2;
            
            h_nphe[cur_pair] = (TH1D*)file_in[run]->Get(Form("h_ch_%d_%d_nphe1", ch1, ch2));
            h_nphe[cur_pair]->SetLineColor(cols_[i_bundle]);

            h_signal[cur_pair] = (TH1D*)file_in[run]->Get(Form("h_ch_%d_%d_signal1", ch1, ch2));
            h_signal[cur_pair]->SetLineColor(cols_[i_bundle]);

            pair<int, int> pair_bundl_ch1 = make_pair(i_bundle, ch1);
            pair<int, int> pair_bundl_ch2 = make_pair(i_bundle, ch2);
            
            //h_nphe_individual 
            h_nphe_individual[pair_bundl_ch1] = (TH1D*)file_in[run]->Get(Form("h_nphe_individual_%d", ch1));
            h_nphe_individual[pair_bundl_ch1]->SetLineColor(cols_[i_bundle]);
            h_nphe_individual[pair_bundl_ch1]->SetTitle("; # of photoelectrons");
            h_nphe_individual[pair_bundl_ch2] = (TH1D*)file_in[run]->Get(Form("h_nphe_individual_%d", ch2));
            h_nphe_individual[pair_bundl_ch2]->SetLineColor(cols_[i_bundle]);
            h_nphe_individual[pair_bundl_ch2]->SetTitle("; # of photoelectrons");
            
            TH1D *h_rB = (TH1D*)h_nphe[cur_pair]->Rebin(4, "Rebinned");
            h_rB->Scale(100./h_rB->Integral());
            h_rB->SetMaximum(1.3*h_rB->GetMaximum());
    
            TH1D *h_rB_signal = (TH1D*)h_signal[cur_pair]->Rebin(4);
            h_rB_signal->Scale(100./h_rB_signal->Integral());
            h_rB_signal->SetMaximum(1.3*h_rB_signal->GetMaximum());
            
            
            TH1D *h_rB_nphe_individual_ch1 = (TH1D*)h_nphe_individual[pair_bundl_ch1]->Rebin(4);
            TH1D *h_rB_nphe_individual_ch2 = (TH1D*)h_nphe_individual[pair_bundl_ch2]->Rebin(4);
            
            h_rB_nphe_individual_ch1->Scale(100./h_rB_nphe_individual_ch1->Integral());
            h_rB_nphe_individual_ch2->Scale(100./h_rB_nphe_individual_ch2->Integral());
            h_rB_nphe_individual_ch1->SetMaximum(1.3*h_rB_nphe_individual_ch1->GetMaximum());
            h_rB_nphe_individual_ch2->SetMaximum(1.3*h_rB_nphe_individual_ch2->GetMaximum());
            
            if( i_bundle == 0 ){
                c1->cd(i_pair + 1);
                h_rB->DrawCopy("hist");
                lat1->DrawLatex(0.65, 0.7, Form("ch%d + ch%d", ch1, ch2));
                
                c2->cd(i_pair + 1);
                h_rB_signal->Draw("hist");
                lat1->DrawLatex(0.65, 0.7, Form("ch%d + ch%d", ch1, ch2));
                
                c3->cd(ch1);
                h_rB_nphe_individual_ch1->DrawCopy("hist");
                lat1->DrawLatex(0.65, 0.7, Form("ch%d ", ch1));
                c3->cd(ch2);
                h_rB_nphe_individual_ch2->DrawCopy("hist");
                lat1->DrawLatex(0.65, 0.7, Form("ch%d ", ch2));
                
            }else{
                c1->cd(i_pair + 1);
                h_rB->DrawCopy("hist same");
                c2->cd(i_pair + 1);
                h_rB_signal->Draw("hist same");

                c3->cd(ch1);
                h_rB_nphe_individual_ch1->DrawCopy("hist same");
                c3->cd(ch2);
                h_rB_nphe_individual_ch2->DrawCopy("hist same");
            }
            
            
            
        }
        
    }
    
    c1->cd(9);
    lat1->SetTextColor(1);
    lat1->DrawLatex(0.15, 0.91, "Distribution of # of photoelectrons");
    lat1->SetTextColor(cols_[0]);
    lat1->DrawLatex(0.15, 0.8, "Bundle #1");
    lat1->SetTextColor(cols_[1]);
    lat1->DrawLatex(0.15, 0.7, "Bundle #2");
    lat1->SetTextColor(cols_[2]);
    lat1->DrawLatex(0.15, 0.6, "Bundle #3");
    c1->Print("Figs/Fiber_Bandles_compare_Nphe.eps");
    c1->Print("Figs/Fiber_Bandles_compare_Nphe.pdf");
    c1->Print("Figs/Fiber_Bandles_compare_Nphe.png");
    
    c2->cd(9);
    lat1->SetTextColor(1);
    lat1->DrawLatex(0.15, 0.91, "Distribution of # ADC");
    lat1->SetTextColor(cols_[0]);
    lat1->DrawLatex(0.15, 0.8, "Bundle #1");
    lat1->SetTextColor(cols_[1]);
    lat1->DrawLatex(0.15, 0.7, "Bundle #2");
    lat1->SetTextColor(cols_[2]);
    lat1->DrawLatex(0.15, 0.6, "Bundle #3");
    c2->Print("Figs/Fiber_Bandles_compare_ADC.eps");
    c2->Print("Figs/Fiber_Bandles_compare_ADC.pdf");
    c2->Print("Figs/Fiber_Bandles_compare_ADC.png");
 
    c3->Print("Figs/Fiber_Bundles_Individual_ch_compare.eps");
    c3->Print("Figs/Fiber_Bundles_Individual_ch_compare.pdf");
    c3->Print("Figs/Fiber_Bundles_Individual_ch_compare.png");
}

