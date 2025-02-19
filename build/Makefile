CXX = g++
CC = gcc
CXXFLAGS = -std=c++17 -g -Wall -Wextra

INCLD_DIR = -I ../include/

GRB_HOME = /apps/software/gurobi1000/linux64

GRB_INCLD_DIR = -I $(GRB_HOME)/include/

GRB_LB_DIR = -L $(GRB_HOME)/lib/

GRB_LB_ADD = -lgurobi_g++5.2 -lgurobi100

OBJS = MATH_Large

$(OBJS).a: \
	../src/Acceptance.cpp \
	../src/ALNS.cpp \
	../src/Callback.cpp \
	../src/Compare.cpp \
	../src/DestroyMethod.cpp \
	../src/ExchangeMethod.cpp \
	../src/InitialSolutionCreator.cpp \
	../src/Instance.cpp \
	../src/main.cpp \
	../src/MasterProblem.cpp \
	../src/PublicFunction.cpp \
	../src/RelocateMethod.cpp \
	../src/RepairMethod.cpp \
	../src/ShakeMethod.cpp \
	../src/Solution.cpp \
	../src/SolutionPool.cpp \
	../src/TabuObject.cpp \
	../src/TabuOperatorList.cpp \
	../src/TabuSolutionList.cpp

	$(CXX) $(CXXFLAGS) $^ -o $@ $(INCLD_DIR) $(GRB_INCLD_DIR) $(GRB_LB_DIR) $(GRB_LB_ADD)

clean:
	rm -f *.out *~ 
