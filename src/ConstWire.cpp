#include "ConstWire.h"


ConstWire::ConstWire(vector<Gate*>output){
	////
	fout.open("./const.csv",ios::out);
	fout<<"simulation time,tem0 size,tem1 size\n";
	for(int output_num=0;output_num < output.size();output_num++)
		DFS(output[output_num],storage);
	Categorize();	
	///
	/*int leng=dfslist.size();
	for(int i=0;i<leng;i++){
		if(dfslist[i]->gateType==Wir){
			if(ProveBySAT(dfslist[i],0))
				const0.push_back((Wire*)dfslist[i]);
			else if(ProveBySAT(dfslist[i],~0))
				const1.push_back((Wire*)dfslist[i]);
		}
		cout<<leng-i-1<<" left"<<endl;
	}
	cout<<const0.size()<<endl<<const1.size()<<endl;
	assert(0);*/
	////
	cout<<"input size: "<<input.size()<<endl;
	cout<<"constinput size: "<<constinput.size()<<endl;
	cout<<"dfslist size:"<<dfslist.size()<<endl;
	//assert(0);
	
	number[0]=1431655765;
	number[1]=858993459;
	number[2]=252645135;
	number[3]=16711935;
	number[4]=65535;
	Find();
	cout<<"const 0 size:"<<const0.size()<<endl;
	cout<<"const 1 size:"<<const1.size()<<endl;
	cout<<"start check..."<<endl;
	Check();
	cout<<"end check..."<<endl;
	//assert(0);
}

//挑出是const的wire
void ConstWire::Find(){
	RandGenerator rand;
	int tmp;
	int time=500;
	vector<Wire*>tem0;
	vector<Wire*>tem1;
	vector<Gate*>tem_dfs;
	for(int i=0;i<time;i++){
		//隨機給予每個input不同的值
		for(int input_num=0; input_num < input.size(); ++input_num){
			tmp = rand.getRand();
			input[input_num] -> curSim = tmp;
 	 	}
		//給cut值
		for(int cut_num=0; cut_num < cutset.size(); ++cut_num){
     	 	tmp = rand.getRand();
			for(int j=0;j < cutset[cut_num]->size();++j){
				cutset[cut_num]->at(j)->curSim=tmp;
			}
		}

		//挑出simulation後結果可能為const的wire
		if(i==0){//第一次		
  			for(int gate_num=0;gate_num < dfslist.size(); gate_num++){
				dfslist[gate_num] -> operate();	
				if(dfslist[gate_num]->gateType == Wir && dfslist[gate_num]->curSim==0 )
					tem0.push_back((Wire*)dfslist[gate_num]);
				else if(dfslist[gate_num]->gateType == Wir && dfslist[gate_num]->curSim==~0)
					tem1.push_back((Wire*)dfslist[gate_num]);
			}
			assert(storage.size()==0);			
			for(int j=0;j<tem0.size();j++){
				DFS(tem0[j],storage);
			}
			for(int j=0;j<tem1.size();j++){
				DFS(tem1[j],storage);
			}	
			Categorize();
		}
		else{//其他次
			for(int gate_num=0;gate_num < dfslist.size(); gate_num++){
				dfslist[gate_num] -> operate();
			}
			for(int k=0;k<tem0.size();k++){
				if(tem0[k]->curSim != 0){
					vector<Wire*>::iterator it=tem0.begin()+k;
					tem0.erase(it);
					--k;
				}
			}
			for(int k=0;k<tem1.size();k++){
				if(tem1[k]->curSim != ~0){
					vector<Wire*>::iterator it=tem1.begin()+k;
					tem1.erase(it);
					--k;
				}
			}
		}
		cout<<"........."<<i<<" time............"<<endl;
		cout<<"tem0 size: "<<tem0.size()<<endl;
		cout<<"tem1 size: "<<tem1.size()<<endl;
		cout<<"........................"<<endl;
		fout<<i<<","<<tem0.size()<<","<<tem1.size()<<"\n";
		//assert(0);
	}
	//assert(0);
	sort(tem0.begin(),tem0.end(),sortcompare);
	sort(tem1.begin(),tem1.end(),sortcompare);
	cout<<tem0.size()<<" wire 0 waiting for check"<<endl;
	cout<<tem1.size()<<" wire 1 waiting for check"<<endl;
	//assert(0);
	int sim=0,sat=0;
	for(int i=0;i<tem0.size();i++){
		cout<<"tem0: "<<i<<endl<<"input size: "<<tem0[i]->total_input.size()<<endl;
		cout<<"gate level: "<<tem0[i]->gateLevel<<endl;
		if(tem0[i]->total_input.size()<=5 && ProveBySimulate(tem0[i],0)){
			cout<<"***simualte***"<<endl;
			const0.push_back(tem0[i]);
			sim++;
		}
		else if(tem0[i]->total_input.size()>5 && ProveBySAT(tem0[i],0)){
			cout<<"***Sat***"<<endl;
			const0.push_back(tem0[i]);
			sat++;
			//assert(0);
		}
		
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
		cout<<"................."<<endl;
	}
		cout<<sim<<endl;
		cout<<sat<<endl;
}

