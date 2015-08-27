#include "GateTool.h"

//operator overloading +
VectorGateAdd VectorGateAdd::operator+(VectorGateAdd s){
	VectorGateAdd ret;
	vector<Gate*> tem,s1=list,s2=s.GetWholeOutput();
	while(s1.size()!=0 || s2.size()!=0){		
		if(s2.size()==0 || (s1.size()!=0 && s1.back()->idd>s2.back()->idd)){
			tem.push_back(s1.back());
			s1.pop_back();
		}						
		else if(s1.size()==0 || (s2.size()!=0 && s1.back()->idd<s2.back()->idd)){
			tem.push_back(s2.back());
			s2.pop_back();
		}
		else{
			tem.push_back(s2.back());
			s1.pop_back();
			s2.pop_back();
		}
	}
	while(tem.size()!=0){
		ret.PushBack(tem.back());
		tem.pop_back();
	}
	return ret;

}

//operator overloading =
void VectorGateAdd::operator=(VectorGateAdd s){
		list.clear();
		for(int i=0;i<s.GetSize();i++){
			list.push_back(s.GetList(i));
		}
}
//將Gate型態的pointer存入
void VectorGateAdd::PushBack(Gate* g){
	list.push_back(g);
}

//return vector<Gate*>output的size
int VectorGateAdd::GetSize(){
	return list.size();
}

//return output[i]
Gate* VectorGateAdd::GetList(int n){
	return list[n];
}

//return整個output
vector<Gate*> VectorGateAdd::GetWholeOutput(){
	return list;	
}


void VectorGateAdd::update(vector<Gate*> newdata){
	list=newdata;
}


