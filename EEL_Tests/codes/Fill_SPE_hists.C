#define Fill_SPE_hists_cxx
#include "Fill_SPE_hists.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void Fill_SPE_hists::Loop()
{
//   In a ROOT session, you can do:
//      root> .L Fill_SPE_hists.C
//      root> Fill_SPE_hists t
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

   const int n_PMT_ch = 16;
   
   TFile *file_out = new TFile(Form("../Data/SPE_hists_%d.root", frun), "Recreate");
   
   TH1D *h_signal_slot3_[n_PMT_ch];
   TH1D *h_signal_slot4_[n_PMT_ch];
   
   for( int i = 0; i < n_PMT_ch; i++ ){
       h_signal_slot3_[i] = new TH1D(Form("h_signal_slot3_%d", i), "", 200, -200., 550.);
       h_signal_slot4_[i] = new TH1D(Form("h_signal_slot4_%d", i), "", 200, -200., 550.);
   }
   
   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;
      
      bool slot4_trigger = ((signal_3_[0] > 150 && signal_3_[1] > 150) || ( signal_3_[4] > 150 && signal_3_[5] > 150 )) &&
      ((signal_3_[8] > 150 && signal_3_[9] > 150) || (signal_3_[12] > 150 && signal_3_[13] > 150) );
    
      bool slot3_trigger = ((signal_3_[2] > 200 && signal_3_[3] > 150) || (signal_3_[6] > 150 && signal_3_[7] > 100 ) ) &&
     ( (signal_3_[10] > 200 ) || (signal_3_[14] > 200) );
      
      
      if( slot3_trigger ){
          
          for( int i_ch = 0; i_ch < n_PMT_ch; i_ch++ ){
            h_signal_slot3_[i_ch]->Fill(signal_1_[i_ch]);
          }
      }

      if( slot4_trigger ){
          
          for( int i_ch = 0; i_ch < n_PMT_ch; i_ch++ ){
            h_signal_slot4_[i_ch]->Fill(signal_2_[i_ch]);
          }
      }

      
   }
   
   gDirectory->Write();
}
