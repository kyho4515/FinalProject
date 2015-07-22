/*
	6/28修改
	bool CircuitCmp::CheckGoodCut(Gate* x, Gate* y)		
	
	6/28新增
	bool CircuitCmp::CheckInputEqual1(Gate* x, Gate* y)
	bool CircuitCmp::CheckInputEqual2(Gate* x, Gate* y)
	void ModifyInput(Gate*);
	void Replace(Gate* gate)
*/

#include "CircuitCmp.h"

bool CircuitCmp::CheckGoodCut(Gate* x, Gate* y){
  for(int i=0; i < circuitOne -> input.size(); ++i){
    circuitOne -> input[i] -> included = false;
    circuitTwo -> input[i] -> included = false;
  }
  for(int i=0; i < cutSet.size(); ++i){
    for(int j=0; j < cutSet[i] -> size(); ++j)
      cutSet[i] -> at(j) -> included = false;
  }
  bool flag = false;
  for(int i=0; i < x -> output.size(); ++i){
    if(x -> output[i] -> gateType != Output)
      flag = true;
  }
  if(!flag) return false;
  flag = false;
  for(int i=0; i < y -> output.size(); ++i){
    if(y -> output[i] -> gateType != Output)
      flag = true;
  }
  if(!flag) return false;
  int inputNumX=0, inputNumY=0;
  
////////////////////////////////////////////6/28修正
	return CheckInputEqual1(x,y);
	//return CheckInputEqual2(x,y);
	
	
/////////////////////////////////////////////////////

}

void CircuitCmp::CountInput(Gate* source, vector<Gate*>& list){
  if(source -> gateType == Input){
    source -> included = true;
    list.push_back(source);
  }
  for(int i = 0; i < source -> input.size(); ++i){
    if(!(source -> input[i] -> traversed)){
      if(source -> input[i] -> gateType == Wir && ((Wire*)(source -> input[i])) -> isCut()){
        source -> input[i] -> traversed = true;
        source -> input[i] -> included = true;
        list.push_back(source -> input[i]);
      }
      else {
        source -> input[i] -> traversed = true;
        CountInput(source -> input[i], list);
      }
    }
  }
}
void CircuitCmp::DFSearch(Gate* source, vector<Gate*>& dfsList){
  for(int i = 0; i < source -> input.size(); ++i){
    if(!(source -> input[i] -> traversed)){
      if(source -> input[i] -> gateType == Wir && ((Wire*)(source -> input[i])) -> isCut()){
        source -> input[i] -> traversed = true;
        dfsList.push_back(source -> input[i]);
      }
      else {
        source -> input[i] -> traversed = true;
        DFSearch(source -> input[i], dfsList);
      }
    }
  }
  dfsList.push_back(source);
}

/*bool CircuitCmp::CheckInputEqual2(Gate* x, Gate* y){
	if(x->total_input.size()!= y->total_input.size() ||x->total_cut.size()!=y->total_cut.size() )
    return false;
 	for(int i=0;i<x->total_input.size();++i){
		if(x->total_input[i]->name!=y->total_input[i]->name)
			return false;
	}
	
	for(int i=0;i<x->total_cut.size();++i)
		if(x->total_cut[i]->name!=y->total_cut[i]->name)
			return false;
	return true;
}*/

bool CircuitCmp::CheckInputEqual1(Gate*x,Gate*y){
  vector<Gate*> inputX, inputY;
  CountInput(x, inputX);
  CountInput(y, inputY);

  circuitOne -> resetTraversed();
  circuitTwo -> resetTraversed();
  
  map<string, Gate*> inputMap;
  map<int, Gate*> cutMap;
  for(int i=0; i < inputX.size(); ++i){
    if(inputX[i] -> gateType == Input)
      inputMap.insert(pair<string, Gate*>(inputX[i] -> name, inputX[i]));
    else{
      cutMap.insert(pair<int, Gate*>(((Wire*)inputX[i]) -> cutNum, inputX[i]));
    }
  }
  for(int i=0; i < inputY.size(); ++i){
    if(inputY[i] -> gateType == Input){
      if(inputMap.find(inputY[i] -> name) == inputMap.end())
        return false;
    }
    else{
      if(cutMap.find(((Wire*)inputY[i]) -> cutNum) == cutMap.end())
        return false;
    }
  }
  return true;
}

