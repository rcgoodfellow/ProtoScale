CXX=clang++
FLEX=/usr/local/Cellar/flex/2.5.37/bin/flex
BISON=/usr/local/Cellar/bison/3.0.2/bin/bison
CXXFLAGS=-x c++ -g -std=c++11 -Wno-deprecated-register

.PHONY: all
all: proto

.PHONY: debug
debug: CXXFLAGS+=-DDEBUG
debug: all

proto: Driver.cpp Driver.hpp libprotoscale_parse.so CompilationException.hpp\
			 Sema.hpp
	$(CXX) $(CXXFLAGS) -o $@ $< -L . -l protoscale_parse

LIB_PARSE_OBJS=\
FileUtil.o \
MetaParser.o MetaScanner.o MetaAST.o MetaASTPrinter.o Sema.o \
CommandParser.o CommandScanner.o CommandAST.o \
ShellParser.o ShellScanner.o ShellAST.o

LIB_PARSE_HDR=MetaAST.hpp CommandAST.hpp MetaASTPrinter.hpp

libprotoscale_parse.so : $(LIB_PARSE_OBJS) $(LIB_PARSE_HDR)
	$(CXX) -shared -o $@ $(LIB_PARSE_OBJS)

# MetaModel Language ----------------------------------------------------------

MetaAST.o : MetaAST.cpp MetaAST.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

FileUtil.o : FileUtil.cpp FileUtil.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

MetaParser.o : MetaParser.cpp MetaParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

Sema.o : Sema.cpp Sema.hpp CompilationException.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

MetaParser.cpp: MetaParser.y MetaAST.hpp
	$(BISON) -p metayy -d -o $@ $<

MetaParser.hpp: MetaParser.cpp
	#^^

MetaScanner.o : MetaScanner.cpp MetaScanner.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

MetaScanner.cpp: MetaScanner.l MetaAST.hpp
	$(FLEX) -P metayy --header-file=MetaScanner.hpp -o $@ $<

MetaScanner.hpp: MetaScanner.cpp
	#^^

# Shell Language --------------------------------------------------------------

ShellAST.o : ShellAST.cpp ShellAST.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

ShellParser.o : ShellParser.cpp ShellParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

ShellParser.cpp : ShellParser.y ShellAST.hpp
	$(BISON) -p shellyy -d -o $@ $<

ShellParser.hpp: ShellParser.cpp
	#^^

ShellScanner.o : ShellScanner.cpp ShellScanner.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

ShellScanner.cpp: ShellScanner.l ShellAST.hpp
	$(FLEX) -P shellyy --header-file=ShellScanner.hpp -o $@ $<

ShellScanner.hpp: ShellScanner.cpp
	#^^

# Command Language ------------------------------------------------------------

MetaASTPrinter.o : MetaASTPrinter.cpp MetaASTPrinter.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandAST.o : CommandAST.cpp CommandAST.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandParser.o : CommandParser.cpp CommandParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandParser.cpp : CommandParser.y CommandAST.hpp MetaAST.hpp
	$(BISON) -p cmdyy -d -o CommandParser.cpp $<

CommandParser.hpp : CommandParser.cpp
	#^^

CommandScanner.o : CommandScanner.cpp CommandScanner.hpp CommandParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

CommandScanner.cpp : CommandScanner.l CommandAST.hpp MetaAST.hpp
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

