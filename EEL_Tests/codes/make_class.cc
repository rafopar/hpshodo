#include <TChain.h>

void make_class()
{
  TChain *ch1 = new TChain("Fill_SPE_hists");
  ch1->Add("../Data/Hodo_tests_329.root/tr1");
  ch1->MakeClass("Fill_SPE_hists");
}
