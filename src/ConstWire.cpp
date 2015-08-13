#include "ConstWire.h"


ConstWire::ConstWire(vector<Gate*>output){
	////
	for(int output_num=0;output_num < output.size();output_num++)
		DFS(output[output_num]);
	for(int i=0;i < storage.size();i++){
		if(storage[i]->gateType==Input)
			input.push_back(storage[i]);
		else if(storage[i]->gateType==Wir && ((Wire*)storage[i])->isCut()){
			bool findset=false;			
			int cutnum=((Wire*)storage[i])->cutNum;
			for(int cut=0;cut<cutset.size();cut++){				
				if(((Wire*)cutset[cut]->at(0))->cutNum == cutnum){
					findset=true;
					cutset[cut]->push_back(storage[i]);
					break;
				}
			}			
			if(!findset){
				cutset.push_back(new vector<Gate*>);
				cutset[cutset.size()-1]->push_back(storage[i]);
			}
		}
		else if(storage[i]->gateType==Const){
			if(constinput.size() < 2){
				if(constinput.size()==0)
					constinput.push_back(storage[i]);
				else if(constinput[0]->phase != storage[i]->phase)
					constinput.push_back(storage[i]);
			}
		}
		else
			dfslist.push_back(storage[i]);
	}
	resetTraversed();
	////
	solver.initialize();	
	genProofModel();
	////
	cout<<"input size: "<<input.size()<<endl;
	cout<<"constinput size: "<<constinput.size()<<endl;
	cout<<"dfslist size:"<<dfslist.size()<<endl;

	
	number[0]=1431655765;
	number[1]=858993459;
	number[2]=252645135;
	number[3]=16711935;
	number[4]=65535;
	Find();
	cout<<"const 0 size:"<<const0.size()<<endl;
	cout<<"const 1 size:"<<const1.size()<<endl;
	//assert(0);
}

//挑出是const的wire
void ConstWire::Find(){
	RandGenerator rand;
	int tmp;
	vector<Wire*>tem0;
	vector<Wire*>tem1;

	//隨機給予每個input不同的值
	for(int input_num=0; input_num < input.size(); ++input_num){
		tmp = rand.getRand();
		input[input_num] -> curSim = tmp;
  	}

	//挑出simulation後結果可能為const的wire
  	for(int gate_num=0;gate_num < dfslist.size(); gate_num++){
		dfslist[gate_num] -> operate();	
		if(dfslist[gate_num]->gateType == Wir && dfslist[gate_num]->curSim==0 )
			tem0.push_back((Wire*)dfslist[gate_num]);
		else if(dfslist[gate_num]->gateType == Wir && dfslist[gate_num]->curSim==~0)
			tem1.push_back((Wire*)dfslist[gate_num]);
	}
	
	sort(tem0.begin(),tem0.end(),sortcompare);
	sort(tem1.begin(),tem1.end(),sortcompare);
	cout<<tem0.size()<<" wire 0 waiting for check"<<endl;
	cout<<tem1.size()<<" wire 1 waiting for check"<<endl;
	
	cout<<"start tem0"<<endl;
	for(int i=0;i<tem0.size();i++){
		cout<<"tem0: "<<i<<endl<<"input size: "<<tem0[i]->total_input.size()<<endl;
		cout<<"gate level: "<<tem0[i]->gateLevel<<endl;
		if(tem0[i]->total_input.size()<=5 && ProveBySimulate(tem0[i],0)){
			cout<<"***simualte***"<<endl;
			const0.push_back(tem0[i]);
		}
		else if(tem0[i]->total_input.size()>5 && ProveBySAT(tem0[i],0)){
			cout<<"***Sat***"<<endl;
			const0.push_back(tem0[i]);
			//assert(0);
		}
		cout<<"clauses size: "<<solver.clause_size()<<endl;
		cout<<"assump size: "<<solver.assump_size()<<endl;
		cout<<"................."<<endl;
	}
	
	//assert(0);
	for(int i=0;i<tem1.size();i++){
		cout<<"tem1: "<<i<<endl<<"input size: "<<tem1[i]->total_input.size()<<endl;
		cout<<"gate level: "<<tem1[i]->gateLevel<<endl;
		if(tem1[i]->total_input.size()<=5 && ProveBySimulate(tem1[i],~0)){	
			cout<<"***simualte***"<<endl;
			const1.push_back(tem1[i]);
		}
		else if(tem1[i]->total_input.size()>5 && ProveBySAT(tem1[i],~0)){
			cout<<"***Sat***"<<endl;
			const1.push_back(tem1[i]);
		}
		cout<<"clauses size: "<<solver.clause_size()<<endl;
		cout<<"assump size: "<<solver.assump_size()<<endl;
		cout<<"................."<<endl;
	}

}

