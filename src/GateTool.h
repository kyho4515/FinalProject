#ifndef FINALOUTPUT
#define FINALOUTPUT
#include "Gate.h"
#include <vector>

class VectorGateAdd{
	public:
		VectorGateAdd(vector<Gate*> in_list){list=in_list;}
		VectorGateAdd(){}
		VectorGateAdd operator+(VectorGateAdd);//operator overloading
		void operator=(VectorGateAdd);//operator overloading =
		void PushBack(Gate*);//將Gate型態的pointer存入
		int GetSize();//return vector<Gate*>output的size
		Gate* GetList(int);//trturn output[i];
		vector<Gate*> GetWholeOutput();//return整個output
		void update(vector<Gate*>);
	private:
		vector<Gate*> list;
};


#endif
