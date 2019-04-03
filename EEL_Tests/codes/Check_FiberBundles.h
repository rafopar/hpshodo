//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Mon Mar  4 10:16:02 2019 by ROOT version 6.16/00
// from TChain Check_FiberBundles/
//////////////////////////////////////////////////////////

#ifndef Check_FiberBundles_h
#define Check_FiberBundles_h

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>

// Header file for the classes stored in the TTree if any.

class Check_FiberBundles {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain
   
   int frun;

// Fixed size dimensions of array or collections stored in the TTree if any.
   static constexpr Int_t kMaxsignal_1 = 1;
   static constexpr Int_t kMaxsignal_2 = 1;
   static constexpr Int_t kMaxpeak_1 = 1;
   static constexpr Int_t kMaxpeak_2 = 1;
   static constexpr Int_t kMaxpeak_pos_1 = 1;
   static constexpr Int_t kMaxpeak_pos_2 = 1;
   static constexpr Int_t kMaxpedestal_1 = 1;
   static constexpr Int_t kMaxpedestal_2 = 1;

   // Declaration of leaf types
   Double_t        signal_1_[16];
   Double_t        signal_2_[16];
   Double_t        peak_1_[16];
   Double_t        peak_2_[16];
   Double_t        peak_pos_1_[16];
   Double_t        peak_pos_2_[16];
   Double_t        pedestal_1_[16];
   Double_t        pedestal_2_[16];

   // List of branches
   TBranch        *b_signal_1_;   //!
   TBranch        *b_signal_2_;   //!
   TBranch        *b_peak_1_;   //!
   TBranch        *b_peak_2_;   //!
   TBranch        *b_peak_pos_1_;   //!
   TBranch        *b_peak_pos_2_;   //!
   TBranch        *b_pedestal_1_;   //!
   TBranch        *b_pedestal_2_;   //!

   Check_FiberBundles(int, TTree *tree=0);
   virtual ~Check_FiberBundles();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop();
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);
};

#endif

#ifdef Check_FiberBundles_cxx
Check_FiberBundles::Check_FiberBundles(int arun, TTree *tree) : fChain(0) 
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
   if (tree == 0) {

       frun = arun;
#ifdef SINGLE_TREE
      // The following code should be used if you want this class to access
      // a single tree instead of a chain
      TFile *f = (TFile*)gROOT->GetListOfFiles()->FindObject("Memory Directory");
      if (!f || !f->IsOpen()) {
         f = new TFile("Memory Directory");
      }
      f->GetObject("Check_FiberBundles",tree);

#else // SINGLE_TREE

      // The following code should be used if you want this class to access a chain
      // of trees.
      TChain * chain = new TChain("Check_FiberBundles","");
      chain->Add(Form("../Data/Hodo_tests_%d.root/tr1", frun));
      //chain->Add("../Data/Hodo_tests_273.root/tr1");
      tree = chain;
#endif // SINGLE_TREE

   }
   Init(tree);
}

Check_FiberBundles::~Check_FiberBundles()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t Check_FiberBundles::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t Check_FiberBundles::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void Check_FiberBundles::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("signal_1_", signal_1_, &b_signal_1_);
   fChain->SetBranchAddress("signal_2_", signal_2_, &b_signal_2_);
   fChain->SetBranchAddress("peak_1_", peak_1_, &b_peak_1_);
   fChain->SetBranchAddress("peak_2_", peak_2_, &b_peak_2_);
   fChain->SetBranchAddress("peak_pos_1_", peak_pos_1_, &b_peak_pos_1_);
   fChain->SetBranchAddress("peak_pos_2_", peak_pos_2_, &b_peak_pos_2_);
   fChain->SetBranchAddress("pedestal_1_", pedestal_1_, &b_pedestal_1_);
   fChain->SetBranchAddress("pedestal_2_", pedestal_2_, &b_pedestal_2_);
   Notify();
}

Bool_t Check_FiberBundles::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void Check_FiberBundles::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t Check_FiberBundles::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef Check_FiberBundles_cxx
