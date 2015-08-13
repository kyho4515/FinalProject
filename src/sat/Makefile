CXX := g++
CPPFLAGS := -O2

all:	sat.o;

sat.o: sat.h Solver.h
	$(CXX) $(CPPFLAGS) -c $< -o $@

