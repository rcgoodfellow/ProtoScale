CXX=clang++
FLEX=/usr/local/Cellar/flex/2.5.37/bin/flex
BISON=/usr/local/Cellar/bison/3.0.2/bin/bison
CXXFLAGS=-x c++ -std=c++11

.PHONY: all
all: psc

psc: Parser.cpp Tokens.cpp
	$(CXX) $(CXXFLAGS) -Wno-deprecated-register -o psc Parser.cpp Tokens.cpp Driver.cpp

Parser.cpp: Parser.y
		$(BISON) -d -o Parser.cpp Parser.y

Tokens.cpp: Tokens.l
		$(FLEX) -o Tokens.cpp Tokens.l

.PHONY: clean
clean:
	rm -f psc Parser.cpp Tokens.cpp Parser.hpp Tokens.hpp
