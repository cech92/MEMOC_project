CC = g++
CPPFLAGS = -g -Wall -O2
CCOPTS = -std=c++11 -c -g -O0 -Wall -Wno-deprecated -Wunused-variable -Wno-sign-compare
LDFLAGS =

CPX_BASE    = /Applications/CPLEX_Studio1210
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_osx/static_pic
CPX_LDFLAGS = -lcplex -lm -pthread -ldl

OBJ = problem.o cplex_solver.o main.o

#%.o: %.cpp %.h
#		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)


main.o: main.cpp
	$(CC) $(CCOPTS) main.cpp -I$(CPX_INCDIR)

problem.o: problem.h problem.cpp
	$(CC) $(CCOPTS) problem.cpp -I$(CPX_INCDIR)

cplex_solver.o: cplex/cplex_solver.h cplex/cplex_solver.cpp
	$(CC) $(CCOPTS) cplex/cplex_solver.cpp -I$(CPX_INCDIR)

clean:
		rm -rf $(OBJ) main

.PHONY: clean
