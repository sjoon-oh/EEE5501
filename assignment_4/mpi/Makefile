CC=mpic++
CFLAG=-Wall -Werror -g -std=c++11

SRC=$(wildcard *.cc)
HDR=$(wildcard *.h)
OBJ=$(SRC:.cc=.o)
EXE=mpi

.PHONY: clean

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ)

%.o: %.cc $(HDR)
	$(CC) $(CFLAG) -o $@ -c $<

clean:
	rm -f $(OBJ) $(EXE) result

