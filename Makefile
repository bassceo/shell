CXX = g++
CXXFLAGS = -std=c++17 -pthread
LDFLAGS = -lstdc++fs

SRCS = algorithms/bin-search.cpp algorithms/ema-sort-int.cpp

all: shell load_tester load_tester_opt

shell: shell.cpp
	$(CXX) $(CXXFLAGS) -o shell shell.cpp $(LDFLAGS)

load_tester: load_tester.cpp $(SRCS)
	$(CXX) $(CXXFLAGS) -o load_tester load_tester.cpp $(SRCS) $(LDFLAGS)

load_tester_opt: load_tester.cpp $(SRCS)
	$(CXX) $(CXXFLAGS) -O3 -o load_tester_opt load_tester.cpp $(SRCS) $(LDFLAGS)

clean:
	rm -f shell load_tester load_tester_opt