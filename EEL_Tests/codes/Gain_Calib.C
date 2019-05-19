#ifndef GAIN_CALIB_H
#define GAIN_CALIB_H

#define Gain_Calib_cxx
#include "Gain_Calib.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TDBTool.hh>
#include <TString.h>


using namespace std;

void Gain_Calib::Loop() {
    //   In a ROOT session, you can do:
    //      root> .L Gain_Calib.C
    //      root> Gain_Calib t
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

    Long64_t nentries = fChain->GetEntriesFast();

    const int n_ch = 16;

    // ============= Lets initialize the DB =====================
    TString db_Name = "hodoDB";
    TString host = "localhost";
    int port = 3306;
    TString user = "hpshodo";

    TDBTool db(db_Name.Data(), host.Data(), port, user.Data());
    
    Double_t *must_have_top_threshold = db.GetEntry_d("MUST_HAVE_ADC_SUM_Top", frun);

    Double_t *peak_pos_max_top_vals = db.GetEntry_d("PEAK_POS_MAX_Top", frun);
    Double_t *peak_pos_min_top_vals = db.GetEntry_d("PEAK_POS_MIN_Top", frun);
    
    Double_t *spe_ADC_top_vals = db.GetEntry_d("tb_name_SPE_ADC_Top", frun);
    
    for( int i = 0; i < n_ch ; i++ ){
        cout<<"MUST_HAVE_ADC_Top["<<i<<"] = "<<must_have_top_threshold[i]<<endl;
    }
    cout<<"  ===================================== "<<endl;
    for( int i = 0; i < n_ch ; i++ ){
        cout<<"PEAK_POS_MAX_Top["<<i<<"] = "<<peak_pos_max_top_vals[i]<<endl;
    }
    cout<<"  ===================================== "<<endl;
    for( int i = 0; i < n_ch ; i++ ){
        cout<<"PEAK_POS_MIN_Top["<<i<<"] = "<<peak_pos_min_top_vals[i]<<endl;
    }

    cout<<"  ===================================== "<<endl;
    for( int i = 0; i < n_ch ; i++ ){
        cout<<"spe_ADC_top_vals["<<i<<"] = "<<spe_ADC_top_vals[i]<<endl;  
   }
    
    // ==== Based on EEL building tests data, more than 99% of peaks 
    // ==== are in the following range
    const double peak_pos_max = 52;
    const double peak_pos_min = 37;

    string half_names[n_half] = {"Bot", "Top"};
    string Layer_names[n_layer] = {"L1", "L2"};
    string Tile_Indexes[n_tile_perlayer] = {"1", "2", "3", "4", "5"};
    string Tile_side_names[n_tile_side] = {"Edge", "Middle"};


    TFile *file_out = new TFile(Form("../Data/Gain_Calib_%d.root", frun), "Recreate");

    TH1D *h_test1 = new TH1D("h_test1", "", 200, -200, 5500.);
    TH1D *h_test2 = new TH1D("h_test2", "", 200, -200, 5500.);
    TH1D *h_test3 = new TH1D("h_test3", "", 200, -200, 5500.);
    
    TH1D *h_test4 = new TH1D("h_test4", "", 200, -200, 2500.);
    TH1D *h_test5 = new TH1D("h_test5", "", 200, -200, 2500.);
    TH2D *h_test_01_1 = new TH2D("h_test_01_1", "", 200, -200., 2500., 200, -200., 2500);
    TH2D *h_test_01_2 = new TH2D("h_test_01_2", "", 200, -200., 2500., 200, -200., 2500);

    TH1D *h_test_All_ch_[n_PMT_ch];

    TH1D *h_RAW_ADC_Top_[n_ch];
    
    TH2D * h_test_cross_talk1_[n_PMT_ch];

    for (int ii = 0; ii < n_PMT_ch; ii++) {
        h_test_cross_talk1_[ii] = new TH2D(Form("h_test_cross_talk1_%d", ii), "", 200, -200., 5500, 200, -200., 900.);
        h_test_All_ch_[ii] = new TH1D(Form("h_test_All_ch_%d", ii), "", 200, -200, 2500.);
        
        h_RAW_ADC_Top_[ii] = new TH1D(Form("h_RAW_ADC_Top_%d", ii), "", 200, -200., 3500.);
    }

    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        nbytes += nb;
        // if (Cut(ientry) < 0) continue;


        bool top_cosmic = (((peak_pos_3_[1] >= peak_pos_min && peak_pos_3_[1] < peak_pos_max) * signal_3_[1] > 200. &&
                (peak_pos_3_[0] >= peak_pos_min && peak_pos_3_[0] < peak_pos_max) * signal_3_[0] > 200. ||
                (peak_pos_3_[4] >= peak_pos_min && peak_pos_3_[4] < peak_pos_max) * signal_3_[4] > 200. &&
                (peak_pos_3_[5] >= peak_pos_min && peak_pos_3_[5] < peak_pos_max) * signal_3_[5] > 200.) &&
                ((peak_pos_3_[8] >= peak_pos_min && peak_pos_3_[8] < peak_pos_max) * signal_3_[8] > 200. &&
                (peak_pos_3_[9] >= peak_pos_min && peak_pos_3_[9] < peak_pos_max) * signal_3_[9] > 200. ||
                (peak_pos_3_[12] >= peak_pos_min && peak_pos_3_[12] < peak_pos_max) * signal_3_[12] > 200. &&
                (peak_pos_3_[13] >= peak_pos_min && peak_pos_3_[13] < peak_pos_max) * signal_3_[13] > 200.));

        bool top_edge_cosmic = signal_3_[1] > 200. && signal_3_[0] > 200. && signal_3_[8] > 200. && signal_3_[9] > 200.;
        bool top_middle_cosmic = (signal_3_[13] > 200) && (signal_3_[4] + signal_3_[5] > 500);

        bool bot_edge_cosmic = signal_3_[2] > 200. && signal_3_[3] > 200 && signal_3_[10] > 200.;
        bool bot_middle_cosmic = signal_3_[7] > 150 && signal_3_[6] > 200. && signal_3_[14] > 200.;


        if (top_cosmic) {

            h_test1->Fill(signal_2_[0] + signal_2_[1]);
            for (int ii = 0; ii < n_PMT_ch; ii++) {
                if( peak_pos_2_[ii] > peak_pos_min && peak_pos_2_[ii] < peak_pos_max ){
                    h_test_All_ch_[ii]->Fill(signal_2_[ii]);
                }
            }

            if (((signal_2_[14] + signal_2_[15] > 500.) || (signal_2_[12] + signal_2_[13] > 500)) && signal_2_[2] < 250. && signal_2_[3] < 250. &&
                    signal_2_[6] < 250.) {
                h_test2->Fill(signal_2_[0] + signal_2_[1]);

                for( int ii = 0; ii < n_PMT_ch; ii++ ){
                    h_test_cross_talk1_[ii]->Fill(signal_2_[0], signal_2_[ii]);
                    h_test_All_ch_[ii]->Fill(signal_2_[ii]);
                }
                
                if ( signal_2_[4]*(peak_pos_2_[4] > peak_pos_min && peak_pos_2_[4] < peak_pos_max) < 100 &&
                        (peak_pos_2_[5] > peak_pos_min && peak_pos_2_[5] < peak_pos_max)*signal_2_[5] < 100.  &&
                       signal_2_[2]*(peak_pos_2_[2] > peak_pos_min && peak_pos_2_[2] < peak_pos_max) < 100. && 
                       signal_2_[3]*(peak_pos_2_[3] > peak_pos_min && peak_pos_2_[3] < peak_pos_max) < 100. &&
                        signal_2_[6]*(peak_pos_2_[6] > peak_pos_min && peak_pos_2_[6] < peak_pos_max) < 100. && 
                        signal_2_[9]*(peak_pos_2_[9] > peak_pos_min && peak_pos_2_[9] < peak_pos_max) < 100. &&
                        signal_2_[10]*(peak_pos_2_[10] > peak_pos_min && peak_pos_2_[10] < peak_pos_max) < 100. && 
                        signal_2_[11]*(peak_pos_2_[11] > peak_pos_min && peak_pos_2_[11] < peak_pos_max) < 100. &&
                        (peak_pos_2_[0] > peak_pos_min && peak_pos_2_[0] < peak_pos_max) &&
                        (peak_pos_2_[1] > peak_pos_min && peak_pos_2_[1] < peak_pos_max) ) {
                    h_test3->Fill(signal_2_[0] + signal_2_[1]);
                    
                    
                    if( (signal_2_[12] + signal_2_[13]) < 300. ){
                        h_test4->Fill(signal_2_[0]);
                        h_test_01_1->Fill(signal_2_[0], signal_2_[1]);
                    }
                    if( (signal_2_[14] + signal_2_[15]) < 300. ){
                        h_test5->Fill(signal_2_[1]);
                        h_test_01_2->Fill(signal_2_[0], signal_2_[1]);
                    }
                }
            }

            
            
            
            // For each channel we want to apply conditions that make it most probable that 
            // particle passed around (close to) that hole., if tile is readout with a single
            // PMT channel, then we just require particle to pass through the tile, and if 
            // the tile is readout with two tiles, we will try to select events, when it is more
            // probably the particle is passed close to the hole which is in probe.
            
            // For each of the channels we should apply conditions, i.e. some channels
            // should have signal ans some of them must not have signal.
            
            for( int ch = 0; ch < n_ch; ch++ ){
                
                // We Want to make sure that the peak is in the correct time range
                
                if( !(peak_pos_2_[ch] > peak_pos_min_top_vals[ch] && peak_pos_2_[ch] < peak_pos_max_top_vals[ch]) ) {continue;}
                                
                // ======== Channels that must have signal
                double must_have_ADC_sum = 0;
                
                for( int ind_must = 0; ind_must < ch_list_must_have[ch].size(); ind_must++ ){
                
                    int cur_ch_must = ch_list_must_have[ch].at(ind_must);
                    
                    must_have_ADC_sum = must_have_ADC_sum + signal_2_[cur_ch_must]*
                            (peak_pos_2_[cur_ch_must] > peak_pos_min_top_vals[cur_ch_must] && peak_pos_2_[cur_ch_must] < peak_pos_max_top_vals[cur_ch_must] );
                }
                
                bool must_have_flag = (must_have_ADC_sum > must_have_top_threshold[ch]);
                
                bool must_not_have_flag = true;
                
                for( int ind_mustNOT = 0; ind_mustNOT < ch_list_mustNOT_have[ch].size(); ind_mustNOT++ ){
                    int cur_ch_mustNOT = ch_list_mustNOT_have[ch].at(ind_mustNOT);
                    must_not_have_flag = must_not_have_flag*(signal_2_[cur_ch_mustNOT] < spe_ADC_top_vals[cur_ch_mustNOT] );
                }
                
                //cout<<must_not_have_flag<<"    "<<
              
                if( must_have_flag && must_not_have_flag ){
                    h_RAW_ADC_Top_[ch]->Fill(signal_2_[ch]);
                }
                
            }
            
        }


    }

    gDirectory->Write();
    
    delete must_have_top_threshold;
}
                 
