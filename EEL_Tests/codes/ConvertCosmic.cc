/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   ConvertCosmic.cc
 * Author: rafopar
 *
 * Created on May 1, 2019, 5:36 PM
 */

#include <cstdlib>
#include <iostream>
#include <evioUtil.hxx>
#include <evioFileChannel.hxx>
#include <list>

#include <ap_int.h>
#include <ap_fixed.h>

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
#define TOP_CRATE_TAG 39
#define BOT_CRATE_TAG 37
#define TI_CRATE_TAG 46
#define TI_TAG 57610

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

    TCanvas *c1 = new TCanvas();
    
    string strrunNo = argv[1];
    int runNo = atoi(argv[1]);

    int n_samples;
    const int NSA_Hodo = 10;
    const int NSB_Hodo = 6;
    const int NSA_ECal = 18;
    const int NSB_ECal = 6;
    const int hodo_slot = 10;
    const int n_ch = 16;
    const int n_sl = 15;  // 14 ECal slots + 1 Hodo slot
    const int n_samp = 96; // # of FADC samples
    
    const int BADVAL = -99999;
    
    int n_run_dgts = int(TMath::Log10(runNo)) + 1; // This will tell how many digits have the run number  
    
    int n_of_channels = n_ch;

    int sl_numbers_[n_sl] = {3, 4, 5, 6, 7, 8, 9, 10, 14, 15, 16, 17, 18, 19, 20};
    
    map<int, int> ind_sl; //
    for( int i_sl = 0; i_sl < n_sl; i_sl++ ){
        ind_sl[sl_numbers_[i_sl]] = i_sl;
    }
    
    double top_signal_[n_ch*n_sl];
    double top_peak_[n_ch*n_sl];
    double top_peak_pos_[n_ch*n_sl];
    double top_pedestal_[n_ch*n_sl];

    double bot_signal_[n_ch*n_sl];
    double bot_peak_[n_ch*n_sl];
    double bot_peak_pos_[n_ch*n_sl];
    double bot_pedestal_[n_ch*n_sl];
    

    TFile *file_out = new TFile(Form("../Data/ECalHodo_%d.root", runNo), "Recreate");
    
    TH1D *h_tmp = new TH1D("h_tmp", "", n_samp + 2, -0.5, double(n_samp) + 1.5);
    TH1D *h_top_FADC_[n_ch*n_sl];
    TH1D *h_bot_FADC_[n_ch*n_sl];
    
    TTree *tr1 = new TTree("tr1", "ECal and Hodo Signals");

    tr1->Branch("n_ch", &n_of_channels, "n_ch/I");
    
    for (int i_sl = 0; i_sl < n_sl; i_sl++) {

        tr1->Branch(Form("top_signal_%d_", sl_numbers_[i_sl]), &top_signal_[i_sl*n_ch], Form("top_signal_%d[n_ch]/D", sl_numbers_[i_sl]));
        tr1->Branch(Form("top_peak_%d_", sl_numbers_[i_sl]), &top_peak_[i_sl*n_ch], Form("top_peak_%d[n_ch]/D", sl_numbers_[i_sl]));
        tr1->Branch(Form("top_peak_pos_%d_", sl_numbers_[i_sl]), &top_peak_pos_[i_sl*n_ch], Form("top_peak_pos_%d[n_ch]/D", sl_numbers_[i_sl]));
        tr1->Branch(Form("top_pedestal_%d_", sl_numbers_[i_sl]), &top_pedestal_[i_sl*n_ch], Form("top_pedestal_%d[n_ch]/D", sl_numbers_[i_sl]));

        tr1->Branch(Form("bot_signal_%d_", sl_numbers_[i_sl]), &bot_signal_[i_sl*n_ch], Form("bot_signal_%d[n_ch]/D", sl_numbers_[i_sl]));
        tr1->Branch(Form("bot_peak_%d_", sl_numbers_[i_sl]), &bot_peak_[i_sl*n_ch], Form("bot_peak_%d[n_ch]/D", sl_numbers_[i_sl]));
        tr1->Branch(Form("bot_peak_pos_%d_", sl_numbers_[i_sl]), &bot_peak_pos_[i_sl*n_ch], Form("bot_peak_pos_%d[n_ch]/D", sl_numbers_[i_sl]));
        tr1->Branch(Form("bot_pedestal_%d_", sl_numbers_[i_sl]), &bot_pedestal_[i_sl*n_ch], Form("bot_pedestal_%d[n_ch]/D", sl_numbers_[i_sl]));
        
        for( int i_ch = 0; i_ch < n_ch; i_ch++ ){
            h_top_FADC_[i_sl*n_ch + i_ch] = new TH1D(Form("h_top_FADC_%d_%d", sl_numbers_[i_sl], i_ch), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
            h_bot_FADC_[i_sl*n_ch + i_ch] = new TH1D(Form("h_bot_FADC_%d_%d", sl_numbers_[i_sl], i_ch), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
        }
    }
    
        
    string fname_base = "../Data/hpsecal_000000";
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

                // Just in case assign all variables to a value, in order them to not pick up value from the previous
                // event, if there is not 
                for (int ii = 0; ii < n_ch * n_sl; ii++) {
                    top_signal_[ii] = BADVAL;
                    top_peak_[ii] = BADVAL;
                    top_peak_pos_[ii] = BADVAL;
                    top_pedestal_[ii] = BADVAL;

                    bot_signal_[ii] = BADVAL;
                    bot_peak_[ii] = BADVAL;
                    bot_peak_pos_[ii] = BADVAL;
                    bot_pedestal_[ii] = BADVAL;
                }

                bool top_crate = false;
                bool bot_crate = false;
                bool TI_Crate = false;
                int cur_crate = -1;
                for (std::list<evioDOMNode*>::iterator it = nodelist1.get()->begin(); it != nodelist1.get()->end(); it++) {

                    if ((*it)->tag > 0 && (*it)->tag < 200) {
                        cur_crate = (*it)->tag;
                    }

                    if (cur_crate == TOP_CRATE_TAG) {
                        top_crate = true;
                        bot_crate = false;
                    } else if ( cur_crate == BOT_CRATE_TAG) {
                        bot_crate = true;
                        top_crate = false;
                    } else if( (*it)->tag == TI_TAG && cur_crate == TI_CRATE_TAG){
                        vector<ap_int<32> > *data_values = (vector<ap_int < 32 >> *)(*it)->getVector<uint32_t>();
                        vector<ap_int<32> >::iterator it_values = data_values->begin();
                        advance(it_values, 4);


                        if (*it_values != 32768) {
                            cout << "Trg = " << *it_values << endl;
                        }
                    }


                    if ((*it)->tag == FADC_1_BANK) {

                        evioCompositeDOMLeafNode *fadc = (evioCompositeDOMLeafNode *) (*it);
                        unsigned char *p = (unsigned char *) fadc->data.data();
                        unsigned int len = fadc->data.size() * 4;

                        int i = 0;
                        int adc[1024];

                        while (len - i > 4) {

                            unsigned char slot = GET_CHAR(p, i);
                            //cout<<"SLOT = "<<slot<<endl;
                            int sl_int = int(slot);
                            //cout<<"SLOTINT = "<<sl_int<<endl;

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

                                int index = n_ch * ind_sl[sl_int] + ch;

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

                                    h_tmp->Fill(n, adc[n]);

                                    if (top_crate) {
                                        h_top_FADC_[index]->Fill(n, adc[n]);
                                    } else if (bot_crate) {
                                        h_bot_FADC_[index]->Fill(n, adc[n]);

                                    }
                                }

                                if (!(top_crate || bot_crate)) {

                                    cout << "Unknown crate..." << endl;
                                    cout << "Exiting" << endl;
                                    exit(1);
                                }


                                
                                //cout<<"Top_ctate = "<<top_crate<<"     Bot crate = "<<bot_crate<<endl;

                                int max_bin = h_tmp->GetMaximumBin();
                                double ped;

                                // Make sure the peak is not at the beginning of thw window
                                if (max_bin > 10) {
                                    ped = h_tmp->Integral(1, 10) / 10.;
                                } else {
                                    ped = h_tmp->Integral(h_tmp->GetNbinsX() - 13, h_tmp->GetNbinsX() - 4) / 10.;
                                }

                                double peak = h_tmp->GetMaximum() - ped;
                                double peak_pos = h_tmp->GetBinCenter(h_tmp->GetMaximumBin());

                                int NSA, NSB;
                                if (sl_int == hodo_slot) {
                                    NSA = NSA_Hodo;
                                    NSB = NSB_Hodo;
                                } else {
                                    NSA = NSA_ECal;
                                    NSB = NSB_ECal;
                                }
                                
                                if( max_bin + NSA > n_samp ){
                                    NSA = n_samp - max_bin;
                                }
                                
                                double signal = h_tmp->Integral(max_bin - NSB, max_bin + NSA) - ped * double(NSA + NSB + 1);


                                //                                cout<<"sl_int = "<<sl_int<<endl;
                                //                                cout<<"ins_sl = "<<ind_sl[sl_int]<<endl;
                                //                                cout<<"index = "<<index<<endl;

                                
//                                cout<<"nch = "<<nch<<endl;
//                                cout<<"Slot = "<<sl_int<<"   Signal = "<<signal<<endl;
                                if (top_crate) {
                                    top_signal_[index] = signal;
                                    top_peak_[index] = peak;
                                    top_peak_pos_[index] = peak_pos;
                                    top_pedestal_[index] = ped;
                                //    cout<<"Filling top crate data"<<endl;
                                } else {
                                    bot_signal_[index] = signal;
                                    bot_peak_[index] = peak;
                                    bot_peak_pos_[index] = peak_pos;
                                    bot_pedestal_[index] = ped;
//                                    cout<<"Filling bot crate data"<<endl;
//                                    
//                                    cout<<"Signal = "<<signal<<endl;
                                }

//                                if( top_crate && slot == 20 && signal > 500 ){
//                                    h_tmp->Draw("hist");
//                                    c1->Print("tmp.eps");
//                                    cin.ignore();
//                                }
//                                
                                h_tmp->Reset();
                            }



                        }



                    }

                }

                tr1->Fill();
            }

        }

    } catch (evioException e) {
        cerr << endl << e.toString() << endl;
        exit(EXIT_FAILURE);
    }




    //tr1->Write();
    gDirectory->Write();
    file_out->Close();
    return 0;
}

