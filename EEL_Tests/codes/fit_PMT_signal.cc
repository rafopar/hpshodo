#include <TF1.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TFile.h>
#include <TMath.h>
#include <TStyle.h>
#include <TCanvas.h>

double PMT_Responce(double *, double *);
double Sum_Responce(double *, double *);

TF1 *f_PMT_Responce = new TF1("f_PMT_Responce", PMT_Responce, 0., 10000., 10);

void fit_PMT_signal()
{
  gStyle->SetOptFit(1);
  TCanvas *c1 = new TCanvas("c1", "", 750, 750);

  const int n_par = 10;
  const int run = 135;
  const double x_max = 2900.;
  //TFile *file_in = new TFile("../histograms_run_403_short2.root", "Read");
  // TFile *file_in = new TFile(Form("../HVStabilityTest/histograms_run_%d.root", run), "Read");
  //ile *file_in = new TFile(Form("../histograms_run_%d.root", run), "Read");
  //TFile *file_in = new TFile(Form("histograms_run_%d_0.root", run), "Read"); //////////////////////////////
  TFile *file_in = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

  TF1 *f_Sum_Responce = new TF1("f_Sum_Responce", Sum_Responce, -400., x_max, n_par);
  f_PMT_Responce->SetNpx(4500); // Nkarelu keteri qanaky

  TH1D *h_signal2_sl3_ch2 = (TH1D*)file_in->Get("h_signal_coincid1_0_5");
  // TH1D *h_signal1_sl3_ch2 = (TH1D*)file_in->Get("h_signal1_sl3_ch2");
  // TH1D *h_pedestal = (TH1D*)file_in->Get("h_pedestal_slot3_channel2");
  //TH1D *h_total1_sl3_ch6 = (TH1D*)file_in->Get("h_total1_sl3_ch6"); /////////////
  
  const int n = 8;
  //double scale = 2.5e-12;
  double scale = 1.;
  double mu = 0.1;
  double omega = .484;
  double Q0 = 0.;
  //double Q0 = h_pedestal->GetMean();
  double Q1 = 50;
  double alpha = 0.001;
  //double sigma0 = 10.02;
  double sigma0 = 405;
  //sigma0 = h_pedestal->GetRMS();
  double sigma1 = 100.;
  double height = 300000;


  f_Sum_Responce->SetParameters(n, mu, omega, Q0, Q1, alpha, sigma0, sigma1, scale, height);
  f_Sum_Responce->Draw();
  
  TF1 *f_sub_peak_[n]; // Qarakusi pakagits -> Massiv  

  f_Sum_Responce->FixParameter(0., n);
  f_Sum_Responce->SetParLimits(1, 0.0, 100);
  //f_Sum_Responce->FixParameter(1, 1.91);
  f_Sum_Responce->SetParLimits(2, 0., 1.);
  //f_Sum_Responce->FixParameter(2,0.393);
  f_Sum_Responce->SetParLimits(3, -200, 200);
  //f_Sum_Responce->FixParameter(3,2232);
  f_Sum_Responce->SetParLimits(4, 10, 1000);
  //f_Sum_Responce->FixParameter(4,250);
  f_Sum_Responce->SetParLimits(5, 0.001, 1);
  //f_Sum_Responce->FixParameter(5,0.005);
  f_Sum_Responce->SetParLimits(6, 0.1, 75);
  //f_Sum_Responce->FixParameter(6,8.949); 
  f_Sum_Responce->SetParLimits(7, 20, 1000);
  //f_Sum_Responce->FixParameter(7,267);
  
  h_signal2_sl3_ch2->Fit(f_Sum_Responce, "MeV", "", -200, x_max);
  //h_total1_sl3_ch6->Fit(f_Sum_Responce, "MeV", "", 2100, 5600);  ///////////////
  
  double pars[n_par];
  f_Sum_Responce->GetParameters(pars);
  cout<<"mean total = "<<f_Sum_Responce->Mean(f_Sum_Responce->GetParameter(3), x_max)<<endl;
  for( int i = 0; i < n; i++ )
    {
      f_sub_peak_[i] = new TF1(Form("f_sub_peak_%d", i), PMT_Responce, -400., x_max, n_par);
      f_sub_peak_[i]->SetNpx(8000);
      f_sub_peak_[i]->SetParameters(pars);
      f_sub_peak_[i]->SetParameter(0, i);
      f_sub_peak_[i]->SetLineColor(i+1);
      f_sub_peak_[i]->Draw("Same");
      
      cout<<"mean ["<<i<<"] = "<<f_sub_peak_[i]->Mean(f_sub_peak_[i]->GetParameter(3), x_max)<<endl;
    }
  //c1->Print(Form("fit_signal2_%d.eps", run));
  c1->Print(Form("fit_signal2_%d.pdf", run));
  //c1->Print(Form("fit_signal2_%d_8.pdf", run));    ////////////////////////////////////////////////////////
  //c1->Print(Form("fit_signal2_%d.gif", run));
}