void Gain_Calib::InitConstants(){

    // ==== Code below is added by Rafo =====

    // List of channels that must have signal when studying a specific channel
    ch_list_must_have[0] = {14, 15};
    ch_list_must_have[1] = {12, 13};    
    ch_list_must_have[2] = {12, 13};    
    ch_list_must_have[3] = {10, 11};    
    ch_list_must_have[4] = {10, 11};    
    ch_list_must_have[5] = {8};
    ch_list_must_have[6] = {9, 14, 15};
    ch_list_must_have[7] = {8};
    ch_list_must_have[8] = {4, 5, 7};
    ch_list_must_have[9] = {6};
    ch_list_must_have[10] = {4, 5};
    ch_list_must_have[11] = {2, 3};
    ch_list_must_have[12] = {2, 3};
    ch_list_must_have[13] = {0, 1};
    ch_list_must_have[14] = {0, 1};
    ch_list_must_have[15] = {6};
    
    
    // List of channels that mustNOT have signal when studying a specific channel
    ch_list_mustNOT_have[0] = {2, 3, 6, 9, 12, 13};
    ch_list_mustNOT_have[1] = {2, 3, 6, 9, 14, 15};
    ch_list_mustNOT_have[2] = {0, 1, 4, 5, 10, 11, 14, 15};
    ch_list_mustNOT_have[3] = {0, 1, 4, 5, 8, 12, 13};
    ch_list_mustNOT_have[4] = {2, 3, 7, 8, 12, 13};
    ch_list_mustNOT_have[5] = {2, 3, 7, 10, 11};
    ch_list_mustNOT_have[6] = {0, 1, 12, 13};
    ch_list_mustNOT_have[7] = {4, 5, 10, 11};
    ch_list_mustNOT_have[8] = {10, 11, 2, 3};
    ch_list_mustNOT_have[9] = {0, 1, 14, 15};
    ch_list_mustNOT_have[10] = {2, 3, 7, 8, 12, 13};
    ch_list_mustNOT_have[11] = {0, 1, 4, 5, 7, 8, 12, 13};
    ch_list_mustNOT_have[12] = {0, 1, 4, 5, 10, 11, 14, 15};
    ch_list_mustNOT_have[13] = {2, 3, 6, 10, 11, 14, 15};
    ch_list_mustNOT_have[14] = {2, 3, 6, 9, 12, 13};
    ch_list_mustNOT_have[15] = {0, 1, 9, 12, 13};

    // 0 -> 160
    // 1 -> 140
    // 2 -> 140
    // 3 -> 140
    // 4 -> 180
    // 5 -> 200
    // 6 -> 220
    // 7 -> 200
    // 8 -> 180
    // 9 -> 200
    // 10 -> 200
    // 11 -> 140
    // 12 -> 140
    // 13 -> 120
    // 14 -> 140
    // 15 -> 140
           
}


#endif	// GAIN_CALIB_H

