/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Analyze_Hodo.cc
 * Author: rafopar
 *
 * Created on September 10, 2018, 10:00 PM
 */

#include <cstdlib>

#include <evioUtil.hxx>
#include <evioFileChannel.hxx>
#include <list>
#include <bitset>         // std::bitset
#include <iomanip>
#include <iostream>
#include <vector>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TMath.h>
#include <TLatex.h>
#include <TCanvas.h>
#include <TChain.h>
#include <TObjArray.h>

#include "trans_tables.h"

#define GET_CHAR(b,i) b[i]; i+=1;
#define GET_SHORT(b,i) ((short *)(&b[i]))[0]; i+=2;
#define GET_INT(b,i) ((int *)(&b[i]))[0]; i+=4;
#define GET_L64(b,i) ((unsigned long long *)(&b[i]))[0]; i+=8;

#define FADC_1_BANK 57601

using namespace evio;
using namespace std;

/*
 * 
 */
int main(int argc, char** argv) {

    if (argc < 2) {
        cout << "Please provide the data Run number " << endl;
        cout << "Exiting" << endl;
        exit(1);
    }

    TLatex *lat1 = new TLatex();
    lat1->SetNDC();
    TCanvas *c1 = new TCanvas("c1", "", 1200, 900);
    c1->Divide(4, 4);

    string strrunNo = argv[1];
    int runNo = atoi(argv[1]);

    const int n_tiles = 8;
    const int n_ch = 16;
    const int n_sl = 2;
    const int n_all_slot = 21; // Only 2 slots will be used for HPS
    const int n_tmp_channels = 4;
    const int n_samp = 112;
    const int n_MaxRunNoDigits = 6; // Number of digits assigned for run number
    const int n_maPMTchan = 16; // Number of channels in the maPMT
    const int n_cosmic_PMTs = 2; // Number of Cosmic PMTs
    const int n_det = 2; // Detectors: 1 maPMT, 2 Cosmic scintilators

    int n_run_dgts = int(TMath::Log10(runNo)) + 1; // This will tell how many digits have the run number

    int n_events_to_Analyze = 100000; // Default value

    if (argc >= 3) {
        n_events_to_Analyze = atoi(argv[2]);
    }


    TFile *file_out = new TFile(Form("../Data/Analyze_Hodo_%d.root", runNo), "Recreate");
    TH1D * h_fADC_maPMT_[n_maPMTchan];
    TH1D * h_fADC_cosmic_[n_cosmic_PMTs];

    TH2D *h_coincid_PMTs1 = new TH2D("h_coincid_PMTs1", "", 200, -200, 1800, 200, -200., 1800);

    map<std::pair<int, int>, TH1D*> h_Hodo_fADC_;
    map<std::pair<int, int>, TH2D*> h_signal_integr_peak_;
    map<std::pair<int, int>, TH1D*> h_signal_integr_;
    map<std::pair<int, int>, TH1D*> h_signal_integr_coincid_;
    map<std::pair<int, int>, TH1D*> h_pedestal_;
    map<std::pair<int, int>, TH1D*> h_peak_pos_;
    map<std::pair<int, int>, TH1D*> h_peak_;
    map<std::pair<int, int>, double> pedestal_;
    map<std::pair<int, int>, double> peak_;
    map<std::pair<int, int>, double> peak_pos_;
    map<std::pair<int, int>, double> signal_integr_;


    for (int i = 0; i < n_maPMTchan; i++) {
        h_fADC_maPMT_[i] = new TH1D(Form("h_fADC_%d", i), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
        h_Hodo_fADC_[std::make_pair(1, i + 1)] = new TH1D(Form("h_Hodo_fADC_%d_%d", 1, i + 1), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
        h_Hodo_fADC_[make_pair(2, i + 1)] = new TH1D(Form("h_Hodo_fADC_%d_%d", 2, i + 1), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
        h_signal_integr_[make_pair(1, i + 1)] = new TH1D(Form("h_signal_integr_%d_%d", 1, i + 1), "", 200, -100., 1200);
        h_signal_integr_[make_pair(2, i + 1)] = new TH1D(Form("h_signal_integr_%d_%d", 2, i + 1), "", 200, -100., 1200);
        h_signal_integr_coincid_[make_pair(1, i + 1)] = new TH1D(Form("h_signal_integr_coincid_%d_%d", 1, i + 1), "", 200, -100., 1200);
        h_signal_integr_coincid_[make_pair(2, i + 1)] = new TH1D(Form("h_signal_integr_coincid_%d_%d", 2, i + 1), "", 200, -100., 1200);
        h_signal_integr_peak_[make_pair(1, i + 1)] = new TH2D(Form("h_signal_integr_peak_%d_%d", 1, i + 1), "", 200, -25., 500., 200, -200., 1400);
        h_signal_integr_peak_[make_pair(2, i + 1)] = new TH2D(Form("h_signal_integr_peak_%d_%d", 2, i + 1), "", 200, -25., 500., 200, -200., 1400);

        h_pedestal_[make_pair(1, i + 1)] = new TH1D(Form("h_pedestal_%d_%d", 1, i + 1), "", 900, 0., 200);
        h_pedestal_[make_pair(2, i + 1)] = new TH1D(Form("h_pedestal_%d_%d", 2, i + 1), "", 900, 0., 200);
        h_peak_pos_[make_pair(1, i + 1)] = new TH1D(Form("h_peak_pos_%d_%d", 1, i + 1), "", n_samp + 2, -0.5, n_samp + 1.5);
        h_peak_pos_[make_pair(2, i + 1)] = new TH1D(Form("h_peak_pos_%d_%d", 2, i + 1), "", n_samp + 2, -0.5, n_samp + 1.5);
        h_peak_[make_pair(1, i + 1)] = new TH1D(Form("h_peak_%d_%d", 1, i + 1), "", 200, 0., 500);
        h_peak_[make_pair(2, i + 1)] = new TH1D(Form("h_peak_%d_%d", 2, i + 1), "", 200, 0., 500);
    }

    for (int i = 0; i < n_cosmic_PMTs; i++) {
        h_fADC_cosmic_[i] = new TH1D(Form("h_fADC_cosmic_%d", i), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
    }

    string fname_base = "../Data/hpshodo_000000";
    //string fname_base = "../Data/test1_000000";
    cout << "fname_base Before replace:  " << fname_base << endl;
    int fname_baselen = fname_base.size();
    fname_base.replace(fname_baselen - n_run_dgts, n_run_dgts, strrunNo);

    cout << "fname_base After replace:  " << fname_base << endl;

    string inp_file_pattern = fname_base + ".evio.*";

    cout << "inp_file_pattern is " << inp_file_pattern << endl;

    TChain *ch1 = new TChain();
    ch1->Add(inp_file_pattern.c_str());

    TObjArray *fname_list = ch1->GetListOfFiles();
    int n_files = fname_list->GetEntries();
    cout << "Number of Files is " << n_files << endl;

    for (int ifile = 0; ifile < n_files; ifile++) {
        cout << fname_list->At(ifile)->GetTitle() << endl;
    }

    string fname = "Kuku.evio";

    try {

        unsigned int i_ev = 0;

        for (int ifile = 0; ifile < n_files; ifile++) {

            string cur_fname = fname_list->At(ifile)->GetTitle();

            evioFileChannel chan(cur_fname, "r");

            cout << "Analyzing the file " << cur_fname << endl;

            chan.open();

            while (chan.read()) {

                i_ev = i_ev + 1;

                evioDOMTree tree(chan);

                evioDOMNodeListP nodelist1 = tree.getNodeList();
                int node_list_size = nodelist1.get()->size();

                for (std::list<evioDOMNode*>::iterator it = nodelist1.get()->begin(); it != nodelist1.get()->end(); it++) {

                    if ((*it)->tag == FADC_1_BANK) {

                        evioCompositeDOMLeafNode *fadc = (evioCompositeDOMLeafNode *) (*it);
                        unsigned char *p = (unsigned char *) fadc->data.data();
                        unsigned int len = fadc->data.size() * 4;

                        int i = 0;
                        int adc[1024];

                        while (len - i > 4) {

                            unsigned char slot = GET_CHAR(p, i);

                            //cout<<" =========== = = = = = = =       slot = "<<int(slot)<<endl;

                            int trig_n = GET_INT(p, i);
                            long long trig_t = GET_L64(p, i);

                            //cout<<"trig_t = "<<trig_t<<endl;

                            int nch = GET_INT(p, i);

                            if (nch > 16) {
                                printf("Error in %s: fadc mode 1 invalid nch\n", __func__);
                                exit(1);
                            }


                            while (nch--) {

                                int ch = GET_CHAR(p, i);
                                int ns = GET_INT(p, i);
                                int np = 0;
                                int x, y;

                                int hodo_d = hodo_det[int(slot)][ch];
                                int hodo_ch = hodo_chan[int(slot)][ch];

                                //                                cout << "slot = " << int(slot) << endl;
                                //                                cout << "ch = " << ch << endl;
                                //                                cout << "hodo_d = " << hodo_d << endl;
                                //                                cout << "hodo_ch = " << hodo_ch << endl;
                                //                                cout << "   === == == == == == == == == == == == ==" << endl;
                                //cout<<"channel = "<<ch<<endl;

                                //cout<<"ns = "<<ns<<endl;
                                //      cout<<" ================= = = = = = = = =  channel = "<<ch<<endl;


                                if (ns > sizeof (adc) / sizeof (adc[0])) {
                                    printf("Warning in %s: fadc mode 1: # samples (%d) > buffer (%d)\n", __func__, ns, (int) (sizeof (adc) / sizeof (adc[0])));
                                    for (int n = 0; n < ns; n++) {
                                        GET_SHORT(p, i);
                                    }
                                    continue;
                                }


                                for (int n = 0; n < ns; n++) {
                                    adc[n] = GET_SHORT(p, i);
                                    //cout<<" n = "<<n<<"     ADC value = "<<adc[n]<<endl;

                                    h_Hodo_fADC_[make_pair(hodo_d, hodo_ch)]->Fill(n, adc[n]);
                                }



                            }


                            //<=>

                        }

                    }


                }


                //<<>>

                bool test = false;
                int n_hist = 0;
                for (std::map<pair<int, int>, TH1D*>::iterator it = h_Hodo_fADC_.begin(); it != h_Hodo_fADC_.end(); it++) {
                    TH1D *cur_hist = it->second;
                    pair<int, int> the_key = it->first;

                    if (cur_hist->GetEntries() > 0) {
                        pedestal_[the_key] = cur_hist->Integral(1, 10) / 10.;
                        //pedestal_[the_key] = cur_hist->Integral(n_samp - 15, n_samp - 5) / 10.;
                        //n_samp
                        peak_[the_key] = cur_hist->GetMaximum() - pedestal_[the_key];
                        peak_pos_[the_key] = cur_hist->GetBinCenter(cur_hist->GetMaximumBin());
                        signal_integr_[the_key] = cur_hist->Integral() - pedestal_[the_key] * n_samp;
                        //cur_hist


                        h_signal_integr_[the_key]->Fill(signal_integr_[the_key]);
                        h_signal_integr_peak_[the_key]->Fill(peak_[the_key], signal_integr_[the_key]);
                        h_pedestal_[the_key]->Fill(pedestal_[the_key]);
                        h_peak_pos_[the_key]->Fill(peak_pos_[the_key]);
                        h_peak_[the_key]->Fill(peak_[the_key]);

//                        cur_hist->SetLineColor(n_hist + 1);
//                        lat1->SetTextColor(n_hist + 1);

//                        if (the_key.first == 1) { // Where maPMTs are
//                            c1->cd(the_key.second);
//                            cur_hist->DrawCopy("hist");
//                        }
                        //                                                
                        //                                                if (!test) {
                        //                                                    cur_hist->SetMaximum(1000.);
                        //                                                    cur_hist->DrawCopy("hist");
                        //                                                } else {
                        //                                                    cur_hist->DrawCopy("Same hist");
                        //                                                }

                        //lat1->DrawLatex(0.5, 0.85 - 0.05 * n_hist, Form("%s", cur_hist->GetName()));
                        //cur_hist->Reset();
                        test = true;
                        n_hist = n_hist + 1;
                    } else {
                        signal_integr_[the_key] = 0.;
                    }
                }


//                if (peak_[make_pair(1, 7)] > 150 && peak_[make_pair(1, 8)] > 150 &&
//                        peak_[make_pair(1, 11)] > 150 && peak_[make_pair(1, 12)] > 150) {
//                    c1->Print("tmp.eps");
//                    cin.ignore();
//                }

                h_coincid_PMTs1->Fill(signal_integr_[make_pair(2, 15)], signal_integr_[make_pair(2, 16)]);


                if (signal_integr_[make_pair(2, 15)] > 200. && signal_integr_[make_pair(2, 16)] > 200.) {
                    for (int i_ch = 0; i_ch < n_maPMTchan; i_ch++) {

                        if (signal_integr_[make_pair(1, i_ch + 1)] != 0) {
                            h_signal_integr_coincid_[make_pair(1, i_ch + 1)]->Fill(signal_integr_[make_pair(1, i_ch + 1)]);
                        }
                    }
                }



                if (i_ev > n_events_to_Analyze) {
                    break;
                }
            }
            if (i_ev > n_events_to_Analyze) {
                break;
            }

        }

    } catch (evioException e) {
        cerr << endl << e.toString() << endl;
        exit(EXIT_FAILURE);
    }

    gDirectory->Write();

    return 0;
}