/*void CircuitCmp::ModifyInput(Gate* source){
	for(int i=0;i<source->output.size();++i){
		if(!(source->output[i]->traversed)){
			source->output[i]->traversed=true;
			//cout<<"start replace...:"<<source->output[i]->name<<endl;
			Replace(source->output[i]);
			//cout<<"end replace...:"<<source->output[i]->name<<endl;
			ModifyInput(source->output[i]);
		}
	}
}*/

/*void CircuitCmp::Replace(Gate* gate){
	bool full=false;	
	vector<string>tem_string;
	vector<int>tem_int;	
	//cout<<"start replace input..."<<endl;
	gate->total_input=gate->input[0]->total_input;	
	for(int i=1;i<gate->input.size();++i){
		full=false;		
		//cout<<"A"<<endl;
		for(int s=0,j=0;s<gate->total_input.size() || j<gate->input[i]->total_input.size();){			
			if(!full){
				if(s==gate->total_input.size() || j==gate->input[i]->total_input_name.size())
					full=true;
			}			
			if(!full && gate->total_input_name[s]>gate->input[i]->total_input_name[j]){
				tem_string.push_back(gate->input[i]->total_input_name[j]);
				j++;	
			}
			else if(!full && gate->total_input_name[s]<gate->input[i]->total_input_name[j]){
				tem_string.push_back(gate->total_input_name[s]);
				s++;			
			}
			else{
				if(full && j==gate->input[i]->total_input_name.size()){
					tem_string.push_back(gate->total_input_name[s]);
					s++;
				}
				else if(full && s==gate->total_input_name.size()){
					tem_string.push_back(gate->input[i]->total_input_name[j]);
					j++;
				}
				else {
					tem_string.push_back(gate->total_input_name[s]);
					s++;
					j++;
				}
			}
			
		}
		gate->total_input_name=tem_string;
		tem_string.clear();		
	}
	//cout<<"end replace input"<<endl<<"start replace cut..."<<endl;
	
	gate->total_cut_name=gate->input[0]->total_cut_name;
	for(int i=1;i<gate->input.size();++i){
		full=false;
		for(int s=0,j=0;s<gate->total_cut_name.size() || j<gate->input[i]->total_cut_name.size();){			
			if(!full){
				if(s==gate->total_cut_name.size() || j==gate->input[i]->total_cut_name.size())
					full=true;
			}			
			if(!full && gate->total_cut_name[s]>gate->input[i]->total_cut_name[j]){
				tem_int.push_back(gate->input[i]->total_cut_name[j]);
				j++;	
			}
			else if(!full && gate->total_cut_name[s]<gate->input[i]->total_cut_name[j]){
				tem_int.push_back(gate->total_cut_name[s]);
				s++;			
			}
			else{
				if(full && j==gate->input[i]->total_cut_name.size()){
					tem_int.push_back(gate->total_cut_name[s]);
					s++;
				}
				else if(full && s==gate->total_cut_name.size()){
					tem_int.push_back(gate->input[i]->total_cut_name[j]);
					j++;
				}
				else {
					tem_int.push_back(gate->total_cut_name[s]);
					s++;
					j++;
				}
			}
			
		}
		gate->total_cut_name=tem_int;
		tem_int.clear();
					
	}
	//cout<<"end replace cut"<<endl;	
}*/



///////////////////7/11////////
/*void CircuitCmp::CheckDivision(){
		cout<<endl<<endl<<"start checking division..."<<endl;
		if(circuitOne->GetoutputSize()==circuitTwo->GetoutputSize()){
			int outputsize=circuitOne->GetoutputSize();			
			for(int i=0;i<outputsize;i++){
				if(circuitOne->Getoutput(i)->total_input!=circuitTwo->Getoutput(i)->total_input){
					cout<<"input differ:"<<circuitOne->Getoutput(i)->name<<"::"<<circuitTwo->Getoutput(i)->name<<endl;
					cout<<"number of different input:";
					cout<<circuitOne->Getoutput(i)->total_input_name.size()-circuitTwo->Getoutput(i)->total_input_name.size()<<endl<<endl;

				}
			}
		}
		else
			cout<<"output size differ"<<endl;
}*/


