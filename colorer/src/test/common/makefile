
all: test-vector.exe


deps.o: deps.cpp
	gcc -I../../shared/ -g -c deps.cpp

test-vector.o: test-vector.cpp
	gcc -I../../shared/ -g -c test-vector.cpp

test-vector.exe: deps.o test-vector.o
	gcc -g deps.o test-vector.o -lstdc++ -o test-vector.exe

clean:
	rm *.o *.exe