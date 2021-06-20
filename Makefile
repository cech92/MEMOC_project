CC = g++
CPPFLAGS = -g -Wall -O2
CCOPTS = -std=c++11 -c -g -O0 -Wall -Wno-deprecated -Wunused-variable -Wno-sign-compare
LDFLAGS =

# labTA paths
#CPX_BASE    = /server1/1/2012/lceccon
CPX_BASE    = /opt/ibm/ILOG/CPLEX_Studio128
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_linux/static_pic

# personal macOS paths
#CPX_BASE    = /Applications/CPLEX_Studio1210
#CPX_INCDIR  = $(CPX_BASE)/cplex/include
#CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_osx/static_pic

CPX_LDFLAGS = -lcplex -lm -pthread -ldl

OBJ = problem.o cplex_solver.o ant_colony_solver.o simulated_annealing_solver.o main.o

#%.o: %.cpp %.h
#		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main.out: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main.out -L$(CPX_LIBDIR) $(CPX_LDFLAGS)


main.o: main.cpp
	$(CC) $(CCOPTS) main.cpp -I$(CPX_INCDIR)

problem.o: problem.h problem.cpp
	$(CC) $(CCOPTS) problem.cpp -I$(CPX_INCDIR)

cplex_solver.o: cplex/cplex_solver.h cplex/cplex_solver.cpp
	$(CC) $(CCOPTS) cplex/cplex_solver.cpp -I$(CPX_INCDIR)

ant_colony_solver.o: ant_colony/ant_colony_solver.h ant_colony/ant_colony_solver.cpp
	$(CC) $(CCOPTS) ant_colony/ant_colony_solver.cpp

simulated_annealing_solver.o: simulated_annealing/simulated_annealing_solver.h simulated_annealing/simulated_annealing_solver.cpp
	$(CC) $(CCOPTS) simulated_annealing/simulated_annealing_solver.cpp

clean:
		rm -rf $(OBJ) main.out problem.o cplex_solver.o ant_colony_solver.o simulated_annealing_solver.o main.o

.PHONY: clean
