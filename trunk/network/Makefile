app : Data.o Network.o Socket.o strip.o fill.o test.o
	g++ -o temp/app temp/Data.o temp/Network.o temp/Socket.o temp/strip.o temp/fill.o temp/test.o

Data.o : Data.cpp Data.h
	g++ -c -o temp/Data.o Data.cpp

Network.o : Network.cpp Network.h
	g++ -c -o temp/Network.o Network.cpp

Socket.o : Socket.cpp Socket.h
	g++ -c -o temp/Socket.o Socket.cpp

strip.o : strip.cpp strip.h
	g++ -c -o temp/strip.o strip.cpp

fill.o : fill.cpp fill.h
	g++ -c -o temp/fill.o fill.cpp

test.o : test.cpp
	g++ -c -o temp/test.o test.cpp

clean :
	rm temp/*.o temp/app
