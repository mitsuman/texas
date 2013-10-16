CXX=g++
CXXFLAGS = -O2
#CXXFLAGS += -DDEBUG

test: test.exe
	./test.exe || echo ""

test.exe: test.o poker.o poker.test.o
	g++ -o test.exe test.o poker.o poker.test.o -lcppunit

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $<

poker.o: poker.h
poker.test.o: poker.h

clean:
	rm -f *.o *.exe *.out
