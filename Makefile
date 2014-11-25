
OBJ = scanner.o parser.o main.o


CPP = g++
BISONCPP = bisonc++
FLEXCPP = flexc++
TGT = burg
SCANNER = scanner1.ll
PARSER = parser1.yy

CFLAGS = -g 


$(TGT): $(OBJ) 
	$(CPP) --std=c++0x $(OBJ) -o $(TGT)

%.o: %.cc $(HEADERS)
	$(CPP) --std=c++0x $(CFLAGS) -c $<

scanner.cc scannerbase.h: $(SCANNER) parserbase.h
	$(FLEXCPP) $(SCANNER)
	# We need to modify the scanner.h file to include parserbase.h so that the 
	# the scanner could return a token defined in the parser.
	# Please visit http://www.cse.iitb.ac.in/~uday/cfglp for more details of the
	# scanner parser interaction in the context of flexc++ and bisonc++.
	cp modified_scanner.h scanner.h
	cp modified_parser.h parser.h


parser.cc parserbase.h: $(PARSER) 
	$(BISONCPP) --verbose $(PARSER)

clean :
	rm -rf *.o $(TGT)  scanner.cc parser.cc *.yy.output scannerbase.h parserbase.h scanner.h scanner.ih parser.h parser.ih
	rm -rf cscope.out tags a.out

