CXX      ?= g++
CXXFLAGS ?= -std=c++20
CPPFLAGS ?= -O3 -Wall -I"../include"


SOURCES = main.cpp
OBJECTS = $(SOURCES: .cpp=.o)
HEADERS = Matrix.hpp Matrix_imp.hpp chrono.hpp random_vec.hpp
EXEC = main

all: $(EXEC)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(EXEC) : $(OBJECTS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -o $(EXEC) $(OBJECTS)
	


clean:
	@rm *.o *.a