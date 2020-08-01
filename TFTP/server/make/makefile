GCC= gcc
CFLAGS= -c -Wall -g
SRC= ./../src/
BIN= ./../bin/
OBJ= ./../obj/
INC= ./../inc/
GCOV_FLAGS= -fprofile-arcs -ftest-coverage
VFLAGS= -v --tool=memcheck --leak-check=yes --show-reachable=yes


all: $(BIN)server $(BIN)client

$(BIN)server: $(OBJ)server.o $(OBJ)serverutil.o $(OBJ)common_function.o
	$(GCC) $(GCOV_FLAGS) $(OBJ)server.o $(OBJ)serverutil.o $(OBJ)common_function.o -o server
	mv server $(BIN)

$(OBJ)server.o: $(SRC)server.c $(INC)serverutil.h
	$(GCC) $(GCOV_FLAGS) $(CFLAGS) $(SRC)server.c $(INC)
	mv server.o $(OBJ)
	
$(OBJ)serverutil.o: $(SRC)serverutil.c $(INC)serverutil.h $(INC)commonheader.h
	$(GCC) $(GCOV_FLAGS) $(CFLAGS) $(SRC)server.c $(INC)
	mv serverutil.o $(OBJ)

$(OBJ)common_function.o: $(SRC)common_function.c $(INC)commonheader.h
	$(GCC) $(GCOV_FLAGS) $(CFLAGS) $(SRC)common_function.c $(INC)
	mv common_function.o $(OBJ)

$(BIN)client: $(OBJ)client.o $(OBJ)clientutil.o $(OBJ)common_function.o
	$(GCC) $(GCOV_FLAGS) $(OBJ)client.o $(OBJ)clientutil.o $(OBJ)common_function.o -o client
	mv client $(BIN)

$(OBJ)client.o: $(SRC)client.c $(INC)clientutil.h
	$(GCC) $(GCOV_FLAGS) $(CFLAGS) $(SRC)client.c $(INC)/
	mv client.o $(OBJ)

$(OBJ)clientutil.o: $(SRC)clientutil.c $(INC)clientutil.h $(INC)commonheader.h
	$(GCC) $(GCOV_FLAGS) $(CFLAGS) $(SRC)clientutil.c $(INC)/
	mv clientutil.o $(OBJ)

clean:
	rm -f $(BIN)server
	rm -f $(BIN)client
	rm -f $(OBJ)*.o



