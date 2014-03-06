all: worker main.cpp
	$(CC) -o main main.cpp

worker: worker.cpp
	$(CC) -c -o worker.o worker.cpp

clean:
	rm -f main
