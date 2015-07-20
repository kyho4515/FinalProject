#include "sat.h"
#include <iostream>
using namespace std;

int main(){
  SatSolver s;
  s.initialize();
  Var v = s.newVar();
  Var v1 = s.newVar();
  Var v2 = s.newVar();
  Var v3 = s.newVar();
  Var v4 = s.newVar();
  Var v5 = s.newVar();
  Var v6 = s.newVar();
  s.addAigCNF(v6, v2, true, v5, false);
  s.addAigCNF(v3, v1, false, v6, false);
  s.addAigCNF(v, v1, false, v2, true);
  s.addXorCNF(v4, v3, false, v, false);
  s.assumeProperty(v4, true);
  s.assumeProperty(v5, true);
  cout << !s.assumpSolve() << endl;
}
