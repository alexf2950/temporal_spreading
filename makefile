IDIR =inc/
CC=g++
CXXFLAGS=-I$(IDIR) -std=c++0x

ODIR=src/obj



_DEPS = graph.h custom_io.h property_tools.h random_tools.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o property_tools.o graph.o custom_io.o random_tools.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: src/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CXXFLAGS)

temporal_spreading: $(OBJ)
	g++ -o $@ $^ $(CXXFLAGS)


clean:
	rm $(ODIR)/*.o
