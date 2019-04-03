/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConverRoot.cc
 * Author: rafopar
 *
 * Created on October 23, 2018, 3:47 PM
 */

#include <cstdlib>
#include <iostream>
#include <evioUtil.hxx>
#include <evioFileChannel.hxx>
#include <list>

#include <TH1D.h>
#include <TFile.h>
#include <TMath.h>
#include <TChain.h>
#include <TCanvas.h>
#include <TObjArray.h>

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

    //TCanvas *c1 = new TCanvas();
    
    string strrunNo = argv[1];
    int runNo = atoi(argv[1]);

    int n_samples;
    const int NSA = 10;
    const int NSB = 8;
    const int n_ch = 16;
    const int n_sl = 3;
    const int n_samp = 112; // # of FADC samples
    int n_run_dgts = int(TMath::Log10(runNo)) + 1; // This will tell how many digits have the run number  

    double signal_1_[n_ch];
    double signal_2_[n_ch];
    double signal_3_[n_ch];
    double peak_1_[n_ch];
    double peak_2_[n_ch];
    double peak_3_[n_ch];
    double peak_pos_1_[n_ch];
    double peak_pos_2_[n_ch];
    double peak_pos_3_[n_ch];
    double pedestal_1_[n_ch];
    double pedestal_2_[n_ch];
    double pedestal_3_[n_ch];

    TH1D * h_Hodo_fADC_[n_sl][n_ch];

    for (int isl = 0; isl < n_sl; isl++) {
        for (int ich = 0; ich < n_ch; ich++) {
            h_Hodo_fADC_[isl][ich] = new TH1D(Form("h_Hodo_fADC_%d_%d", isl, ich), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
        }

    }

    TFile *file_out = new TFile(Form("../Data/Hodo_tests_%d.root", runNo), "Recreate");

    TTree *tr1 = new TTree("tr1", "EEL Test signals");
    tr1->Branch("signal_1_", &signal_1_, Form("signal_1_[%d]/D", n_ch));
    tr1->Branch("signal_2_", &signal_2_, Form("signal_2_[%d]/D", n_ch));
    tr1->Branch("signal_3_", &signal_3_, Form("signal_3_[%d]/D", n_ch));
    tr1->Branch("peak_1_", &peak_1_, Form("peak_1_[%d]/D", n_ch));
    tr1->Branch("peak_2_", &peak_2_, Form("peak_2_[%d]/D", n_ch));
    tr1->Branch("peak_3_", &peak_3_, Form("peak_3_[%d]/D", n_ch));
    tr1->Branch("peak_pos_1_", &peak_pos_1_, Form("peak_pos_1_[%d]/D", n_ch));
    tr1->Branch("peak_pos_2_", &peak_pos_2_, Form("peak_pos_2_[%d]/D", n_ch));
    tr1->Branch("peak_pos_3_", &peak_pos_3_, Form("peak_pos_3_[%d]/D", n_ch));
    tr1->Branch("pedestal_1_", &pedestal_1_, Form("pedestal_1_[%d]/D", n_ch));
    tr1->Branch("pedestal_2_", &pedestal_2_, Form("pedestal_2_[%d]/D", n_ch));
    tr1->Branch("pedestal_3_", &pedestal_3_, Form("pedestal_3_[%d]/D", n_ch));

    //string fname_base = "../Data/test1_000000";
    string fname_base = "../Data/hpshodo_000160";
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

    try {

        for (int ifile = 0; ifile < n_files; ifile++) {

            string cur_fname = fname_list->At(ifile)->GetTitle();

            evioFileChannel chan(cur_fname, "r");

            cout << "Analyzing the file " << cur_fname << endl;

            chan.open();

            while (chan.read()) {

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

                                int hodo_d = int(slot) - 3; // Slots are 3 and 4

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

                                n_samples = ns;
                                for (int n = 0; n < ns; n++) {
                                    adc[n] = GET_SHORT(p, i);
                                    //cout<<" n = "<<n<<"     ADC value = "<<adc[n]<<endl;

                                    h_Hodo_fADC_[hodo_d][ch]->Fill(n, adc[n]);
                                }



                            }



                        }


                    }

	

                }


                //<<< ==== >>>
		
                for (int idet = 0; idet < n_sl; idet++) {
		  
                    for (int ich = 0; ich < n_ch; ich++) {
                        double ped;
                        double peak;
                        double peak_pos;
                        double signal;


                        if (h_Hodo_fADC_[idet][ich]->GetEntries() > 0) {
                            //ped = h_Hodo_fADC_[idet][ich]->Integral(81, 90) / 10.;
                            ped = h_Hodo_fADC_[idet][ich]->Integral(1, 10) / 10.;
                            peak = h_Hodo_fADC_[idet][ich]->GetMaximum() - ped;
                            int max_bin = h_Hodo_fADC_[idet][ich]->GetMaximumBin();
                            peak_pos = h_Hodo_fADC_[idet][ich]->GetBinCenter(h_Hodo_fADC_[idet][ich]->GetMaximumBin());
                            

                            if (peak_pos < NSB) {
                                signal = h_Hodo_fADC_[idet][ich]->Integral(1, NSB + NSA + 1) - ped * (NSA + NSB + 1);
                            } else if (peak_pos > n_samples - NSA) {
                                signal = h_Hodo_fADC_[idet][ich]->Integral(1, NSB + NSA + 1) - ped * (NSA + NSB + 1);
                            } else {
                                signal = h_Hodo_fADC_[idet][ich]->Integral(max_bin - NSB, max_bin + NSA) - ped * (NSA + NSB + 1);
                            }
                            
                            //cout<<"nsamples = "<<n_samples<<endl;
                            //signal = h_Hodo_fADC_[idet][ich]->Integral() - ped*n_samp;
                        } else {
                            ped = 0;
                            peak = 0;
                            peak_pos = 0;
                            signal = 0;
                        }

                        if (idet == 0) {
                            pedestal_1_[ich] = ped;
                            peak_1_[ich] = peak;
                            peak_pos_1_[ich] = peak_pos;
                            signal_1_[ich] = signal;
                        } else if (idet == 1) {
                            pedestal_2_[ich] = ped;
                            peak_2_[ich] = peak;
                            peak_pos_2_[ich] = peak_pos;
                            signal_2_[ich] = signal;
                        } else if (idet == 2) {
                            pedestal_3_[ich] = ped;
                            peak_3_[ich] = peak;
                            peak_pos_3_[ich] = peak_pos;
                            signal_3_[ich] = signal;
                        } else {
                            cout << "======= WRONG SLOT ======" << endl;
                            cout << " ===== EXITING " << endl;
                            exit(1);
                        }


//                        if (signal < -50) {
//                            h_Hodo_fADC_[idet][ich]->Draw("hist");
//                            cout<<" Peakpos =  "<<peak_pos<<endl;
//                            c1->Print("tmp.eps");
//                            cin.ignore();
//                        }
                        h_Hodo_fADC_[idet][ich]->Reset();
                        
                    }

                }


                tr1->Fill();
            }

        }


    } catch (evioException e) {
        cerr << endl << e.toString() << endl;
        exit(EXIT_FAILURE);
    }

    gDirectory->Write();


    return 0;
}

