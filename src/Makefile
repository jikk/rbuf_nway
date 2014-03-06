#CFLAGS=-O3
CFLAGS=-g

all: worker main.cpp
	$(CXX) $(CFLAGS) -pthread -o main main.cpp worker.o

worker: worker.cpp
	$(CXX) $(CFLAGS) -c -o worker.o worker.cpp

clean:
	rm -f main worker.o core
