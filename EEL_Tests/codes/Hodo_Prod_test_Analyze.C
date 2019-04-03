#ifndef Hodo_Prod_test_Analyze_cxx
#define Hodo_Prod_test_Analyze_cxx
#include "Hodo_Prod_test_Analyze.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

#include <TFile.h>
#include <TLatex.h>

void Hodo_Prod_test_Analyze::Loop() {
    //   In a ROOT session, you can do:
    //      Root > .L Hodo_Prod_test_Analyze.C
    //      Root > Hodo_Prod_test_Analyze t
    //      Root > t.GetEntry(12); // Fill t data members with entry number 12
    //      Root > t.Show();       // Show values of entry 12
    //      Root > t.Show(16);     // Read and show values of entry 16
    //      Root > t.Loop();       // Loop on all entries
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

    //
    //                                      ---                |  
    //                                      |*|                |   #1
    //                                      -----              |
    //                                      | * |              |   #2
    //                                  -----------            |
    //                                  | *     * |            |   #7     #8
    //                              -------------------        |
    //                              |     *     *     |        |   #9     #10, This SC tile has optical glue on sides   
    //                              -------------------        |   
    //                              |     *     *     |        |   #5     #6,  #ch 5 is Fixed to the Mylar with a UV glue
    //                              -------------------        |
    //                                |   *     *   |          |   #11   #12
    //                                ---------------          |
    //                                      |*|                |   #13
    //                                      ---                |
    //                                      |*|                |   #14
    //                                      ---                |
    //
    //




    Long64_t nentries = fChain->GetEntriesFast();

    const int run = 244;
    const int n_PMT_ch = 16;

    double SPE_positions[n_PMT_ch] = {120.287, 105.002, 105.788, 104.256, 120.126, 132.128, 131.153, 102.15, 110.261, 121.911, 131.954, 94.5466, 97.5445, 95.3675, 95.1175, 90.4373};
    double Nphe_[n_PMT_ch];

    TFile *file_out = new TFile(Form("../Data/Hodo_Prod_test_Analyze_%d.root", run), "Recreate");

    TH1D *h_tile_otical_Glue1 = new TH1D("h_tile_otical_Glue1", "", 200, -5., 60.);
    TH1D *h_tile_otical_Glue2 = new TH1D("h_tile_otical_Glue2", "", 200, -5., 60.);

    TH1D *h_tile_NoOtical_Glue1 = new TH1D("h_tile_NoOtical_Glue1", "", 200, -5., 60.);

    TH1D *h_UV_glue1 = new TH1D("h_UV_glue1", "", 200, -5., 60.);
    TH1D *h_NoUV_glue1 = new TH1D("h_NoUV_glue1", "", 200, -5., 60.); // The other channel of the same tile (ch#5 is UV glued, while ch#6 is not)

    TH1D *h_tile_L1_1_trg_smallnphe = new TH1D("h_tile_L1_1_trg_smallnphe", "", 200, -5., 60.);
    TH1D *h_tile_L1_1_trg_bignphe = new TH1D("h_tile_L1_1_trg_bignphe", "", 200, -5., 60.);

    TH1D *h_tile_L1_2_sum_trg_1_2_13_14_smallnphe = new TH1D("h_tile_L1_2_sum_trg_1_2_13_14_smallnphe", "", 200, -5., 60.);
    TH1D *h_tile_L1_2_sum_trg_1_2_13_14_largenphe = new TH1D("h_tile_L1_2_sum_trg_1_2_13_14_largenphe", "", 200, -5., 60.);
    
    TH1D *h_tile_L2_5_sum_trg_1_2_13_14_smallnphe = new TH1D("h_tile_L2_5_sum_trg_1_2_13_14_smallnphe", "", 200, -5., 60.);

    TH1D *h_tile_L2_1_trg_1_13_14_smallnphe = new TH1D("h_tile_L2_1_trg_1_13_14_smallnphe", "", 200, -5., 60.);
    
    
    Long64_t nbytes = 0, nb = 0;
    for (Long64_t jentry = 0; jentry < nentries; jentry++) {
        Long64_t ientry = LoadTree(jentry);
        if (ientry < 0) break;
        nb = fChain->GetEntry(jentry);
        nbytes += nb;
        // if (Cut(ientry) < 0) continue;


        for (int ii = 0; ii < n_PMT_ch; ii++) {
            Nphe_[ii] = signal_2_[ii] / SPE_positions[ii];
        }


        // Channels #9 and #10 are inside the SC tile which has optical glue on it's sides.
        if (Nphe_[0] > 2 && Nphe_[1] > 2 && Nphe_[12] > 2 && Nphe_[13] > 2 && Nphe_[10] > 2 && Nphe_[11]) {
            //if( signal_2_[0] > 150 && signal_2_[1] > 150 && signal_2_[12] > 150 && signal_2_[13] > 150 ){
            //h_tile_otical_Glue1->Fill(signal_2_[8]/110. + signal_2_[8]/122.);
            h_tile_otical_Glue1->Fill(Nphe_[8] + Nphe_[9]);
        }

        // With this condition we want to check if events from the strict vertical trigger (tr=1,2,13 and 14 ) are similar
        // other triggers with an angeled tracks (tr = 2, 11 and 12)
        if (Nphe_[1] > 2 && Nphe_[10] > 2 && Nphe_[11] > 2) {
            h_tile_otical_Glue2->Fill(Nphe_[8] + Nphe_[9]);

            // Here we want to check if the tile with an optical glue on sides makes difference
            // ch #5 and #6 are inside a tile that has no optical glue on sides

            h_tile_NoOtical_Glue1->Fill(Nphe_[4] + Nphe_[5]);

            // Below we want to test the effect of the UV glueing
            // We will compate signla of the channel that is UV glued on the Mylar with the channel that is not UV glued to the Mylar
            h_UV_glue1->Fill(Nphe_[4]);

            h_NoUV_glue1->Fill(Nphe_[5]);
        }


        // Now we want to test small tiles, the ones with a single hole

        if (Nphe_[0] > 2 && Nphe_[1] > 2 && Nphe_[13] > 2) {
            h_tile_L1_1_trg_smallnphe->Fill(Nphe_[12]);
        }
        if (Nphe_[0] > 4 && Nphe_[1] > 4 && Nphe_[13] > 4) {
            h_tile_L1_1_trg_bignphe->Fill(Nphe_[12]);
        }



        // Now we want to check tile L(1,2) these are channels 11 and 12
        if (Nphe_[0] > 2 && Nphe_[1] > 2 && Nphe_[12] > 2 && Nphe_[13] > 2 /*&& Nphe_[8] > 4 && Nphe_[9] > 4 && Nphe_[4] > 4 && Nphe_[5] > 4*/) {
            h_tile_L1_2_sum_trg_1_2_13_14_smallnphe->Fill(Nphe_[10] + Nphe_[11]);
            if (Nphe_[0] > 3 && Nphe_[1] > 3 && Nphe_[12] > 3 && Nphe_[13] > 3 /*&& Nphe_[8] > 4 && Nphe_[9] > 4 && Nphe_[4] > 4 && Nphe_[5] > 4*/) {
                h_tile_L1_2_sum_trg_1_2_13_14_largenphe->Fill(Nphe_[10] + Nphe_[11]);
            }

        }



	// Now checking tile L(2,5)-1, here fibers of channels #7 and #8 are inside the SC bar

	if( Nphe_[0] > 2 && Nphe_[1] > 2 && /*Nphe_[12] > 2 &&  Nphe_[13] > 2 && */ Nphe_[10] > 2 && Nphe_[11] > 2){
	  h_tile_L2_5_sum_trg_1_2_13_14_smallnphe->Fill(Nphe_[6] + Nphe_[7]);
	}


	// Now we should chack L(2,1) tile

	if( Nphe_[0] > 2 && Nphe_[12] > 2 && Nphe_[13] > 2 ){
	  h_tile_L2_1_trg_1_13_14_smallnphe->Fill(Nphe_[1]);
	}


    }

    TCanvas *c1 = new TCanvas("c1", "", 750, 750);
    
    TLatex *lat1 = new TLatex();
    lat1->SetNDC();
    lat1->SetTextSize(0.035);
    
    h_tile_otical_Glue2->Draw();
    h_tile_otical_Glue2->SetTitle("; #nphe");
    h_tile_otical_Glue1->SetLineColor(2);
    //h_tile_otical_Glue1->Scale(h_tile_otical_Glue2->GetMaximum()/h_tile_otical_Glue1->GetMaximum());
    h_tile_otical_Glue1->Scale(4.55);
    h_tile_otical_Glue1->Draw("hist same");
    lat1->SetTextColor(2);
    lat1->DrawLatex(0.45, 0.7, "trg ch# 1x2x11x12x13x14 (nphe > 2)");
    lat1->SetTextColor(4);
    lat1->DrawLatex(0.45, 0.65, "trg ch# 2x11x12 (nphe > 2)");
    c1->Print("Figs/Glued_SC_bar_trg_tight_vs_llose.eps");
    c1->Print("Figs/Glued_SC_bar_trg_tight_vs_llose.pdf");
    c1->Print("Figs/Glued_SC_bar_trg_tight_vs_llose.png");


    h_tile_otical_Glue2->Draw();
    h_tile_NoOtical_Glue1->SetLineColor(2);
    h_tile_NoOtical_Glue1->Draw("Same");
    lat1->SetTextColor(4);
    lat1->DrawLatex(0.45, 0.7, "W/ optical glue");
    lat1->SetTextColor(2);
    lat1->DrawLatex(0.45, 0.65, "W/O optical glue");
    c1->Print("Figs/Compare_OptGlue_With_NonoptGLue.eps");
    c1->Print("Figs/Compare_OptGlue_With_NonoptGLue.pdf");
    c1->Print("Figs/Compare_OptGlue_With_NonoptGLue.png");


    h_UV_glue1->SetTitle("; #nphe");
    h_UV_glue1->Draw();
    h_NoUV_glue1->SetLineColor(2);
    h_NoUV_glue1->Draw("Same");
    lat1->SetTextColor(4);
    lat1->DrawLatex(0.45, 0.7, "Fibers fixed w/ UV glue");
    lat1->SetTextColor(2);
    lat1->DrawLatex(0.45, 0.65, "Fibers not fixed");
    c1->Print("Figs/Compare_UV_glue_with_nonUVglue.eps");
    c1->Print("Figs/Compare_UV_glue_with_nonUVglue.pdf");
    c1->Print("Figs/Compare_UV_glue_with_nonUVglue.png");


    h_tile_L1_1_trg_smallnphe->Draw();
    h_tile_L1_1_trg_smallnphe->SetTitle("; #nphe");
    h_tile_L1_1_trg_bignphe->SetLineColor(2);
    h_tile_L1_1_trg_bignphe->Draw("Same");
    lat1->SetTextColor(1);
    lat1->DrawLatex(0.45, 0.8, "trg: 1x2x14");
    lat1->SetTextColor(4);
    lat1->DrawLatex(0.25, 0.7, "trig. #phe > 2");
    lat1->SetTextColor(2);
    lat1->DrawLatex(0.25, 0.65, "trig. #phe > 4");
    c1->Print("Figs/L1_1_tile_signal_diff_trg_thresholds.eps");
    c1->Print("Figs/L1_1_tile_signal_diff_trg_thresholds.pdf");
    c1->Print("Figs/L1_1_tile_signal_diff_trg_thresholds.png");



    h_tile_L1_2_sum_trg_1_2_13_14_smallnphe->Draw();
    h_tile_L1_2_sum_trg_1_2_13_14_smallnphe->SetTitle("; #nphe");
    h_tile_L1_2_sum_trg_1_2_13_14_largenphe->SetLineColor(2);
    h_tile_L1_2_sum_trg_1_2_13_14_largenphe->Draw("Same");
    lat1->SetTextColor(1);
    lat1->DrawLatex(0.45, 0.8, "trg: 1x2x13x14");
    lat1->SetTextColor(4);
    lat1->DrawLatex(0.55, 0.7, "trig. #phe > 2");
    lat1->SetTextColor(2);
    lat1->DrawLatex(0.55, 0.65, "trig. #phe > 3");
    c1->Print("Figs/L1_2_tile_diff_trg_thresholds.eps");
    c1->Print("Figs/L1_2_tile_diff_trg_thresholds.pdf");
    c1->Print("Figs/L1_2_tile_diff_trg_thresholds.png");

    
    gDirectory->Write();
}

#endif
