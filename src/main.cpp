#include <iostream>
#include <fstream>
#include "CircuitCmp.h"
#include <sstream>
#include "../lib/tm_usage.h"
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

  cout << "Starting Sim..." << endl;
  if(circuit.Simulation())cout << "Equivalent" << endl;
  else cout << "Non-equivalent" << endl;
  cout << "Ending Simulation..." << endl<<endl;
  circuit.Sat();
  
  for(int i=0; i < 2; ++i){
    circuit.Simulation(1);
    circuit.Sat();
  }
  circuit.CheckResult();
  circuit.WriteFile(argv[3], argv[4]);
  tm_usage.getTotalUsage(tm_stat);
  cout << tm_stat.rTime / 1000.0 << "ms" << endl;
  cout << tm_stat.vmPeak / 1024.0 << "MB" << endl;
}
