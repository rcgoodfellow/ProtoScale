CXX=clang++
FLEX=/usr/local/Cellar/flex/2.5.37/bin/flex
BISON=/usr/local/Cellar/bison/3.0.2/bin/bison
CXXFLAGS=-x c++ -g -std=c++11 -Wno-deprecated-register

.PHONY: all
all: proto

.PHONY: debug
debug: CXXFLAGS+=-DDEBUG
debug: all

proto: Driver.cpp Driver.hpp libprotoscale_parse.so
	$(CXX) $(CXXFLAGS) -o $@ $< -L . -l protoscale_parse

LIB_PARSE_OBJS=MetaParser.o    MetaScanner.o \
							 CommandParser.o CommandScanner.o \
							 CommandAST.o \
							 MetaASTPrinter.o \
							 FileUtil.o

LIB_PARSE_HDR=MetaAST.hpp CommandAST.hpp MetaASTPrinter.hpp

libprotoscale_parse.so : $(LIB_PARSE_OBJS) $(LIB_PARSE_HDR)
	$(CXX) -shared -o $@ $(LIB_PARSE_OBJS)

# MetaModel Language ----------------------------------------------------------

FileUtil.o : FileUtil.cpp FileUtil.hpp
	$(CXX) $(CSSFLAGS) -fpic -c $< -o $@

MetaParser.o : MetaParser.cpp MetaParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

MetaParser.cpp: MetaParser.y
	$(BISON) -p metayy -d -o $@ $<

MetaParser.hpp: MetaParser.cpp
	#^^

MetaScanner.o : MetaScanner.cpp MetaScanner.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

MetaScanner.cpp: MetaScanner.l
	$(FLEX) -P metayy --header-file=MetaScanner.hpp -o $@ $<

MetaScanner.hpp: MetaScanner.cpp
	#^^

# Command Language ------------------------------------------------------------

MetaASTPrinter.o : MetaASTPrinter.cpp MetaASTPrinter.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandAST.o : CommandAST.cpp CommandAST.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandParser.o : CommandParser.cpp CommandParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandParser.cpp : CommandParser.y
	$(BISON) -p cmdyy -d -o CommandParser.cpp $<

CommandParser.hpp : CommandParser.cpp
	#^^

CommandScanner.o : CommandScanner.cpp CommandScanner.hpp CommandParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandScanner.cpp : CommandScanner.l
	$(FLEX) -P cmdyy --header-file=CommandScanner.hpp -o $@ $<

CommandScanner.hpp : CommandScanner.cpp
	#^^

.PHONY: clean
clean:
	rm -f *.o
	rm -f *.so
	rm -f proto
	rm -f MetaParser.cpp MetaScanner.cpp MetaParser.hpp MetaScanner.hpp
	rm -f CommandScanner.cpp CommandScanner.hpp CommandParser.hpp CommandParser.cpp

