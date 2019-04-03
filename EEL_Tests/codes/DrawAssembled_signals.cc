/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   DrawAssembled_signals.cc
 * Author: rafopar
 *
 * Created on March 31, 2019, 11:20 PM
 */

#include <cstdlib>
#include <iostream>
#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>
#include <TLatex.h>

using namespace std;

/*
 * 
 */
void DrawAssembled_signals() {
    
    const int n_half = 2; // top and bottom
    const int n_layer = 2; // L1 and L2
    const int n_tile_perlayer = 5; // Five tiles per layer
    const int n_tile_side = 2; // small angle and high angle
    
    string half_names[n_half] = {"Bot", "Top"};
    string Layer_names[n_layer] = {"L1", "L2"};
    string Tile_Indexes[n_tile_perlayer] = {"1", "2", "3", "4", "5"};
    string Tile_side_names[n_tile_side] = {"Edge", "Middle"};
    int hist_col[n_tile_side] = {2, 4};
    
    TLatex *lat1 = new TLatex();
    lat1->SetNDC();
    
    int run = 322;
    
    TFile *file_in = new TFile(Form("../Data/Check_Assembled_%d.root", run));
    
    TF1 *f_Pois = new TF1("f_Pois", "[0]*TMath::Poisson([1]*x, [2])", 0., 55.);
    
    TH1D * h_signal_[n_half][n_layer][n_tile_perlayer][n_tile_side];
    TH1D * h_Nphe_[n_half][n_layer][n_tile_perlayer][n_tile_side];
    
    TCanvas *c1 = new TCanvas("c1", "", 1500, 1200);
    c1->Divide(5, 4, 0., 0.);

    TCanvas *c2 = new TCanvas("c2", "", 1500, 1200);
    c2->Divide(5, 4, 0., 0.);

    for (int i_half = 0; i_half < n_half; i_half++) {
        for (int i_layer = 0; i_layer < n_layer; i_layer++) {
            for (int i_ind = 0; i_ind < n_tile_perlayer; i_ind++) {
                for (int i_side = 0; i_side < n_tile_side; i_side++) {
                    
                    string hist_name = "h_signal_"+half_names[i_half]+"_"+Layer_names[i_layer]+"_"+
                          Tile_Indexes[i_ind]+"_"+Tile_side_names[i_side];
                    
                    h_signal_[i_half][i_layer][i_ind][i_side] = (TH1D*)file_in->Get(hist_name.c_str());
                    h_signal_[i_half][i_layer][i_ind][i_side]->SetLineColor(hist_col[i_side]);
      
                    
                    string nphe_hist_name = "h_Nphe_"+half_names[i_half]+"_"+Layer_names[i_layer]+"_"+
                          Tile_Indexes[i_ind]+"_"+Tile_side_names[i_side];
                    
                    h_Nphe_[i_half][i_layer][i_ind][i_side] = (TH1D*)file_in->Get(nphe_hist_name.c_str());
                    h_Nphe_[i_half][i_layer][i_ind][i_side]->SetLineColor(hist_col[i_side]);
                }
                
                c1->cd( i_half*n_layer*n_tile_perlayer + n_tile_perlayer*i_layer + i_ind + 1);
                h_signal_[i_half][i_layer][i_ind][1]->Draw();
                h_signal_[i_half][i_layer][i_ind][0]->Draw("Same");

                double max = h_Nphe_[i_half][i_layer][i_ind][1]->GetMaximum();
                double max_bin_center = h_Nphe_[i_half][i_layer][i_ind][1]->GetBinCenter(h_Nphe_[i_half][i_layer][i_ind][1]->GetMaximumBin());
                //f_Pois->SetParameters(3*max, 0.4, 0.4*max_bin_center);
                
                c2->cd( i_half*n_layer*n_tile_perlayer + n_tile_perlayer*i_layer + i_ind + 1);
                h_Nphe_[i_half][i_layer][i_ind][1]->Draw();
                //h_Nphe_[i_half][i_layer][i_ind][1]->Fit(f_Pois, "MeV", "", 8., 54);
                h_Nphe_[i_half][i_layer][i_ind][0]->Draw("Same");
//                double mu = f_Pois->GetParameter(2);
//                double scale = f_Pois->GetParameter(1);
//                double Height = f_Pois->GetParameter(0);
//                lat1->DrawLatex(0.65, 0.8, Form("H = %1.2f", Height));
//                lat1->DrawLatex(0.65, 0.75, Form("#mu = %1.2f", mu));
            }
        }

    }
    
    c1->Print(Form("Figs/Assembled_Signals_%d.eps", run));
    c1->Print(Form("Figs/Assembled_Signals_%d.pdf", run));
    c1->Print(Form("Figs/Assembled_Signals_%d.png", run));
    
    c2->Print(Form("Figs/Assembled_Nphe_%d.eps", run));
    c2->Print(Form("Figs/Assembled_Nphe_%d.pdf", run));
    c2->Print(Form("Figs/Assembled_Nphe_%d.png", run));
    
}

