CC = g++
CPPFLAGS = -std=c++11 -g
OBJ = main.o AsyncLibraryUnSynced.o AsyncLibrarySynced.o

Output: $(OBJ)
	$(CC) $(CPPFLAGS) $(OBJ)

AsyncLibraryUnSynced.o:
	$(CC) $(CPPFLAGS) -c AsyncLibraryUnSynced.hpp -o AsyncLibraryUnSynced.o

AsyncLibrarySynced.o:
	$(CC) $(CPPFLAGS) -c AsyncLibrarySynced.hpp -o AsyncLibrarySynced.o

main.o:
	$(CC) $(CPPFLAGS) -c main.cpp -o main.o

clean:
	rm *.o a.out
