#ifndef CHECK_ASSEMBLED_TILES_H
#define CHECK_ASSEMBLED_TILES_H

#define Check_Assembled_tiles_cxx
#include "Check_Assembled_tiles.h"
#include <TF1.h>
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TDBTool.hh>

#include <fstream>

using namespace std;

void Check_Assembled_tiles::Loop() {
    //   In a ROOT session, you can do:
    //      root> .L Check_Assembled_tiles.C
    //      root> Check_Assembled_tiles t
    //      root> t.GetEntry(12); // Fill t data members with entry number 12
    //      root> t.Show();       // Show values of entry 12
    //      root> t.Show(16);     // Read and show values of entry 16
    //      root> t.Loop();       // Loop on all entries
    //

    //     This is the loop skeleton where:
    //    jentry is the global entry number in the chain
    //    ientry is the entry number in the current Tree
    //  Note that the argument to GetEntry must be:
    //    jentry for TChain::GetEntry
    //    ientry for TTree::GetEntry and TBranch::GetEntry
    //
    //       To read only selected branches, Insert statements like:
    // METHOD1:
    //    fChain->SetBranchStatus("*",0);  // disable all branches
    //    fChain->SetBranchStatus("branchname",1);  // activate branchname
    // METHOD2: replace line
    //    fChain->GetEntry(jentry);       //read all branches
    //by  b_branchname->GetEntry(ientry); //read only this branch
    if (fChain == 0) return;

    // ============= Lets initialize the DB =====================
    TString db_Name = "hodoDB";
    TString host = "localhost";
    int port = 3306;
    TString user = "hpshodo";

    TDBTool db(db_Name.Data(), host.Data(), port, user.Data());    
    
    Double_t *ADC2Energy_Top_vals = db.GetEntry_d("ADC2Energy_Top", frun);
    Double_t *ADC2Energy_Bot_vals = db.GetEntry_d("ADC2Energy_Bot", frun);
       
    const int n_half = 2; // top and bottom
    const int n_layer = 2; // L1 and L2
    const int n_tile_perlayer = 5; // Five tiles per layer
    const int n_tile_side = 2; // small angle and high angle
    const int n_PMT_ch = 16;
    
    const double two_hole_scale = 1.25;
    
    
    string half_names[n_half] = {"Top", "Bot"};
    string Layer_names[n_layer] = {"L1", "L2"};
    string Tile_Indexes[n_tile_perlayer] = {"1", "2", "3", "4", "5"};
    string Tile_side_names[n_tile_side] = {"Edge", "Middle"};

    double gain_signal1_[n_PMT_ch];
    double gain_signal2_[n_PMT_ch];
    double Nphe1_[n_PMT_ch];
    double Nphe2_[n_PMT_ch];
    
    double Energy_Top[n_PMT_ch];
    double Energy_Bot[n_PMT_ch];
    
//    ifstream inp_gains_pmt1("Gains_SA3857_910V.dat");
//    ifstream inp_gains_pmt2("Gains_SA2060_985V.dat");
    ifstream inp_gains_pmt1("Gains_SA3980_910V.dat");
    ifstream inp_gains_pmt2("Gains_SA3857_910V.dat");

    int ch;
    double gain;

    //while( !inp_gains_pmt2.eof() ){
    while( inp_gains_pmt2>>ch>>gain_signal2_[ch] ){
    }

    while( inp_gains_pmt1>>ch>>gain_signal1_[ch] ){
    }
    
//    while( !inp_gains_pmt1.eof() ){
//       int ch;
//       double gain;
//       
//       inp_gains_pmt1>>ch;
//       inp_gains_pmt1>>gain;
//       
//       gain_signal1_[ch] = gain;
//    }    
    
    cout<<"Kuku"<<endl;

    TFile *file_out = new TFile(Form("../Data/Check_Assembled_%d.root", frun), "Recreate");
    TH1D * h_signal_[n_half][n_layer][n_tile_perlayer][n_tile_side];
    TH1D * h_Nphe_[n_half][n_layer][n_tile_perlayer][n_tile_side];
    TH1D * h_Edep_[n_half][n_layer][n_tile_perlayer][n_tile_side];


    for (int i_half = 0; i_half < n_half; i_half++) {
        for (int i_layer = 0; i_layer < n_layer; i_layer++) {
            for (int i_ind = 0; i_ind < n_tile_perlayer; i_ind++) {
                for (int i_side = 0; i_side < n_tile_side; i_side++) {
                    h_signal_[i_half][i_layer][i_ind][i_side] = new TH1D(Form("h_signal_%s_%s_%s_%s", 
                            half_names[i_half].c_str(), Layer_names[i_layer].c_str(), Tile_Indexes[i_ind].c_str(), Tile_side_names[i_side].c_str()), "",
                            200, -200, 5500);


                    h_Nphe_[i_half][i_layer][i_ind][i_side] = new TH1D(Form("h_Nphe_%s_%s_%s_%s", 
                            half_names[i_half].c_str(), Layer_names[i_layer].c_str(), Tile_Indexes[i_ind].c_str(), Tile_side_names[i_side].c_str()), "",
                            200, -1., 55.);
                 
                    h_Edep_[i_half][i_layer][i_ind][i_side] = new TH1D( Form("h_Edep_%s_%s_%s_%s",  half_names[i_half].c_str(), Layer_names[i_layer].c_str(), 
                            Tile_Indexes[i_ind].c_str(), Tile_side_names[i_side].c_str()), "", 200, -200., 3000 );
                                        
                }

            }

        }

    }

    
    TH1D *h_ped_[n_half][n_PMT_ch];

    for (int i_half = 0; i_half < n_half; i_half++) {

        for (int i_ch = 0; i_ch < n_PMT_ch; i_ch++) {
            h_ped_[i_half][i_ch] = new TH1D(Form("h_ped_%d_%d", i_half, i_ch), "", 600, 60., 160.);
        }
    }
    
    
    
    Long64_t nentries = fChain->GetEntriesFast();

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        nbytes += nb;
        // if (Cut(ientry) < 0) continue;
        
        for( int i_ch = 0; i_ch < n_PMT_ch; i_ch++ ){
            Nphe1_[i_ch] = signal_1_[i_ch]/gain_signal1_[i_ch];
            Nphe2_[i_ch] = signal_2_[i_ch]/gain_signal2_[i_ch];
            
            Energy_Top[i_ch] = signal_2_[i_ch]*ADC2Energy_Top_vals[i_ch];
            Energy_Bot[i_ch] = signal_1_[i_ch]*ADC2Energy_Bot_vals[i_ch];
        }
        
        
        bool top_edge_cosmic = signal_3_[1] > 200. && signal_3_[0] > 200. && signal_3_[8] > 200. && signal_3_[9] > 200.;
        bool top_middle_cosmic = (signal_3_[12] + signal_3_[13] > 500) && (signal_3_[4] + signal_3_[5] > 500);
        
        bool bot_edge_cosmic = signal_3_[2] > 200. && signal_3_[3] > 200 && signal_3_[10] > 200.;
        bool bot_middle_cosmic = signal_3_[6] + signal_3_[7] > 450  && signal_3_[14] > 200.;
                
        // ================================  L1 Top tiles =======================================
        
        // L1 -1
        //if( signal_2_[8] > 700. && signal_2_[4] < 200. && signal_2_[5] < 200. && (signal_2_[10] < 250 && signal_2_[11] < 250.) ){
        if( signal_2_[8] > 700. && signal_2_[4] < 80. && signal_2_[5] < 80. && (signal_2_[10] < 80 && signal_2_[11] < 80.) ){
            if( top_edge_cosmic ){
                h_signal_[0][0][0][0]->Fill(signal_2_[7]);
                h_Nphe_[0][0][0][0]->Fill(Nphe2_[7]);
                h_Edep_[0][0][0][0]->Fill(Energy_Top[7]);
            }
            if( top_middle_cosmic ){
                h_signal_[0][0][0][1]->Fill(signal_2_[7]);
                h_Nphe_[0][0][0][1]->Fill(Nphe2_[7]);
                h_Edep_[0][0][0][1]->Fill(Energy_Top[7]);
            }
        }
        
        // L1-2
        //if( ((signal_2_[10] + signal_2_[11] > 1000.) || signal_2_[8] > 700. ) && signal_2_[7] < 350. && signal_2_[3] < 350. && signal_2_[2] < 350. ){
        if( ((signal_2_[10] + signal_2_[11] > 1000.) || signal_2_[8] > 700. ) && signal_2_[7] < 80. && signal_2_[3] < 80. && signal_2_[2] < 80. ){
            
            if( top_edge_cosmic ){
                h_signal_[0][0][1][0]->Fill(signal_2_[4] + signal_2_[5]);
                h_Nphe_[0][0][1][0]->Fill(Nphe2_[4] + Nphe2_[5]);
                h_Edep_[0][0][1][0]->Fill(two_hole_scale*(Energy_Top[4] + Energy_Top[5])/2.);
            }
            
            if( top_middle_cosmic ){
                h_signal_[0][0][1][1]->Fill(signal_2_[4] + signal_2_[5]);
                h_Nphe_[0][0][1][1]->Fill(Nphe2_[4] + Nphe2_[5]);
                h_Edep_[0][0][1][1]->Fill(two_hole_scale*(Energy_Top[4] + Energy_Top[5])/2.);
            }
            
        }
        
        // L1-3
        //if( ((signal_2_[10] + signal_2_[11] > 1000.) || signal_2_[12] + signal_2_[13] > 600 ) && signal_2_[4] < 250. && signal_2_[5] < 250. 
        if( ((signal_2_[10] + signal_2_[11] > 1000.) || signal_2_[12] + signal_2_[13] > 600 ) && signal_2_[4] < 80. && signal_2_[5] < 80. 
                && signal_2_[0] < 80. && signal_2_[1] < 80. ){
            
            if( top_edge_cosmic ){
                h_signal_[0][0][2][0]->Fill(signal_2_[2] + signal_2_[3]);
                h_Nphe_[0][0][2][0]->Fill(Nphe2_[2] + Nphe2_[3]);
                h_Edep_[0][0][2][0]->Fill(two_hole_scale*(Energy_Top[2] + Energy_Top[3])/2.);
            }
            if( top_middle_cosmic ){
                h_signal_[0][0][2][1]->Fill(signal_2_[2] + signal_2_[3]);
                h_Nphe_[0][0][2][1]->Fill(Nphe2_[2] + Nphe2_[3]);
                h_Edep_[0][0][2][1]->Fill(two_hole_scale*(Energy_Top[2] + Energy_Top[3])/2.);
            }
            
        }
        
        // L1-4
        //if( ((signal_2_[14] + signal_2_[15] > 800.) || signal_2_[12] + signal_2_[13] > 600 ) && signal_2_[2] < 250. && signal_2_[3] < 250. && 
        if( ((signal_2_[14] + signal_2_[15] > 800.) || signal_2_[12] + signal_2_[13] > 600 ) && signal_2_[2] < 80. && signal_2_[3] < 80. && 
             signal_2_[6] < 80. ){
            
            if( top_edge_cosmic ){
                h_signal_[0][0][3][0]->Fill(signal_2_[0] + signal_2_[1]);
                h_Nphe_[0][0][3][0]->Fill(Nphe2_[0] + Nphe2_[1]);
                h_Edep_[0][0][3][0]->Fill(two_hole_scale*(Energy_Top[0] + Energy_Top[1])/2.);
            }
            if( top_middle_cosmic ){
                h_signal_[0][0][3][1]->Fill(signal_2_[0] + signal_2_[1]);
                h_Nphe_[0][0][3][1]->Fill(Nphe2_[0] + Nphe2_[1]);
                h_Edep_[0][0][3][1]->Fill(two_hole_scale*(Energy_Top[0] + Energy_Top[1])/2. );
            }
        
        }
        
        // L1-5
        //if( ((signal_2_[14] + signal_2_[15] > 800.) || signal_2_[9] > 600 ) && signal_2_[0] < 250. && signal_2_[1] < 250. ){
        if( ((signal_2_[14] + signal_2_[15] > 800.) || signal_2_[9] > 600 ) && signal_2_[0] < 80. && signal_2_[1] < 80. ){
            
            if( top_edge_cosmic ){
                h_signal_[0][0][4][0]->Fill(signal_2_[6]);
                h_Nphe_[0][0][4][0]->Fill(Nphe2_[6]);
                h_Edep_[0][0][4][0]->Fill(Energy_Top[6]);
            }
            if( top_middle_cosmic ){
                h_signal_[0][0][4][1]->Fill(signal_2_[6]);
                h_Nphe_[0][0][4][1]->Fill(Nphe2_[6]);
                h_Edep_[0][0][4][1]->Fill(Energy_Top[6]);
            }
        }
        
        
         // ================================  L2 Top tiles =======================================
        
        // L2-1
        //if( ((signal_2_[4] + signal_2_[5] > 800.) || signal_2_[7] > 800 ) && signal_2_[10] < 250. && signal_2_[11] < 250. ){
        if( ((signal_2_[4] + signal_2_[5] > 800.) || signal_2_[7] > 800 ) && signal_2_[10] < 80. && signal_2_[11] < 80. ){
            
            if( top_edge_cosmic ){
                h_signal_[0][1][0][0]->Fill(signal_2_[8]);
                h_Nphe_[0][1][0][0]->Fill(Nphe2_[8]);
                h_Edep_[0][1][0][0]->Fill(Energy_Top[8]);
            }
            if( top_middle_cosmic ){
                h_signal_[0][1][0][1]->Fill(signal_2_[8]);
                h_Nphe_[0][1][0][1]->Fill(Nphe2_[8]);
                h_Edep_[0][1][0][1]->Fill(Energy_Top[8]);
            }
            
        }
        
        // L2-2
//        if( ((signal_2_[4] + signal_2_[5] > 800.) || (signal_2_[2] + signal_2_[3]) > 1000 ) && signal_2_[8] < 250. 
//                && signal_2_[12] < 250. && signal_2_[13] < 250. ){
        if( ((signal_2_[4] + signal_2_[5] > 800.) || (signal_2_[2] + signal_2_[3]) > 1000 ) && signal_2_[8] < 80. 
                && signal_2_[12] < 80. && signal_2_[13] < 80. ){
        
            if( top_edge_cosmic ){
                h_signal_[0][1][1][0]->Fill(signal_2_[10] + signal_2_[11]);
                h_Nphe_[0][1][1][0]->Fill(Nphe2_[10] + Nphe2_[11]);
                h_Edep_[0][1][1][0]->Fill(two_hole_scale*(Energy_Top[10] + Energy_Top[11])/2.);
            }
            if( top_middle_cosmic ){
                h_signal_[0][1][1][1]->Fill(signal_2_[10] + signal_2_[11]);
                h_Nphe_[0][1][1][1]->Fill(Nphe2_[10] + Nphe2_[11]);
                h_Edep_[0][1][1][1]->Fill(two_hole_scale*(Energy_Top[10] + Energy_Top[11])/2.);
            }
            
        }
       
        
        // L2-3
//        if( ((signal_2_[0] + signal_2_[1] > 600.) || (signal_2_[2] + signal_2_[3]) > 1000 ) && signal_2_[10] < 250.  && signal_2_[11] < 250. 
//                && signal_2_[14] < 250. && signal_2_[15] < 250. ){
        if( ((signal_2_[0] + signal_2_[1] > 600.) || (signal_2_[2] + signal_2_[3]) > 1000 ) && signal_2_[10] < 80.  && signal_2_[11] < 80. 
                && signal_2_[14] < 80. && signal_2_[15] < 80. ){
        
            if( top_edge_cosmic ){
                h_signal_[0][1][2][0]->Fill(signal_2_[12] + signal_2_[13]);
                h_Nphe_[0][1][2][0]->Fill(Nphe2_[12] + Nphe2_[13]);
                h_Edep_[0][1][2][0]->Fill(two_hole_scale*(Energy_Top[12] + Energy_Top[13])/2.);
            }
            if( top_middle_cosmic ){
                h_signal_[0][1][2][1]->Fill(signal_2_[12] + signal_2_[13]);
                h_Nphe_[0][1][2][1]->Fill(Nphe2_[12] + Nphe2_[13]);
                h_Edep_[0][1][2][1]->Fill(two_hole_scale*(Energy_Top[12] + Energy_Top[13])/2.);
            }
            
        }
        
        // L2-4
//        if( ( (signal_2_[0] + signal_2_[1] > 600.) || signal_2_[6] > 1000 ) && signal_2_[9] < 250.  && signal_2_[12] < 250. 
//                && signal_2_[13] < 250.){
        if( ( (signal_2_[0] + signal_2_[1] > 600.) || signal_2_[6] > 1000 ) && signal_2_[9] < 80.  && signal_2_[12] < 80. 
                && signal_2_[13] < 80.){
        
            if( top_edge_cosmic ){
                h_signal_[0][1][3][0]->Fill(signal_2_[14] + signal_2_[15]);
                h_Nphe_[0][1][3][0]->Fill(Nphe2_[14] + Nphe2_[15]);
                h_Edep_[0][1][3][0]->Fill(two_hole_scale*(Energy_Top[14] + Energy_Top[15])/2.);
            }
            if( top_middle_cosmic ){
                h_signal_[0][1][3][1]->Fill(signal_2_[14] + signal_2_[15]);
                h_Nphe_[0][1][3][1]->Fill(Nphe2_[14] + Nphe2_[15]);
                h_Edep_[0][1][3][1]->Fill(two_hole_scale*(Energy_Top[14] + Energy_Top[15])/2.);
            }
            
        }
        
        
        // L2-5 
//        if(  (signal_2_[6] > 1000 ) && signal_2_[14] < 250. && signal_2_[15] < 250. ){
        if(  (signal_2_[6] > 1000 ) && signal_2_[14] < 80. && signal_2_[15] < 80. ){
            
            if( top_edge_cosmic ){
                h_signal_[0][1][4][0]->Fill(signal_2_[9]);
                h_Nphe_[0][1][4][0]->Fill(Nphe2_[9]);
                h_Edep_[0][1][4][0]->Fill(Energy_Top[9]);
            }
            if( top_middle_cosmic ){
                h_signal_[0][1][4][1]->Fill(signal_2_[9]);
                h_Nphe_[0][1][4][1]->Fill(Nphe2_[9]);
                h_Edep_[0][1][4][1]->Fill(Energy_Top[9]);
            }
        }
        
        
        
        // ================================  L1 Bot tiles =======================================
        
        // L1 -1
        //if( signal_1_[8] > 600. && signal_1_[4] < 200. && signal_1_[5] < 200. && signal_1_[10] < 200 && signal_1_[11] < 200. ){
        if( signal_1_[8] > 600. && signal_1_[4] < 80. && signal_1_[5] < 80. && signal_1_[10] < 80 && signal_1_[11] < 80. ){
            if( bot_edge_cosmic ){
                h_signal_[1][0][0][0]->Fill(signal_1_[7]);
                h_Nphe_[1][0][0][0]->Fill(Nphe1_[7]);
                h_Edep_[1][0][0][0]->Fill(Energy_Bot[7]);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][0][0][1]->Fill(signal_1_[7]);
                h_Nphe_[1][0][0][1]->Fill(Nphe1_[7]);
                h_Edep_[1][0][0][1]->Fill(Energy_Bot[7]);
            }
        }
 
        
        // L1-2
        //if( ((signal_1_[10] + signal_1_[11] > 600.) || signal_1_[8] > 600. ) && signal_1_[7] < 200. && signal_1_[3] < 200. && signal_1_[2] < 200. ){
        if( ((signal_1_[10] + signal_1_[11] > 700.) || signal_1_[8] > 600. ) && signal_1_[7] < 80. && signal_1_[3] < 80. && signal_1_[2] < 80. ){
            
            if( bot_edge_cosmic ){
                h_signal_[1][0][1][0]->Fill(signal_1_[4] + signal_1_[5]);
                h_Nphe_[1][0][1][0]->Fill(Nphe1_[4] + Nphe1_[5]);
                h_Edep_[1][0][1][0]->Fill(two_hole_scale*(Energy_Bot[4] + Energy_Bot[5])/2.);
            }
            
            if( bot_middle_cosmic ){
                h_signal_[1][0][1][1]->Fill(signal_1_[4] + signal_1_[5]);
                h_Nphe_[1][0][1][1]->Fill(Nphe1_[4] + Nphe1_[5]);
                h_Edep_[1][0][1][1]->Fill(two_hole_scale*(Energy_Bot[4] + Energy_Bot[5])/2.);
            }
            
        }
        
        
        // L1-3