double Sum_Responce(double *xx, double *par)
{
  double x = xx[0];
  int n = int(par[0]);
  
  double value = 0;
  for( int i = 0; i < n; i++ )
    {
      f_PMT_Responce->SetParameters(par);
      f_PMT_Responce->SetParameter(0, i);
      value = value + f_PMT_Responce->Eval(x);
    }
  return value;
}

double PMT_Responce(double *xx, double *par)
{
  double scale = par[8];
  double x = xx[0]*scale;
  int n = int(par[0]);
  double mu = par[1];
  double omega = par[2];
  double Q0 = par[3]*scale;
  double Q1 = par[4]*scale;
  double alpha = par[5];
  double sigma0 = par[6]*scale;
  double sigma1 = par[7]*scale;
  double height = par[9];
  
  const double PI = TMath::Pi();

  double Signal = 0;
  //  for( int n = 1; n < N; n++ )
    {
      double Gn;
      if( n == 0 )
	{Gn = (1/(sigma0*sqrt(2*PI)))*exp( -(x - n*Q1 - Q0)*(x - n*Q1 - Q0)/(2*sigma0*sigma0) ) ; }
      else
	{
	  Gn = (1/(sigma1*sqrt(2*PI*n)))*exp( -(x - n*Q1 - Q0)*(x - n*Q1 - Q0)/(2*n*sigma1*sigma1) );
	}
      //      cout<<" Gn =  "<<Gn<<endl;
      // cout<<" x = "<<x<<endl;
      // cout<<"(x - n*Q1 - Q0) = "<<(x - n*Q1 - Q0)<<endl;
      // cout<<"(2*n*sigma1*sigma1) = "<<(2*n*sigma1*sigma1)<<endl;
      
      double Qn = Q0 + n*Q1;
      double sigman = sqrt(sigma0*sigma0 + n*sigma1*sigma1);
      
      double pp1 = Qn + alpha*sigman*sigman;
      double erf1 = TMath::Erf(TMath::Abs( (Q0 - pp1))/(sqrt(2)*sigman) );
      double sign1;
      if( x - pp1 > 0)
	{
	  sign1 = 1;
	}
      else
	{
	  sign1 = -1;
	}
      
      double erf2 = TMath::Erf( TMath::Abs( x - pp1 )/(sigman*sqrt(2)) );

      double IGn = (alpha/2)*exp( -alpha*( x - Qn - alpha*sigman*sigman/2 ) )*( erf1 + sign1*erf2 );
      // cout<<"alpha = "<<alpha<<endl;
      // cout<<"N = "<<n<<endl;
      // cout<<" x= "<<x<<endl;
      // cout<<" Qn =  "<<Qn<<endl;
      // cout<<" alpha =  "<<alpha<<endl;
      // cout<<" sigman =  "<<sigman<<endl;
      // cout<<"( x - Qn - alpha*sigman*sigman/2 ) = "<<( x - Qn - alpha*sigman*sigman/2 )<<endl;
      
      Signal = height*(TMath::Power(mu, n)*exp(-mu)/TMath::Factorial(n))*( (1 - omega)*Gn + omega*IGn);
      //cout<<"mean "Signal" = "<<Signal->Mean(Signal->GetParameter(3), x_max)<<endl;

      //Signal = height*(TMath::Power(mu, n)*exp(-mu)/TMath::Factorial(n))*Gn;

      //Signal = height*(TMath::Power(mu, n)*exp(-mu)/TMath::Factorial(n))*Gn;
      // cout<<" erf1= "<<erf1<<endl;
      // cout<<" erf2= "<<erf1<<endl;
      // cout<<" IGn = "<<IGn<<endl;
      // cout<<"Signal = "<<Signal<<endl;
    }
  
  return Signal;
}


