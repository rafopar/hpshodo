#ifndef CHECK_FIBERBUNDLES_H
#define CHECK_FIBERBUNDLES_H

#define Check_FiberBundles_cxx
#include "Check_FiberBundles.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <fstream>

#include <TFile.h>

using namespace std;

void Check_FiberBundles::Loop()
{
//   In a ROOT session, you can do:
//      root> .L Check_FiberBundles.C
//      root> Check_FiberBundles t
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

   const double peak_threshold = 100.;
   
   const int n_PMT_ch = 16;
   
   map<int, double> PMT_SPE;
   
   ifstream inp_spe("PMT_SPE_SA3857.dat");
   
   if( inp_spe.is_open() ){
       while( !inp_spe.eof() ){
           int ch;
           double spe_pos;
           inp_spe>>ch>>spe_pos;
           //inp_spe>>spe_pos;
           
           PMT_SPE[ch - 1] = spe_pos;
           
           cout<<"ch = "<<ch<<"    spe = "<<spe_pos<<endl;
       }
   }else{
       cout<<"Can't open the fIle PMT_SPE_SA3857.dat, Exiting"<<endl;
       exit(1);
   }
   
   TFile *file_out = new TFile(Form("../Data/Check_FiberBundles_%d.root", frun), "Recreate");
   TH1D *h_ch_1_2_nphe1 = new TH1D("h_ch_1_2_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_1_2_signal1 = new TH1D("h_ch_1_2_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_1_2_peak1 = new TH1D("h_ch_1_2_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_3_4_nphe1 = new TH1D("h_ch_3_4_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_3_4_signal1 = new TH1D("h_ch_3_4_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_3_4_peak1 = new TH1D("h_ch_3_4_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_5_6_nphe1 = new TH1D("h_ch_5_6_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_5_6_signal1 = new TH1D("h_ch_5_6_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_5_6_peak1 = new TH1D("h_ch_5_6_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_7_8_nphe1 = new TH1D("h_ch_7_8_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_7_8_signal1 = new TH1D("h_ch_7_8_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_7_8_peak1 = new TH1D("h_ch_7_8_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_9_10_nphe1 = new TH1D("h_ch_9_10_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_9_10_signal1 = new TH1D("h_ch_9_10_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_9_10_peak1 = new TH1D("h_ch_9_10_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_11_12_nphe1 = new TH1D("h_ch_11_12_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_11_12_signal1 = new TH1D("h_ch_11_12_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_11_12_peak1 = new TH1D("h_ch_11_12_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_13_14_nphe1 = new TH1D("h_ch_13_14_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_13_14_signal1 = new TH1D("h_ch_13_14_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_13_14_peak1 = new TH1D("h_ch_13_14_peak1", "", 200, -2., 1200.);
   TH1D *h_ch_15_16_nphe1 = new TH1D("h_ch_15_16_nphe1", "", 200, -1.5, 45.);
   TH1D *h_ch_15_16_signal1 = new TH1D("h_ch_15_16_signal1", "", 200, -200., 5500.);
   TH1D *h_ch_15_16_peak1 = new TH1D("h_ch_15_16_peak1", "", 200, -2., 1200.);
   
   TH1D *h_nphe_individual_[n_PMT_ch];
   
   for( int i = 0; i < n_PMT_ch; i++ ){
   
       h_nphe_individual_[i] = new TH1D(Form("h_nphe_individual_%d", i+1), "", 200, -1.5, 45.);
   }
   
   
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      
      
      bool ch_1_2_trg = peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold &&
      peak_1_[10] > peak_threshold && peak_1_[11] > peak_threshold && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold
      && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold && peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold;

      bool ch_3_4_trg = peak_1_[0] > peak_threshold && peak_1_[1] > peak_threshold && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold &&
      peak_1_[10] > peak_threshold && peak_1_[11] > peak_threshold && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold
      && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold && peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold;

      bool ch_5_6_trg = peak_1_[0] > peak_threshold && peak_1_[1] > peak_threshold && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold &&
      peak_1_[10] > peak_threshold && peak_1_[11] > peak_threshold && peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold
      && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold && peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold;

      bool ch_7_8_trg = peak_1_[0] > peak_threshold && peak_1_[1] > peak_threshold && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold &&
      peak_1_[10] > peak_threshold && peak_1_[11] > peak_threshold && peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold
      && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold && peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold;

      bool ch_9_10_trg = peak_1_[0] > peak_threshold && peak_1_[1]> peak_threshold  && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold &&
      peak_1_[10] > peak_threshold && peak_1_[11] > peak_threshold && peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold
      && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold;

      bool ch_11_12_trg = peak_1_[0] > peak_threshold && peak_1_[1] > peak_threshold && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold &&
      peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold && peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold
      && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold;

      bool ch_13_14_trg = peak_1_[0] > peak_threshold && peak_1_[1] > peak_threshold && peak_1_[14] > peak_threshold && peak_1_[15] > peak_threshold && 
      peak_1_[10] > 2*peak_threshold && peak_1_[11] > 2*peak_threshold &&
      peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold && peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold
      && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold;

      bool ch_15_16_trg = peak_1_[0] > peak_threshold && peak_1_[1] > peak_threshold && peak_1_[12] > peak_threshold && peak_1_[13] > peak_threshold && 
      peak_1_[10] > 2*peak_threshold && peak_1_[11] > 2*peak_threshold &&
      peak_1_[8] > peak_threshold && peak_1_[9] > peak_threshold && peak_1_[2] > peak_threshold && peak_1_[3] > peak_threshold
      && peak_1_[4] > peak_threshold && peak_1_[5] > peak_threshold && peak_1_[6] > peak_threshold && peak_1_[7] > peak_threshold;
      
      
      
      
      if( ch_1_2_trg ){
          h_ch_1_2_signal1->Fill(signal_1_[0] + signal_1_[1]);
          h_ch_1_2_peak1->Fill(peak_1_[0] + peak_1_[1]);
          
          double nphe = signal_1_[0]/PMT_SPE[0] + signal_1_[1]/PMT_SPE[1];
          h_ch_1_2_nphe1->Fill(nphe);
          h_nphe_individual_[0]->Fill(signal_1_[0]/PMT_SPE[0]);
          h_nphe_individual_[1]->Fill(signal_1_[1]/PMT_SPE[1]);
      }

      if( ch_3_4_trg ){
          h_ch_3_4_signal1->Fill(signal_1_[2] + signal_1_[3]);
          h_ch_3_4_peak1->Fill(peak_1_[2] + peak_1_[3]);
          double nphe = signal_1_[2]/PMT_SPE[2] + signal_1_[3]/PMT_SPE[3];
          h_ch_3_4_nphe1->Fill(nphe);
          h_nphe_individual_[2]->Fill(signal_1_[2]/PMT_SPE[2]);
          h_nphe_individual_[3]->Fill(signal_1_[3]/PMT_SPE[3]);
      }
      
      if( ch_5_6_trg ){
          h_ch_5_6_signal1->Fill(signal_1_[4] + signal_1_[5]);
          h_ch_5_6_peak1->Fill(peak_1_[4] + peak_1_[5]);
          double nphe = signal_1_[4]/PMT_SPE[4] + signal_1_[5]/PMT_SPE[5];
          h_ch_5_6_nphe1->Fill(nphe);
          h_nphe_individual_[4]->Fill(signal_1_[4]/PMT_SPE[4]);
          h_nphe_individual_[5]->Fill(signal_1_[5]/PMT_SPE[5]);
      }

      if( ch_7_8_trg ){
          h_ch_7_8_signal1->Fill(signal_1_[6] + signal_1_[7]);
          h_ch_7_8_peak1->Fill(peak_1_[6] + peak_1_[7]);
          double nphe = signal_1_[6]/PMT_SPE[6] + signal_1_[7]/PMT_SPE[7];
          h_ch_7_8_nphe1->Fill(nphe);
          h_nphe_individual_[6]->Fill(signal_1_[6]/PMT_SPE[6]);
          h_nphe_individual_[7]->Fill(signal_1_[7]/PMT_SPE[7]);
      }

      if( ch_9_10_trg ){
          h_ch_9_10_signal1->Fill(signal_1_[8] + signal_1_[9]);
          h_ch_9_10_peak1->Fill(peak_1_[8] + peak_1_[9]);
          double nphe = signal_1_[8]/PMT_SPE[8] + signal_1_[9]/PMT_SPE[9];
          h_ch_9_10_nphe1->Fill(nphe);
          h_nphe_individual_[8]->Fill(signal_1_[8]/PMT_SPE[8]);
          h_nphe_individual_[9]->Fill(signal_1_[9]/PMT_SPE[9]);
      }

      if( ch_11_12_trg ){
          h_ch_11_12_signal1->Fill(signal_1_[10] + signal_1_[11]);
          h_ch_11_12_peak1->Fill(peak_1_[10] + peak_1_[11]);
          double nphe = signal_1_[10]/PMT_SPE[10] + signal_1_[11]/PMT_SPE[11];
          h_ch_11_12_nphe1->Fill(nphe);
          h_nphe_individual_[10]->Fill(signal_1_[10]/PMT_SPE[10]);
          h_nphe_individual_[11]->Fill(signal_1_[11]/PMT_SPE[11]);
      }

      if( ch_13_14_trg ){
          h_ch_13_14_signal1->Fill(signal_1_[12] + signal_1_[13]);
          h_ch_13_14_peak1->Fill(peak_1_[12] + peak_1_[13]);
          double nphe = signal_1_[12]/PMT_SPE[12] + signal_1_[13]/PMT_SPE[13];
          h_ch_13_14_nphe1->Fill(nphe);
          h_nphe_individual_[12]->Fill(signal_1_[12]/PMT_SPE[12]);
          h_nphe_individual_[13]->Fill(signal_1_[13]/PMT_SPE[13]);
      }

      if( ch_15_16_trg ){
          h_ch_15_16_signal1->Fill(signal_1_[14] + signal_1_[15]);
          h_ch_15_16_peak1->Fill(peak_1_[14] + peak_1_[15]);
          double nphe = signal_1_[14]/PMT_SPE[15] + signal_1_[14]/PMT_SPE[15];
          h_ch_15_16_nphe1->Fill(nphe);
          h_nphe_individual_[14]->Fill(signal_1_[14]/PMT_SPE[14]);
          h_nphe_individual_[15]->Fill(signal_1_[15]/PMT_SPE[15]);
      }
      
   }
   
   
   gDirectory->Write();
}

#endif	// CHECK_FIBERBUNDLES_H