//        if( ((signal_1_[10] + signal_1_[11] > 600.) || signal_1_[12] + signal_1_[13] > 600 ) && signal_1_[4] < 250. && signal_1_[5] < 250. 
//                && signal_1_[0] < 250. && signal_1_[1] < 250. ){
        if( ((signal_1_[10] + signal_1_[11] > 700.) || signal_1_[12] + signal_1_[13] > 700 ) && signal_1_[4] < 80. && signal_1_[5] < 80. 
                && signal_1_[0] < 80. && signal_1_[1] < 80. ){
            
            if( bot_edge_cosmic ){
                h_signal_[1][0][2][0]->Fill(signal_1_[2] + signal_1_[3]);
                h_Nphe_[1][0][2][0]->Fill(Nphe1_[2] + Nphe1_[3]);
                h_Edep_[1][0][2][0]->Fill(two_hole_scale*(Energy_Bot[2] + Energy_Bot[3])/2.);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][0][2][1]->Fill(signal_1_[2] + signal_1_[3]);
                h_Nphe_[1][0][2][1]->Fill(Nphe1_[2] + Nphe1_[3]);
                h_Edep_[1][0][2][1]->Fill(two_hole_scale*(Energy_Bot[2] + Energy_Bot[3])/2.);
            }
            
        }
        
        
        // L1-4
