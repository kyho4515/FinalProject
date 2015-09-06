#include <iostream>
#include <fstream>
#include "CircuitCmp.h"
#include <sstream>
#include "../lib/tm_usage.h"
#include <algorithm>
using namespace std;
using namespace CommonNs;

int main(const int argc, const char* argv[]) {
/*if(argc != 5){
    cout << "usage: ./CircuitCut <input_1> <input_2> <output_1> <output_2>" << endl;
    return false;
  }*/	
	
  TmUsage tm_usage;
  TmStat tm_stat;
  tm_usage.totalStart();

  CircuitCmp circuit(argv[1], argv[2]);

	
  /*
  cout << "Starting Strash..." << endl;
  circuit.Strash();
  cout << "Ending Strash..." << endl;
  */

	if(circuit.CircuitEquiCheck()){
		cout << "Equivalent" << endl;
		circuit.SetOutputPhase();
	}
 	else 
		cout << "Non-equivalent" << endl;

	while(true){
		cout << "Starting Sim..." << endl;  
		circuit.RebuiltDFSlist();		
		if(circuit.Simulation()){
			cout << "Ending Simulation..." << endl<<endl;	
			circuit.Sat(1);
		}
		else{
			break;
		}
	}
	cout<<"***************"<<endl;
	if(!circuit.CheckOutputEqualSAT())
		assert(0);
  	circuit.CountScore();
	/*cout<<"SAT end..."<<endl;
  circuit.CheckResult();
  circuit.WriteFile(argv[3], argv[4]);*/
  	tm_usage.getTotalUsage(tm_stat);
  	cout << tm_stat.rTime / 60000000 << "m : "<<(tm_stat.rTime % 60000000) / 1000000 << "s : "<<((tm_stat.rTime % 60000000) % 1000000) / 1000.0 << "ms" << endl;
  	cout << tm_stat.vmPeak / 1024.0 << "MB" << endl;
}
