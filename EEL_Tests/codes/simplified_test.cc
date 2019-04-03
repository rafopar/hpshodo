#include <cstdlib>
#include <iostream>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TCanvas.h>

using namespace std;

void simplified_test() {
    
    const int n_half = 2; // top and bottom
    const int n_layer = 2; // L1 and L2
    const int n_tile_perlayer = 5; // Five tiles per layer
    const int n_tile_side = 2; // small angle and high angle
    
    string half_names[n_half] = {"Bot", "Top"};
    string Layer_names[n_layer] = {"L1", "L2"};
    string Tile_Indexes[n_tile_perlayer] = {"1", "2", "3", "4", "5"};
    string Tile_side_names[n_tile_side] = {"Edge", "Middle"};
    int hist_col[n_tile_side] = {2, 5};
    

    int run = 320;
    
    TFile *file_in = new TFile(Form("../Data/Check_Assembled_%d.root", run));
    
    TH1D * h_signal_[n_half][n_layer][n_tile_perlayer][n_tile_side];
    
    for (int i_half = 0; i_half < n_half; i_half++) {
        for (int i_layer = 0; i_layer < n_layer; i_layer++) {
            for (int i_ind = 0; i_ind < n_tile_perlayer; i_ind++) {
                for (int i_side = 0; i_side < n_tile_side; i_side++) {
                    
                    string hist_name = "h_signal_"+half_names[i_half]+"_"+Layer_names[i_layer]+"_"+
                          Tile_Indexes[i_ind]+"_"+Tile_side_names[i_side];
                    
                    cout<<"hist name is: "<<hist_name<<endl;
                                        
		    TH1D *h_tmp = (TH1D*)file_in->Get(hist_name.c_str());
                    cout<<"Kuku1"<<endl;
                    cout<<"h_tmp = "<<h_tmp<<endl;
                    h_tmp->SetLineColor(hist_col[i_side]);
                    cout<<"Kuku2"<<endl;
                }
                
            }
        }

    }
    
    
}