//        if( ((signal_1_[14] + signal_1_[15] > 600.) || signal_1_[12] + signal_1_[13] > 600 ) && signal_1_[2] < 250. && signal_1_[3] < 250. && 
//             signal_1_[6] < 250. && signal_1_[9] < 250. ){
        if( ((signal_1_[14] + signal_1_[15] > 700.) || signal_1_[12] + signal_1_[13] > 700 ) && signal_1_[2] < 80. && signal_1_[3] < 80. && 
             signal_1_[6] < 80. && signal_1_[9] < 80. ){
            
            if( bot_edge_cosmic ){
                h_signal_[1][0][3][0]->Fill(signal_1_[0] + signal_1_[1]);
                h_Nphe_[1][0][3][0]->Fill(Nphe1_[0] + Nphe1_[1]);
                h_Edep_[1][0][3][0]->Fill(two_hole_scale*(Energy_Bot[0] + Energy_Bot[1])/2.);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][0][3][1]->Fill(signal_1_[0] + signal_1_[1]);
                h_Nphe_[1][0][3][1]->Fill(Nphe1_[0] + Nphe1_[1]);
                h_Edep_[1][0][3][1]->Fill(two_hole_scale*(Energy_Bot[0] + Energy_Bot[1])/2.);
            }
        
        }
        
        // L1-5
        //if( ((signal_1_[14] + signal_1_[15] > 600.) || signal_1_[9] > 500 ) && signal_1_[0] < 250. && signal_1_[1] < 250. ){
        if( ((signal_1_[14] + signal_1_[15] > 700.) || signal_1_[9] > 500 ) && signal_1_[0] < 80. && signal_1_[1] < 80. ){
            
            if( bot_edge_cosmic ){
                h_signal_[1][0][4][0]->Fill(signal_1_[6]);
                h_Nphe_[1][0][4][0]->Fill(Nphe1_[6]);
                h_Edep_[1][0][4][0]->Fill(Energy_Bot[6]);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][0][4][1]->Fill(signal_1_[6]);
                h_Nphe_[1][0][4][1]->Fill(Nphe1_[6]);
                h_Edep_[1][0][4][1]->Fill(Energy_Bot[6]);
            }
        }
        
        
    // ================================  L2 Bot tiles =======================================
        
        // L2-1
        //if( ((signal_1_[4] + signal_1_[5] > 500.) || signal_1_[7] > 300 ) && signal_1_[10] < 250. && signal_1_[11] < 250. ){
        if( ((signal_1_[4] + signal_1_[5] > 600.) || signal_1_[7] > 500 ) && signal_1_[10] < 80. && signal_1_[11] < 80. ){
            
            if( bot_edge_cosmic ){
                h_signal_[1][1][0][0]->Fill(signal_1_[8]);
                h_Nphe_[1][1][0][0]->Fill(Nphe1_[8]);
                h_Edep_[1][1][0][0]->Fill(Energy_Bot[8]);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][1][0][1]->Fill(signal_1_[8]);
                h_Nphe_[1][1][0][1]->Fill(Nphe1_[8]);
                h_Edep_[1][1][0][1]->Fill(Energy_Bot[8]);
            }
            
        }
        
        
        // L2-2
        //if( ((signal_1_[4] + signal_1_[5] > 500.) || (signal_1_[2] + signal_1_[3]) > 400 ) && signal_1_[8] < 250. && signal_1_[12] < 250. && signal_1_[13] < 250. ){
        if( ((signal_1_[4] + signal_1_[5] > 650.) || (signal_1_[2] + signal_1_[3]) > 600 ) && signal_1_[8] < 80. && signal_1_[12] < 80. && signal_1_[13] < 80. ){
        
            if( bot_edge_cosmic ){
                h_signal_[1][1][1][0]->Fill(signal_1_[10] + signal_1_[11]);
                h_Nphe_[1][1][1][0]->Fill(Nphe1_[10] + Nphe1_[11]);
                h_Edep_[1][1][1][0]->Fill(two_hole_scale*(Energy_Bot[10] + Energy_Bot[11])/2.);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][1][1][1]->Fill(signal_1_[10] + signal_1_[11]);
                h_Nphe_[1][1][1][1]->Fill(Nphe1_[10] + Nphe1_[11]);
                h_Edep_[1][1][1][1]->Fill(two_hole_scale*(Energy_Bot[10] + Energy_Bot[11])/2.);
            }
            
        }
        
        
         // L2-3
