CXXFLAGS = -std=c++11 -O3

all: viterbi

viterbi: viterbi.cpp

clean:
	rm -f viterbi
