#include <evioUtil.hxx>
#include <evioFileChannel.hxx>
#include <list>
#include <bitset>         // std::bitset
#include <iomanip>
#include <vector>

#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TCanvas.h>

#define GET_CHAR(b,i) b[i]; i+=1;
#define GET_SHORT(b,i) ((short *)(&b[i]))[0]; i+=2;
#define GET_INT(b,i) ((int *)(&b[i]))[0]; i+=4;
#define GET_L64(b,i) ((unsigned long long *)(&b[i]))[0]; i+=8;

#define FADC_1_BANK 57601

using namespace evio;
using namespace std;

int main(int argc, char **argv){

  TCanvas *c1 = new TCanvas("c1", "", 750, 750);
  c1->Divide(4, 4);

  TFile *file_out = new TFile("Read_Hodo_test1.root", "Recreate");
  
  TH1D *h_nch1 = new TH1D("h_n_ch1", "", 21, -0.5, 20.5);
  TH1D *h_ns1 = new TH1D("h_n_ns1", "", 111, -0.5, 109.5);
  TH1D *h_FADC_1 = new TH1D("h_FADC_1", "", 106, -0.5, 105.5);

  const int n_tiles = 8;
  const int n_ch = 16;
  const int n_sl = 3;
  const int n_all_slot = 21; // Only 2 slots will be used for HPS
  const int n_tmp_channels = 4;
  const int n_samp = 112;

  int EEL_slots[n_sl] = {3, 4, 5};
  TH1D *h_FADC_1_[n_all_slot][n_ch];

  TH1D *h_peak1_[n_sl][n_ch];
  TH1D *h_signal_integr1_[n_sl][n_ch];
  TH1D *h_signal_integr2_[n_sl][n_ch];
  TH1D *h_pedestal1_[n_sl][n_ch];
  TH1D *h_peak_pos1_[n_sl][n_ch];

  TH2D *h_signal_integr_pair_[n_tiles];
  
  double peak_[n_sl][n_ch];
  double signal_integr_[n_sl][n_ch];
  double pedestal_[n_sl][n_ch];
  double peak_pos_[n_sl][n_ch];
  
  for( int i = 0; i < n_all_slot; i++ ){
    for( int j = 0; j < n_ch; j++ ){
      h_FADC_1_[i][j] = new TH1D(Form("h_FADC_1_%d_%d",  i, j), "", n_samp + 2, -0.5, double(n_samp) + 1.5);
    }
  }

  for( int i_sl = 0; i_sl < n_sl; i_sl++ ){
    for( int i = 0; i < n_ch; i++ ){
      
      h_peak1_[i_sl][i] = new TH1D(Form("h_peak1_%d_%d", i_sl, i), "", 200, 0., 1200.);
      h_signal_integr1_[i_sl][i] = new TH1D(Form("h_signal_integr1_%d_%d", i_sl, i), "", 200, 0., 2500.);
      h_signal_integr2_[i_sl][i] = new TH1D(Form("h_signal_integr2_%d_%d", i_sl, i), "", 200, 0., 2500.);
      h_pedestal1_[i_sl][i] = new TH1D(Form("h_pedestal1_%d_%d", i_sl, i), "", 800, 0., 200.);
      h_peak_pos1_[i_sl][i] = new TH1D(Form("h_peak_pos1_%d_%d", i_sl, i), "", 101, -0.5, 100.5);
    }
  }


  TH2D *h_signal_integrs_LR1 = new TH2D("h_signal_integrs_LR1", "", 200, 0., 2500., 200, 0., 2500.);
  TH2D *h_signal_integrs_LR2 = new TH2D("h_signal_integrs_LR2", "", 200, 0., 2500., 200, 0., 2500.);

  
  for( int i_tile = 0; i_tile < n_tiles; i_tile++ ){
    h_signal_integr_pair_[i_tile] = new TH2D(Form("h_signal_integr_pair_%d", i_tile), "", 200, 0., 2500, 200, 0., 2500.);
  }
  
  if( argc < 2 ){
    cout<<"Please provide the data file name "<<endl;
    cout<<"Exiting"<<endl;
    exit(1);
  }
  
  try{
    evioFileChannel chan(argv[1], "r");
    
    chan.open();


    while( chan.read() ){

      evioDOMTree tree(chan);

      evioDOMNodeListP nodelist1 = tree.getNodeList();
      int node_list_size = nodelist1.get()->size();
 
      for( std::list<evioDOMNode*>::iterator it = nodelist1.get()->begin();  it != nodelist1.get()->end(); it++ ){


	if( (*it)->tag == FADC_1_BANK ){


	  evioCompositeDOMLeafNode *fadc = (evioCompositeDOMLeafNode *)(*it);
	  unsigned char *p = (unsigned char *)fadc->data.data();
	  unsigned int len = fadc->data.size() * 4;

	  int i = 0;
	  int adc[1024];

          cout<<"Kuku1"<<endl;
          
	  while(len-i>4){

	    unsigned char slot = GET_CHAR(p,i);
            int slot_int = int(slot);

	    //cout<<" =========== = = = = = = =       slot = "<<int(slot)<<endl;
	    
	    int trig_n = GET_INT(p,i);
	    long long trig_t = GET_L64(p,i);
	    
	    //cout<<"trig_t = "<<trig_t<<endl;
	    
	    int nch = GET_INT(p,i);
	    h_nch1->Fill(nch);
	    
	    if(nch > 16) { printf("Error in %s: fadc mode 1 invalid nch\n", __func__); exit(1); }
	    
	    while(nch--){

	      int ch = GET_CHAR(p,i);
	      int ns = GET_INT(p,i);
	      int np = 0;				
	      int x, y;

	      //cout<<"channel = "<<ch<<endl;

	      h_ns1->Fill(ns);
	      //cout<<"ns = "<<ns<<endl;
	      //      cout<<" ================= = = = = = = = =  channel = "<<ch<<endl;

	      
	      if(ns > sizeof(adc)/sizeof(adc[0]))
		{
		  printf("Warning in %s: fadc mode 1: # samples (%d) > buffer (%d)\n", __func__, ns, (int)(sizeof(adc)/sizeof(adc[0])));
		  for(int n = 0; n < ns; n++) { GET_SHORT(p,i); }
		  continue;
		}


	      for(int n = 0; n < ns; n++) {
		adc[n] = GET_SHORT(p,i);
		h_FADC_1->Fill(n, adc[n]);
		h_FADC_1_[slot_int][ch]->Fill(n, adc[n]);
		//cout<<" n = "<<n<<"     ADC value = "<<adc[n]<<endl;
	      }
	      
	      
	      
	    }

	    
	  }

	  // for( int ii = 0; ii < 100; ii++ ){
	  //   cout<<h_FADC_1_[3][3]->GetBinContent(ii+1)<<endl;
	  // }
	  // cout<<"  =*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*+*="<<endl;
	  
	  for( int i_sl = 0; i_sl < n_sl; i_sl++ ){

	    int cur_slot = EEL_slots[i_sl];
	    for( int i_ch = 0; i_ch < n_ch; i_ch++ ){
	       c1->cd(i_ch + 1);
	       h_FADC_1_[cur_slot][i_ch]->Draw("hist");
          cout<<"Kuku2"<<endl;

	      pedestal_[i_sl][i_ch] = h_FADC_1_[cur_slot][i_ch]->Integral(1, 10)/10.;
	      
	      peak_[i_sl][i_ch] = h_FADC_1_[cur_slot][i_ch]->GetMaximum() - pedestal_[i_sl][i_ch];
	      peak_pos_[i_sl][i_ch] = h_FADC_1_[cur_slot][i_ch]->GetBinCenter(h_FADC_1_[cur_slot][i_ch]->GetMaximumBin());
	      signal_integr_[i_sl][i_ch] =  h_FADC_1_[cur_slot][i_ch]->Integral() - n_samp*pedestal_[i_sl][i_ch];

	      if( pedestal_[i_sl][i_ch] == 0 ) {continue;}
              cout<<peak_[i_sl][i_ch]<<endl;
              cout<<h_peak1_[i_sl][i_ch]<<endl;
              cout<<"i_sl = "<<i_sl<<"    i_ch = "<<i_ch<<endl;
	      //h_peak1_[0][0]->Fill( peak_[i_sl][i_ch] );
	    //  h_peak1_[i_sl][i_ch]->Fill( peak_[i_sl][i_ch] );
              cout<<"Kuku3"<<endl;
	      h_signal_integr1_[i_sl][i_ch]->Fill(signal_integr_[i_sl][i_ch]);
	      h_pedestal1_[i_sl][i_ch]->Fill(pedestal_[i_sl][i_ch]);

	      h_peak_pos1_[i_sl][i_ch]->Fill(peak_pos_[i_sl][i_ch]);
	    }
	     c1->Print("tmp.eps");
	     cin.ignore();
	  }

	  for( int i_sl = 0; i_sl < n_sl; i_sl++ ){
	    int cur_slot = EEL_slots[i_sl];
	    for( int i_ch = 0; i_ch < n_ch; i_ch++ ){
	      h_FADC_1_[cur_slot][i_ch]->Reset();
	    }
	  }

	  h_signal_integrs_LR1->Fill(signal_integr_[0][1], signal_integr_[0][2]);
	  if( signal_integr_[1][15] > 160. && signal_integr_[1][14] > 100. ){
	    h_signal_integrs_LR2->Fill(signal_integr_[0][1], signal_integr_[0][2]);
	  }
	  

	  // ==== I want to check distributions distributions of channels from the 1st PMT, when ===
	  // ==== both trigger PMTs are fired                                                    ===

	  for( int i_ch = 0; i_ch < n_ch; i_ch++ ){

	    if( signal_integr_[0][i_ch] < 1 ){continue;}
	    if( signal_integr_[1][15] > 160. && signal_integr_[1][14] > 100. ){
	      
	      h_signal_integr2_[0][i_ch]->Fill(signal_integr_[0][i_ch]);
	    }
	  }


	  // ==== We want to check channels that are coming from the same tile ====
	  for( int i_tile = 0; i_tile < n_tiles; i_tile++ ){
	    if( signal_integr_[0][2*i_tile] || signal_integr_[0][2*i_tile + 1] ){
	      h_signal_integr_pair_[i_tile]->Fill(signal_integr_[0][2*i_tile], signal_integr_[0][2*i_tile + 1]);
	    }
	  }
	}
	
      }
      
    }

    gDirectory->Write("hist_Dir", TObject::kOverwrite);
    file_out->Close();

  } catch(evioException e){
    cerr<<endl<<e.toString()<<endl;
    exit(EXIT_FAILURE);
  }



  exit(EXIT_SUCCESS);
  
  
}
