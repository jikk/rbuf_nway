all: worker main.cpp
	$(CXX) -o main main.cpp worker.o

worker: worker.cpp
	$(CXX) -c -o worker.o worker.cpp

clean:
	rm -f main
