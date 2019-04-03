#include "Hodo_tests_Analyze.C"
#include <iostream>

using namespace std;

int main(int argc, char **argv){

	
	if( argc != 2 ){
	   cout<<"Please provide the run number"<<endl;
	   cout<<"Exiting "<<endl;
	}

	int run = atoi(argv[1]);

	Hodo_tests_Analyze t(run);
	t.Loop();

return 0;
}