bool ConstWire::ProveBySimulate(Gate* source,int value){
	assert(storage.size()==0);
	DFS(source);
	for(int input_num=0; input_num < source->total_input.size(); ++input_num){
		assert(input_num < 5);
		source->total_input[input_num]->curSim=number[input_num];
  	}
	for(int index=0;index<storage.size();index++)
		storage[index]->operate();
	
	resetTraversed();
	if(source->curSim==value){
		if(value==~0)
			solver.assumeProperty(source->getVar(),true);
		else {
			assert(value==0);
			solver.assumeProperty(source->getVar(),false);
	}
		return true;
	}
	else
		return false;	
}
bool ConstWire::ProveBySAT(Gate* source,int value){
	assert(source->gateType==Wir);
	DFS(source);
	//add Aig
	for(int i=0;i<storage.size();i++){
		storage[i]->constructSat(solver,T);
	}
	Var fin=solver.newVar();
	if(value==~0)
		solver.addXorCNF(fin, source->getVar(), false, T, false);
	else
		solver.addXorCNF(fin, source->getVar(), false, F, false);
	//assume properties
	//solver.assumeRelease();
	cout<<"assump size: "<<solver.assump_size()<<endl;
	solver.assumeProperty(fin, true);
	cout<<"assump size: "<<solver.assump_size()<<endl;
	cout<<"clauses size: "<<solver.clause_size()<<endl;
	cout<<"store size: "<<storage.size()<<endl;
	resetTraversed();
	bool result=solver.assumpSolve();
	solver.pop_assump();
	if(!result && value==~0)
		solver.assumeProperty(source->getVar(),true);
	else if(!result){
		assert(value==0);
		solver.assumeProperty(source->getVar(),false);
	}
	
	
	return !(result);
}

void ConstWire::genProofModel(){
	//generate input var	
	for(int i=0;i < input.size();i++){
		Var v = solver.newVar();
		input[i]->setVar(v);
	}
	//generate cut var
	for(int i=0; i < cutset.size(); ++i){
      Var v = solver.newVar();
		for(int j=0;j < cutset[i]->size();++j){
			cutset[i]->at(j)->setVar(v);
		}
	}
	//generate wire and output var
	for(int i=0;i < dfslist.size();i++){
		if((dfslist[i]->gateType==Wir && !(((Wire*)dfslist[i])->isCut())) || dfslist[i]->gateType==Output){
			Var v = solver.newVar();
			dfslist[i]->setVar(v);
		}	
	}
	//generate constinput var
	T=solver.newVar();
	F=solver.newVar();
	for(int i=0;i < constinput.size();i++){
		if(constinput[i]->phase)
			constinput[i]->setVar(T);
		else 
			constinput[i]->setVar(F);
	}
	solver.assumeProperty(T,true);
	solver.assumeProperty(F,false);

	
}
void ConstWire::Out()const{



}

void ConstWire::DFS(Gate* source){
	source->traversed=true;
	if(source->gateType!=Wir || !(((Wire*)source)->isCut())){
		for(int i=0;i<source->input.size();i++){
			if(!(source->input[i]->traversed))
				DFS(source->input[i]);
		}
	}
	storage.push_back(source);
}

void ConstWire::resetTraversed(){
   for(int i=0; i < storage.size(); ++i)
     storage[i] -> traversed = false;
	storage.clear();
}
