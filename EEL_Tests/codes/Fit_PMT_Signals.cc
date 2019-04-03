/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   Fit_PMT_Signals.cc
 * Author: rafopar
 *
 * Created on November 1, 2018, 1:03 PM
 */

#include <TF1.h>
#include <TH1D.h>
#include <TH1D.h>
#include <TFile.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <cstdlib>

using namespace std;

double PMT_Responce(double *, double *);
double Sum_Responce(double *, double *);

TF1 *f_PMT_Responce = new TF1("f_PMT_Responce", PMT_Responce, 0., 10000., 10);

/*
 * 
 */
int Fit_PMT_Signals() {

    gStyle->SetOptFit(1);
    TCanvas *c1 = new TCanvas("c1", "", 750, 750);

    const int n_par = 10;
    const int run = 210;
    const int ch_prob = 6;
    const double x_max = 550.;

    TFile *file_in = new TFile(Form("../Data/Analyze_Root_%d.root", run), "Read");

    TH1D *h_signal = (TH1D*) file_in->Get(Form("h_signal_coincid1_0_%d", ch_prob));
    h_signal->SetTitle("; ADC");

    TF1 *f_Sum_Responce = new TF1("f_Sum_Responce", Sum_Responce, -400., x_max, n_par);
    f_Sum_Responce->SetNpx(4500);
    
    const int n = 4;
    double scale = 1.49;
    double mu = 0.028;
    double omega = 0.74;
    double Q0 = 4.5;
    double Q1 = 50;
    double alpha = 2.e-5;
    double sigma0 = 12;
    double sigma1 = 21.;
    double height = 84000;

    f_Sum_Responce->SetParameters(n, mu, omega, Q0, Q1, alpha, sigma0, sigma1, scale, height);

    TF1 * f_sub_peak_[n];

    f_Sum_Responce->FixParameter(0., n);
    f_Sum_Responce->SetParLimits(1, 0.0, 100);
    f_Sum_Responce->SetParLimits(2, 0., 1.);
    f_Sum_Responce->SetParLimits(3, -200, 200);
    f_Sum_Responce->SetParLimits(4, 10, 200);
    f_Sum_Responce->SetParLimits(5, 0.000000001, 1);
    f_Sum_Responce->SetParLimits(6, 0.1, 75);
    f_Sum_Responce->SetParLimits(7, 10, 1000);
    f_Sum_Responce->SetParNames("n", "#mu", "#omega", "Q_{0}", "Q_{1}", "#alpha", "#sigma_{0}", "#sigma_{1}", "scale", "height");
    
    h_signal->Fit(f_Sum_Responce, "MeV", "", -200, x_max);


    double pars[n_par];
    f_Sum_Responce->GetParameters(pars);
    cout << "mean total = " << f_Sum_Responce->Mean(f_Sum_Responce->GetParameter(3), x_max) << endl;
    for (int i = 0; i < n; i++) {
        f_sub_peak_[i] = new TF1(Form("f_sub_peak_%d", i), PMT_Responce, -400., x_max, n_par);
        f_sub_peak_[i]->SetNpx(8000);
        f_sub_peak_[i]->SetParameters(pars);
        f_sub_peak_[i]->SetParameter(0, i);
        f_sub_peak_[i]->SetLineColor(i + 1);
        f_sub_peak_[i]->Draw("Same");

        cout << "mean [" << i << "] = " << f_sub_peak_[i]->Mean(f_sub_peak_[i]->GetParameter(3), x_max) << endl;
    }
    h_signal->SetAxisRange(-100, 900., "X");
    
    c1->Print(Form("Figs/fit_signal_Run_%d.eps", run));
    c1->Print(Form("Figs/fit_signal_Run_%d.pdf", run));
    c1->Print(Form("Figs/fit_signal_Run_%d.png", run));


    return 0;
}

double Sum_Responce(double *xx, double *par) {
    double x = xx[0];
    int n = int(par[0]);

    double value = 0;
    for (int i = 0; i < n; i++) {
        f_PMT_Responce->SetParameters(par);
        f_PMT_Responce->SetParameter(0, i);
        value = value + f_PMT_Responce->Eval(x);
    }
    return value;
}

double PMT_Responce(double *xx, double *par) {
    double scale = par[8];
    double x = xx[0] * scale;
    int n = int(par[0]);
    double mu = par[1];
    double omega = par[2];
    double Q0 = par[3] * scale;
    double Q1 = par[4] * scale;
    double alpha = par[5];
    double sigma0 = par[6] * scale;
    double sigma1 = par[7] * scale;
    double height = par[9];

    const double PI = TMath::Pi();

    double Signal = 0;
    //  for( int n = 1; n < N; n++ )
    {
        double Gn;
        if (n == 0) {
            Gn = (1 / (sigma0 * sqrt(2 * PI))) * exp(-(x - n * Q1 - Q0)*(x - n * Q1 - Q0) / (2 * sigma0 * sigma0));
        } else {
            Gn = (1 / (sigma1 * sqrt(2 * PI * n))) * exp(-(x - n * Q1 - Q0)*(x - n * Q1 - Q0) / (2 * n * sigma1 * sigma1));
        }
        //      cout<<" Gn =  "<<Gn<<endl;
        // cout<<" x = "<<x<<endl;
        // cout<<"(x - n*Q1 - Q0) = "<<(x - n*Q1 - Q0)<<endl;
        // cout<<"(2*n*sigma1*sigma1) = "<<(2*n*sigma1*sigma1)<<endl;

        double Qn = Q0 + n*Q1;
        double sigman = sqrt(sigma0 * sigma0 + n * sigma1 * sigma1);

        double pp1 = Qn + alpha * sigman*sigman;
        double erf1 = TMath::Erf(TMath::Abs((Q0 - pp1)) / (sqrt(2) * sigman));
        double sign1;
        if (x - pp1 > 0) {
            sign1 = 1;
        } else {
            sign1 = -1;
        }

        double erf2 = TMath::Erf(TMath::Abs(x - pp1) / (sigman * sqrt(2)));

        double IGn = (alpha / 2) * exp(-alpha * (x - Qn - alpha * sigman * sigman / 2))*(erf1 + sign1 * erf2);
        // cout<<"alpha = "<<alpha<<endl;
        // cout<<"N = "<<n<<endl;
        // cout<<" x= "<<x<<endl;
        // cout<<" Qn =  "<<Qn<<endl;
        // cout<<" alpha =  "<<alpha<<endl;
        // cout<<" sigman =  "<<sigman<<endl;
        // cout<<"( x - Qn - alpha*sigman*sigman/2 ) = "<<( x - Qn - alpha*sigman*sigman/2 )<<endl;

        Signal = height * (TMath::Power(mu, n) * exp(-mu) / TMath::Factorial(n))*((1 - omega) * Gn + omega * IGn);
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
