#define Hodo_tests_Analyze_cxx
#include "Hodo_tests_Analyze.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Hodo_tests_Analyze::Loop()
{
//   In a ROOT session, you can do:
//      root> .L Hodo_tests_Analyze.C
//      root> Hodo_tests_Analyze t
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

   const int n_fADC_ch = 16;
   
   TFile *file_out = new TFile(Form("../Data/Hodo_tests_Analyze_%d.root", frun), "Recreate");

   TH1D *h_signal_1_[n_fADC_ch];
   TH1D *h_signal_2_[n_fADC_ch];
   
   for( int i_ch = 0; i_ch  <n_fADC_ch; i_ch++ ){
       h_signal_1_[i_ch] = new TH1D(Form("h_signal_1_%d", i_ch), "", 400, -200, 6000);
       h_signal_2_[i_ch] = new TH1D(Form("h_signal_2_%d", i_ch), "", 400, -200, 6000);
   }
   
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

      bool cut_coincidence1 = false;
      
      if( frun >= 217 && frun <= 221 ){
          cut_coincidence1 = (peak_1_[6] > 150. && peak_1_[7] > 150. && peak_1_[10] > 150. && peak_1_[11] > 150.);
      } else if ( frun >= 214 && frun <= 215 ){
          cut_coincidence1 = (peak_1_[0] > 150 && peak_1_[1] > 150 && peak_1_[3] > 150 && peak_1_[4] > 150.);
      }
      
      if( cut_coincidence1 ){
          
          for( int i_ch = 0; i_ch < n_fADC_ch; i_ch++ ){
              h_signal_1_[i_ch]->Fill(signal_1_[i_ch]);
              h_signal_2_[i_ch]->Fill(signal_2_[i_ch]);
          }
      }
      
      
   }
   
   gDirectory->Write();
}
