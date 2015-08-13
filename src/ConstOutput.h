#ifndef CONSTOUTPUT
#define CONSTOUTPUT

#include "Gate.h"

class ConstOutput{
	public:
		ConstOutput(vector<Gate*>);
		~ConstOutput();
		
	private:
		vector<Gate*> constoutput;




};
#endif
