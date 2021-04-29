CC=g++
CFLAG=-Wall -Werror -g -std=c++11

SRC=$(wildcard *.cc)
HDR=$(wildcard *.h)
OBJ=$(SRC:.cc=.o)
EXE=thread

.PHONY: clean

$(EXE): $(OBJ)
	$(CC) -o $@ $(OBJ) -pthread

%.o: %.cc $(HDR)
	$(CC) $(CFLAG) -o $@ -c $<

clean:
	rm -f $(OBJ) $(EXE)