//        if( ((signal_1_[0] + signal_1_[1] > 600.) || (signal_1_[2] + signal_1_[3]) > 400 ) && signal_1_[10] < 250.  && signal_1_[11] < 250. 
//                && signal_1_[14] < 250. && signal_1_[15] < 250. ){
        if( ((signal_1_[0] + signal_1_[1] > 600.) || (signal_1_[2] + signal_1_[3]) > 400 ) && signal_1_[10] < 80.  && signal_1_[11] < 80. 
                && signal_1_[14] < 80. && signal_1_[15] < 80. ){
                    
            if( bot_edge_cosmic ){
                h_signal_[1][1][2][0]->Fill(signal_1_[12] + signal_1_[13]);
                h_Nphe_[1][1][2][0]->Fill(Nphe1_[12] + Nphe1_[13]);
                h_Edep_[1][1][2][0]->Fill(two_hole_scale*(Energy_Bot[12] + Energy_Bot[13])/2.);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][1][2][1]->Fill(signal_1_[12] + signal_1_[13]);
                h_Nphe_[1][1][2][1]->Fill(Nphe1_[12] + Nphe1_[13]);
                h_Edep_[1][1][2][1]->Fill(two_hole_scale*(Energy_Bot[12] + Energy_Bot[13])/2.);
            }
            
        }
       
        
        // L2-4
