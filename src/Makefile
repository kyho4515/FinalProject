CXX := g++
CPPFLAGS := -O3 -I../lib
DBGFLAGS := -g -I../lib
WHERE := cudd
LIB := $(WHERE)/obj/libobj.a $(WHERE)/cudd/libcudd.a $(WHERE)/mtr/libmtr.a	$(WHERE)/st/libst.a $(WHERE)/util/libutil.a $(WHERE)/epd/libepd.a
all: ../bin/equivalenceCut
	@echo -n ""

dbg: ../bin/equivalenceCut_dbg
	@echo -n ""

../bin/equivalenceCut: main.o CirMgr.o Gate.o Simulation.o util.o CircuitCmp.o Strash.o Sat.o ../lib/tm_usage.o ../lib/libsat.a DepList.o ConstWire.o pair.o shortcut.o
	$(CXX) $^ -o $@


../bin/equivalenceCut_dbg: main_dbg.o CirMgr_dbg.o Gate_dbg.o util_dbg.o Simulation_dbg.o CircuitCmp_dbg.o Strash_dbg.o Sat_dbg.o ../lib/tm_usage.o ../lib/libsat.a DepList_dbg.o ConstWire_dbg.o pair_dbg.o shortcut_dbg.o
	$(CXX) $^ -o $@

main.o: main.cpp Gate.h CirMgr.h CircuitCmp.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

CirMgr.o: CirMgr.cpp CirMgr.h Gate.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

Gate.o: Gate.cpp Gate.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

Simulation.o: Simulation.cpp util.cpp CircuitCmp.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

util.o: util.cpp util.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

CircuitCmp.o: CircuitCmp.cpp CircuitCmp.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

Strash.o: Strash.cpp CircuitCmp.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

shortcut.o: shortcut.cpp CircuitCmp.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

Sat.o: Sat.cpp CircuitCmp.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

DepList.o: DepList.cpp DepList.h Gate.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

ConstWire.o: ConstWire.cpp ConstWire.h Gate.h util.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

pair.o: pair.cpp pair.h Gate.h util.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

main_dbg.o: main.cpp Gate.h CirMgr.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

CirMgr_dbg.o: CirMgr.cpp CirMgr.h Gate.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

Gate_dbg.o: Gate.cpp Gate.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

Simulation_dbg.o: Simulation.cpp CircuitCmp.h CirMgr.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

util_dbg.o: util.cpp util.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

CircuitCmp_dbg.o: CircuitCmp.cpp CircuitCmp.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

Strash_dbg.o: Strash.cpp CircuitCmp.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

shortcut_dbg.o: shortcut.cpp CircuitCmp.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

Sat_dbg.o: Sat.cpp CircuitCmp.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

DepList_dbg.o: DepList.cpp DepList.h Gate.h
	$(CXX) $(DBGFLAGS) -c $< -o $@

ConstWire_dbg.o: ConstWire.cpp ConstWire.h Gate.h util.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

pair_dbg.o: pair.cpp pair.h Gate.h util.h
	$(CXX) $(CPPFLAGS) -c $< -o $@
