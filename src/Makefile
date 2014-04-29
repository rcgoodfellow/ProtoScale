CXX=clang++
FLEX=/usr/local/Cellar/flex/2.5.37/bin/flex
BISON=/usr/local/Cellar/bison/3.0.2/bin/bison
CXXFLAGS=-x c++ -g -std=c++11 -Wno-deprecated-register

.PHONY: all
all: proto lib/libprotoscale_core.so

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
ModelParser.o ModelScanner.o ModelAST.o \
CppGen.o DynamicsSniffer.o

LIB_PARSE_HDR=MetaAST.hpp CommandAST.hpp MetaASTPrinter.hpp

libprotoscale_parse.so : $(LIB_PARSE_OBJS) $(LIB_PARSE_HDR)
	$(CXX) -shared -o $@ $(LIB_PARSE_OBJS)

LIB_CORE_OBJS=lib/Core.o

LIB_CORE_HDR=lib/Core.hpp

lib/libprotoscale_core.so : $(LIB_CORE_OBJS) $(LIB_CORE_HDR)
	$(CXX) -shared -o $@ $(LIB_CORE_OBJS)

DynamicsSniffer.o: DynamicsSniffer.cpp DynamicsSniffer.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

# CodeGen ---------------------------------------------------------------------

CppGen.o: CppGen.cpp CppGen.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

# Core ------------------------------------------------------------------------

lib/Core.o : lib/Core.cpp lib/Core.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

# MetaModel Language ----------------------------------------------------------

MetaAST.o : MetaAST.cpp MetaAST.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

FileUtil.o : FileUtil.cpp FileUtil.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

MetaParser.o : MetaParser.cpp MetaParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

Sema.o : Sema.cpp Sema.hpp CompilationException.hpp MetaParser.hpp ModelParser.hpp \
				 MetaScanner.hpp ModelScanner.hpp
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

# Model Language --------------------------------------------------------------

ModelAST.o : ModelAST.cpp ModelAST.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

ModelParser.o : ModelParser.cpp ModelParser.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

ModelParser.cpp : ModelParser.y ModelAST.hpp
	$(BISON) -p modelyy -d -o $@ $<

ModelParser.hpp: ModelParser.cpp
	#^^

ModelScanner.o : ModelScanner.cpp ModelScanner.hpp
	$(CXX) $(CXXFLAGS) -fpic -c $< -o $@

ModelScanner.cpp: ModelScanner.l ModelAST.hpp
	$(FLEX) -P modelyy --header-file=ModelScanner.hpp -o $@ $<

ModelScanner.hpp: ModelScanner.cpp
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
	rm -f *.pmm.hpp
	rm -f proto
	rm -f MetaParser.cpp MetaScanner.cpp MetaParser.hpp MetaScanner.hpp
	rm -f ModelParser.cpp ModelParser.hpp ModelScanner.cpp ModelScanner.hpp
	rm -f CommandScanner.cpp CommandScanner.hpp CommandParser.hpp CommandParser.cpp

