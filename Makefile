CC = g++
CPPFLAGS = -g -Wall -O2
LDFLAGS =

CPX_BASE    = /Applications/CPLEX_Studio1210
CPX_INCDIR  = $(CPX_BASE)/cplex/include
CPX_LIBDIR  = $(CPX_BASE)/cplex/lib/x86-64_osx/static_pic
CPX_LDFLAGS = -lcplex -lm -pthread -ldl

OBJ = main.o

%.o: %.cpp
		$(CC) $(CPPFLAGS) -I$(CPX_INCDIR) -c $^ -o $@

main: $(OBJ)
		$(CC) $(CPPFLAGS) $(OBJ) -o main -L$(CPX_LIBDIR) $(CPX_LDFLAGS)

clean:
		rm -rf $(OBJ) main

.PHONY: clean
