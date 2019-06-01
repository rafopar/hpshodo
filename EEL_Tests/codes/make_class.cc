#include <TChain.h>

void make_class()
{
  TChain *ch1 = new TChain("Gain_Calib");
  ch1->Add("../Data/Hodo_tests_349.root/tr1");
  ch1->MakeClass("Gain_Calib");
}