bool ConstWire::ProveBySimulate(Gate* source,int value){
	assert(storage.size()==0);
	DFS(source,storage);
	for(int input_num=0; input_num < source->total_input.size(); ++input_num){
		assert(input_num < 5);
		source->total_input[input_num]->curSim=number[input_num];
  	}
	for(int index=0;index<storage.size();index++)
		storage[index]->operate();
	
	resetTraversed(storage);
	if(source->curSim==value){
		/*if(value==~0)
			solver.assumeProperty(source->getVar(),true);
		else {
			assert(value==0);
			solver.assumeProperty(source->getVar(),false);
		}*/
		return true;
	}
	else
		return false;	
}
bool ConstWire::ProveBySAT(Gate* source,int value){
	DFS(source,storage);
	Categorize();
	genProofModel();
	assert(source->gateType==Wir);
		
	//add Aig
	for(int i=0;i<dfslist.size();i++){
		dfslist[i]->constructSat(solver,T);
	}
	Var fin=solver.newVar();
	if(value==~0)
		solver.addXorCNF(fin, source->getVar(), false, T, false);
	else
		solver.addXorCNF(fin, source->getVar(), false, F, false);
	//assume properties
	solver.assumeProperty(fin, true);
/*
	cout<<"assump size: "<<solver.assump_size()<<endl;
	cout<<"clauses size: "<<solver.clause_size()<<endl;
	cout<<"store size: "<<storage.size()<<endl;*/
	resetTraversed(storage);
	bool result=solver.assumpSolve();
	return !(result);
}

void ConstWire::genProofModel(){
	solver.initialize();
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

void ConstWire::Categorize(){
	input.clear();
	dfslist.clear();
	assert(storage.size()!=0);
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
	resetTraversed(storage);
}

void ConstWire::Check(){
	vector<Gate*> _input;
	vector<Gate*> _wir;
	vector<Gate*> _const;
	vector<Gate*> other;
	
	for(int i=0;i<const0.size();i++){
		assert(const0[i]->gateType==Wir);
		assert(storage.size()==0);
		DFS(const0[i],storage);
		for(int j=0;j<storage.size();j++){
			if(storage[j]->gateType==Input)
				_input.push_back(storage[j]);
			else if(storage[j]->gateType==Wir)
				_wir.push_back(storage[j]);
			else if (storage[j]->gateType==Const)
				_const.push_back(storage[j]);
			else
				other.push_back(storage[j]);
		}
		//cout<<storage.size()<<endl;
		resetTraversed(storage);

		solver.initialize();
		Var T=solver.newVar();
		Var F=solver.newVar();
		solver.assumeProperty(T,true);
		solver.assumeProperty(F,false);		
		for(int j=0;j<_input.size();j++){
			Var v=solver.newVar();
			_input[j]->setVar(v);
		}
		for(int j=0;j<_wir.size();j++){
			Var v=solver.newVar();
			_wir[j]->setVar(v);
		}
		for(int j=0;j<_const.size();j++){
			if(_const[j]->phase)
				_const[j]->setVar(T);
			else
				_const[j]->setVar(F);
		}
		for(int j=0;j<other.size();j++){
			other[j]->constructSat(solver,T);
		}
		Var fin=solver.newVar();
		solver.addXorCNF(fin, const0[i]->getVar(), false, F, false);
		solver.assumeProperty(fin, true);
		assert(!solver.assumpSolve());
		_input.clear();
		_wir.clear();
		_const.clear();
		other.clear();
	}

	for(int i=0;i<const1.size();i++){
		assert(const1[i]->gateType==Wir);
		assert(storage.size()==0);
		DFS(const1[i],storage);
		for(int j=0;j<storage.size();j++){
			if(storage[j]->gateType==Input)
				_input.push_back(storage[j]);
			else if(storage[j]->gateType==Wir)
				_wir.push_back(storage[j]);
			else if (storage[j]->gateType==Const)
				_const.push_back(storage[j]);
			else
				other.push_back(storage[j]);
		}
		//cout<<storage.size()<<endl;
		resetTraversed(storage);

		solver.initialize();
		Var T=solver.newVar();
		Var F=solver.newVar();
		solver.assumeProperty(T,true);
		solver.assumeProperty(F,false);		
		for(int j=0;j<_input.size();j++){
			Var v=solver.newVar();
			_input[j]->setVar(v);
		}
		for(int j=0;j<_wir.size();j++){
			Var v=solver.newVar();
			_wir[j]->setVar(v);
		}
		for(int j=0;j<_const.size();j++){
			if(_const[j]->phase)
				_const[j]->setVar(T);
			else
				_const[j]->setVar(F);
		}
		for(int j=0;j<other.size();j++){
			other[j]->constructSat(solver,T);
		}
		Var fin=solver.newVar();
		solver.addXorCNF(fin, const1[i]->getVar(), false, T, false);
		solver.assumeProperty(fin, true);
		assert(!solver.assumpSolve());
		_input.clear();
		_wir.clear();
		_const.clear();
		other.clear();
	}
}


void ConstWire::DFS(Gate* source,vector<Gate*> &list){
	source->traversed=true;
	if(source->gateType!=Wir || !(((Wire*)source)->isCut())){
		for(int i=0;i<source->input.size();i++){
			if(!(source->input[i]->traversed))
				DFS(source->input[i],list);
		}
	}
	list.push_back(source);
}

void ConstWire::resetTraversed(vector<Gate*>&list){
   for(int i=0; i < list.size(); ++i)
     list[i] -> traversed = false;
	list.clear();
}
