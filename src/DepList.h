#ifndef DEPLIST
#define DEPLIST
#include "Gate.h"

class DepList{
	public:
		DepList();
		~DepList();
		void　Add(Gate*,Gate*);//兩個Gate是dependent
		bool Check(Gate*,Gate*);//檢查兩個Gate是不是dependent
	private:
		vector<vector<Gate*>*>;
};

#endif DEPLIST