//        if( ( (signal_1_[0] + signal_1_[1] > 600.) || signal_1_[6] > 300 ) && signal_1_[9] < 250.  && signal_1_[12] < 250. && signal_1_[13] < 250.){
        if( ( (signal_1_[0] + signal_1_[1] > 600.) || signal_1_[6] > 500 ) && signal_1_[9] < 80. && signal_1_[12] < 80. && signal_1_[13] < 80.){
        
            if( bot_edge_cosmic ){
                h_signal_[1][1][3][0]->Fill(signal_1_[14] + signal_1_[15]);
                h_Nphe_[1][1][3][0]->Fill(Nphe1_[14] + Nphe1_[15]);
                h_Edep_[1][1][3][0]->Fill(two_hole_scale*(Energy_Bot[14] + Energy_Bot[15])/2.);
            }
            if( bot_middle_cosmic ){
                h_signal_[1][1][3][1]->Fill(signal_1_[14] + signal_1_[15]);
                h_Nphe_[1][1][3][1]->Fill(Nphe1_[14] + Nphe1_[15]);
                h_Edep_[1][1][3][1]->Fill(two_hole_scale*(Energy_Bot[14] + Energy_Bot[15])/2.);
            }
            
        }
        
        // L2-5 
//        if(  (signal_1_[6] > 300 ) && signal_1_[14] < 250. && signal_1_[15] < 250. ){
        if(  (signal_1_[6] > 400 ) && signal_1_[14] < 80. && signal_1_[15] < 80. ){
            
            if( bot_edge_cosmic ){
                h_signal_[1][1][4][0]->Fill(signal_1_[9]);
                h_Nphe_[1][1][4][0]->Fill(Nphe1_[9]);
                h_Edep_[1][1][4][0]->Fill(Energy_Bot[9]);
            }
            if (bot_middle_cosmic) {
                h_signal_[1][1][4][1]->Fill(signal_1_[9]);
                h_Nphe_[1][1][4][1]->Fill(Nphe1_[9]);
                h_Edep_[1][1][4][1]->Fill(Energy_Bot[9]);
            }
        }


        //= ================================== PEDESTALS ==================================

        for (int i_half = 0; i_half < n_half; i_half++) {
            for (int i_ch = 0; i_ch < n_PMT_ch; i_ch++) {
                h_ped_[0][i_ch]->Fill(pedestal_1_[i_ch]);
                h_ped_[1][i_ch]->Fill(pedestal_2_[i_ch]);
            }
        }
    }      


    TF1 *f_Gaus = new TF1("f_Gaus", "[0]*TMath::Gaus(x, [1], [2])", 50., 250);

    string top_bot_names[2] = {"Bot", "Top"};

    ofstream out_ped("Hodo_Pedestals.dat");

    
    for (int i_half = 0; i_half < n_half; i_half++) {
        for (int i_ch = 0; i_ch < n_PMT_ch; i_ch++) {
            
            double max = h_ped_[i_half][i_ch]->GetMaximum();
            double mean = h_ped_[i_half][i_ch]->GetBinCenter(h_ped_[i_half][i_ch]->GetMaximumBin());
            double sigm = h_ped_[i_half][i_ch]->GetRMS(1);
            f_Gaus->SetParameters(max, mean, sigm);
            h_ped_[i_half][i_ch]->Fit(f_Gaus, "MeV", "", mean - 5*sigm, mean + 5*sigm);
            
            out_ped<<setw(3)<<top_bot_names[i_half]<<setw(6)<<i_ch<<setw(12)<<f_Gaus->GetParameter(1)<<setw(12)<<f_Gaus->GetParameter(2)<<endl;
                        
        }
    }
 
    
    gDirectory->Write();
}

#endif	// CHECK_ASSEMBLED_TILES_H

